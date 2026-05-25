#pragma once

#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>

#include "settings.h"
#include "pet.h"

#define SCREEN_W 128
#define SCREEN_H 64

// Render/animation runs at this cadence (4 fps) for smooth idle motion; the
// simulation steps once per second (every 4th frame). Fast Metabolism then
// multiplies stat decay on top of the 1 Hz sim.
#define TAMA_TICK_MS 250
#define FRAMES_PER_SEC 4

typedef enum {
    SceneMain,
    SceneMenu,
    SceneFeed, // food picker
    SceneStats,
    SceneLab, // the feature-toggle playground
    SceneAbout,
    SceneCount,
} Scene;

#define POPUP_MAX 28
#define PARTICLE_COUNT 6

typedef struct {
    int8_t x;
    int8_t y;
    int8_t vy;
    uint8_t ttl;
    char glyph;
} Particle;

typedef struct TamagotchiApp {
    Gui* gui;
    ViewPort* view_port;
    FuriMessageQueue* event_queue;
    FuriMutex* mutex;
    FuriTimer* timer;
    NotificationApp* notifications;

    Scene scene;
    Pet pet;
    Settings settings;

    // ui cursors
    uint8_t menu_index;
    uint8_t food_index;
    uint8_t lab_index;
    uint8_t lab_scroll;

    // transient feedback
    char popup[POPUP_MAX];
    uint8_t popup_ttl;

    // cosmetic animation state
    uint32_t frame; // 4 fps, drives idle animation + face cycling
    uint32_t sec; // 1 Hz simulation clock
    uint8_t weather; // 0 sunny, 1 cloudy, 2 rain
    bool blink; // eyes closed this frame
    Particle particles[PARTICLE_COUNT];

    uint32_t rng;
    bool running;
} TamagotchiApp;

// --- small xorshift PRNG so we don't depend on libc rand quirks ---
static inline uint32_t tama_rand(TamagotchiApp* app) {
    uint32_t x = app->rng;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    app->rng = x;
    return x;
}

static inline uint32_t tama_rand_max(TamagotchiApp* app, uint32_t n) {
    return n ? (tama_rand(app) % n) : 0;
}

// Semantic feedback events. tama_emote() decides which of sound / LED / vibro
// to fire based on the individual Lab toggles, so each channel is independent.
typedef enum {
    EmoteHappy,
    EmoteEat,
    EmoteSleep,
    EmotePlay,
    EmoteClean,
    EmoteHeal,
    EmoteSad,
    EmoteSick,
    EmoteLevelUp,
    EmoteDeath,
    EmoteHatch,
} Emote;

// --- shared helpers ---
void tama_popup(TamagotchiApp* app, const char* msg);
void tama_spawn_particles(TamagotchiApp* app, char glyph, uint8_t count);
void tama_emote(TamagotchiApp* app, Emote emote);

// --- scene draw + input handlers (one pair per scene file) ---
void scene_main_draw(Canvas* canvas, TamagotchiApp* app);
void scene_main_input(TamagotchiApp* app, InputEvent* event);

void scene_menu_draw(Canvas* canvas, TamagotchiApp* app);
void scene_menu_input(TamagotchiApp* app, InputEvent* event);

void scene_feed_draw(Canvas* canvas, TamagotchiApp* app);
void scene_feed_input(TamagotchiApp* app, InputEvent* event);

void scene_stats_draw(Canvas* canvas, TamagotchiApp* app);
void scene_stats_input(TamagotchiApp* app, InputEvent* event);

void scene_lab_draw(Canvas* canvas, TamagotchiApp* app);
void scene_lab_input(TamagotchiApp* app, InputEvent* event);

void scene_about_draw(Canvas* canvas, TamagotchiApp* app);
void scene_about_input(TamagotchiApp* app, InputEvent* event);

// shared widgets
void widget_stat_bar(Canvas* canvas, int x, int y, int w, const char* label, uint8_t value);
