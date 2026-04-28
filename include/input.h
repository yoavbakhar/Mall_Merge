#ifndef INPUT_H
#define INPUT_H

#include "game_state.h"
#include "raylib.h"

/* Process mouse input for drag/tap operations */
void ProcessInput(GameState *state, Vector2 gridTopLeft, float slotSize, 
                  int screenWidth, int screenHeight);

/* Get grid slot from screen position; returns -1, -1 if none */
void GetSlotAtPosition(Vector2 pos, Vector2 gridTopLeft, float slotSize,
                       int *out_row, int *out_col);

#endif
