#include "render.h"
#include "item.h"
#include "items.h"
#include "task.h"
#include "particles.h"

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
int IsRequiredItemOnGrid(Task *task, GameState *state) {
    if (!task) {
        return 0;
    }
    
    for (int r = 0; r < GRID_ROWS; r++) {
        for (int c = 0; c < GRID_COLS; c++) {
            if (state->grid[r][c].item.item_id == task->required_item_id) {
                return 1;
            }
        }
    }
    return 0;
}

/* Check if the complete task button was clicked (inside task panel) */
int CheckTaskButtonClick(GameState *state, Vector2 mousePos, int screenWidth, int screenHeight) {
    Task *activeTask = GetActiveTask(state);
    if (!activeTask || !state->task_panel_visible) {
        return 0;
    }
    
    if (!IsRequiredItemOnGrid(activeTask, state)) {
        return 0;
    }
    
    int panelX = screenWidth - TASK_PANEL_WIDTH;
    int btnY = screenHeight - 160;
    
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
    Task *activeTask = GetActiveTask(state);
    if (!activeTask) {
        return;
    }
    
    if (!state->task_panel_visible) {
        return;
    }
    
    int panelX = screenWidth - TASK_PANEL_WIDTH;
    int panelY = 60;
    int panelHeight = screenHeight - 180;
    
    DrawRectangle(panelX, panelY, TASK_PANEL_WIDTH, panelHeight, (Color){35, 35, 45, 255});
    DrawRectangleLines(panelX, panelY, TASK_PANEL_WIDTH, panelHeight, (Color){100, 100, 120, 255});
    
    const char *title = "TASK";
    int titleWidth = MeasureText(title, 16);
    DrawText(title, panelX + (TASK_PANEL_WIDTH - titleWidth) / 2, panelY + 10, 16, YELLOW);
    
    DrawLine(panelX + 10, panelY + 30, panelX + TASK_PANEL_WIDTH - 10, panelY + 30, (Color){100, 100, 120, 255});
    
    int itemY = panelY + 50;
    int centerX = panelX + TASK_PANEL_WIDTH / 2;
    int itemSlotSize = TASK_PANEL_ICON_SIZE + 8;
    
    int hasRequiredItem = IsRequiredItemOnGrid(activeTask, state);
    
    ItemID requiredID = activeTask->required_item_id;
    const ItemDefinition *requiredDef = GetItemDefinition(requiredID);
    Texture2D *requiredTex = GetItemTexture(state, requiredID);
    
    int requiredX = centerX - itemSlotSize / 2;
    int requiredY = itemY;
    
    DrawRectangle(requiredX, requiredY, itemSlotSize, itemSlotSize, (Color){50, 50, 65, 255});
    DrawRectangleLines(requiredX, requiredY, itemSlotSize, itemSlotSize, (Color){120, 120, 140, 255});
    
    if (requiredTex && requiredTex->id > 0) {
        DrawTextureEx(*requiredTex,
                     (Vector2){requiredX + 4, requiredY + 4},
                     0.0f,
                     (float)(itemSlotSize - 8) / requiredTex->width,
                     WHITE);
    } else if (requiredDef) {
        DrawRectangle(requiredX + 4, requiredY + 4, itemSlotSize - 8, itemSlotSize - 8, requiredDef->placeholder_color);
    }
    
    if (hasRequiredItem) {
        DrawRectangleLines(requiredX - 2, requiredY - 2, itemSlotSize + 4, itemSlotSize + 4, GREEN);
        DrawRectangleLines(requiredX - 4, requiredY - 4, itemSlotSize + 8, itemSlotSize + 8, (Color){0, 255, 0, 128});
        
        int checkX = requiredX + itemSlotSize - 12;
        int checkY = requiredY + 12;
        DrawText("✓", checkX, checkY, 18, GREEN);
    }
    
    const char *requiredLabel = requiredDef ? requiredDef->name : "Unknown";
    int labelWidth = MeasureText(requiredLabel, 10);
    DrawText(requiredLabel, centerX - labelWidth / 2, requiredY + itemSlotSize + 4, 10, LIGHTGRAY);
    
    int arrowY = itemY + itemSlotSize + 25;
    DrawText("↓", centerX - 5, arrowY, 18, hasRequiredItem ? GREEN : GRAY);
    
    ItemID rewardID = activeTask->reward_item_id;
    const ItemDefinition *rewardDef = GetItemDefinition(rewardID);
    Texture2D *rewardTex = GetItemTexture(state, rewardID);
    
    int rewardY = arrowY + 30;
    int rewardX = centerX - itemSlotSize / 2;
    
    DrawRectangle(rewardX, rewardY, itemSlotSize, itemSlotSize, (Color){50, 50, 65, 255});
    DrawRectangleLines(rewardX, rewardY, itemSlotSize, itemSlotSize, (Color){120, 120, 140, 255});
    
    if (rewardTex && rewardTex->id > 0) {
        DrawTextureEx(*rewardTex,
                     (Vector2){rewardX + 4, rewardY + 4},
                     0.0f,
                     (float)(itemSlotSize - 8) / rewardTex->width,
                     WHITE);
    } else if (rewardDef) {
        DrawRectangle(rewardX + 4, rewardY + 4, itemSlotSize - 8, itemSlotSize - 8, rewardDef->placeholder_color);
    }
    
    if (rewardDef && rewardDef->is_generator) {
        DrawText("⚡", rewardX + itemSlotSize - 14, rewardY + 4, 12, YELLOW);
    }
    
    const char *rewardLabel = rewardDef ? rewardDef->name : "Unknown";
    labelWidth = MeasureText(rewardLabel, 10);
    DrawText(rewardLabel, centerX - labelWidth / 2, rewardY + itemSlotSize + 4, 10, LIGHTGRAY);
    
    int statusY = rewardY + itemSlotSize + 25;
    if (hasRequiredItem) {
        DrawText("READY!", centerX - 25, statusY, 14, GREEN);
    } else {
        DrawText("Find the", centerX - 28, statusY, 12, GRAY);
    }
    
    int completeBtnY = screenHeight - 160;
    Rectangle completeBtnRect = (Rectangle){
        (float)(panelX + 10),
        (float)completeBtnY,
        (float)(TASK_PANEL_WIDTH - 20),
        (float)TASK_BTN_HEIGHT
    };
    
    Color btnColor;
    if (hasRequiredItem) {
        btnColor = (Color){ 34, 139, 34, 255 };
    } else {
        btnColor = (Color){ 80, 80, 80, 255 };
    }
    DrawRectangleRec(completeBtnRect, btnColor);
    DrawRectangleLinesEx(completeBtnRect, 2.0f, hasRequiredItem ? GREEN : DARKGRAY);
    
    const char *btnText = hasRequiredItem ? "COMPLETE!" : "Not Ready";
    int btnTextWidth = MeasureText(btnText, 12);
    DrawText(btnText, panelX + (TASK_PANEL_WIDTH - 20 - btnTextWidth) / 2, completeBtnY + 8, 12, WHITE);
}

/* Trash button dimensions */
#define TRASH_BTN_WIDTH 40
#define TRASH_BTN_HEIGHT 40

/* Check if the trash button was clicked */
int CheckTrashButtonClick(GameState *state, Vector2 mousePos, int screenWidth, int screenHeight) {
    if (state->selected_row < 0 || state->selected_col < 0) {
        return 0;
    }
    
    Slot *selected = &state->grid[state->selected_row][state->selected_col];
    if (ItemIsEmpty(&selected->item)) {
        return 0;
    }
    
    if (selected->item.is_generator) {
        return 0;
    }
    
    int panelX = screenWidth - TASK_PANEL_WIDTH;
    int trashBtnY = screenHeight - 120;
    
    Rectangle trashBtnRect = (Rectangle){
        (float)(panelX + (TASK_PANEL_WIDTH - TRASH_BTN_WIDTH) / 2),
        (float)trashBtnY,
        (float)TRASH_BTN_WIDTH,
        (float)TRASH_BTN_HEIGHT
    };
    
    if (mousePos.x >= trashBtnRect.x && mousePos.x <= trashBtnRect.x + TRASH_BTN_WIDTH &&
        mousePos.y >= trashBtnRect.y && mousePos.y <= trashBtnRect.y + TRASH_BTN_HEIGHT) {
        return 1;
    }
    return 0;
}

/* Delete the currently selected item (if not a generator) */
int DeleteSelectedItem(GameState *state) {
    if (state->selected_row < 0 || state->selected_col < 0) {
        return 0;
    }
    
    Slot *selected = &state->grid[state->selected_row][state->selected_col];
    if (ItemIsEmpty(&selected->item)) {
        return 0;
    }
    
    if (selected->item.is_generator) {
        return 0;
    }
    
    selected->item.item_id = ITEM_ID_EMPTY;
    selected->item.is_generator = 0;
    
    state->selected_row = -1;
    state->selected_col = -1;
    
    return 1;
}

/* Draw the trash button in the task panel */
void DrawTrashButton(GameState *state, int screenWidth, int screenHeight) {
    int panelX = screenWidth - TASK_PANEL_WIDTH;
    int trashBtnY = screenHeight - 120;
    
    Rectangle trashBtnRect = (Rectangle){
        (float)(panelX + (TASK_PANEL_WIDTH - TRASH_BTN_WIDTH) / 2),
        (float)trashBtnY,
        (float)TRASH_BTN_WIDTH,
        (float)TRASH_BTN_HEIGHT
    };
    
    int canDelete = 0;
    if (state->selected_row >= 0 && state->selected_col >= 0) {
        Slot *selected = &state->grid[state->selected_row][state->selected_col];
        if (!ItemIsEmpty(&selected->item) && !selected->item.is_generator) {
            canDelete = 1;
        }
    }
    
    Color btnColor = canDelete ? (Color){ 139, 34, 34, 255 } : (Color){ 80, 80, 80, 150 };
    DrawRectangleRec(trashBtnRect, btnColor);
    DrawRectangleLinesEx(trashBtnRect, 2.0f, canDelete ? RED : DARKGRAY);
    
    const char *trashIcon = "🗑";
    int iconWidth = MeasureText(trashIcon, 18);
    DrawText(trashIcon, 
             (int)(trashBtnRect.x + (TRASH_BTN_WIDTH - iconWidth) / 2),
             (int)(trashBtnRect.y + 8), 18, WHITE);
}

/* Mall shop button dimensions */
#define MALL_BTN_WIDTH 200
#define MALL_BTN_HEIGHT 60

/* Mall upgrade button dimensions */
#define UPGRADE_BTN_WIDTH 80
#define UPGRADE_BTN_HEIGHT 25

/* Cost for boutique upgrade */
#define BOUTIQUE_UPGRADE_COST 50

/* Draw the Mall screen - visual corridor with storefronts */
void DrawMallScreen(GameState *state, int screenWidth, int screenHeight) {
    ClearBackground((Color){30, 30, 45, 255});
    
    for (int y = 100; y < screenHeight; y += 40) {
        DrawLine(0, y, screenWidth, y, (Color){50, 50, 70, 100});
    }
    
    DrawRectangle(0, 0, screenWidth, 50, (Color){30, 30, 40, 255});
    DrawText(TextFormat("Energy: %d/%d", state->energy, state->max_energy),
             20, 15, 16, YELLOW);
    DrawText(TextFormat("Coins: %d", state->coins),
             screenWidth - 120, 15, 16, (Color){144, 238, 144, 255});
    
    int centerX = screenWidth / 2;
    int startY = 120;
    int spacing = 160;
    
    Rectangle janitorStore = (Rectangle){
        (float)(centerX - MALL_BTN_WIDTH / 2),
        (float)(startY),
        (float)MALL_BTN_WIDTH,
        (float)MALL_BTN_HEIGHT
    };
    
    DrawRectangleRec(janitorStore, (Color){60, 80, 100, 255});
    DrawRectangleLinesEx(janitorStore, 3.0f, (Color){100, 150, 200, 255});
    DrawCircle(janitorStore.x + janitorStore.width - 25, janitorStore.y + janitorStore.height / 2, 6, (Color){180, 200, 220, 255});
    
    const char *janitorLabel = "Janitor's Closet";
    int janLabelWidth = MeasureText(janitorLabel, 14);
    DrawText(janitorLabel, centerX - janLabelWidth / 2, startY + 20, 14, WHITE);
    
    Rectangle boutiqueStore = (Rectangle){
        (float)(centerX - MALL_BTN_WIDTH / 2),
        (float)(startY + spacing),
        (float)MALL_BTN_WIDTH,
        (float)MALL_BTN_HEIGHT
    };
    
    Color storeColor, borderColor;
    const char *storeLabel;
    
    if (state->boutiqueRestored) {
        storeColor = (Color){50, 180, 80, 255};
        borderColor = (Color){80, 255, 120, 255};
        storeLabel = "Boutique - Restored!";
    } else if (state->boutiqueUnlocked) {
        storeColor = (Color){100, 120, 200, 255};
        borderColor = (Color){150, 180, 255, 255};
        storeLabel = "Enter Boutique";
    } else {
        storeColor = (Color){50, 50, 60, 200};
        borderColor = (Color){80, 80, 100, 200};
        storeLabel = "Locked";
    }
    
    DrawRectangleRec(boutiqueStore, storeColor);
    DrawRectangleLinesEx(boutiqueStore, 3.0f, borderColor);
    
    if (state->boutiqueUnlocked) {
        DrawCircle(boutiqueStore.x + boutiqueStore.width - 25, boutiqueStore.y + boutiqueStore.height / 2, 6, (Color){180, 200, 220, 255});
    }
    
    int labelWidth = MeasureText(storeLabel, 14);
    DrawText(storeLabel, centerX - labelWidth / 2, startY + spacing + 20, 14, WHITE);
    
    if (!state->boutiqueUnlocked) {
        DrawText("🔒", centerX - 8, startY + spacing + 10, 16, (Color){150, 150, 180, 200});
        const char *lockedHint = "Requires: Skeleton Key";
        int hintWidth = MeasureText(lockedHint, 10);
        DrawText(lockedHint, centerX - hintWidth / 2, startY + spacing + 40, 10, GRAY);
    }
    
    if (state->boutiqueUpgraded) {
        DrawRectangle(boutiqueStore.x, boutiqueStore.y - 15, boutiqueStore.width, 10, (Color){255, 215, 0, 255});
        DrawText("★", boutiqueStore.x + 20, boutiqueStore.y - 18, 12, YELLOW);
        DrawText("★", boutiqueStore.x + 50, boutiqueStore.y - 18, 12, YELLOW);
        DrawText("★", boutiqueStore.x + 80, boutiqueStore.y - 18, 12, YELLOW);
        DrawText("★", boutiqueStore.x + 110, boutiqueStore.y - 18, 12, YELLOW);
        DrawText("GRAND OPENING!", centerX - 45, startY + spacing - 30, 12, (Color){255, 215, 0, 255});
    }
    
    if (state->boutiqueUnlocked && !state->boutiqueUpgraded) {
        Rectangle upgradeBtn = (Rectangle){
            (float)(centerX + MALL_BTN_WIDTH / 2 - UPGRADE_BTN_WIDTH - 10),
            (float)(startY + spacing + MALL_BTN_HEIGHT + 10),
            (float)UPGRADE_BTN_WIDTH,
            (float)UPGRADE_BTN_HEIGHT
        };
        
        Color upgradeColor = state->coins >= BOUTIQUE_UPGRADE_COST ? (Color){100, 180, 100, 255} : (Color){80, 80, 80, 200};
        DrawRectangleRec(upgradeBtn, upgradeColor);
        DrawRectangleLinesEx(upgradeBtn, 2.0f, state->coins >= BOUTIQUE_UPGRADE_COST ? YELLOW : DARKGRAY);
        
        const char *upgradeText = "Upgrade (50)";
        int upgradeTextWidth = MeasureText(upgradeText, 10);
        DrawText(upgradeText, 
                 (int)(upgradeBtn.x + (UPGRADE_BTN_WIDTH - upgradeTextWidth) / 2),
                 (int)(upgradeBtn.y + 7), 10, WHITE);
    }
    
    Rectangle arcadeStore = (Rectangle){
        (float)(centerX - MALL_BTN_WIDTH / 2),
        (float)(startY + spacing * 2),
        (float)MALL_BTN_WIDTH,
        (float)MALL_BTN_HEIGHT
    };
    
    if (state->arcadeRestored) {
        storeColor = (Color){100, 180, 255, 255};
        borderColor = (Color){80, 255, 255, 255};
        storeLabel = "Arcade - Restored!";
    } else if (state->arcadeUnlocked) {
        storeColor = (Color){80, 80, 180, 255};
        borderColor = (Color){150, 180, 255, 255};
        storeLabel = "Enter Arcade";
    } else {
        storeColor = (Color){50, 50, 60, 200};
        borderColor = (Color){80, 80, 100, 200};
        storeLabel = "Locked";
    }
    
    DrawRectangleRec(arcadeStore, storeColor);
    DrawRectangleLinesEx(arcadeStore, 3.0f, borderColor);
    
    if (state->arcadeUnlocked) {
        DrawCircle(arcadeStore.x + arcadeStore.width - 25, arcadeStore.y + arcadeStore.height / 2, 6, (Color){180, 200, 220, 255});
    }
    
    labelWidth = MeasureText(storeLabel, 14);
    DrawText(storeLabel, centerX - labelWidth / 2, startY + spacing * 2 + 20, 14, WHITE);
    
    if (!state->arcadeUnlocked) {
        DrawText("🔒", centerX - 8, startY + spacing * 2 + 10, 16, (Color){150, 150, 180, 200});
        const char *arcadeLockedHint = "Complete Boutique Tasks";
        int arcadeHintWidth = MeasureText(arcadeLockedHint, 10);
        DrawText(arcadeLockedHint, centerX - arcadeHintWidth / 2, startY + spacing * 2 + 40, 10, GRAY);
    }
    
    if (state->arcadeRestored) {
        DrawRectangle(arcadeStore.x, arcadeStore.y - 15, arcadeStore.width, 10, (Color){0, 255, 255, 200});
        DrawText("★", arcadeStore.x + 20, arcadeStore.y - 18, 12, (Color){255, 100, 255, 255});
        DrawText("★", arcadeStore.x + 50, arcadeStore.y - 18, 12, (Color){255, 100, 255, 255});
        DrawText("★", arcadeStore.x + 80, arcadeStore.y - 18, 12, (Color){255, 100, 255, 255});
        DrawText("★", arcadeStore.x + 110, arcadeStore.y - 18, 12, (Color){255, 100, 255, 255});
        DrawText("HIGH SCORE!", centerX - 40, startY + spacing * 2 - 30, 12, (Color){0, 255, 255, 255});
    }
    
    DrawText("MALL VIEW", centerX - MeasureText("MALL VIEW", 24) / 2, 65, 24, YELLOW);
}

/* Check if a shop button was clicked in Mall view */
int CheckMallShopClick(GameState *state, Vector2 mousePos, int screenWidth) {
    int centerX = screenWidth / 2;
    int startY = 120;
    int spacing = 160;
    
    Rectangle boutiqueBtn = (Rectangle){
        (float)(centerX - MALL_BTN_WIDTH / 2),
        (float)(startY + spacing),
        (float)MALL_BTN_WIDTH,
        (float)MALL_BTN_HEIGHT
    };
    
    if (mousePos.x >= boutiqueBtn.x && mousePos.x <= boutiqueBtn.x + MALL_BTN_WIDTH &&
        mousePos.y >= boutiqueBtn.y && mousePos.y <= boutiqueBtn.y + MALL_BTN_HEIGHT) {
        if (state->boutiqueUnlocked) {
            return 1;
        }
        return 0;
    }
    
    Rectangle arcadeBtn = (Rectangle){
        (float)(centerX - MALL_BTN_WIDTH / 2),
        (float)(startY + spacing * 2),
        (float)MALL_BTN_WIDTH,
        (float)MALL_BTN_HEIGHT
    };
    
    if (mousePos.x >= arcadeBtn.x && mousePos.x <= arcadeBtn.x + MALL_BTN_WIDTH &&
        mousePos.y >= arcadeBtn.y && mousePos.y <= arcadeBtn.y + MALL_BTN_HEIGHT) {
        if (state->arcadeUnlocked) {
            return 3;
        }
        return 0;
    }
    
    Rectangle janitorBtn = (Rectangle){
        (float)(centerX - MALL_BTN_WIDTH / 2),
        (float)(startY),
        (float)MALL_BTN_WIDTH,
        (float)MALL_BTN_HEIGHT
    };
    
    if (mousePos.x >= janitorBtn.x && mousePos.x <= janitorBtn.x + MALL_BTN_WIDTH &&
        mousePos.y >= janitorBtn.y && mousePos.y <= janitorBtn.y + MALL_BTN_HEIGHT) {
        return 2;
    }
    
    return 0;
}

/* Check if the boutique upgrade button was clicked */
int CheckMallUpgradeClick(GameState *state, Vector2 mousePos, int screenWidth) {
    if (!state->boutiqueUnlocked || state->boutiqueUpgraded) {
        return 0;
    }
    
    int centerX = screenWidth / 2;
    int startY = 120;
    int spacing = 160;
    
    Rectangle upgradeBtn = (Rectangle){
        (float)(centerX + MALL_BTN_WIDTH / 2 - UPGRADE_BTN_WIDTH - 10),
        (float)(startY + spacing + MALL_BTN_HEIGHT + 10),
        (float)UPGRADE_BTN_WIDTH,
        (float)UPGRADE_BTN_HEIGHT
    };
    
    if (mousePos.x >= upgradeBtn.x && mousePos.x <= upgradeBtn.x + UPGRADE_BTN_WIDTH &&
        mousePos.y >= upgradeBtn.y && mousePos.y <= upgradeBtn.y + UPGRADE_BTN_HEIGHT) {
        if (state->coins >= BOUTIQUE_UPGRADE_COST) {
            return 1;
        }
    }
    return 0;
}

/* Purchase boutique upgrade (costs 50 coins) */
int PurchaseBoutiqueUpgrade(GameState *state) {
    if (state->boutiqueUpgraded || !state->boutiqueUnlocked) {
        return 0;
    }
    
    if (state->coins < BOUTIQUE_UPGRADE_COST) {
        return 0;
    }
    
    state->coins -= BOUTIQUE_UPGRADE_COST;
    state->boutiqueUpgraded = 1;
    
    return 1;
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
    
    Color btnColor = isVisible ? (Color){ 100, 100, 60, 255 } : (Color){ 60, 60, 100, 255 };
    DrawRectangleRec(btnRect, btnColor);
    DrawRectangleLinesEx(btnRect, 2.0f, YELLOW);
    
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
    
    if (state->current_screen == SCREEN_MALL) {
        DrawMallScreen(state, screenWidth, screenHeight);
    } else {
        ClearBackground((Color){40, 40, 50, 255});
        
        DrawRectangle(0, 0, screenWidth, 50, (Color){30, 30, 40, 255});
        DrawText(TextFormat("Energy: %d/%d", state->energy, state->max_energy),
                 20, 15, 16, YELLOW);
        DrawText(TextFormat("Coins: %d", state->coins),
                 screenWidth - 120, 15, 16, (Color){144, 238, 144, 255});
        
        for (int r = 0; r < GRID_ROWS; r++) {
            for (int c = 0; c < GRID_COLS; c++) {
                Slot *slot = &state->grid[r][c];
                
                if (r == state->selected_row && c == state->selected_col) {
                    DrawRectangleRec(slot->bounds, (Color){80, 120, 180, 255});
                } else {
                    DrawRectangleRec(slot->bounds, (Color){60, 60, 70, 255});
                }
                
                if (r == state->selected_row && c == state->selected_col) {
                    DrawRectangleLinesEx(slot->bounds, 3.0f, YELLOW);
                } else {
                    DrawRectangleLinesEx(slot->bounds, 2.0f, (Color){100, 100, 120, 255});
                }
                
                if (!ItemIsEmpty(&slot->item)) {
                    const ItemDefinition *def = GetItemDefinition(slot->item.item_id);
                    Texture2D *item_tex = GetItemTexture(state, slot->item.item_id);
                    
                    if (item_tex && item_tex->id > 0) {
                        DrawTextureEx(*item_tex,
                                     (Vector2){slot->bounds.x + 2, slot->bounds.y + 2},
                                     0.0f,
                                     (slot->bounds.width - 4) / item_tex->width,
                                     WHITE);
                    } else if (def) {
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
                    
                    int level = def ? def->level : 0;
                    if (level > 0) {
                        DrawText(TextFormat("L%d", level),
                                 (int)(slot->bounds.x + 5),
                                 (int)(slot->bounds.y + 5),
                                 14, WHITE);
                    }
                    
                    if (def && def->is_generator) {
                        DrawText("⚡",
                                 (int)(slot->bounds.x + slot->bounds.width - 20),
                                 (int)(slot->bounds.y + 5),
                                 16, YELLOW);
                    }
                }
            }
        }
        
        if (state->dragging) {
            Vector2 mousePos = GetMousePosition();
            float dragItemSize = slotSize - 4.0f;
            const ItemDefinition *def = GetItemDefinition(state->dragged_item.item_id);
            Texture2D *drag_tex = GetItemTexture(state, state->dragged_item.item_id);
            
            if (drag_tex && drag_tex->id > 0) {
                DrawTextureEx(*drag_tex,
                             (Vector2){mousePos.x - dragItemSize / 2, mousePos.y - dragItemSize / 2},
                             0.0f,
                             dragItemSize / drag_tex->width,
                             WHITE);
            } else {
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
        
        Vector2 mousePos = GetMousePosition();
        DrawTaskPanel(state, mousePos, screenWidth, screenHeight);
        DrawTrashButton(state, screenWidth, screenHeight);
        
        Task *activeTask = GetActiveTask(state);
        if (activeTask) {
            DrawToggleButton(screenWidth, screenHeight, state->task_panel_visible);
        }
        
        if (!activeTask) {
            const char *completedText = "All Tasks Complete!";
            int textWidth = MeasureText(completedText, 16);
            DrawText(completedText, (screenWidth - textWidth) / 2, screenHeight - 110, 16, GREEN);
        }
        
        DrawText("Tap generator (⚡) or drag to merge!", 20, screenHeight - 80, 12, LIGHTGRAY);
        
        int backBtnX = screenWidth - 150;
        int backBtnY = 10;
        Rectangle backBtnRect = (Rectangle){(float)backBtnX, (float)backBtnY, 120, 30};
        DrawRectangleRec(backBtnRect, (Color){100, 100, 150, 255});
        DrawRectangleLinesEx(backBtnRect, 2.0f, YELLOW);
        DrawText("Back to Mall", backBtnX + 15, backBtnY + 8, 12, WHITE);
        
        DrawParticles();
    }
    EndDrawing();
}

void RenderCleanup(GameState *state) {
    GameStateCleanup(state);
}