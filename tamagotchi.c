#include "tamagotchi.h"

// ---------------- shared helpers ----------------

void tama_popup(TamagotchiApp* app, const char* msg) {
    strncpy(app->popup, msg, POPUP_MAX - 1);
    app->popup[POPUP_MAX - 1] = '\0';
    app->popup_ttl = 8; // ~2s at 4 fps
}

void tama_spawn_particles(TamagotchiApp* app, char glyph, uint8_t count) {
    if(!settings_on(&app->settings, FeatureParticles)) return;
    for(uint8_t i = 0; i < PARTICLE_COUNT && count; i++) {
        Particle* p = &app->particles[i];
        if(p->ttl > 0) continue;
        p->glyph = glyph;
        p->x = 54 + (int8_t)tama_rand_max(app, 20);
        p->y = 30 + (int8_t)tama_rand_max(app, 8);
        p->vy = -1 - (int8_t)tama_rand_max(app, 2);
        p->ttl = 8 + (uint8_t)tama_rand_max(app, 6);
        count--;
    }
}

static void update_particles(TamagotchiApp* app) {
    for(uint8_t i = 0; i < PARTICLE_COUNT; i++) {
        Particle* p = &app->particles[i];
        if(p->ttl == 0) continue;
        p->y = (int8_t)(p->y + p->vy);
        if((app->frame & 1) == 0) p->x += (int8_t)(tama_rand_max(app, 3) - 1);
        p->ttl--;
    }
}

void widget_stat_bar(Canvas* canvas, int x, int y, int w, const char* label, uint8_t value) {
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, x, y + 6, label);
    int bx = x + 16;
    int bw = w - 16;
    if(bw < 6) bw = 6;
    canvas_draw_frame(canvas, bx, y, bw, 7);
    int fill = (bw - 2) * value / 100;
    if(fill > 0) canvas_draw_box(canvas, bx + 1, y + 1, fill, 5);
}

// ---------------- callbacks ----------------

static void draw_callback(Canvas* canvas, void* ctx) {
    TamagotchiApp* app = ctx;
    furi_mutex_acquire(app->mutex, FuriWaitForever);

    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    switch(app->scene) {
    case SceneMain: scene_main_draw(canvas, app); break;
    case SceneMenu: scene_menu_draw(canvas, app); break;
    case SceneFeed: scene_feed_draw(canvas, app); break;
    case SceneStats: scene_stats_draw(canvas, app); break;
    case SceneLab: scene_lab_draw(canvas, app); break;
    case SceneAbout: scene_about_draw(canvas, app); break;
    default: break;
    }

    furi_mutex_release(app->mutex);
}

static void input_callback(InputEvent* input_event, void* ctx) {
    FuriMessageQueue* queue = ctx;
    furi_message_queue_put(queue, input_event, FuriWaitForever);
}

static void timer_callback(void* ctx) {
    TamagotchiApp* app = ctx;
    furi_mutex_acquire(app->mutex, FuriWaitForever);

    app->frame++;

    // idle animation: a single-frame blink every ~4s
    app->blink = settings_on(&app->settings, FeatureIdleAnim) && ((app->frame % 16) == 0);

    update_particles(app);
    if(app->popup_ttl > 0) app->popup_ttl--;
    if(app->pet.play_anim > 0) app->pet.play_anim--;

    // weather drifts every ~20s, independent of pet state
    if(settings_on(&app->settings, FeatureWeather) && (app->frame % (20 * FRAMES_PER_SEC)) == 0) {
        app->weather = (uint8_t)tama_rand_max(app, 3);
    }

    // step the simulation at 1 Hz
    if((app->frame % FRAMES_PER_SEC) == 0) {
        app->sec++;
        pet_tick(app);
    }

    furi_mutex_release(app->mutex);
    view_port_update(app->view_port);
}

// ---------------- input dispatch ----------------

static void handle_input(TamagotchiApp* app, InputEvent* event) {
    switch(app->scene) {
    case SceneMain: scene_main_input(app, event); break;
    case SceneMenu: scene_menu_input(app, event); break;
    case SceneFeed: scene_feed_input(app, event); break;
    case SceneStats: scene_stats_input(app, event); break;
    case SceneLab: scene_lab_input(app, event); break;
    case SceneAbout: scene_about_input(app, event); break;
    default: break;
    }
}

// ---------------- lifecycle ----------------

static TamagotchiApp* tamagotchi_alloc(void) {
    TamagotchiApp* app = malloc(sizeof(TamagotchiApp));
    memset(app, 0, sizeof(TamagotchiApp));

    app->rng = (furi_get_tick() * 2654435761u) | 1u;
    app->scene = SceneMain;
    app->running = true;
    settings_defaults(&app->settings);
    pet_init(&app->pet);

    app->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    app->event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    app->view_port = view_port_alloc();
    view_port_draw_callback_set(app->view_port, draw_callback, app);
    view_port_input_callback_set(app->view_port, input_callback, app->event_queue);

    app->gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    app->notifications = furi_record_open(RECORD_NOTIFICATION);

    app->timer = furi_timer_alloc(timer_callback, FuriTimerTypePeriodic, app);
    furi_timer_start(app->timer, furi_ms_to_ticks(TAMA_TICK_MS));

    return app;
}

static void tamagotchi_free(TamagotchiApp* app) {
    furi_timer_stop(app->timer);
    furi_timer_free(app->timer);

    gui_remove_view_port(app->gui, app->view_port);
    view_port_free(app->view_port);
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_NOTIFICATION);

    furi_message_queue_free(app->event_queue);
    furi_mutex_free(app->mutex);
    free(app);
}

int32_t tamagotchi_app(void* p) {
    UNUSED(p);
    TamagotchiApp* app = tamagotchi_alloc();
    notification_message(app->notifications, &sequence_display_backlight_on);

    InputEvent event;
    while(app->running) {
        if(furi_message_queue_get(app->event_queue, &event, 100) != FuriStatusOk) continue;
        if(event.type != InputTypeShort && event.type != InputTypeLong &&
           event.type != InputTypeRepeat)
            continue;

        // Long Back always exits, from any scene.
        if(event.key == InputKeyBack && event.type == InputTypeLong) {
            app->running = false;
            break;
        }

        furi_mutex_acquire(app->mutex, FuriWaitForever);
        handle_input(app, &event);
        furi_mutex_release(app->mutex);
        view_port_update(app->view_port);
    }

    tamagotchi_free(app);
    return 0;
}
