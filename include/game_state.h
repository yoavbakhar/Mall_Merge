#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "raylib.h"
#include "item.h"
#include "items.h"
#include "constants.h"

/* Screen states */
typedef enum {
    SCREEN_MALL,
    SCREEN_BOARD
} GameScreen;

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
    int coinReward;
} Task;

typedef struct GameState {
    Item item;
    Rectangle bounds;
} Slot;

/* Texture cache entry */
typedef struct {
    ItemID item_id;
    Texture2D texture;
} TextureCacheEntry;

#define TEXTURE_CACHE_SIZE 20

typedef struct {
    /* Current screen state */
    GameScreen current_screen;
    
    /* Grid state */
    Slot grid[GRID_ROWS][GRID_COLS];
    
    /* Drag-and-drop state machine */
    int dragging;
    int drag_from_row, drag_from_col;
    Item dragged_item;
    int tap_row, tap_col;
    float tap_timer;
    
    /* Game economy */
    int energy;
    int max_energy;
    float energy_regen_rate;
    float energy_regen_timer;
    
    int coins;
    
    /* Generator cooldown */
    float generator_cooldown;
    float generator_cooldown_max;
    
    /* Texture cache */
    TextureCacheEntry texture_cache[TEXTURE_CACHE_SIZE];
    int texture_cache_count;
    
    /* Task system */
    Task skeleton_key_task;
    Task mannequin_task;
    Task arcade_task;
    int task_panel_visible;
    
    /* Selection state */
    int selected_row;
    int selected_col;
    
    /* Game progression flags */
    int boutiqueUnlocked;
    int boutiqueRestored;
    int boutiqueUpgraded;
    int arcadeUnlocked;
    int arcadeRestored;
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