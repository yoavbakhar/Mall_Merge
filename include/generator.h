#ifndef GENERATOR_H
#define GENERATOR_H

#include "game_state.h"

/* Attempt to activate a generator (tapped on grid) */
int ActivateGenerator(GameState *state);

/* Spawn a normal item into first available slot; returns 1 on success */
int SpawnItem(GameState *state, int type);

#endif
