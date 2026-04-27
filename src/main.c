#include "raylib.h"
#include <stdlib.h>
#include <string.h>

/* ============================================================================
   DATA STRUCTURES & CONSTANTS
   ============================================================================ */

#define GRID_COLS 6
#define GRID_ROWS 6
#define MAX_LEVEL 10

/* Item types */
#define ITEM_TYPE_NORMAL 1
#define ITEM_TYPE_GENERATOR 2

typedef struct {
    int type;           /* ITEM_TYPE_NORMAL, ITEM_TYPE_GENERATOR, 0=empty */
    int level;          /* Item level (1=base, higher=merged) */
    int is_generator;   /* 1 if this is a generator, 0 otherwise */
} Item;

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
} GameState;

/* ============================================================================
   UTILITY FUNCTIONS
   ============================================================================ */

/* Get grid slot dimensions from screen size */
static float GetSlotSize(int screenWidth) {
    /* Leave margins on left/right; 6 columns fit in available width */
    float margin = 20.0f;
    float available_width = screenWidth - (2 * margin);
    return available_width / GRID_COLS;
}

/* Get screen position of grid top-left corner */
static Vector2 GetGridTopLeft(int screenWidth, float slotSize) {
    return (Vector2){
        (screenWidth - (GRID_COLS * slotSize)) / 2.0f,
        60.0f  /* Top margin */
    };
}

/* Get generator button bounds (bottom of screen) */
static Rectangle GetGeneratorButtonBounds(int screenWidth, int screenHeight) {
    int btnWidth = 150;
    int btnHeight = 50;
    return (Rectangle){
        (screenWidth - btnWidth) / 2.0f,
        (float)(screenHeight - btnHeight - 15),
        (float)btnWidth,
        (float)btnHeight
    };
}

/* Find which grid cell a screen position falls into; returns -1, -1 if none */
static void GetSlotAtPosition(Vector2 pos, Vector2 gridTopLeft, float slotSize,
                              int *out_row, int *out_col) {
    float rel_x = pos.x - gridTopLeft.x;
    float rel_y = pos.y - gridTopLeft.y;
    
    int col = (int)(rel_x / slotSize);
    int row = (int)(rel_y / slotSize);
    
    if (row >= 0 && row < GRID_ROWS && col >= 0 && col < GRID_COLS) {
        *out_row = row;
        *out_col = col;
    } else {
        *out_row = -1;
        *out_col = -1;
    }
}

/* Check if slot is empty */
static int SlotIsEmpty(const Slot *slot) {
    return slot->item.type == 0;
}

/* Spawn a new Level 1 item into the first available slot; returns 1 on success */
static int SpawnItem(GameState *state, int type) {
    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            if (SlotIsEmpty(&state->grid[r][c])) {
                state->grid[r][c].item.type = type;
                state->grid[r][c].item.level = 1;
                return 1;
            }
        }
    }
    return 0;  /* No empty slots */
}

/* Attempt to activate a generator (tapped on grid) */
static int ActivateGenerator(GameState *state) {
    #define GENERATOR_SPAWN_COST 15
    
    /* Check cooldown */
    if (state->generator_cooldown > 0.0f) {
        return 0;  /* Still on cooldown */
    }
    
    /* Check energy */
    if (state->energy < GENERATOR_SPAWN_COST) {
        return 0;  /* Not enough energy */
    }
    
    /* Deduct energy */
    state->energy -= GENERATOR_SPAWN_COST;
    
    /* Spawn normal item (type 1 = basic item from generator) */
    if (!SpawnItem(state, ITEM_TYPE_NORMAL)) {
        /* Grid full; refund energy */
        state->energy += GENERATOR_SPAWN_COST;
        return 0;
    }
    
    /* Start cooldown */
    state->generator_cooldown = state->generator_cooldown_max;
    return 1;
}

/* ============================================================================
   DRAG-AND-DROP STATE MACHINE
   ============================================================================ */

/* Called when mouse is pressed over a slot */
static void StartDrag(GameState *state, int row, int col) {
    if (SlotIsEmpty(&state->grid[row][col])) {
        return;  /* Can't drag empty slot */
    }
    
    state->dragging = 1;
    state->drag_from_row = row;
    state->drag_from_col = col;
    state->dragged_item = state->grid[row][col].item;
    
    /* Clear origin slot */
    state->grid[row][col].item.type = 0;
}

/* Called when mouse is released; handles drop logic */
static void EndDrag(GameState *state, int target_row, int target_col, 
                    Vector2 gridTopLeft, float slotSize) {
    if (!state->dragging) return;
    
    state->dragging = 0;
    
    /* Invalid target or out of bounds */
    if (target_row < 0 || target_row >= GRID_ROWS || 
        target_col < 0 || target_col >= GRID_COLS) {
        /* Snap back to origin */
        state->grid[state->drag_from_row][state->drag_from_col].item = state->dragged_item;
        return;
    }
    
    Slot *target = &state->grid[target_row][target_col];
    
    /* Target slot is empty: simple placement */
    if (SlotIsEmpty(target)) {
        target->item = state->dragged_item;
        return;
    }
    
    /* Target slot has an item */
    if (target->item.type == state->dragged_item.type && 
        target->item.level == state->dragged_item.level) {
        /* Same type & level: MERGE! */
        target->item.level++;
        /* dragged_item is discarded */
        return;
    }
    
    /* Different type or level: snap back to origin */
    state->grid[state->drag_from_row][state->drag_from_col].item = state->dragged_item;
}

/* ============================================================================
   MAIN
   ============================================================================ */

int main(void) {
    /* Standard Mobile/Vibe Resolution (9:16 aspect ratio) */
    const int screenWidth = 450;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "Mall Merge: Abandoned Vibes");
    SetTargetFPS(60);

    /* Initialize game state */
    GameState state = {0};
    state.max_energy = 100;
    state.energy = 50;
    state.energy_regen_rate = 2.0f;  /* Restore 2 energy/sec */
    state.coins = 0;
    state.generator_cooldown_max = 2.0f;  /* 2 second cooldown between spawns */
    state.generator_cooldown = 0.0f;  /* Not on cooldown initially */

    /* Initialize grid layout */
    float slotSize = GetSlotSize(screenWidth);
    Vector2 gridTopLeft = GetGridTopLeft(screenWidth, slotSize);
    
    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            state.grid[r][c].item.type = 0;  /* Empty */
            state.grid[r][c].bounds = (Rectangle){
                gridTopLeft.x + c * slotSize,
                gridTopLeft.y + r * slotSize,
                slotSize,
                slotSize
            };
        }
    }

    /* Spawn a test generator and items */
    state.grid[0][0].item.type = ITEM_TYPE_GENERATOR;
    state.grid[0][0].item.level = 1;
    state.grid[0][0].item.is_generator = 1;
    SpawnItem(&state, ITEM_TYPE_NORMAL);
    SpawnItem(&state, ITEM_TYPE_NORMAL);
    SpawnItem(&state, ITEM_TYPE_NORMAL);

    /* ========================================================================
       MAIN LOOP
       ======================================================================== */
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        
        /* ====== UPDATE ====== */
        
        /* Regenerate energy */
        state.energy_regen_timer += dt;
        while (state.energy_regen_timer >= 1.0f && state.energy < state.max_energy) {
            state.energy += (int)(state.energy_regen_rate);
            state.energy_regen_timer -= 1.0f;
        }
        if (state.energy > state.max_energy) {
            state.energy = state.max_energy;
        }
        
        /* Update generator cooldown */
        if (state.generator_cooldown > 0.0f) {
            state.generator_cooldown -= dt;
        }
        
        /* Handle drag-and-drop / tap input */
        Vector2 mousePos = GetMousePosition();
        
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            /* Clicked on grid: start drag or tap */
            int row = -1, col = -1;
            GetSlotAtPosition(mousePos, gridTopLeft, slotSize, &row, &col);
            if (row >= 0 && col >= 0) {
                state.tap_row = row;
                state.tap_col = col;
                state.tap_timer = 0.0f;
                StartDrag(&state, row, col);
            }
        }
        
        /* Track tap duration */
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && 
            state.tap_row >= 0 && state.tap_col >= 0) {
            state.tap_timer += dt;
        }
        
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            int row = -1, col = -1;
            GetSlotAtPosition(mousePos, gridTopLeft, slotSize, &row, &col);
            
            /* If released in same slot and was quick tap, check for generator tap */
            if (state.dragging && row == state.tap_row && col == state.tap_col && state.tap_timer < 0.2f) {
                if (state.dragged_item.is_generator) {
                    /* Tap on generator: activate it and restore generator to slot */
                    if (ActivateGenerator(&state)) {
                        /* Success: restore generator */
                        state.grid[state.tap_row][state.tap_col].item = state.dragged_item;
                    } else {
                        /* Failed (no energy/cooldown): restore but don't activate */
                        state.grid[state.tap_row][state.tap_col].item = state.dragged_item;
                    }
                    state.dragging = 0;  /* Cancel any drag */
                } else {
                    /* Normal item: do normal drag-drop */
                    EndDrag(&state, row, col, gridTopLeft, slotSize);
                }
            } else if (state.dragging) {
                /* Either moved or held too long: do normal drag-drop */
                EndDrag(&state, row, col, gridTopLeft, slotSize);
            }
            
            state.tap_row = -1;
            state.tap_col = -1;
            state.tap_timer = 0.0f;
        }
        
        /* ====== RENDER ====== */
        BeginDrawing();
            ClearBackground((Color){40, 40, 50, 255});  /* Dark grungy background */
            
            /* Draw UI bar at top */
            DrawRectangle(0, 0, screenWidth, 50, (Color){30, 30, 40, 255});
            DrawText(TextFormat("Energy: %d/%d", state.energy, state.max_energy), 
                     20, 15, 16, YELLOW);
            DrawText(TextFormat("Coins: %d", state.coins), 
                     screenWidth - 120, 15, 16, (Color){144, 238, 144, 255});
            
            /* Draw grid slots */
            for (int r = 0; r < GRID_ROWS; r++) {
                for (int c = 0; c < GRID_COLS; c++) {
                    Slot *slot = &state.grid[r][c];
                    
                    /* Draw slot background */
                    DrawRectangleRec(slot->bounds, (Color){60, 60, 70, 255});
                    DrawRectangleLinesEx(slot->bounds, 2.0f, (Color){100, 100, 120, 255});
                    
                    /* Draw item if present and not being dragged */
                    if (!SlotIsEmpty(slot)) {
                        /* Color based on item type */
                        Color itemColor = slot->item.is_generator ? 
                            (Color){200, 180, 100, 255} : (Color){200, 150, 100, 255};
                        DrawRectangleRec(
                            (Rectangle){
                                slot->bounds.x + 2,
                                slot->bounds.y + 2,
                                slot->bounds.width - 4,
                                slot->bounds.height - 4
                            },
                            itemColor
                        );
                        DrawText(TextFormat("L%d", slot->item.level),
                                 (int)(slot->bounds.x + 5),
                                 (int)(slot->bounds.y + 5),
                                 14, WHITE);
                        
                        /* Draw lightning icon for generators */
                        if (slot->item.is_generator) {
                            DrawText("⚡",
                                     (int)(slot->bounds.x + slot->bounds.width - 20),
                                     (int)(slot->bounds.y + 5),
                                     16, YELLOW);
                        }
                    }
                }
            }
            
            /* Draw dragged item at cursor if dragging */
            if (state.dragging) {
                float dragItemSize = slotSize - 4.0f;
                DrawRectangle(
                    (int)(mousePos.x - dragItemSize / 2),
                    (int)(mousePos.y - dragItemSize / 2),
                    (int)dragItemSize,
                    (int)dragItemSize,
                    (Color){255, 200, 100, 200}  /* Semi-transparent highlight */
                );
                DrawText(TextFormat("L%d", state.dragged_item.level),
                         (int)(mousePos.x - 10),
                         (int)(mousePos.y - 10),
                         14, WHITE);
            }
            
            /* Instructional text */
            DrawText("Tap generator (lightning icon) or drag to merge!", 20, screenHeight - 80, 12, LIGHTGRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}