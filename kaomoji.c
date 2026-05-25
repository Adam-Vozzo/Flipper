#include "kaomoji.h"

// ASCII-only kaomoji so they render in the stock Flipper fonts. Each mood has a
// few variants we cycle through for liveliness.
static const char* const k_happy[] = {"(^o^)", "(^_^)", "(*^o^*)", "\\(^o^)/"};
static const char* const k_neutral[] = {"(o_o)", "(._.)", "( o_o )", "(o.o)"};
static const char* const k_hungry[] = {"(@_@)", "(o_o)?", "(>_<)", "(@_@)~"};
static const char* const k_sad[] = {"(T_T)", "(;_;)", "(._.)", "(u_u)"};
static const char* const k_sleepy[] = {"(-_-)", "(=_=)", "(u_u)", "(-.-)"};
static const char* const k_asleep[] = {"(-_-) z", "(u_u) zz", "(=_=) zZ", "(-_-) zzZ"};
static const char* const k_sick[] = {"(x_x)", "(+_+)", "(@_@)", "(>_<#)"};
static const char* const k_dirty[] = {"(>_<)", "(o_O;)", "(;-;)", "(>.<)"};
static const char* const k_playing[] = {"\\(^o^)/", "(^o^)/", "\\(>w<)", "(>w<)/"};
static const char* const k_dead[] = {"(x_x)", "(X_X)", "(+_+)"};
static const char* const k_ghost[] = {"( o_o)~", "~(o_o )", "( -_-)~", "[ o_o ]"};

// Cat-flavoured variants (toggle: Cat Mode).
static const char* const c_happy[] = {"(=^o^=)", "(=^.^=)", "\\(=^o^=)/", "(=^w^=)"};
static const char* const c_neutral[] = {"(=o.o=)", "(=-.-=)", "(=^.^=)", "(=o_o=)"};
static const char* const c_hungry[] = {"(=@.@=)", "(=o.o=)?", "(=>.<=)", "(=^q^=)"};
static const char* const c_sad[] = {"(=T.T=)", "(=;.;=)", "(=u.u=)", "(=._.=)"};
static const char* const c_sleepy[] = {"(=-.-=)", "(=u.u=)", "(=_._=)", "(=-.-=)"};
static const char* const c_asleep[] = {"(=-.-=)z", "(=u.u=)zz", "(=_._=)zZ", "(=-.-=)zzZ"};
static const char* const c_sick[] = {"(=x.x=)", "(=+.+=)", "(=@.@=)", "(=>.<=)"};
static const char* const c_dirty[] = {"(=>.<=)", "(=o.O=)", "(=;.;=)", "(=>_<=)"};
static const char* const c_playing[] = {"\\(=^o^=)/", "(=^o^=)/", "\\(=>w<=)", "(=>w<=)/"};

#define PICK(arr, frame, slow) (arr[((frame) / (slow)) % (sizeof(arr) / sizeof(arr[0]))])

const char* kaomoji_for_mood(Mood mood, bool blink, uint32_t frame, bool cat) {
    // A short, periodic blink reads as "alive" for the calm moods.
    if(blink && (mood == MoodNeutral || mood == MoodHappy)) return cat ? "(=-.-=)" : "(-_-)";

    if(cat) {
        switch(mood) {
        case MoodHappy: return PICK(c_happy, frame, 7);
        case MoodNeutral: return PICK(c_neutral, frame, 9);
        case MoodHungry: return PICK(c_hungry, frame, 5);
        case MoodSad: return PICK(c_sad, frame, 8);
        case MoodSleepy: return PICK(c_sleepy, frame, 8);
        case MoodAsleep: return PICK(c_asleep, frame, 4);
        case MoodSick: return PICK(c_sick, frame, 5);
        case MoodDirty: return PICK(c_dirty, frame, 6);
        case MoodPlaying: return PICK(c_playing, frame, 2);
        case MoodEgg: return "( o )";
        default: break; // dead/ghost share the base set below
        }
    }

    switch(mood) {
    case MoodHappy: return PICK(k_happy, frame, 7);
    case MoodNeutral: return PICK(k_neutral, frame, 9);
    case MoodHungry: return PICK(k_hungry, frame, 5);
    case MoodSad: return PICK(k_sad, frame, 8);
    case MoodSleepy: return PICK(k_sleepy, frame, 8);
    case MoodAsleep: return PICK(k_asleep, frame, 4);
    case MoodSick: return PICK(k_sick, frame, 5);
    case MoodDirty: return PICK(k_dirty, frame, 6);
    case MoodPlaying: return PICK(k_playing, frame, 2);
    case MoodDead: return PICK(k_dead, frame, 10);
    case MoodGhost: return PICK(k_ghost, frame, 4);
    case MoodEgg: return "( o )";
    default: return "(o_o)";
    }
}

const char* kaomoji_face(const Pet* p, bool blink, uint32_t frame, bool cat) {
    return kaomoji_for_mood(p->mood, blink, frame, cat);
}
