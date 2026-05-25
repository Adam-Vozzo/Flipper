#pragma once

#include <stdint.h>
#include <stdbool.h>

// pet.c needs the app for settings + feedback, but the app embeds a Pet by
// value, so we only forward-declare here to avoid an include cycle.
typedef struct TamagotchiApp TamagotchiApp;

typedef enum {
    StageEgg,
    StageBaby,
    StageChild,
    StageTeen,
    StageAdult,
    StageElder,
    StageAngel, // karma-evo "good" branch
    StageGoblin, // karma-evo "bad" branch
    StageCount,
} LifeStage;

typedef enum {
    MoodEgg,
    MoodHappy,
    MoodNeutral,
    MoodHungry,
    MoodSad,
    MoodSleepy,
    MoodAsleep,
    MoodSick,
    MoodDirty,
    MoodPlaying,
    MoodDead,
    MoodGhost,
    MoodCount,
} Mood;

typedef enum {
    FoodSnack, // quick, fattening, a little messy
    FoodMeal, // big hunger restore
    FoodVeg, // healthy, less happy
    FoodKindCount,
} FoodKind;

typedef enum {
    TraitEasy, // balanced, no modifiers
    TraitGlutton, // gets hungry faster
    TraitLazy, // tires slowly but bores quickly
    TraitBouncy, // burns energy fast, stays cheerful
    TraitFussy, // gets dirty faster
    TraitCount,
} Trait;

typedef struct {
    char name[12];

    // core needs, 0..100, higher is better
    uint8_t food;
    uint8_t happy;
    uint8_t energy;
    uint8_t hygiene;
    uint8_t health;

    LifeStage stage;
    Mood mood; // cached each tick for rendering

    uint32_t age_ticks; // ticks since hatch
    uint16_t weight; // cosmetic chonk meter
    uint8_t poop; // piles on the floor
    int32_t care_score; // good care up, neglect down -> karma evo
    uint16_t neglect; // consecutive neglect ticks

    uint8_t personality; // Trait

    bool hatched;
    bool alive;
    bool asleep;
    bool sick;

    // pacing accumulators
    uint16_t poop_timer;
    uint16_t play_anim; // frames left of a play wiggle
    uint16_t sick_timer;
} Pet;

void pet_init(Pet* p);
void pet_hatch(TamagotchiApp* app);
void pet_reset_egg(TamagotchiApp* app);

// advance the simulation by one tick (driven by the game timer)
void pet_tick(TamagotchiApp* app);

// player actions
void pet_feed(TamagotchiApp* app, FoodKind kind);
void pet_play(TamagotchiApp* app);
void pet_toggle_sleep(TamagotchiApp* app);
void pet_clean(TamagotchiApp* app);
void pet_heal(TamagotchiApp* app);
void pet_cuddle(TamagotchiApp* app);

Mood pet_compute_mood(const Pet* p);
const char* pet_stage_name(LifeStage stage);
const char* food_name(FoodKind kind);
const char* trait_name(uint8_t trait);

// average of the four day-to-day needs (excludes health) for quick display
uint8_t pet_wellbeing(const Pet* p);
