#include "input.h"
#include "drag.h"
#include "generator.h"
#include "item.h"

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
        int row = -1, col = -1;
        GetSlotAtPosition(mousePos, gridTopLeft, slotSize, &row, &col);
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
                if (ActivateGenerator(state)) {
                    /* Success: restore generator */
                    state->grid[state->tap_row][state->tap_col].item = state->dragged_item;
                } else {
                    /* Failed (no energy/cooldown): restore but don't activate */
                    state->grid[state->tap_row][state->tap_col].item = state->dragged_item;
                }
                state->dragging = 0;
            } else {
                /* Normal item: do normal drag-drop */
                EndDrag(state, row, col, gridTopLeft, slotSize);
            }
        } else if (state->dragging) {
            /* Either moved or held too long: do normal drag-drop */
            EndDrag(state, row, col, gridTopLeft, slotSize);
        }
        
        state->tap_row = -1;
        state->tap_col = -1;
        state->tap_timer = 0.0f;
    }
}
