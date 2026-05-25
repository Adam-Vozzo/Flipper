#include "tamagotchi.h"

void scene_stats_draw(Canvas* canvas, TamagotchiApp* app) {
    Pet* p = &app->pet;

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 11, "Stats");
    canvas_draw_str_aligned(canvas, 126, 11, AlignRight, AlignBottom, pet_stage_name(p->stage));
    canvas_draw_line(canvas, 0, 13, 127, 13);

    widget_stat_bar(canvas, 2, 15, 124, "Eat", p->food);
    widget_stat_bar(canvas, 2, 24, 124, "Joy", p->happy);
    widget_stat_bar(canvas, 2, 33, 124, "Zzz", p->energy);
    widget_stat_bar(canvas, 2, 42, 124, "Wsh", p->hygiene);
    widget_stat_bar(canvas, 2, 51, 124, "HP", p->health);

    canvas_set_font(canvas, FontSecondary);
    unsigned long days = (unsigned long)(p->age_ticks / 240u);
    char left[24];
    snprintf(left, sizeof(left), "Age %lud  Wt %u", days, p->weight);
    canvas_draw_str(canvas, 2, 63, left);

    char right[16];
    if(p->sick) {
        snprintf(right, sizeof(right), "SICK!");
    } else if(settings_on(&app->settings, FeaturePersonality)) {
        snprintf(right, sizeof(right), "%s", trait_name(p->personality));
    } else {
        snprintf(right, sizeof(right), "Care %ld", (long)p->care_score);
    }
    canvas_draw_str_aligned(canvas, 126, 63, AlignRight, AlignBottom, right);
}

void scene_stats_input(TamagotchiApp* app, InputEvent* event) {
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
