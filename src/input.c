#include "input.h"
#include "drag.h"
#include "generator.h"
#include "item.h"
#include "task.h"
#include "render.h"
#include "audio.h"

/* Task button dimensions (must match render.c) */
#define TASK_BTN_WIDTH 200
#define TASK_BTN_HEIGHT 40

void GetSlotAtPosition(Vector2 pos, Vector2 gridTopLeft, float slotSize,
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

void ProcessInput(GameState *state, Vector2 gridTopLeft, float slotSize,
                  int screenWidth, int screenHeight) {
    Vector2 mousePos = GetMousePosition();
    
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        /* Handle Mall screen clicks - check shop buttons FIRST */
        if (state->current_screen == SCREEN_MALL) {
            int shopResult = CheckMallShopClick(state, mousePos, screenWidth);
            if (shopResult == 1) {  /* Enter boutique (unlocked) */
                state->current_screen = SCREEN_BOARD;
                return;
            } else if (shopResult == 2) {  /* Enter janitor's closet */
                state->current_screen = SCREEN_BOARD;
                return;
            } else if (shopResult == 3) {  /* Enter arcade */
                state->current_screen = SCREEN_BOARD;
                return;
            }
            /* Check for boutique upgrade button click */
            if (CheckMallUpgradeClick(state, mousePos, screenWidth)) {
                PurchaseBoutiqueUpgrade(state);
                PlayCoinSound();  /* Coin sound for upgrade purchase */
                SaveGameState(state, "save.dat");
                return;
            }
        }
        
        int row = -1, col = -1;
        GetSlotAtPosition(mousePos, gridTopLeft, slotSize, &row, &col);
        
        /* Handle Board screen clicks */
        if (state->current_screen == SCREEN_BOARD) {
            /* Check for Back to Mall button (always check in top area) */
            int backBtnX = screenWidth - 150;
            int backBtnY = 10;
            if (mousePos.x >= backBtnX && mousePos.x <= backBtnX + 120 &&
                mousePos.y >= backBtnY && mousePos.y <= backBtnY + 30) {
                state->current_screen = SCREEN_MALL;
                state->selected_row = -1;
                state->selected_col = -1;
                return;
            }
            
            /* Check for other UI buttons when click is outside grid */
            if (row < 0 && col < 0) {
                /* Check for task panel UI clicks */
                Task *activeTask = GetActiveTask(state);
                if (activeTask) {
                    /* Check toggle button */
                    if (CheckToggleButtonClick(mousePos, screenWidth, screenHeight)) {
                        state->task_panel_visible = !state->task_panel_visible;
                        return;
                    }
                    /* Check complete task button */
                    if (CheckTaskButtonClick(state, mousePos, screenWidth, screenHeight)) {
                        TaskOutcome outcome = ExecuteTask(activeTask, state);
                        PlayCoinSound();  /* Coin sound for task completion */
                        SaveGameState(state, "save.dat");
                        return;
                    }
                }
                /* Check for trash button click */
                if (CheckTrashButtonClick(state, mousePos, screenWidth, screenHeight)) {
                    DeleteSelectedItem(state);
                    PlayTrashSound();  /* Trash sound for deletion */
                    SaveGameState(state, "save.dat");
                    return;
                }
            }
        }
        
        /* Handle grid clicks - start drag */
        if (row >= 0 && col >= 0) {
            state->tap_row = row;
            state->tap_col = col;
            state->tap_timer = 0.0f;
            StartDrag(state, row, col);
        }
    }
    
    /* Track tap duration */
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && 
        state->tap_row >= 0 && state->tap_col >= 0) {
        state->tap_timer += GetFrameTime();
    }
    
    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        int row = -1, col = -1;
        GetSlotAtPosition(mousePos, gridTopLeft, slotSize, &row, &col);
        
        /* If released in same slot and was quick tap, check for generator tap */
        if (state->dragging && row == state->tap_row && col == state->tap_col && 
            state->tap_timer < TAP_THRESHOLD) {
            if (state->dragged_item.is_generator) {
                /* Tap on generator: activate it and restore generator to slot */
                if (ActivateGenerator(state, state->dragged_item.item_id, state->tap_row, state->tap_col)) {
                    /* Success: restore generator */
                    state->grid[state->tap_row][state->tap_col].item = state->dragged_item;
                } else {
                    /* Failed (no energy/cooldown): restore but don't activate */
                    state->grid[state->tap_row][state->tap_col].item = state->dragged_item;
                }
                state->dragging = 0;
            } else {
                /* Quick tap on regular item: SELECT IT (don't delete!) */
                /* Restore item back to slot first (clear for drag was done in StartDrag) */
                state->grid[state->tap_row][state->tap_col].item = state->dragged_item;
                
                /* Toggle selection state */
                if (state->selected_row == state->tap_row && state->selected_col == state->tap_col) {
                    /* Already selected - deselect it */
                    state->selected_row = -1;
                    state->selected_col = -1;
                } else {
                    /* Select this slot */
                    state->selected_row = state->tap_row;
                    state->selected_col = state->tap_col;
                }
                state->dragging = 0;
            }
        } else if (state->dragging) {
            /* Either moved or held too long: do normal drag-drop (merge) */
            EndDrag(state, row, col, gridTopLeft, slotSize);
        }
        
        state->tap_row = -1;
        state->tap_col = -1;
        state->tap_timer = 0.0f;
    }
}