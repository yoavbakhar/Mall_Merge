#include "generator.h"
#include "item.h"

int SpawnItem(GameState *state, int type) {
    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            if (ItemIsEmpty(&state->grid[r][c].item)) {
                state->grid[r][c].item.type = type;
                state->grid[r][c].item.level = 1;
                state->grid[r][c].item.is_generator = (type == ITEM_TYPE_GENERATOR);
                return 1;
            }
        }
    }
    return 0;  /* No empty slots */
}

int ActivateGenerator(GameState *state) {
    /* Check cooldown */
    if (state->generator_cooldown > 0.0f) {
        return 0;
    }
    
    /* Check energy */
    if (state->energy < GENERATOR_SPAWN_COST) {
        return 0;
    }
    
    /* Deduct energy */
    state->energy -= GENERATOR_SPAWN_COST;
    
    /* Spawn normal item */
    if (!SpawnItem(state, ITEM_TYPE_NORMAL)) {
        /* Grid full; refund energy */
        state->energy += GENERATOR_SPAWN_COST;
        return 0;
    }
    
    /* Start cooldown */
    state->generator_cooldown = state->generator_cooldown_max;
    return 1;
}
