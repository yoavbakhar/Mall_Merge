#ifndef DRAG_H
#define DRAG_H

#include "game_state.h"
#include "raylib.h"

/* Start dragging an item from a grid slot */
void StartDrag(GameState *state, int row, int col);

/* End drag and handle drop logic */
void EndDrag(GameState *state, int target_row, int target_col, 
             Vector2 gridTopLeft, float slotSize);

#endif
