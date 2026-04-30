#include "task.h"
#include "game_state.h"
#include "item.h"
#include "constants.h"

TaskOutcome ExecuteTask(Task *task, GameState *state) {
    /* Check if task is already completed */
    if (!task->is_available) {
        return TASK_OUTCOME_ALREADY_COMPLETED;
    }
    
    /* Scan the grid for the required item */
    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            Slot *slot = &state->grid[r][c];
            
            /* Found the required item */
            if (slot->item.item_id == task->required_item_id) {
                /* Replace with reward item */
                slot->item.item_id = task->reward_item_id;
                
                /* Update is_generator flag based on reward definition */
                const ItemDefinition *reward_def = GetItemDefinition(task->reward_item_id);
                if (reward_def) {
                    slot->item.is_generator = reward_def->is_generator;
                }
                
                /* Mark task as completed */
                task->is_available = 0;
                
                return TASK_OUTCOME_SUCCESS;
            }
        }
    }
    
    /* Required item not found on grid */
    return TASK_OUTCOME_ITEM_NOT_FOUND;
}

Task CreateSkeletonKeyExchangeTask(void) {
    return (Task){
        .required_item_id = ITEM_ID_SKELETON_KEY,
        .reward_item_id = ITEM_ID_GENERATOR_SEWING_KIT,
        .is_available = 1,
        .description = "Trade the Skeleton Key for the Sewing Kit Generator"
    };
}
