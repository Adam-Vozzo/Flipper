#include "settings.h"

// Order must match the FeatureId enum.
const FeatureInfo feature_table[FeatureCount] = {
    [FeatureSound] = {"Sound FX", "Chirps & beeps on actions", true},
    [FeatureHaptics] = {"Haptics", "Vibrate on actions", true},
    [FeatureLed] = {"LED Mood", "RGB blinks match mood", true},
    [FeatureAmbientLed] = {"Ambient LED", "Slow mood glow when idle", false},
    [FeatureBacklightPet] = {"Attention", "Wake screen when neglected", false},

    [FeatureIdleAnim] = {"Idle Anim", "Blink + breathing bob", true},
    [FeatureParticles] = {"Particles", "Floating hearts / zzz", true},
    [FeatureFloor] = {"Room", "Draw a little room", true},
    [FeatureThoughts] = {"Thoughts", "Bubble shows its wish", true},

    [FeatureAging] = {"Aging", "Grow through life stages", true},
    [FeaturePoop] = {"Poop", "Makes a mess to clean", true},
    [FeatureSickness] = {"Sickness", "Can get sick, needs meds", true},
    [FeatureFastMetabolism] = {"Fast Meta", "Stats drain fast (test)", false},
    [FeatureWeather] = {"Weather", "Drifting sky & rain", false},
    [FeatureDayNight] = {"Day/Night", "Sun & moon cycle", false},
    [FeatureEvolveByCare] = {"Karma Evo", "Care shapes final form", false},
    [FeatureMinigame] = {"Mini-game", "Guessing game in menu", true},
    [FeaturePersonality] = {"Traits", "Random temperaments", false},

    [FeatureBigFace] = {"Big Face", "Large centered kaomoji", true},
    [FeatureCatMode] = {"Cat Mode", "Cat-style kaomoji", false},
    [FeatureHomeBars] = {"Home Bars", "Mini stat bars on home", true},
    [FeatureClock] = {"Clock", "Show in-game time", false},
    [FeatureMoodText] = {"Mood Text", "Name the mood on home", false},

    [FeatureAutosave] = {"Autosave", "Keep pet between runs", true},
};

void settings_defaults(Settings* s) {
    for(size_t i = 0; i < FeatureCount; i++) {
        s->on[i] = feature_table[i].def;
    }
}
