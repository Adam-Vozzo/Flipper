#include "settings.h"

// Order must match the FeatureId enum.
const FeatureInfo feature_table[FeatureCount] = {
    [FeatureSound] = {"Sound FX", "Chirps & beeps on actions", true},
    [FeatureHaptics] = {"Haptics", "Vibrate on actions", true},
    [FeatureLed] = {"LED Mood", "RGB blinks match mood", true},
    [FeatureBacklightPet] = {"Attention", "Wake screen when neglected", false},

    [FeatureIdleAnim] = {"Idle Anim", "Blink + breathing bob", true},
    [FeatureParticles] = {"Particles", "Floating hearts / zzz", true},
    [FeatureFloor] = {"Room", "Draw a little room", true},

    [FeatureAging] = {"Aging", "Grow through life stages", true},
    [FeaturePoop] = {"Poop", "Makes a mess to clean", true},
    [FeatureSickness] = {"Sickness", "Can get sick, needs meds", true},
    [FeatureFastMetabolism] = {"Fast Meta", "Stats drain fast (test)", false},
    [FeatureWeather] = {"Weather", "Drifting sky & rain", false},
    [FeatureDayNight] = {"Day/Night", "Sun & moon cycle", false},
    [FeatureEvolveByCare] = {"Karma Evo", "Care shapes final form", false},
    [FeatureMinigame] = {"Mini-game", "Guessing game in menu", true},

    [FeatureBigFace] = {"Big Face", "Large centered kaomoji", true},
    [FeatureHomeBars] = {"Home Bars", "Mini stat bars on home", true},
    [FeatureClock] = {"Clock", "Show in-game time", false},

    [FeatureAutosave] = {"Autosave", "Keep pet between runs", true},
};

void settings_defaults(Settings* s) {
    for(size_t i = 0; i < FeatureCount; i++) {
        s->on[i] = feature_table[i].def;
    }
}
