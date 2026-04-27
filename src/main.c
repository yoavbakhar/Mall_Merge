#include "raylib.h"

int main(void) {
    // Standard Mobile/Vibe Resolution (9:16 aspect ratio)
    const int screenWidth = 450;
    const int screenHeight = 800;

    InitWindow(screenWidth, screenHeight, "Mall Merge: Abandoned Vibes");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // Update Logic here (Energy timers, Dragging items)
        
        BeginDrawing();
            ClearBackground(DARKGRAY); 
            DrawText("The Lost & Found Box Awaits...", 50, 400, 20, LIGHTGRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}