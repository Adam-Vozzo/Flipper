#pragma once

#include "pet.h"

// Returns the kaomoji string for the pet's current mood. `blink` and `frame`
// drive idle animation (eye blinks, wiggling limbs). Egg/dead are drawn as
// shapes by the scene, but faces are provided here too as a fallback.
const char* kaomoji_face(const Pet* p, bool blink, uint32_t frame);
const char* kaomoji_for_mood(Mood mood, bool blink, uint32_t frame);
