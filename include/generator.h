#ifndef GENERATOR_H
#define GENERATOR_H

#include "game_state.h"
#include "items.h"

/* Attempt to activate a generator (tapped on grid) */
int ActivateGenerator(GameState *state, ItemID generator_id);

/* Spawn an item into first available slot; returns 1 on success */
int SpawnItem(GameState *state, ItemID item_id);

#endif
