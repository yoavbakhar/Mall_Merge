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

/* Trash button dimensions */
#define TRASH_BTN_WIDTH 40
#define TRASH_BTN_HEIGHT 40

/* Check if the trash button was clicked */
int CheckTrashButtonClick(GameState *state, Vector2 mousePos, int screenWidth, int screenHeight) {
    if (state->selected_row < 0 || state->selected_col < 0) {
        return 0;  /* No selection */
    }
    
    /* Get the selected item */
    Slot *selected = &state->grid[state->selected_row][state->selected_col];
    if (ItemIsEmpty(&selected->item)) {
        return 0;  /* Empty slot selected */
    }
    
    /* Don't allow deleting generators */
    if (selected->item.is_generator) {
        return 0;  /* Generators cannot be deleted */
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
        return 0;  /* No selection */
    }
    
    Slot *selected = &state->grid[state->selected_row][state->selected_col];
    if (ItemIsEmpty(&selected->item)) {
        return 0;  /* Empty slot selected */
    }
    
    /* Don't allow deleting generators */
    if (selected->item.is_generator) {
        return 0;  /* Generators cannot be deleted */
    }
    
    /* Clear the slot */
    selected->item.item_id = ITEM_ID_EMPTY;
    selected->item.is_generator = 0;
    
    /* Clear selection */
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
    
    /* Check if there's a valid deletable item selected */
    int canDelete = 0;
    if (state->selected_row >= 0 && state->selected_col >= 0) {
        Slot *selected = &state->grid[state->selected_row][state->selected_col];
        if (!ItemIsEmpty(&selected->item) && !selected->item.is_generator) {
            canDelete = 1;
        }
    }
    
    /* Draw button background - red if can delete, gray if not */
    Color btnColor = canDelete ? (Color){ 139, 34, 34, 255 } : (Color){ 80, 80, 80, 150 };
    DrawRectangleRec(trashBtnRect, btnColor);
    DrawRectangleLinesEx(trashBtnRect, 2.0f, canDelete ? RED : DARKGRAY);
    
    /* Draw trash icon */
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
    /* Draw background - liminal mall corridor vibe */
    ClearBackground((Color){30, 30, 45, 255});
    
    /* Draw corridor floor lines */
    for (int y = 100; y < screenHeight; y += 40) {
        DrawLine(0, y, screenWidth, y, (Color){50, 50, 70, 100});
    }
    
    /* Draw UI bar at top */
    DrawRectangle(0, 0, screenWidth, 50, (Color){30, 30, 40, 255});
    DrawText(TextFormat("Energy: %d/%d", state->energy, state->max_energy),
             20, 15, 16, YELLOW);
    DrawText(TextFormat("Coins: %d", state->coins),
             screenWidth - 120, 15, 16, (Color){144, 238, 144, 255});
    
    int centerX = screenWidth / 2;
    int startY = 120;
    int spacing = 120;
    
    /* Draw Janitor's Closet storefront - baseline accessible room */
    Rectangle janitorStore = (Rectangle){
        (float)(centerX - MALL_BTN_WIDTH / 2),
        (float)(startY),
        (float)MALL_BTN_WIDTH,
        (float)MALL_BTN_HEIGHT
    };
    
    /* Draw storefront panel with door-like appearance */
    DrawRectangleRec(janitorStore, (Color){60, 80, 100, 255});
    DrawRectangleLinesEx(janitorStore, 3.0f, (Color){100, 150, 200, 255});
    /* Door handle */
    DrawCircle(janitorStore.x + janitorStore.width - 25, janitorStore.y + janitorStore.height / 2, 6, (Color){180, 200, 220, 255});
    /* Store label */
    const char *janitorLabel = "Janitor's Closet";
    int janLabelWidth = MeasureText(janitorLabel, 14);
    DrawText(janitorLabel, centerX - janLabelWidth / 2, startY + 20, 14, WHITE);
    
    /* Draw Boutique storefront - visual states based on progression */
    Rectangle boutiqueStore = (Rectangle){
        (float)(centerX - MALL_BTN_WIDTH / 2),
        (float)(startY + spacing),
        (float)MALL_BTN_WIDTH,
        (float)MALL_BTN_HEIGHT
    };
    
    Color storeColor, borderColor;
    const char *storeLabel;
    
    if (state->boutiqueRestored) {
        /* Fully Restored - vibrant with Grand Opening decorations */
        storeColor = (Color){50, 180, 80, 255};
        borderColor = (Color){80, 255, 120, 255};
        storeLabel = "Boutique - Restored!";
    } else if (state->boutiqueUnlocked) {
        /* Unlocked - can enter but not fully restored */
        storeColor = (Color){100, 120, 200, 255};
        borderColor = (Color){150, 180, 255, 255};
        storeLabel = "Enter Boutique";
    } else {
        /* Locked - dark with lock icon */
        storeColor = (Color){50, 50, 60, 200};
        borderColor = (Color){80, 80, 100, 200};
        storeLabel = "Locked";
    }
    
    /* Draw storefront panel */
    DrawRectangleRec(boutiqueStore, storeColor);
    DrawRectangleLinesEx(boutiqueStore, 3.0f, borderColor);
    
    /* Draw door handle if unlocked/restored */
    if (state->boutiqueUnlocked) {
        DrawCircle(boutiqueStore.x + boutiqueStore.width - 25, boutiqueStore.y + boutiqueStore.height / 2, 6, (Color){180, 200, 220, 255});
    }
    
    /* Draw store label */
    int labelWidth = MeasureText(storeLabel, 14);
    DrawText(storeLabel, centerX - labelWidth / 2, startY + spacing + 20, 14, WHITE);
    
    /* Draw lock icon if boutique is locked */
    if (!state->boutiqueUnlocked) {
        DrawText("🔒", centerX - 8, startY + spacing + 10, 16, (Color){150, 150, 180, 200});
        const char *lockedHint = "Requires: Skeleton Key";
        int hintWidth = MeasureText(lockedHint, 10);
        DrawText(lockedHint, centerX - hintWidth / 2, startY + spacing + 40, 10, GRAY);
    }
    
    /* Draw Grand Opening decorations if boutique is fully upgraded */
    if (state->boutiqueUpgraded) {
        /* Bunting/banner across top */
        DrawRectangle(boutiqueStore.x, boutiqueStore.y - 15, boutiqueStore.width, 10, (Color){255, 215, 0, 255});
        DrawText("★", boutiqueStore.x + 20, boutiqueStore.y - 18, 12, YELLOW);
        DrawText("★", boutiqueStore.x + 50, boutiqueStore.y - 18, 12, YELLOW);
        DrawText("★", boutiqueStore.x + 80, boutiqueStore.y - 18, 12, YELLOW);
        DrawText("★", boutiqueStore.x + 110, boutiqueStore.y - 18, 12, YELLOW);
        /* Sign */
        DrawText("GRAND OPENING!", centerX - 45, startY + spacing - 30, 12, (Color){255, 215, 0, 255});
    }
    
    /* Draw upgrade button inside boutique area (only when unlocked but not upgraded) */
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
    
    /* Title - properly centered below UI bar */
    DrawText("MALL VIEW", centerX - MeasureText("MALL VIEW", 24) / 2, 65, 24, YELLOW);
}

/* Check if a shop button was clicked in Mall view */
int CheckMallShopClick(GameState *state, Vector2 mousePos, int screenWidth) {
    int centerX = screenWidth / 2;
    int startY = 120;
    int spacing = 120;
    
    /* Boutique button */
    Rectangle boutiqueBtn = (Rectangle){
        (float)(centerX - MALL_BTN_WIDTH / 2),
        (float)(startY + spacing),
        (float)MALL_BTN_WIDTH,
        (float)MALL_BTN_HEIGHT
    };
    
    if (mousePos.x >= boutiqueBtn.x && mousePos.x <= boutiqueBtn.x + MALL_BTN_WIDTH &&
        mousePos.y >= boutiqueBtn.y && mousePos.y <= boutiqueBtn.y + MALL_BTN_HEIGHT) {
        /* Boutique clicked - check if unlocked */
        if (state->boutiqueUnlocked) {
            return 1;  /* Enter boutique */
        }
        return 0;  /* Locked, no action */
    }
    
    /* Janitor's Closet button */
    Rectangle janitorBtn = (Rectangle){
        (float)(centerX - MALL_BTN_WIDTH / 2),
        (float)(startY),
        (float)MALL_BTN_WIDTH,
        (float)MALL_BTN_HEIGHT
    };
    
    if (mousePos.x >= janitorBtn.x && mousePos.x <= janitorBtn.x + MALL_BTN_WIDTH &&
        mousePos.y >= janitorBtn.y && mousePos.y <= janitorBtn.y + MALL_BTN_HEIGHT) {
        return 2;  /* Enter janitor's closet (always accessible) */
    }
    
    return 0;  /* No shop clicked */
}

/* Check if the boutique upgrade button was clicked */
int CheckMallUpgradeClick(GameState *state, Vector2 mousePos, int screenWidth) {
    /* Only show upgrade button when unlocked but not upgraded */
    if (!state->boutiqueUnlocked || state->boutiqueUpgraded) {
        return 0;
    }
    
    int centerX = screenWidth / 2;
    int startY = 120;
    int spacing = 120;
    
    Rectangle upgradeBtn = (Rectangle){
        (float)(centerX + MALL_BTN_WIDTH / 2 - UPGRADE_BTN_WIDTH - 10),
        (float)(startY + spacing + MALL_BTN_HEIGHT + 10),
        (float)UPGRADE_BTN_WIDTH,
        (float)UPGRADE_BTN_HEIGHT
    };
    
    if (mousePos.x >= upgradeBtn.x && mousePos.x <= upgradeBtn.x + UPGRADE_BTN_WIDTH &&
        mousePos.y >= upgradeBtn.y && mousePos.y <= upgradeBtn.y + UPGRADE_BTN_HEIGHT) {
        /* Check if player has enough coins */
        if (state->coins >= BOUTIQUE_UPGRADE_COST) {
            return 1;  /* Can purchase */
        }
    }
    return 0;
}

/* Purchase boutique upgrade (costs 50 coins) */
int PurchaseBoutiqueUpgrade(GameState *state) {
    if (state->boutiqueUpgraded || !state->boutiqueUnlocked) {
        return 0;  /* Already upgraded or not unlocked */
    }
    
    if (state->coins < BOUTIQUE_UPGRADE_COST) {
        return 0;  /* Not enough coins */
    }
    
    /* Deduct coins and mark as upgraded */
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
    
    /* Screen switching */
    if (state->current_screen == SCREEN_MALL) {
        DrawMallScreen(state, screenWidth, screenHeight);
    } else {
        /* SCREEN_BOARD - existing grid rendering */
        /* Draw background */
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
                
                /* Highlight selected slot */
                if (r == state->selected_row && c == state->selected_col) {
                    DrawRectangleRec(slot->bounds, (Color){80, 120, 180, 255});
                } else {
                    DrawRectangleRec(slot->bounds, (Color){60, 60, 70, 255});
                }
                
                /* Draw border - highlight gold if selected */
                if (r == state->selected_row && c == state->selected_col) {
                    DrawRectangleLinesEx(slot->bounds, 3.0f, YELLOW);
                } else {
                    DrawRectangleLinesEx(slot->bounds, 2.0f, (Color){100, 100, 120, 255});
                }
                
                /* Draw item if present and not being dragged */
                if (!ItemIsEmpty(&slot->item)) {
                    const ItemDefinition *def = GetItemDefinition(slot->item.item_id);
                    Texture2D *item_tex = GetItemTexture(state, slot->item.item_id);
                    
                    /* Draw item texture if available, otherwise fallback to colored rectangle */
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
        
        /* Draw task panel on the right side */
        Vector2 mousePos = GetMousePosition();
        DrawTaskPanel(state, mousePos, screenWidth, screenHeight);
        
        /* Draw trash button in task panel */
        DrawTrashButton(state, screenWidth, screenHeight);
        
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
        
        /* Back to Mall button */
        int backBtnX = screenWidth - 150;
        int backBtnY = 10;
        Rectangle backBtnRect = (Rectangle){(float)backBtnX, (float)backBtnY, 120, 30};
        DrawRectangleRec(backBtnRect, (Color){100, 100, 150, 255});
        DrawRectangleLinesEx(backBtnRect, 2.0f, YELLOW);
        DrawText("Back to Mall", backBtnX + 15, backBtnY + 8, 12, WHITE);
    }
    EndDrawing();
}

void RenderCleanup(GameState *state) {
    GameStateCleanup(state);
}