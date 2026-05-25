#include "tamagotchi.h"
#include "kaomoji.h"

#define DAY_SECONDS 240 // one in-game day == 4 real minutes

static const int8_t bob_table[4] = {0, 1, 2, 1};

static bool is_night(const TamagotchiApp* app) {
    uint32_t t = app->sec % DAY_SECONDS;
    return (t < DAY_SECONDS / 4) || (t >= (DAY_SECONDS * 3) / 4);
}

static void draw_sky(Canvas* canvas, TamagotchiApp* app) {
    if(settings_on(&app->settings, FeatureDayNight)) {
        if(is_night(app)) {
            // moon (crescent: a disc with a bite cut by an overlapping white disc)
            canvas_draw_disc(canvas, 110, 15, 5);
            canvas_set_color(canvas, ColorWhite);
            canvas_draw_disc(canvas, 113, 13, 5);
            canvas_set_color(canvas, ColorBlack);
            // a few twinkling stars
            static const uint8_t sx[] = {8, 30, 50, 72, 95};
            static const uint8_t sy[] = {14, 10, 16, 12, 18};
            for(uint8_t i = 0; i < 5; i++) {
                if(((app->frame / 4) + i) % 5 != 0) canvas_draw_dot(canvas, sx[i], sy[i]);
            }
        } else {
            // sun with little rays
            canvas_draw_disc(canvas, 110, 15, 4);
            for(int a = 0; a < 8; a++) {
                int dx = (a % 3) - 1;
                int dy = (a / 3) - 1;
                if(dx || dy) canvas_draw_dot(canvas, 110 + dx * 7, 15 + dy * 7);
            }
        }
    }

    if(settings_on(&app->settings, FeatureWeather) && app->weather >= 1) {
        // cloud
        canvas_draw_disc(canvas, 24, 14, 4);
        canvas_draw_disc(canvas, 31, 13, 5);
        canvas_draw_disc(canvas, 38, 15, 4);
        if(app->weather == 2) {
            // animated rain
            for(int i = 0; i < 4; i++) {
                int x = 22 + i * 6;
                int y = 20 + ((app->frame * 2 + i * 5) % 18);
                canvas_draw_line(canvas, x, y, x, y + 2);
            }
        }
    }
}

static void draw_room(Canvas* canvas) {
    canvas_draw_line(canvas, 0, 52, 127, 52);
    // a little rug under the pet
    canvas_draw_line(canvas, 40, 52, 88, 52);
    canvas_draw_line(canvas, 44, 54, 84, 54);
}

static void draw_poop(Canvas* canvas, TamagotchiApp* app) {
    if(!settings_on(&app->settings, FeaturePoop)) return;
    for(uint8_t i = 0; i < app->pet.poop && i < 4; i++) {
        int x = 14 + i * 14;
        int y = 50;
        canvas_draw_disc(canvas, x, y, 2);
        canvas_draw_disc(canvas, x + 2, y - 1, 2);
        canvas_draw_dot(canvas, x + 1, y - 4);
    }
}

static void draw_particles(Canvas* canvas, TamagotchiApp* app) {
    canvas_set_font(canvas, FontSecondary);
    for(uint8_t i = 0; i < PARTICLE_COUNT; i++) {
        Particle* p = &app->particles[i];
        if(p->ttl == 0) continue;
        if(p->glyph == 'h') {
            // tiny heart
            canvas_draw_disc(canvas, p->x, p->y, 1);
            canvas_draw_disc(canvas, p->x + 2, p->y, 1);
            canvas_draw_dot(canvas, p->x + 1, p->y + 2);
        } else {
            char s[2] = {p->glyph, 0};
            canvas_draw_str(canvas, p->x, p->y, s);
        }
    }
}

static void draw_egg(Canvas* canvas, TamagotchiApp* app) {
    int dx = (app->frame % 8 < 4) ? -1 : 1; // wobble
    int cx = 64 + dx;
    canvas_draw_rbox(canvas, cx - 11, 22, 22, 26, 9);
    canvas_set_color(canvas, ColorWhite);
    // zig-zag crack
    canvas_draw_line(canvas, cx - 6, 32, cx - 2, 36);
    canvas_draw_line(canvas, cx - 2, 36, cx + 2, 31);
    canvas_draw_line(canvas, cx + 2, 31, cx + 6, 36);
    // a couple of spots
    canvas_draw_dot(canvas, cx - 4, 42);
    canvas_draw_dot(canvas, cx + 5, 40);
    canvas_set_color(canvas, ColorBlack);
}

static void draw_grave(Canvas* canvas, TamagotchiApp* app) {
    // headstone
    canvas_draw_rbox(canvas, 52, 38, 24, 16, 6);
    canvas_set_color(canvas, ColorWhite);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 64, 47, AlignCenter, AlignCenter, "RIP");
    canvas_set_color(canvas, ColorBlack);
    // a ghost drifting up
    int bob = bob_table[(app->frame / 3) % 4];
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(
        canvas, 64, 22 - bob, AlignCenter, AlignCenter, kaomoji_for_mood(MoodGhost, false, app->frame));
}

static void draw_pet(Canvas* canvas, TamagotchiApp* app) {
    Pet* p = &app->pet;
    if(!p->hatched) {
        draw_egg(canvas, app);
        return;
    }
    if(!p->alive) {
        draw_grave(canvas, app);
        return;
    }

    int dy = 0, dx = 0;
    if(settings_on(&app->settings, FeatureIdleAnim) && !p->asleep) {
        dy = bob_table[(app->frame / 3) % 4];
    }
    if(p->mood == MoodPlaying) {
        dx = (app->frame % 4 < 2) ? -3 : 3; // wiggle
    }

    // ground shadow
    if(settings_on(&app->settings, FeatureFloor)) {
        canvas_draw_line(canvas, 56, 53, 72, 53);
    }

    const char* face = kaomoji_face(p, app->blink, app->frame);
    canvas_set_font(
        canvas, settings_on(&app->settings, FeatureBigFace) ? FontPrimary : FontSecondary);
    canvas_draw_str_aligned(canvas, 64 + dx, 32 + dy, AlignCenter, AlignCenter, face);
}

static void draw_topbar(Canvas* canvas, TamagotchiApp* app) {
    Pet* p = &app->pet;
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 8, p->name);
    canvas_draw_str_aligned(canvas, 126, 8, AlignRight, AlignBottom, pet_stage_name(p->stage));

    if(settings_on(&app->settings, FeatureClock)) {
        uint32_t mins = (app->sec * 1440u / DAY_SECONDS);
        uint32_t hh = (mins / 60u) % 24u;
        uint32_t mm = mins % 60u;
        char clk[8];
        snprintf(clk, sizeof(clk), "%02lu:%02lu", (unsigned long)hh, (unsigned long)mm);
        canvas_draw_str_aligned(canvas, 64, 8, AlignCenter, AlignBottom, clk);
    }
}

static void draw_bottom(Canvas* canvas, TamagotchiApp* app) {
    Pet* p = &app->pet;
    canvas_set_font(canvas, FontSecondary);

    if(!p->hatched) {
        canvas_draw_str_aligned(canvas, 64, 60, AlignCenter, AlignBottom, "Press OK to hatch!");
        return;
    }
    if(!p->alive) {
        canvas_draw_str_aligned(canvas, 64, 60, AlignCenter, AlignBottom, "OK: new egg");
        return;
    }

    if(settings_on(&app->settings, FeatureHomeBars)) {
        widget_stat_bar(canvas, 1, 55, 42, "Eat", p->food);
        widget_stat_bar(canvas, 44, 55, 42, "Joy", p->happy);
        widget_stat_bar(canvas, 87, 55, 41, "Zzz", p->energy);
    } else {
        canvas_draw_str_aligned(canvas, 64, 62, AlignCenter, AlignBottom, "OK: menu");
    }
}

static void draw_popup(Canvas* canvas, TamagotchiApp* app) {
    if(app->popup_ttl == 0) return;
    canvas_set_font(canvas, FontSecondary);
    int tw = canvas_string_width(canvas, app->popup);
    int w = tw + 8;
    int x = 64 - w / 2;
    int y = 40;
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_box(canvas, x, y, w, 12);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_rframe(canvas, x, y, w, 12, 3);
    canvas_draw_str_aligned(canvas, 64, y + 6, AlignCenter, AlignCenter, app->popup);
}

void scene_main_draw(Canvas* canvas, TamagotchiApp* app) {
    draw_sky(canvas, app);
    if(settings_on(&app->settings, FeatureFloor)) draw_room(canvas);
    draw_poop(canvas, app);
    draw_pet(canvas, app);
    draw_particles(canvas, app);
    draw_topbar(canvas, app);
    draw_bottom(canvas, app);
    draw_popup(canvas, app);
}

void scene_main_input(TamagotchiApp* app, InputEvent* event) {
    Pet* p = &app->pet;

    if(event->type == InputTypeShort) {
        switch(event->key) {
        case InputKeyOk:
            if(!p->hatched) {
                pet_hatch(app);
            } else if(!p->alive) {
                pet_reset_egg(app);
            } else {
                app->scene = SceneMenu;
                app->menu_index = 0;
            }
            break;
        case InputKeyUp:
            pet_feed(app, FoodMeal);
            break;
        case InputKeyDown:
            pet_toggle_sleep(app);
            break;
        case InputKeyLeft:
            pet_cuddle(app);
            break;
        case InputKeyRight:
            pet_play(app);
            break;
        case InputKeyBack:
            // short Back is a no-op on home; long Back exits (handled globally)
            break;
        default:
            break;
        }
    }
}
