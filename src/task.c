#include "task.h"
#include "game_state.h"
#include "item.h"
#include "constants.h"

TaskOutcome ExecuteTask(Task *task, GameState *state) {
    if (!task->is_available) {
        return TASK_OUTCOME_ALREADY_COMPLETED;
    }
    
    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            Slot *slot = &state->grid[r][c];
            
            if (slot->item.item_id == task->required_item_id) {
                slot->item.item_id = task->reward_item_id;
                
                const ItemDefinition *reward_def = GetItemDefinition(task->reward_item_id);
                if (reward_def) {
                    slot->item.is_generator = reward_def->is_generator;
                }
                
                task->is_available = 0;
                
                state->coins += task->coinReward;
                
                if (task->required_item_id == ITEM_ID_SKELETON_KEY) {
                    state->boutiqueUnlocked = 1;
                    state->mannequin_task.is_available = 1;
                } else if (task->required_item_id == ITEM_ID_DESIGNERS_MANNEQUIN) {
                    state->boutiqueRestored = 1;
                    state->arcadeUnlocked = 1;
                    state->arcade_task.is_available = 1;
                } else if (task->required_item_id == ITEM_ID_HIGH_SCORE_TROPHY) {
                    state->arcadeRestored = 1;
                }
                
                return TASK_OUTCOME_SUCCESS;
            }
        }
    }
    
    return TASK_OUTCOME_ITEM_NOT_FOUND;
}

Task CreateSkeletonKeyExchangeTask(void) {
    return (Task){
        .required_item_id = ITEM_ID_SKELETON_KEY,
        .reward_item_id = ITEM_ID_GENERATOR_SEWING_KIT,
        .is_available = 1,
        .description = "Trade the Skeleton Key for the Sewing Kit Generator",
        .coinReward = 100
    };
}

Task* GetActiveTask(GameState *state) {
    if (state->skeleton_key_task.is_available) {
        return &state->skeleton_key_task;
    }
    if (state->mannequin_task.is_available) {
        return &state->mannequin_task;
    }
    if (state->arcade_task.is_available) {
        return &state->arcade_task;
    }
    return NULL;
}