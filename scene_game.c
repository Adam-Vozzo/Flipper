#include "tamagotchi.h"
#include "kaomoji.h"

#define GAME_ROUNDS 5

static uint8_t clamp_add(uint8_t v, int by) {
    int n = (int)v + by;
    if(n < 0) n = 0;
    if(n > 100) n = 100;
    return (uint8_t)n;
}

void scene_game_draw(Canvas* canvas, TamagotchiApp* app) {
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 11, "Guess!");
    canvas_set_font(canvas, FontSecondary);
    char hdr[20];
    snprintf(
        hdr,
        sizeof(hdr),
        "R%u/%u  Sc%u",
        (unsigned)(app->game.round + 1 > GAME_ROUNDS ? GAME_ROUNDS : app->game.round + 1),
        (unsigned)GAME_ROUNDS,
        (unsigned)app->game.score);
    canvas_draw_str_aligned(canvas, 126, 11, AlignRight, AlignBottom, hdr);
    canvas_draw_line(canvas, 0, 13, 127, 13);

    if(app->game.phase == 0) {
        // guessing: pet in the middle, pick a side
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 18, 36, AlignCenter, AlignCenter, "<");
        canvas_draw_str_aligned(canvas, 110, 36, AlignCenter, AlignCenter, ">");
        canvas_draw_str_aligned(
            canvas, 64, 34, AlignCenter, AlignCenter, kaomoji_for_mood(MoodNeutral, false, app->frame));
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str_aligned(canvas, 64, 60, AlignCenter, AlignBottom, "Which way will it lean?");
    } else if(app->game.phase == 1) {
        // reveal: pet leans to its chosen side
        int px = app->game.choice == 0 ? 40 : 88;
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(
            canvas,
            px,
            34,
            AlignCenter,
            AlignCenter,
            kaomoji_for_mood(app->game.correct ? MoodHappy : MoodSad, false, app->frame));
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str_aligned(
            canvas, 64, 60, AlignCenter, AlignBottom, app->game.correct ? "Yes! (OK)" : "Nope! (OK)");
    } else {
        canvas_set_font(canvas, FontPrimary);
        char res[20];
        snprintf(res, sizeof(res), "Score %u/%u", (unsigned)app->game.score, (unsigned)GAME_ROUNDS);
        canvas_draw_str_aligned(canvas, 64, 32, AlignCenter, AlignCenter, res);
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str_aligned(canvas, 64, 60, AlignCenter, AlignBottom, "OK to collect joy");
    }
}

static void game_finish(TamagotchiApp* app) {
    Pet* p = &app->pet;
    p->happy = clamp_add(p->happy, app->game.score * 5);
    p->energy = clamp_add(p->energy, -8);
    p->care_score++;
    char msg[POPUP_MAX];
    snprintf(msg, sizeof(msg), "Played! %u/%u", (unsigned)app->game.score, (unsigned)GAME_ROUNDS);
    tama_popup(app, msg);
    if(app->game.score >= GAME_ROUNDS - 1) {
        tama_emote(app, EmoteLevelUp);
        tama_spawn_particles(app, 'h', 5);
    } else {
        tama_emote(app, EmotePlay);
    }
    app->scene = SceneMain;
}

void scene_game_input(TamagotchiApp* app, InputEvent* event) {
    if(event->type != InputTypeShort) return;

    if(event->key == InputKeyBack) {
        app->scene = SceneMenu;
        return;
    }

    if(app->game.phase == 0) {
        int guess = -1;
        if(event->key == InputKeyLeft) guess = 0;
        if(event->key == InputKeyRight) guess = 1;
        if(guess < 0) return;
        app->game.correct = (guess == app->game.choice);
        if(app->game.correct) {
            app->game.score++;
            tama_emote(app, EmoteHappy);
        } else {
            tama_emote(app, EmoteSad);
        }
        app->game.phase = 1;
    } else if(app->game.phase == 1) {
        app->game.round++;
        if(app->game.round >= GAME_ROUNDS) {
            app->game.phase = 2;
        } else {
            app->game.choice = (uint8_t)tama_rand_max(app, 2);
            app->game.phase = 0;
        }
    } else {
        game_finish(app);
    }
}
