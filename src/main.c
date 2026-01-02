#include <raylib.h>
#define RAYGUI_IMPLEMENTATION


int main(void)
{
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "FallingDown");

    SetTargetFPS(120);

    // Размеры дверей
    const int doorWidth = 320;
    const int doorHeight = 60;

    // Позиции
    Vector2 door1Pos = { 400, 200 };
    Vector2 door2Pos = { 400, 300 };

    // Углы поворота
    float door1Rotation = 0.0f;
    float door2Rotation = 0.0f;

    // RenderTextures
    RenderTexture2D doorRT = LoadRenderTexture(doorWidth, doorHeight);

    // Отрисовываем rounded rect один раз в текстуру
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