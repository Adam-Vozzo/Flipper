#include "tamagotchi.h"
#include "pet.h"

// base decay intervals in ticks (== seconds). Fast Metabolism divides these.
#define IV_FOOD 6
#define IV_HAPPY 9
#define IV_ENERGY 8
#define IV_HYGIENE 12
#define IV_HEALTH 4

#define CARE_GOOD 40
#define CARE_BAD (-25)

// life-stage age gates in ticks (seconds since hatch)
#define AGE_CHILD 90
#define AGE_TEEN 240
#define AGE_ADULT 480
#define AGE_ELDER 900

static uint8_t u8_dec(uint8_t v, int by) {
    int n = (int)v - by;
    return n < 0 ? 0 : (uint8_t)n;
}

static uint8_t u8_inc(uint8_t v, int by) {
    int n = (int)v + by;
    return n > 100 ? 100 : (uint8_t)n;
}

static int scale_iv(const TamagotchiApp* app, int iv) {
    if(settings_on(&app->settings, FeatureFastMetabolism)) {
        iv /= 4;
        if(iv < 1) iv = 1;
    }
    return iv;
}

static bool due(const TamagotchiApp* app, int iv) {
    iv = scale_iv(app, iv);
    return (app->sec % (uint32_t)iv) == 0;
}

const char* pet_stage_name(LifeStage stage) {
    switch(stage) {
    case StageEgg: return "Egg";
    case StageBaby: return "Baby";
    case StageChild: return "Child";
    case StageTeen: return "Teen";
    case StageAdult: return "Adult";
    case StageElder: return "Elder";
    case StageAngel: return "Angel";
    case StageGoblin: return "Goblin";
    default: return "?";
    }
}

const char* food_name(FoodKind kind) {
    switch(kind) {
    case FoodSnack: return "Snack";
    case FoodMeal: return "Meal";
    case FoodVeg: return "Veggies";
    default: return "Food";
    }
}

uint8_t pet_wellbeing(const Pet* p) {
    return (uint8_t)(((int)p->food + p->happy + p->energy + p->hygiene) / 4);
}

Mood pet_compute_mood(const Pet* p) {
    if(!p->hatched) return MoodEgg;
    if(!p->alive) return MoodDead;
    if(p->asleep) return MoodAsleep;
    if(p->play_anim > 0) return MoodPlaying;
    if(p->sick) return MoodSick;
    if(p->hygiene < 25 || p->poop >= 3) return MoodDirty;
    if(p->food < 25) return MoodHungry;
    if(p->energy < 20) return MoodSleepy;
    if(p->happy < 30) return MoodSad;
    if(p->happy > 72 && p->food > 55 && p->health > 55) return MoodHappy;
    return MoodNeutral;
}

void pet_init(Pet* p) {
    memset(p, 0, sizeof(Pet));
    strncpy(p->name, "Critter", sizeof(p->name) - 1);
    p->food = 70;
    p->happy = 70;
    p->energy = 80;
    p->hygiene = 85;
    p->health = 100;
    p->stage = StageEgg;
    p->weight = 5;
    p->hatched = false;
    p->alive = true;
    p->mood = MoodEgg;
}

void pet_reset_egg(TamagotchiApp* app) {
    pet_init(&app->pet);
    tama_popup(app, "A fresh egg!");
    tama_spawn_particles(app, '.', 4);
}

void pet_hatch(TamagotchiApp* app) {
    Pet* p = &app->pet;
    if(p->hatched) return;
    p->hatched = true;
    p->alive = true;
    p->stage = StageBaby;
    p->age_ticks = 0;
    tama_emote(app, EmoteHatch);
    tama_popup(app, "It hatched! <3");
    tama_spawn_particles(app, 'h', 5);
}

// ---- player actions ----

static bool action_blocked(TamagotchiApp* app) {
    Pet* p = &app->pet;
    if(!p->hatched) {
        tama_popup(app, "Tap to hatch!");
        return true;
    }
    if(!p->alive) {
        tama_popup(app, "It's resting...");
        return true;
    }
    return false;
}

void pet_feed(TamagotchiApp* app, FoodKind kind) {
    if(action_blocked(app)) return;
    Pet* p = &app->pet;
    if(p->asleep) {
        tama_popup(app, "Zzz... shh");
        return;
    }
    bool was_hungry = p->food < 50;
    bool overfull = p->food > 90;

    switch(kind) {
    case FoodSnack:
        p->food = u8_inc(p->food, 15);
        p->happy = u8_inc(p->happy, 5);
        p->hygiene = u8_dec(p->hygiene, 3);
        p->weight += 3;
        break;
    case FoodMeal:
        p->food = u8_inc(p->food, 35);
        p->happy = u8_inc(p->happy, 2);
        p->weight += 5;
        break;
    case FoodVeg:
        p->food = u8_inc(p->food, 25);
        p->health = u8_inc(p->health, 4);
        p->happy = u8_dec(p->happy, 2);
        p->weight += 1;
        break;
    default: break;
    }

    if(was_hungry) p->care_score++;
    tama_emote(app, EmoteEat);
    tama_spawn_particles(app, '~', 3);

    if(overfull) {
        tama_popup(app, "Urp! Too full");
        p->happy = u8_dec(p->happy, 4);
    } else {
        char msg[POPUP_MAX];
        snprintf(msg, sizeof(msg), "%s! yum", food_name(kind));
        tama_popup(app, msg);
    }
}

void pet_play(TamagotchiApp* app) {
    if(action_blocked(app)) return;
    Pet* p = &app->pet;
    if(p->asleep) {
        tama_popup(app, "Zzz... shh");
        return;
    }
    if(p->energy < 15) {
        tama_popup(app, "Too sleepy...");
        tama_emote(app, EmoteSad);
        return;
    }
    p->happy = u8_inc(p->happy, 18);
    p->energy = u8_dec(p->energy, 12);
    p->food = u8_dec(p->food, 4);
    if(p->weight > 5) p->weight -= 2;
    p->play_anim = 8;
    p->care_score++;
    tama_emote(app, EmotePlay);
    tama_spawn_particles(app, 'h', 4);
    tama_popup(app, "Wheee!");
}

void pet_toggle_sleep(TamagotchiApp* app) {
    if(action_blocked(app)) return;
    Pet* p = &app->pet;
    p->asleep = !p->asleep;
    if(p->asleep) {
        tama_emote(app, EmoteSleep);
        tama_spawn_particles(app, 'z', 3);
        tama_popup(app, "Goodnight...");
    } else {
        tama_popup(app, "*yawn* hi!");
    }
}

void pet_clean(TamagotchiApp* app) {
    if(action_blocked(app)) return;
    Pet* p = &app->pet;
    p->poop = 0;
    p->hygiene = u8_inc(p->hygiene, 40);
    p->happy = u8_inc(p->happy, 3);
    p->care_score++;
    tama_emote(app, EmoteClean);
    tama_spawn_particles(app, '~', 5);
    tama_popup(app, "Squeaky clean!");
}

void pet_heal(TamagotchiApp* app) {
    if(action_blocked(app)) return;
    Pet* p = &app->pet;
    if(!p->sick) {
        tama_popup(app, "Not sick :)");
        return;
    }
    p->sick = false;
    p->sick_timer = 0;
    p->health = u8_inc(p->health, 25);
    p->care_score++;
    tama_emote(app, EmoteHeal);
    tama_popup(app, "All better!");
}

void pet_cuddle(TamagotchiApp* app) {
    if(action_blocked(app)) return;
    Pet* p = &app->pet;
    if(p->asleep) {
        tama_popup(app, "Zzz...");
        return;
    }
    p->happy = u8_inc(p->happy, 8);
    tama_emote(app, EmoteHappy);
    tama_spawn_particles(app, 'h', 3);
    tama_popup(app, "<3 <3 <3");
}

// ---- per-tick simulation ----

static void advance_stage(TamagotchiApp* app) {
    Pet* p = &app->pet;
    if(!settings_on(&app->settings, FeatureAging)) return;
    if(p->stage >= StageAngel) return; // terminal evo forms

    LifeStage want = p->stage;
    uint32_t a = p->age_ticks;
    if(a < AGE_CHILD)
        want = StageBaby;
    else if(a < AGE_TEEN)
        want = StageChild;
    else if(a < AGE_ADULT)
        want = StageTeen;
    else if(a < AGE_ELDER)
        want = StageAdult;
    else
        want = StageElder;

    // Karma branch on reaching adulthood.
    if(want == StageAdult && settings_on(&app->settings, FeatureEvolveByCare)) {
        if(p->care_score >= CARE_GOOD)
            want = StageAngel;
        else if(p->care_score <= CARE_BAD)
            want = StageGoblin;
    }

    if(want != p->stage) {
        p->stage = want;
        tama_emote(app, EmoteLevelUp);
        tama_spawn_particles(app, '*', 6);
        char msg[POPUP_MAX];
        snprintf(msg, sizeof(msg), "Evolved: %s!", pet_stage_name(want));
        tama_popup(app, msg);
    }
}

void pet_tick(TamagotchiApp* app) {
    Pet* p = &app->pet;
    // play_anim counts down at frame rate in the timer handler, not here.

    if(!p->hatched || !p->alive) {
        p->mood = pet_compute_mood(p);
        return;
    }

    if(p->asleep) {
        if(due(app, 3)) p->energy = u8_inc(p->energy, 2);
        if(due(app, IV_FOOD * 2)) p->food = u8_dec(p->food, 1);
        if(due(app, IV_HYGIENE * 2)) p->hygiene = u8_dec(p->hygiene, 1);
        if(p->energy >= 100) {
            p->asleep = false;
            tama_popup(app, "*yawn* morning!");
        }
    } else {
        if(due(app, IV_FOOD)) p->food = u8_dec(p->food, 1);
        if(due(app, IV_HAPPY)) {
            int extra = (p->food < 25 ? 1 : 0) + (p->hygiene < 25 ? 1 : 0);
            p->happy = u8_dec(p->happy, 1 + extra);
        }
        if(due(app, IV_ENERGY)) p->energy = u8_dec(p->energy, 1);
        if(due(app, IV_HYGIENE)) {
            int extra = (settings_on(&app->settings, FeaturePoop) && p->poop >= 2) ? 1 : 0;
            p->hygiene = u8_dec(p->hygiene, 1 + extra);
        }
    }

    // poop
    if(settings_on(&app->settings, FeaturePoop) && !p->asleep) {
        p->poop_timer++;
        uint16_t target = scale_iv(app, 45);
        if(p->poop_timer >= target) {
            p->poop_timer = 0;
            if(p->poop < 4 && tama_rand_max(app, 3) == 0) {
                p->poop++;
                tama_popup(app, "...plop");
            }
        }
    }

    // sickness
    if(settings_on(&app->settings, FeatureSickness) && !p->sick && due(app, 5)) {
        bool risk = p->hygiene < 20 || p->food < 15 || p->poop >= 3 || p->neglect > 40;
        if(risk && tama_rand_max(app, 6) == 0) {
            p->sick = true;
            p->sick_timer = 0;
            tama_emote(app, EmoteSick);
            tama_popup(app, "Feels icky...");
        }
    }

    // health
    if(due(app, IV_HEALTH)) {
        bool bad = p->food == 0 || p->energy == 0 || p->hygiene == 0 || p->sick ||
                   (settings_on(&app->settings, FeaturePoop) && p->poop >= 4);
        if(bad) {
            p->health = u8_dec(p->health, 1 + (p->sick ? 1 : 0));
        } else if(p->food > 60 && p->happy > 55 && p->energy > 40 && p->hygiene > 55) {
            p->health = u8_inc(p->health, 1);
        }
        if(p->health == 0) {
            p->alive = false;
            tama_emote(app, EmoteDeath);
            tama_popup(app, "It passed away...");
        }
    }

    // care / neglect bookkeeping
    uint8_t wb = pet_wellbeing(p);
    if(wb < 35) {
        p->neglect++;
        if(due(app, 4) && p->care_score > CARE_BAD * 4) p->care_score--;
    } else {
        if(p->neglect > 0) p->neglect--;
        if(wb > 70 && due(app, 6) && p->care_score < CARE_GOOD * 4) p->care_score++;
    }

    // attention: nudge the backlight when neglected
    if(settings_on(&app->settings, FeatureBacklightPet) && wb < 30 && due(app, 10)) {
        notification_message(app->notifications, &sequence_display_backlight_on);
    }

    // aging
    if(settings_on(&app->settings, FeatureAging)) {
        p->age_ticks++;
        advance_stage(app);
    }

    p->mood = pet_compute_mood(p);
}
