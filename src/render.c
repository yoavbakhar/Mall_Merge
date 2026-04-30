#include "render.h"
#include "item.h"
#include "items.h"
#include "task.h"

/* Task button dimensions */
#define TASK_BTN_WIDTH 200
#define TASK_BTN_HEIGHT 40

/* Task panel dimensions */
#define TASK_PANEL_WIDTH 140
#define TASK_PANEL_ICON_SIZE 50
#define TASK_PANEL_PADDING 10

/* Toggle button dimensions */
#define TOGGLE_BTN_WIDTH 160
#define TOGGLE_BTN_HEIGHT 30

/* Check if the required item is present on the grid */
int IsRequiredItemOnGrid(GameState *state) {
    if (!state->skeleton_key_task.is_available) {
        return 0;  /* Task already completed */
    }
    
    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            if (state->grid[r][c].item.item_id == state->skeleton_key_task.required_item_id) {
                return 1;  /* Found the required item */
            }
        }
    }
    return 0;  /* Not found */
}

/* Check if the complete task button was clicked (inside task panel) */
int CheckTaskButtonClick(GameState *state, Vector2 mousePos, int screenWidth, int screenHeight) {
    if (!state->skeleton_key_task.is_available || !state->task_panel_visible) {
        return 0;
    }
    
    /* Only allow click if required item is on grid */
    if (!IsRequiredItemOnGrid(state)) {
        return 0;
    }
    
    int panelX = screenWidth - TASK_PANEL_WIDTH;
    int btnY = screenHeight - 160;  /* Position inside panel */
    
    if (mousePos.x >= panelX + 10 && mousePos.x <= panelX + TASK_PANEL_WIDTH - 10 &&
        mousePos.y >= btnY && mousePos.y <= btnY + TASK_BTN_HEIGHT) {
        return 1;
    }
    return 0;
}

/* Check if the toggle button was clicked */
int CheckToggleButtonClick(Vector2 mousePos, int screenWidth, int screenHeight) {
    int btnX = (screenWidth - TOGGLE_BTN_WIDTH) / 2;
    int btnY = screenHeight - 40;
    
    if (mousePos.x >= btnX && mousePos.x <= btnX + TOGGLE_BTN_WIDTH &&
        mousePos.y >= btnY && mousePos.y <= btnY + TOGGLE_BTN_HEIGHT) {
        return 1;
    }
    return 0;
}

/* Draw the task panel on the right side of the screen */
void DrawTaskPanel(GameState *state, Vector2 mousePos, int screenWidth, int screenHeight) {
    /* Only draw if task is available */
    if (!state->skeleton_key_task.is_available) {
        return;
    }
    
    /* Don't draw panel if collapsed */
    if (!state->task_panel_visible) {
        return;
    }
    
    int panelX = screenWidth - TASK_PANEL_WIDTH;
    int panelY = 60;  /* Below the top UI bar */
    int panelHeight = screenHeight - 180;  /* Leave space for bottom toggle button */
    
    /* Draw panel background */
    DrawRectangle(panelX, panelY, TASK_PANEL_WIDTH, panelHeight, (Color){35, 35, 45, 255});
    DrawRectangleLines(panelX, panelY, TASK_PANEL_WIDTH, panelHeight, (Color){100, 100, 120, 255});
    
    /* Draw title */
    const char *title = "TASK";
    int titleWidth = MeasureText(title, 16);
    DrawText(title, panelX + (TASK_PANEL_WIDTH - titleWidth) / 2, panelY + 10, 16, YELLOW);
    
    /* Draw separator line */
    DrawLine(panelX + 10, panelY + 30, panelX + TASK_PANEL_WIDTH - 10, panelY + 30, (Color){100, 100, 120, 255});
    
    /* Calculate item positions */
    int itemY = panelY + 50;
    int centerX = panelX + TASK_PANEL_WIDTH / 2;
    int itemSlotSize = TASK_PANEL_ICON_SIZE + 8;
    
    /* Check if required item is on grid */
    int hasRequiredItem = IsRequiredItemOnGrid(state);
    
    /* Draw required item (Skeleton Key) */
    ItemID requiredID = state->skeleton_key_task.required_item_id;
    const ItemDefinition *requiredDef = GetItemDefinition(requiredID);
    Texture2D *requiredTex = GetItemTexture(state, requiredID);
    
    int requiredX = centerX - itemSlotSize / 2;
    int requiredY = itemY;
    
    /* Draw item slot background */
    DrawRectangle(requiredX, requiredY, itemSlotSize, itemSlotSize, (Color){50, 50, 65, 255});
    DrawRectangleLines(requiredX, requiredY, itemSlotSize, itemSlotSize, (Color){120, 120, 140, 255});
    
    /* Draw item texture or placeholder */
    if (requiredTex && requiredTex->id > 0) {
        DrawTextureEx(*requiredTex,
                     (Vector2){requiredX + 4, requiredY + 4},
                     0.0f,
                     (float)(itemSlotSize - 8) / requiredTex->width,
                     WHITE);
    } else if (requiredDef) {
        DrawRectangle(requiredX + 4, requiredY + 4, itemSlotSize - 8, itemSlotSize - 8, requiredDef->placeholder_color);
    }
    
    /* Draw green glow if item is on grid */
    if (hasRequiredItem) {
        /* Green glow effect */
        DrawRectangleLines(requiredX - 2, requiredY - 2, itemSlotSize + 4, itemSlotSize + 4, GREEN);
        DrawRectangleLines(requiredX - 4, requiredY - 4, itemSlotSize + 8, itemSlotSize + 8, (Color){0, 255, 0, 128});
        
        /* Draw checkmark */
        int checkX = requiredX + itemSlotSize - 12;
        int checkY = requiredY + 12;
        DrawText("✓", checkX, checkY, 18, GREEN);
    }
    
    /* Draw item label */
    const char *requiredLabel = requiredDef ? requiredDef->name : "Unknown";
    int labelWidth = MeasureText(requiredLabel, 10);
    DrawText(requiredLabel, centerX - labelWidth / 2, requiredY + itemSlotSize + 4, 10, LIGHTGRAY);
    
    /* Draw arrow pointing down */
    int arrowY = itemY + itemSlotSize + 25;
    DrawText("↓", centerX - 5, arrowY, 18, hasRequiredItem ? GREEN : GRAY);
    
    /* Draw reward item (Sewing Kit Generator) */
    ItemID rewardID = state->skeleton_key_task.reward_item_id;
    const ItemDefinition *rewardDef = GetItemDefinition(rewardID);
    Texture2D *rewardTex = GetItemTexture(state, rewardID);
    
    int rewardY = arrowY + 30;
    int rewardX = centerX - itemSlotSize / 2;
    
    /* Draw item slot background */
    DrawRectangle(rewardX, rewardY, itemSlotSize, itemSlotSize, (Color){50, 50, 65, 255});
    DrawRectangleLines(rewardX, rewardY, itemSlotSize, itemSlotSize, (Color){120, 120, 140, 255});
    
    /* Draw item texture or placeholder */
    if (rewardTex && rewardTex->id > 0) {
        DrawTextureEx(*rewardTex,
                     (Vector2){rewardX + 4, rewardY + 4},
                     0.0f,
                     (float)(itemSlotSize - 8) / rewardTex->width,
                     WHITE);
    } else if (rewardDef) {
        DrawRectangle(rewardX + 4, rewardY + 4, itemSlotSize - 8, itemSlotSize - 8, rewardDef->placeholder_color);
    }
    
    /* Draw generator icon */
    if (rewardDef && rewardDef->is_generator) {
        DrawText("⚡", rewardX + itemSlotSize - 14, rewardY + 4, 12, YELLOW);
    }
    
    /* Draw item label */
    const char *rewardLabel = rewardDef ? rewardDef->name : "Unknown";
    labelWidth = MeasureText(rewardLabel, 10);
    DrawText(rewardLabel, centerX - labelWidth / 2, rewardY + itemSlotSize + 4, 10, LIGHTGRAY);
    
    /* Draw status text */
    int statusY = rewardY + itemSlotSize + 25;
    if (hasRequiredItem) {
        DrawText("READY!", centerX - 25, statusY, 14, GREEN);
    } else {
        DrawText("Find the", centerX - 28, statusY, 12, GRAY);
        DrawText("Skeleton Key", centerX - 38, statusY + 16, 12, GRAY);
        DrawText("to complete", centerX - 40, statusY + 34, 12, GRAY);
    }
    
    /* Draw Complete button inside the panel */
    int completeBtnY = screenHeight - 160;
    Rectangle completeBtnRect = (Rectangle){
        (float)(panelX + 10),
        (float)completeBtnY,
        (float)(TASK_PANEL_WIDTH - 20),
        (float)TASK_BTN_HEIGHT
    };
    
    /* Draw button background - green if ready, gray if not */
    Color btnColor;
    if (hasRequiredItem) {
        btnColor = (Color){ 34, 139, 34, 255 };  /* Forest green when ready */
    } else {
        btnColor = (Color){ 80, 80, 80, 255 };  /* Gray when not ready */
    }
    DrawRectangleRec(completeBtnRect, btnColor);
    DrawRectangleLinesEx(completeBtnRect, 2.0f, hasRequiredItem ? GREEN : DARKGRAY);
    
    /* Draw button text */
    const char *btnText = hasRequiredItem ? "COMPLETE!" : "Not Ready";
    int btnTextWidth = MeasureText(btnText, 12);
    DrawText(btnText, panelX + (TASK_PANEL_WIDTH - 20 - btnTextWidth) / 2, completeBtnY + 8, 12, WHITE);
}

/* Draw the toggle button at the bottom of the screen */
void DrawToggleButton(int screenWidth, int screenHeight, int isVisible) {
    int btnX = (screenWidth - TOGGLE_BTN_WIDTH) / 2;
    int btnY = screenHeight - 40;
    
    Rectangle btnRect = (Rectangle){
        (float)btnX,
        (float)btnY,
        (float)TOGGLE_BTN_WIDTH,
        (float)TOGGLE_BTN_HEIGHT
    };
    
    /* Draw button background */
    Color btnColor = isVisible ? (Color){ 100, 100, 60, 255 } : (Color){ 60, 60, 100, 255 };
    DrawRectangleRec(btnRect, btnColor);
    DrawRectangleLinesEx(btnRect, 2.0f, YELLOW);
    
    /* Draw button text */
    const char *btnText = isVisible ? "Hide Task Panel" : "Show Task Panel";
    int btnTextWidth = MeasureText(btnText, 12);
    DrawText(btnText, btnX + (TOGGLE_BTN_WIDTH - btnTextWidth) / 2, btnY + 8, 12, WHITE);
}

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
                    const ItemDefinition *def = GetItemDefinition(slot->item.item_id);
                    Texture2D *item_tex = GetItemTexture(state, slot->item.item_id);
                    
                    /* Draw item texture if available, otherwise fallback to colored rectangle */
                    if (item_tex && item_tex->id > 0) {
                        /* Draw item texture */
                        DrawTextureEx(*item_tex,
                                     (Vector2){slot->bounds.x + 2, slot->bounds.y + 2},
                                     0.0f,
                                     (slot->bounds.width - 4) / item_tex->width,
                                     WHITE);
                    } else if (def) {
                        /* Fallback to placeholder color */
                        Color itemColor = def->placeholder_color;
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
                    int level = def ? def->level : 0;
                    if (level > 0) {
                        DrawText(TextFormat("L%d", level),
                                 (int)(slot->bounds.x + 5),
                                 (int)(slot->bounds.y + 5),
                                 14, WHITE);
                    }
                    
                    /* Draw lightning icon for generators */
                    if (def && def->is_generator) {
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
            const ItemDefinition *def = GetItemDefinition(state->dragged_item.item_id);
            Texture2D *drag_tex = GetItemTexture(state, state->dragged_item.item_id);
            
            if (drag_tex && drag_tex->id > 0) {
                /* Draw dragged texture */
                DrawTextureEx(*drag_tex,
                             (Vector2){mousePos.x - dragItemSize / 2, mousePos.y - dragItemSize / 2},
                             0.0f,
                             dragItemSize / drag_tex->width,
                             WHITE);
            } else {
                /* Fallback to placeholder color */
                Color dragColor = def ? def->placeholder_color : (Color){255, 200, 100, 200};
                DrawRectangle(
                    (int)(mousePos.x - dragItemSize / 2),
                    (int)(mousePos.y - dragItemSize / 2),
                    (int)dragItemSize,
                    (int)dragItemSize,
                    dragColor
                );
            }
        }
        
        /* Draw task panel on the right side */
        Vector2 mousePos = GetMousePosition();
        DrawTaskPanel(state, mousePos, screenWidth, screenHeight);
        
        /* Draw toggle button at the bottom */
        if (state->skeleton_key_task.is_available) {
            DrawToggleButton(screenWidth, screenHeight, state->task_panel_visible);
        }
        
        /* Show completion message if task is done */
        if (!state->skeleton_key_task.is_available) {
            const char *completedText = "Task Completed!";
            int textWidth = MeasureText(completedText, 16);
            DrawText(completedText, (screenWidth - textWidth) / 2, screenHeight - 110, 16, GREEN);
        }
        
        /* Instructional text */
        DrawText("Tap generator (⚡) or drag to merge!", 20, screenHeight - 80, 12, LIGHTGRAY);
    EndDrawing();
}

void RenderCleanup(GameState *state) {
    GameStateCleanup(state);
}
