#include "game_state.h"
#include "item.h"
#include "generator.h"
#include "render.h"

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
            state->grid[r][c].item.type = 0;  /* Empty */
            state->grid[r][c].bounds = (Rectangle){
                gridTopLeft.x + c * slotSize,
                gridTopLeft.y + r * slotSize,
                slotSize,
                slotSize
            };
        }
    }
    
    /* Place initial generator at (0,0) */
    state->grid[0][0].item.type = ITEM_TYPE_GENERATOR;
    state->grid[0][0].item.level = 1;
    state->grid[0][0].item.is_generator = 1;
    
    /* Spawn test items */
    SpawnItem(state, ITEM_TYPE_NORMAL);
    SpawnItem(state, ITEM_TYPE_NORMAL);
    SpawnItem(state, ITEM_TYPE_NORMAL);
    
    /* Load generator texture */
    state->generator_texture = LoadTexture("assets/Lost_And_Found.png");
}
