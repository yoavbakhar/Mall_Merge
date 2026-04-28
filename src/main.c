// Minimal main: initialize and delegate to modules
#include "raylib.h"
#include "game_state.h"
#include "input.h"
#include "render.h"
#include "constants.h"

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Mall Merge: Abandoned Vibes");
    SetTargetFPS(60);

    /* Initialize game state */
    GameState state;
    GameStateInit(&state, SCREEN_WIDTH, SCREEN_HEIGHT);

    /* Pre-calculate grid layout */
    float slotSize = GetSlotSize(SCREEN_WIDTH);
    Vector2 gridTopLeft = GetGridTopLeft(SCREEN_WIDTH, slotSize);

    /* Main game loop */
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        /* ====== UPDATE ====== */
        
        /* Regenerate energy */
        state.energy_regen_timer += dt;
        while (state.energy_regen_timer >= 1.0f && state.energy < state.max_energy) {
            state.energy += (int)(state.energy_regen_rate);
            state.energy_regen_timer -= 1.0f;
        }
        if (state.energy > state.max_energy) {
            state.energy = state.max_energy;
        }

        /* Update generator cooldown */
        if (state.generator_cooldown > 0.0f) {
            state.generator_cooldown -= dt;
        }

        /* Process input */
        ProcessInput(&state, gridTopLeft, slotSize, SCREEN_WIDTH, SCREEN_HEIGHT);

        /* ====== RENDER ====== */
        RenderGame(&state, gridTopLeft, slotSize, SCREEN_WIDTH, SCREEN_HEIGHT);
    }

    RenderCleanup(&state);
    CloseWindow();
    return 0;
}
