#include "render.h"
#include "item.h"

float GetSlotSize(int screenWidth) {
    float margin = 20.0f;
    float available_width = screenWidth - (2 * margin);
    return available_width / GRID_COLS;
}

Vector2 GetGridTopLeft(int screenWidth, float slotSize) {
    return (Vector2){
        (screenWidth - (GRID_COLS * slotSize)) / 2.0f,
        60.0f
    };
}

void RenderGame(GameState *state, Vector2 gridTopLeft, float slotSize,
                int screenWidth, int screenHeight) {
    BeginDrawing();
        ClearBackground((Color){40, 40, 50, 255});
        
        /* Draw UI bar at top */
        DrawRectangle(0, 0, screenWidth, 50, (Color){30, 30, 40, 255});
        DrawText(TextFormat("Energy: %d/%d", state->energy, state->max_energy),
                 20, 15, 16, YELLOW);
        DrawText(TextFormat("Coins: %d", state->coins),
                 screenWidth - 120, 15, 16, (Color){144, 238, 144, 255});
        
        /* Draw grid slots */
        for (int r = 0; r < GRID_ROWS; r++) {
            for (int c = 0; c < GRID_COLS; c++) {
                Slot *slot = &state->grid[r][c];
                
                /* Draw slot background */
                DrawRectangleRec(slot->bounds, (Color){60, 60, 70, 255});
                DrawRectangleLinesEx(slot->bounds, 2.0f, (Color){100, 100, 120, 255});
                
                /* Draw item if present and not being dragged */
                if (!ItemIsEmpty(&slot->item)) {
                    /* Draw item based on type */
                    if (slot->item.is_generator && state->generator_texture.id > 0) {
                        /* Draw generator texture */
                        DrawTextureEx(state->generator_texture,
                                     (Vector2){slot->bounds.x + 2, slot->bounds.y + 2},
                                     0.0f,
                                     (slot->bounds.width - 4) / state->generator_texture.width,
                                     WHITE);
                    } else {
                        /* Draw regular item as rectangle */
                        Color itemColor = slot->item.is_generator ? 
                            (Color){200, 180, 100, 255} : (Color){200, 150, 100, 255};
                        DrawRectangleRec(
                            (Rectangle){
                                slot->bounds.x + 2,
                                slot->bounds.y + 2,
                                slot->bounds.width - 4,
                                slot->bounds.height - 4
                            },
                            itemColor
                        );
                    }
                    
                    /* Draw level number */
                    DrawText(TextFormat("L%d", slot->item.level),
                             (int)(slot->bounds.x + 5),
                             (int)(slot->bounds.y + 5),
                             14, WHITE);
                    
                    /* Draw lightning icon for generators */
                    if (slot->item.is_generator) {
                        DrawText("⚡",
                                 (int)(slot->bounds.x + slot->bounds.width - 20),
                                 (int)(slot->bounds.y + 5),
                                 16, YELLOW);
                    }
                }
            }
        }
        
        /* Draw dragged item at cursor if dragging */
        if (state->dragging) {
            Vector2 mousePos = GetMousePosition();
            float dragItemSize = slotSize - 4.0f;
            DrawRectangle(
                (int)(mousePos.x - dragItemSize / 2),
                (int)(mousePos.y - dragItemSize / 2),
                (int)dragItemSize,
                (int)dragItemSize,
                (Color){255, 200, 100, 200}
            );
            DrawText(TextFormat("L%d", state->dragged_item.level),
                     (int)(mousePos.x - 10),
                     (int)(mousePos.y - 10),
                     14, WHITE);
        }
        
        /* Instructional text */
        DrawText("Tap generator (⚡) or drag to merge!", 20, screenHeight - 80, 12, LIGHTGRAY);
    EndDrawing();
}

void RenderCleanup(GameState *state) {
    if (state->generator_texture.id > 0) {
        UnloadTexture(state->generator_texture);
    }
}
