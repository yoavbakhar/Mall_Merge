#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "raylib.h"
#include "item.h"
#include "constants.h"

typedef struct {
    Item item;
    Rectangle bounds;  /* Screen position & size of this slot */
} Slot;

typedef struct {
    /* Grid state */
    Slot grid[GRID_ROWS][GRID_COLS];
    
    /* Drag-and-drop state machine */
    int dragging;                    /* 1 if currently dragging, 0 otherwise */
    int drag_from_row, drag_from_col;  /* Origin slot of dragged item */
    Item dragged_item;               /* Copy of item being dragged */
    int tap_row, tap_col;            /* Slot where mouse was pressed (for tap detection) */
    float tap_timer;                 /* Time since mouse pressed (detect tap vs drag) */
    
    /* Game economy */
    int energy;
    int max_energy;
    float energy_regen_rate;  /* Energy points per second */
    float energy_regen_timer;
    
    int coins;
    
    /* Generator cooldown (shared across all generators for now) */
    float generator_cooldown;
    float generator_cooldown_max;
    
    /* Assets */
    Texture2D generator_texture;  /* Texture for generator item */
} GameState;

/* Initialize game state */
void GameStateInit(GameState *state, int screenWidth, int screenHeight);

#endif
