#include "drag.h"
#include "item.h"

void StartDrag(GameState *state, int row, int col) {
    if (ItemIsEmpty(&state->grid[row][col].item)) {
        return;
    }
    
    state->dragging = 1;
    state->drag_from_row = row;
    state->drag_from_col = col;
    state->dragged_item = state->grid[row][col].item;
    
    /* Clear origin slot */
    state->grid[row][col].item.type = 0;
}

void EndDrag(GameState *state, int target_row, int target_col,
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
    if (ItemIsEmpty(&target->item)) {
        target->item = state->dragged_item;
        return;
    }
    
    /* Target slot has an item */
    if (target->item.type == state->dragged_item.type && 
        target->item.level == state->dragged_item.level) {
        /* Same type & level: MERGE! */
        target->item.level++;
        return;
    }
    
    /* Different type or level: snap back to origin */
    state->grid[state->drag_from_row][state->drag_from_col].item = state->dragged_item;
}
