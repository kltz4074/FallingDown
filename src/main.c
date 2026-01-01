#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "../lib/raygui.h"           

int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "FallingDown");

    SetTargetFPS(120); 
    Rectangle TestRec = {screenWidth/2.0, screenHeight/2.0, 320, 80};

    Color BackroundColor = {18, 18, 18, 1};

    while (!WindowShouldClose())  
    {
        
        BeginDrawing();

        ClearBackground(BackroundColor);

        DrawRectangleRounded(TestRec, 0.3f, 0.0f, WHITE);
        EndDrawing();
    }

    CloseWindow();      

    return 0;
}