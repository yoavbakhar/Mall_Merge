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

/* Draw the task panel on the right side of the screen */
void DrawTaskPanel(GameState *state, Vector2 mousePos, int screenWidth, int screenHeight);

/* Draw the toggle button at the bottom of the screen */
void DrawToggleButton(int screenWidth, int screenHeight, int isVisible);

/* Check if the required item is present on the grid */
int IsRequiredItemOnGrid(GameState *state);

/* Check if the complete task button was clicked (inside task panel) */
int CheckTaskButtonClick(GameState *state, Vector2 mousePos, int screenWidth, int screenHeight);

/* Check if the toggle button was clicked */
int CheckToggleButtonClick(Vector2 mousePos, int screenWidth, int screenHeight);

#endif
