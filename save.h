#pragma once

#include "pet.h"

// Loads the saved pet + settings into the app if a valid save exists.
void save_load(TamagotchiApp* app);

// Snapshots the app state under the mutex, then writes it to storage. Safe to
// call from the main loop (does file IO without holding the mutex).
void save_store(TamagotchiApp* app);
