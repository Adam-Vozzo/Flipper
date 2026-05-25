#include "tamagotchi.h"

#define LAB_VISIBLE 4

void scene_lab_draw(Canvas* canvas, TamagotchiApp* app) {
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 11, "Lab");

    uint8_t on_count = 0;
    for(uint8_t i = 0; i < FeatureCount; i++)
        if(app->settings.on[i]) on_count++;
    char hdr[16];
    snprintf(hdr, sizeof(hdr), "%u/%u on", on_count, (unsigned)FeatureCount);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 126, 11, AlignRight, AlignBottom, hdr);
    canvas_draw_line(canvas, 0, 13, 127, 13);

    uint8_t scroll = app->lab_index >= LAB_VISIBLE ? app->lab_index - LAB_VISIBLE + 1 : 0;
    for(uint8_t row = 0; row < LAB_VISIBLE; row++) {
        uint8_t idx = scroll + row;
        if(idx >= FeatureCount) break;
        int y = 15 + row * 9;
        bool selected = idx == app->lab_index;
        if(selected) {
            canvas_draw_box(canvas, 0, y, 128, 9);
            canvas_set_color(canvas, ColorWhite);
        }
        canvas_draw_str(canvas, 4, y + 8, feature_table[idx].name);
        canvas_draw_str_aligned(
            canvas, 124, y + 8, AlignRight, AlignBottom, app->settings.on[idx] ? "ON" : "off");
        canvas_set_color(canvas, ColorBlack);
    }

    // blurb of the highlighted feature
    canvas_draw_line(canvas, 0, 53, 127, 53);
    canvas_draw_str_aligned(
        canvas, 64, 62, AlignCenter, AlignBottom, feature_table[app->lab_index].blurb);
}

void scene_lab_input(TamagotchiApp* app, InputEvent* event) {
    bool nav = event->type == InputTypeShort || event->type == InputTypeRepeat;
    switch(event->key) {
    case InputKeyUp:
        if(nav) app->lab_index = (app->lab_index + FeatureCount - 1) % FeatureCount;
        break;
    case InputKeyDown:
        if(nav) app->lab_index = (app->lab_index + 1) % FeatureCount;
        break;
    case InputKeyLeft:
    case InputKeyRight:
    case InputKeyOk:
        if(event->type == InputTypeShort)
            settings_toggle(&app->settings, (FeatureId)app->lab_index);
        break;
    case InputKeyBack:
        if(event->type == InputTypeShort) app->scene = SceneMenu;
        break;
    default:
        break;
    }
}
