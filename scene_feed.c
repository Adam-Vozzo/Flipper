#include "tamagotchi.h"

static const char* food_blurb(FoodKind k) {
    switch(k) {
    case FoodSnack: return "+Food +Joy, messy";
    case FoodMeal: return "++Food, filling";
    case FoodVeg: return "+Food +Health";
    default: return "";
    }
}

void scene_feed_draw(Canvas* canvas, TamagotchiApp* app) {
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 11, "Feed");
    canvas_draw_line(canvas, 0, 13, 127, 13);

    FoodKind k = (FoodKind)app->food_index;

    // plate
    canvas_draw_disc(canvas, 64, 36, 12);
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_disc(canvas, 64, 36, 9);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 64, 36, AlignCenter, AlignCenter, food_name(k));

    // left/right chooser arrows
    canvas_draw_str_aligned(canvas, 30, 36, AlignCenter, AlignCenter, "<");
    canvas_draw_str_aligned(canvas, 98, 36, AlignCenter, AlignCenter, ">");

    canvas_draw_str_aligned(canvas, 64, 56, AlignCenter, AlignCenter, food_blurb(k));
    canvas_draw_str_aligned(canvas, 64, 64, AlignCenter, AlignBottom, "OK: feed   Back: cancel");
}

void scene_feed_input(TamagotchiApp* app, InputEvent* event) {
    bool nav = event->type == InputTypeShort || event->type == InputTypeRepeat;
    switch(event->key) {
    case InputKeyLeft:
    case InputKeyUp:
        if(nav) app->food_index = (app->food_index + FoodKindCount - 1) % FoodKindCount;
        break;
    case InputKeyRight:
    case InputKeyDown:
        if(nav) app->food_index = (app->food_index + 1) % FoodKindCount;
        break;
    case InputKeyOk:
        if(event->type == InputTypeShort) {
            pet_feed(app, (FoodKind)app->food_index);
            app->scene = SceneMain;
        }
        break;
    case InputKeyBack:
        if(event->type == InputTypeShort) app->scene = SceneMenu;
        break;
    default:
        break;
    }
}
