#include "drag.h"
#include "item.h"
#include "items.h"

void StartDrag(GameState *state, int row, int col) {
    if (ItemIsEmpty(&state->grid[row][col].item)) {
        return;
    }
    
    state->dragging = 1;
    state->drag_from_row = row;
    state->drag_from_col = col;
    state->dragged_item = state->grid[row][col].item;
    
    /* Clear origin slot */
    state->grid[row][col].item.item_id = ITEM_ID_EMPTY;
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
    if (target->item.item_id == state->dragged_item.item_id) {
        /* Same item ID: check if we can merge */
        ItemID next_id = GetNextItemInChain(target->item.item_id);
        if (next_id != ITEM_ID_EMPTY) {
            /* Successfully merge: target becomes next level in chain */
            target->item.item_id = next_id;
            
            /* Update is_generator flag if necessary */
            const ItemDefinition *def = GetItemDefinition(next_id);
            if (def) {
                target->item.is_generator = def->is_generator;
            }
            return;
        }
    }
    
    /* Different item or cannot merge: snap back to origin */
    state->grid[state->drag_from_row][state->drag_from_col].item = state->dragged_item;
}
