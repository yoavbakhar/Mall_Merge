#ifndef RENDER_H
#define RENDER_H

#include "game_state.h"
#include "raylib.h"

/* Render the entire game frame */
void RenderGame(GameState *state, Vector2 gridTopLeft, float slotSize,
                int screenWidth, int screenHeight);

/* Get grid slot size from screen width */
float GetSlotSize(int screenWidth);

/* Get grid top-left position */
Vector2 GetGridTopLeft(int screenWidth, float slotSize);

/* Cleanup and unload textures */
void RenderCleanup(GameState *state);

#endif
