#include <raylib.h>
#define RAYGUI_IMPLEMENTATION


int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "FallingDown");

    SetTargetFPS(120);

    const int doorWidth = 400;
    const int doorHeight = 60;

    Vector2 door1Pos = { 200, 100 };
    Vector2 door2Pos = { 600, 100 };

    // rotations
    float door1Rotation = 15.0f;
    float door2Rotation = -15.0f;

    // RenderTextures
    RenderTexture2D doorRT = LoadRenderTexture(doorWidth, doorHeight);

    BeginTextureMode(doorRT);
        ClearBackground(BLANK);
        DrawRectangleRounded(
            (Rectangle){ 0, 0, doorWidth, doorHeight },
            0.3f,
            16,
            WHITE
        );
    EndTextureMode();

    Color backgroundColor = { 18, 18, 18, 255 };

    while (!WindowShouldClose())
    {

        BeginDrawing();
        ClearBackground(backgroundColor);

        // First door
        DrawTexturePro(
            doorRT.texture,
            (Rectangle){ 0, 0, (float)doorRT.texture.width, -(float)doorRT.texture.height },
            (Rectangle){ door1Pos.x, door1Pos.y, doorWidth, doorHeight },
            (Vector2){ doorWidth / 2.0f, doorHeight / 2.0f },
            door1Rotation,
            WHITE
        );

        // Second door
        DrawTexturePro(
            doorRT.texture,
            (Rectangle){ 0, 0, (float)doorRT.texture.width, -(float)doorRT.texture.height },
            (Rectangle){ door2Pos.x, door2Pos.y, doorWidth, doorHeight },
            (Vector2){ doorWidth / 2.0f, doorHeight / 2.0f },
            door2Rotation,
            WHITE
        );

        EndDrawing();
    }

    UnloadRenderTexture(doorRT);
    CloseWindow();

    return 0;
}