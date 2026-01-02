#include <raylib.h>
#define RAYGUI_IMPLEMENTATION

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "FallingDown");

    SetTargetFPS(120); 
    Rectangle Door1 = {400, 20, 320, 60};
    Rectangle Door2 = {50, 20, 320, 60};

    Color BackroundColor = {18, 18, 18, 1};

    while (!WindowShouldClose())  
    {
        
        BeginDrawing();

        ClearBackground(BackroundColor);

        DrawRectangleRounded(Door1, 0.3f, 0.0f, WHITE);
        DrawRectangleRounded(Door2, 0.3f, 0.0f, WHITE);

        EndDrawing();
    }

    CloseWindow();      

    return 0;
}