#include "game_state.h"
#include "item.h"
#include "generator.h"
#include "render.h"
#include "task.h"
#include <stdio.h>

void GameStateInit(GameState *state, int screenWidth, int screenHeight) {
    *state = (GameState){0};
    
    state->current_screen = SCREEN_MALL;
    state->max_energy = 100;
    state->energy = 50;
    state->energy_regen_rate = 2.0f;
    state->coins = 0;
    state->generator_cooldown_max = GENERATOR_COOLDOWN_MAX;
    state->generator_cooldown = 0.0f;
    
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
    
    state->grid[0][0].item.item_id = ITEM_ID_GENERATOR_LOST_FOUND;
    state->grid[0][0].item.is_generator = 1;
    
    SpawnItem(state, ITEM_ID_FADED_RECEIPT, -1, -1);
    SpawnItem(state, ITEM_ID_FADED_RECEIPT, -1, -1);
    SpawnItem(state, ITEM_ID_FADED_RECEIPT, -1, -1);
    
    state->skeleton_key_task = CreateSkeletonKeyExchangeTask();
    state->task_panel_visible = 1;
    
    state->mannequin_task = (Task){
        .required_item_id = ITEM_ID_DESIGNERS_MANNEQUIN,
        .reward_item_id = ITEM_ID_NEON_TOKEN,
        .is_available = 0,
        .description = "Restore the Boutique by trading the Designer's Mannequin",
        .coinReward = 250
    };
    
    state->arcade_task = (Task){
        .required_item_id = ITEM_ID_HIGH_SCORE_TROPHY,
        .reward_item_id = ITEM_ID_EMPTY,
        .is_available = 0,
        .description = "Master the Arcade by trading the High Score Trophy",
        .coinReward = 500
    };
    
    state->boutiqueUnlocked = 0;
    state->boutiqueRestored = 0;
    state->boutiqueUpgraded = 0;
    state->arcadeUnlocked = 0;
    state->arcadeRestored = 0;
    
    /* Load custom font with star symbol support */
    int codepoints[100];
    int count = 0;
    for (int i = 32; i <= 126 && count < 99; i++) {
        codepoints[count++] = i;
    }
    codepoints[count++] = 0x2605;  /* Star symbol ★ */
    state->customFont = LoadFontEx("assets/fonts/Roboto-Regular.ttf", 32, codepoints, count);
}

Texture2D *GetItemTexture(GameState *state, ItemID item_id) {
    for (int i = 0; i < state->texture_cache_count; i++) {
        if (state->texture_cache[i].item_id == item_id) {
            return &state->texture_cache[i].texture;
        }
    }
    
    const ItemDefinition *def = GetItemDefinition(item_id);
    if (!def || !def->asset_path) {
        return NULL;
    }
    
    if (state->texture_cache_count >= TEXTURE_CACHE_SIZE) {
        return NULL;
    }
    
    Texture2D tex = LoadTexture(def->asset_path);
    if (tex.id <= 0) {
        return NULL;
    }
    
    state->texture_cache[state->texture_cache_count].item_id = item_id;
    state->texture_cache[state->texture_cache_count].texture = tex;
    state->texture_cache_count++;
    
    return &state->texture_cache[state->texture_cache_count - 1].texture;
}

void GameStateCleanup(GameState *state) {
    for (int i = 0; i < state->texture_cache_count; i++) {
        if (state->texture_cache[i].texture.id > 0) {
            UnloadTexture(state->texture_cache[i].texture);
        }
    }
    
    if (state->customFont.texture.id > 0) {
        UnloadFont(state->customFont);
    }
}

int SaveGameState(GameState *state, const char *filename) {
    FILE *file = fopen(filename, "wb");
    if (!file) {
        return 0;
    }
    
    size_t written = fwrite(state, sizeof(GameState), 1, file);
    fclose(file);
    
    return (written == 1) ? 1 : 0;
}

int LoadGameState(GameState *state, const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        return 0;
    }
    
    size_t read = fread(state, sizeof(GameState), 1, file);
    fclose(file);
    
    return (read == 1) ? 1 : 0;
}