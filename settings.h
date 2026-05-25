#pragma once

#include <furi.h>

// The Lab. Every experimental direction lives here as a flag so we can A/B
// ideas live on the device instead of committing to one. Add a new idea by:
//   1. adding an enum entry before FeatureCount
//   2. adding a matching row to feature_table[] in settings.c
//   3. checking settings_on(&app->settings, FeatureX) wherever it matters
typedef enum {
    // feel / juice
    FeatureSound,
    FeatureHaptics,
    FeatureLed,
    FeatureAmbientLed, // slow mood-coloured LED pulse while idle
    FeatureBacklightPet, // nudge the backlight when the pet wants attention
    // animation
    FeatureIdleAnim, // blinking + breathing bob
    FeatureParticles, // floating hearts / zzz / crumbs
    FeatureFloor, // draw a little room + floor instead of empty void
    FeatureThoughts, // thought bubble showing what the pet wants
    // simulation
    FeatureAging, // grow through life stages
    FeaturePoop, // produces poop that must be cleaned
    FeatureSickness, // can get sick and need medicine
    FeatureFastMetabolism, // stats decay much faster (good for testing)
    FeatureWeather, // ambient weather that drifts over time
    FeatureDayNight, // day/night cycle with sun/moon
    FeatureEvolveByCare, // final form depends on how well you cared
    FeatureMinigame, // unlock the guessing mini-game
    FeaturePersonality, // each pet gets a random temperament
    // ui
    FeatureBigFace, // render the kaomoji large & centered
    FeatureCatMode, // swap to a cat-flavoured kaomoji set
    FeatureHomeBars, // show the mini stat bars on the home screen
    FeatureClock, // show the in-game clock on home
    FeatureMoodText, // print the current mood as a word under the pet
    // system
    FeatureAutosave, // persist the pet + toggles between runs

    FeatureCount,
} FeatureId;

typedef struct {
    const char* name; // short label for the Lab list
    const char* blurb; // one-line description shown under the cursor
    bool def; // default on/off
} FeatureInfo;

extern const FeatureInfo feature_table[FeatureCount];

typedef struct {
    bool on[FeatureCount];
} Settings;

void settings_defaults(Settings* s);

static inline bool settings_on(const Settings* s, FeatureId id) {
    return s->on[id];
}

static inline void settings_toggle(Settings* s, FeatureId id) {
    s->on[id] = !s->on[id];
}
