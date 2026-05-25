#include "tamagotchi.h"

typedef enum {
    MenuFeed,
    MenuPlay,
    MenuSleep,
    MenuClean,
    MenuHeal,
    MenuCuddle,
    MenuStats,
    MenuLab,
    MenuNewEgg,
    MenuAbout,
    MenuCount,
} MenuItem;

static const char* menu_label(const TamagotchiApp* app, MenuItem item) {
    switch(item) {
    case MenuFeed: return "Feed";
    case MenuPlay: return "Play";
    case MenuSleep: return app->pet.asleep ? "Wake up" : "Sleep";
    case MenuClean: return "Clean";
    case MenuHeal: return "Medicine";
    case MenuCuddle: return "Cuddle";
    case MenuStats: return "Stats";
    case MenuLab: return "Lab (toggles)";
    case MenuNewEgg: return "New Egg";
    case MenuAbout: return "About";
    default: return "?";
    }
}

#define MENU_VISIBLE 5

void scene_menu_draw(Canvas* canvas, TamagotchiApp* app) {
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 11, "Menu");
    canvas_draw_line(canvas, 0, 13, 127, 13);

    uint8_t scroll = app->menu_index >= MENU_VISIBLE ? app->menu_index - MENU_VISIBLE + 1 : 0;
    canvas_set_font(canvas, FontSecondary);
    for(uint8_t row = 0; row < MENU_VISIBLE; row++) {
        uint8_t idx = scroll + row;
        if(idx >= MenuCount) break;
        int y = 15 + row * 10;
        if(idx == app->menu_index) {
            canvas_draw_box(canvas, 0, y, 128, 10);
            canvas_set_color(canvas, ColorWhite);
        }
        canvas_draw_str(canvas, 4, y + 8, menu_label(app, idx));
        canvas_set_color(canvas, ColorBlack);
    }

    // scroll hint
    if(MenuCount > MENU_VISIBLE) {
        canvas_draw_str_aligned(canvas, 126, 11, AlignRight, AlignBottom, "v");
    }
}

static void menu_select(TamagotchiApp* app) {
    switch((MenuItem)app->menu_index) {
    case MenuFeed:
        app->scene = SceneFeed;
        app->food_index = 0;
        break;
    case MenuPlay:
        pet_play(app);
        app->scene = SceneMain;
        break;
    case MenuSleep:
        pet_toggle_sleep(app);
        app->scene = SceneMain;
        break;
    case MenuClean:
        pet_clean(app);
        app->scene = SceneMain;
        break;
    case MenuHeal:
        pet_heal(app);
        app->scene = SceneMain;
        break;
    case MenuCuddle:
        pet_cuddle(app);
        app->scene = SceneMain;
        break;
    case MenuStats:
        app->scene = SceneStats;
        break;
    case MenuLab:
        app->scene = SceneLab;
        app->lab_index = 0;
        app->lab_scroll = 0;
        break;
    case MenuNewEgg:
        pet_reset_egg(app);
        app->scene = SceneMain;
        break;
    case MenuAbout:
        app->scene = SceneAbout;
        break;
    default:
        break;
    }
}

void scene_menu_input(TamagotchiApp* app, InputEvent* event) {
    bool nav = event->type == InputTypeShort || event->type == InputTypeRepeat;
    switch(event->key) {
    case InputKeyUp:
        if(nav) app->menu_index = (app->menu_index + MenuCount - 1) % MenuCount;
        break;
    case InputKeyDown:
        if(nav) app->menu_index = (app->menu_index + 1) % MenuCount;
        break;
    case InputKeyOk:
        if(event->type == InputTypeShort) menu_select(app);
        break;
    case InputKeyBack:
        if(event->type == InputTypeShort) app->scene = SceneMain;
        break;
    default:
        break;
    }
}
