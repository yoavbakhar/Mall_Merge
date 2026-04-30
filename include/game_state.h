#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "raylib.h"
#include "item.h"
#include "items.h"
#include "constants.h"

/* Task outcome flags */
typedef enum {
    TASK_OUTCOME_SUCCESS = 1,
    TASK_OUTCOME_ITEM_NOT_FOUND = 0,
    TASK_OUTCOME_ALREADY_COMPLETED = -1
} TaskOutcome;

/* Task definition */
typedef struct Task {
    ItemID required_item_id;
    ItemID reward_item_id;
    int is_available;
    const char *description;
} Task;

typedef struct GameState {
    Item item;
    Rectangle bounds;  /* Screen position & size of this slot */
} Slot;

/* Texture cache entry */
typedef struct {
    ItemID item_id;
    Texture2D texture;
} TextureCacheEntry;

#define TEXTURE_CACHE_SIZE 20

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
    
    /* Texture cache */
    TextureCacheEntry texture_cache[TEXTURE_CACHE_SIZE];
    int texture_cache_count;
    
    /* Task system */
    Task skeleton_key_task;
    Task mannequin_task;  /* Second task: Mannequin -> Final reward */
    int task_panel_visible;  /* 1 if task panel is visible, 0 if collapsed */
    
    /* Game progression flags */
    int boutiqueUnlocked;   /* 1 if boutique chain is unlocked (after Skeleton Key task) */
    int boutiqueRestored;   /* 1 if boutique is fully restored (after Mannequin task) */
} GameState;

/* Initialize game state */
void GameStateInit(GameState *state, int screenWidth, int screenHeight);

/* Get texture for an item (load on demand) */
Texture2D *GetItemTexture(GameState *state, ItemID item_id);

/* Cleanup all loaded textures */
void GameStateCleanup(GameState *state);

/* Save game state to binary file */
int SaveGameState(GameState *state, const char *filename);

/* Load game state from binary file */
int LoadGameState(GameState *state, const char *filename);

#endif
