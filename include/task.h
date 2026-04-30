#ifndef TASK_H
#define TASK_H

#include "game_state.h"

/* Execute a task - scans grid for required item and performs exchange */
TaskOutcome ExecuteTask(Task *task, GameState *state);

/* Initialize the default task: Skeleton Key → Sewing Kit Generator */
Task CreateSkeletonKeyExchangeTask(void);

#endif
