#include "drag.h"
#include "item.h"
#include "items.h"
#include "particles.h"
#include "audio.h"

void StartDrag(GameState *state, int row, int col) {
    if (ItemIsEmpty(&state->grid[row][col].item)) {
        return;
    }
    
    state->dragging = 1;
    state->drag_from_row = row;
    state->drag_from_col = col;
    state->dragged_item = state->grid[row][col].item;
    
    state->grid[row][col].item.item_id = ITEM_ID_EMPTY;
}

void EndDrag(GameState *state, int target_row, int target_col,
             Vector2 gridTopLeft, float slotSize) {
    if (!state->dragging) return;
    
    state->dragging = 0;
    
    if (target_row < 0 || target_row >= GRID_ROWS || 
        target_col < 0 || target_col >= GRID_COLS) {
        state->grid[state->drag_from_row][state->drag_from_col].item = state->dragged_item;
        return;
    }
    
    Slot *target = &state->grid[target_row][target_col];
    
    if (ItemIsEmpty(&target->item)) {
        target->item = state->dragged_item;
        return;
    }
    
    if (target->item.item_id == state->dragged_item.item_id) {
        ItemID next_id = GetNextItemInChain(target->item.item_id);
        if (next_id != ITEM_ID_EMPTY) {
            target->item.item_id = next_id;
            
            /* Spawn particles and play sound at merge position */
            Vector2 mergePos = (Vector2){
                target->bounds.x + target->bounds.width / 2,
                target->bounds.y + target->bounds.height / 2
            };
            const ItemDefinition *mergedDef = GetItemDefinition(next_id);
            Color mergeColor = mergedDef ? mergedDef->placeholder_color : (Color){255, 100, 200, 255};
            SpawnMergeBurst(mergePos, mergeColor);
            PlayMergeSound();
            
            const ItemDefinition *def = GetItemDefinition(next_id);
            if (def) {
                target->item.is_generator = def->is_generator;
            }
            return;
        }
    }
    
    state->grid[state->drag_from_row][state->drag_from_col].item = state->dragged_item;
}