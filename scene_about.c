#include "tamagotchi.h"

void scene_about_draw(Canvas* canvas, TamagotchiApp* app) {
    UNUSED(app);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 10, AlignCenter, AlignBottom, "Pocket Critter");

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 4, 24, "Up:Feed   Down:Sleep");
    canvas_draw_str(canvas, 4, 34, "Left:Cuddle  Right:Play");
    canvas_draw_str(canvas, 4, 44, "OK:Menu   Lab=toggles");
    canvas_draw_str(canvas, 4, 54, "Hold Back: quit");

    canvas_draw_str_aligned(canvas, 64, 64, AlignCenter, AlignBottom, "\\(^o^)/  v0.1");
}

void scene_about_input(TamagotchiApp* app, InputEvent* event) {
    if(event->type != InputTypeShort) return;
    switch(event->key) {
    case InputKeyBack:
        app->scene = SceneMenu;
        break;
    case InputKeyOk:
        app->scene = SceneMain;
        break;
    default:
        break;
    }
}
