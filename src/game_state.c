#include "game_state.h"
#include "item.h"
#include "generator.h"
#include "render.h"
#include "task.h"
#include <stdio.h>

void GameStateInit(GameState *state, int screenWidth, int screenHeight) {
    *state = (GameState){0};
    
    state->max_energy = 100;
    state->energy = 50;
    state->energy_regen_rate = 2.0f;
    state->coins = 0;
    state->generator_cooldown_max = GENERATOR_COOLDOWN_MAX;
    state->generator_cooldown = 0.0f;
    
    /* Initialize grid layout */
    float slotSize = GetSlotSize(screenWidth);
    Vector2 gridTopLeft = GetGridTopLeft(screenWidth, slotSize);
    
    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            state->grid[r][c].item.item_id = ITEM_ID_EMPTY;
            state->grid[r][c].item.is_generator = 0;
            state->grid[r][c].bounds = (Rectangle){
                gridTopLeft.x + c * slotSize,
                gridTopLeft.y + r * slotSize,
                slotSize,
                slotSize
            };
        }
    }
    
    /* Place initial generator at (0,0) */
    state->grid[0][0].item.item_id = ITEM_ID_GENERATOR_LOST_FOUND;
    state->grid[0][0].item.is_generator = 1;
    
    /* Spawn test items from Chain A */
    SpawnItem(state, ITEM_ID_FADED_RECEIPT);
    SpawnItem(state, ITEM_ID_FADED_RECEIPT);
    SpawnItem(state, ITEM_ID_FADED_RECEIPT);
    
    /* Initialize task system */
    state->skeleton_key_task = CreateSkeletonKeyExchangeTask();
    state->task_panel_visible = 1;  /* Task panel visible by default */
    
    /* Initialize second task: Mannequin -> Final reward */
    state->mannequin_task = (Task){
        .required_item_id = ITEM_ID_DESIGNERS_MANNEQUIN,
        .reward_item_id = ITEM_ID_EMPTY,  /* No physical reward, just completion */
        .is_available = 0,  /* Not available until boutique is unlocked */
        .description = "Restore the Boutique by trading the Designer's Mannequin"
    };
    
    /* Initialize progression flags */
    state->boutiqueUnlocked = 0;
    state->boutiqueRestored = 0;
}

Texture2D *GetItemTexture(GameState *state, ItemID item_id) {
    /* Check if already in cache */
    for (int i = 0; i < state->texture_cache_count; i++) {
        if (state->texture_cache[i].item_id == item_id) {
            return &state->texture_cache[i].texture;
        }
    }
    
    /* Not in cache, load it */
    const ItemDefinition *def = GetItemDefinition(item_id);
    if (!def || !def->asset_path) {
        return NULL;
    }
    
    /* Check cache not full */
    if (state->texture_cache_count >= TEXTURE_CACHE_SIZE) {
        return NULL;  /* Cache full, shouldn't happen with our limited items */
    }
    
    /* Load texture and add to cache */
    Texture2D tex = LoadTexture(def->asset_path);
    if (tex.id <= 0) {
        return NULL;  /* Failed to load */
    }
    
    state->texture_cache[state->texture_cache_count].item_id = item_id;
    state->texture_cache[state->texture_cache_count].texture = tex;
    state->texture_cache_count++;
    
    return &state->texture_cache[state->texture_cache_count - 1].texture;
}

void GameStateCleanup(GameState *state) {
    /* Unload all cached textures */
    for (int i = 0; i < state->texture_cache_count; i++) {
        if (state->texture_cache[i].texture.id > 0) {
            UnloadTexture(state->texture_cache[i].texture);
        }
    }
}

/* Save game state to binary file */
int SaveGameState(GameState *state, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        return 0;  /* Failed to open file */
    }
    
    /* Write the entire GameState struct to file */
    size_t written = fwrite(state, sizeof(GameState), 1, file);
    fclose(file);
    
    return (written == 1) ? 1 : 0;
}

/* Load game state from binary file */
int LoadGameState(GameState *state, const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        return 0;  /* Failed to open file (no save exists) */
    }
    
    /* Read the entire GameState struct from file */
    size_t read = fread(state, sizeof(GameState), 1, file);
    fclose(file);
    
    return (read == 1) ? 1 : 0;
}
