#include "tamagotchi.h"

// --- note primitives (frequency in Hz) ---
#define NOTE(name, freq)              \
    static const NotificationMessage name = { \
        .type = NotificationMessageTypeSoundOn, \
        .data.sound = {.frequency = (freq), .volume = 1.0f}}

NOTE(n_c4, 261.63f);
NOTE(n_e4, 329.63f);
NOTE(n_g4, 392.00f);
NOTE(n_c5, 523.25f);
NOTE(n_e5, 659.25f);
NOTE(n_g5, 783.99f);
NOTE(n_c6, 1046.50f);

// --- melodies (sound channel only) ---
static const NotificationSequence snd_happy = {
    &n_c5, &message_delay_50, &n_e5, &message_delay_50, &message_sound_off, NULL};
static const NotificationSequence snd_eat = {
    &n_g4, &message_delay_50, &n_c5, &message_delay_50, &message_sound_off, NULL};
static const NotificationSequence snd_sleep = {
    &n_c5, &message_delay_100, &n_g4, &message_delay_100, &message_sound_off, NULL};
static const NotificationSequence snd_play = {
    &n_c5,
    &message_delay_50,
    &n_e5,
    &message_delay_50,
    &n_g5,
    &message_delay_50,
    &message_sound_off,
    NULL};
static const NotificationSequence snd_clean = {
    &n_e5, &message_delay_50, &n_c5, &message_delay_50, &message_sound_off, NULL};
static const NotificationSequence snd_heal = {
    &n_c5, &message_delay_50, &n_g5, &message_delay_50, &message_sound_off, NULL};
static const NotificationSequence snd_sad = {
    &n_e4, &message_delay_100, &n_c4, &message_delay_100, &message_sound_off, NULL};
static const NotificationSequence snd_sick = {
    &n_c4, &message_delay_100, &message_sound_off, NULL};
static const NotificationSequence snd_levelup = {
    &n_c5,
    &message_delay_50,
    &n_e5,
    &message_delay_50,
    &n_g5,
    &message_delay_50,
    &n_c6,
    &message_delay_100,
    &message_sound_off,
    NULL};
static const NotificationSequence snd_death = {
    &n_g4,
    &message_delay_100,
    &n_e4,
    &message_delay_100,
    &n_c4,
    &message_delay_250,
    &message_sound_off,
    NULL};

typedef struct {
    const NotificationSequence* sound;
    const NotificationSequence* led; // predefined RGB blink
    const NotificationSequence* vibro;
} EmoteFx;

static const EmoteFx fx[EmoteHatch + 1] = {
    [EmoteHappy] = {&snd_happy, &sequence_blink_green_100, &sequence_single_vibro},
    [EmoteEat] = {&snd_eat, &sequence_blink_yellow_10, &sequence_single_vibro},
    [EmoteSleep] = {&snd_sleep, &sequence_blink_blue_100, &sequence_single_vibro},
    [EmotePlay] = {&snd_play, &sequence_blink_cyan_100, &sequence_single_vibro},
    [EmoteClean] = {&snd_clean, &sequence_blink_cyan_100, &sequence_single_vibro},
    [EmoteHeal] = {&snd_heal, &sequence_blink_magenta_100, &sequence_single_vibro},
    [EmoteSad] = {&snd_sad, &sequence_blink_blue_10, &sequence_single_vibro},
    [EmoteSick] = {&snd_sick, &sequence_blink_red_10, &sequence_double_vibro},
    [EmoteLevelUp] = {&snd_levelup, &sequence_blink_magenta_100, &sequence_double_vibro},
    [EmoteDeath] = {&snd_death, &sequence_blink_red_100, &sequence_double_vibro},
    [EmoteHatch] = {&snd_levelup, &sequence_blink_green_100, &sequence_double_vibro},
};

void tama_emote(TamagotchiApp* app, Emote emote) {
    if(emote > EmoteHatch) return;
    const EmoteFx* e = &fx[emote];
    if(settings_on(&app->settings, FeatureSound) && e->sound) {
        notification_message(app->notifications, e->sound);
    }
    if(settings_on(&app->settings, FeatureLed) && e->led) {
        notification_message(app->notifications, e->led);
    }
    if(settings_on(&app->settings, FeatureHaptics) && e->vibro) {
        notification_message(app->notifications, e->vibro);
    }
}
