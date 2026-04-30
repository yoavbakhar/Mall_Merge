#include "generator.h"
#include "item.h"
#include "items.h"

int SpawnItem(GameState *state, ItemID item_id) {
    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            if (ItemIsEmpty(&state->grid[r][c].item)) {
                state->grid[r][c].item.item_id = item_id;
                const ItemDefinition *def = GetItemDefinition(item_id);
                state->grid[r][c].item.is_generator = def->is_generator;
                return 1;
            }
        }
    }
    return 0;  /* No empty slots */
}

int ActivateGenerator(GameState *state, ItemID generator_id) {
    /* Check cooldown */
    if (state->generator_cooldown > 0.0f) {
        return 0;
    }
    
    /* Check energy */
    if (state->energy < GENERATOR_SPAWN_COST) {
        return 0;
    }
    
    /* Get generator definition to find its chain */
    const ItemDefinition *gen_def = GetItemDefinition(generator_id);
    if (!gen_def || !gen_def->is_generator) {
        return 0;
    }
    
    /* Get level 1 item from this generator's chain */
    ItemID spawn_item = GetChainLevel1Item(gen_def->chain_id);
    if (spawn_item == ITEM_ID_EMPTY) {
        return 0;
    }
    
    /* Deduct energy */
    state->energy -= GENERATOR_SPAWN_COST;
    
    /* Spawn the level 1 item */
    if (!SpawnItem(state, spawn_item)) {
        /* Grid full; refund energy */
        state->energy += GENERATOR_SPAWN_COST;
        return 0;
    }
    
    /* Start cooldown */
    state->generator_cooldown = state->generator_cooldown_max;
    return 1;
}
