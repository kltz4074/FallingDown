#include <raylib.h>
#include <stdlib.h>
#include <time.h>

#define SCREEN_WIDTH   800
#define SCREEN_HEIGHT  450

#define FIXED_DT      (1.0f / 60.0f)

#define GRAVITY        1200.0f
#define JUMP_FORCE    -420.0f

#define PIPE_SPEED     220.0f
#define PIPE_WIDTH      90
#define GAP_HEIGHT     160
#define PIPE_COUNT       3

#define ROUNDNESS     0.35f
#define SEGMENTS        14
#define SHADOW_OFFSET   8

#define PIPE_SPACING 300

typedef struct Pipe {
    float x;
    float prevX;
    float gapY;
    bool passed;
} Pipe;

typedef struct CloudLayer {
    float offset;
    float speed;
    Color color;
} CloudLayer;

void DrawPipe(Rectangle r, Color color)
{
    DrawRectangleRounded(r, ROUNDNESS, SEGMENTS, color);
}

void DrawPipeShadow(Rectangle r)
{
    Rectangle shadow = {
        r.x + SHADOW_OFFSET,
        r.y + SHADOW_OFFSET,
        r.width,
        r.height
    };

    DrawRectangleRounded(
        shadow,
        ROUNDNESS,
        SEGMENTS,
        (Color){ 0, 0, 0, 80 }
    );
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Flappy Bird — Interpolated");
    SetTargetFPS(120);

    srand((unsigned int)time(NULL));

    // Bird
    Vector2 birdPos = { 200, SCREEN_HEIGHT / 2.0f };
    Vector2 birdPrevPos = birdPos;
    float birdVel = 0.0f;
    const float birdRadius = 18.0f;

    // Pipes
    Pipe pipes[PIPE_COUNT];
    for (int i = 0; i < PIPE_COUNT; i++)
    {
        pipes[i].x = SCREEN_WIDTH + i * 300;
        pipes[i].prevX = pipes[i].x;
        pipes[i].gapY = 140 + rand() % (SCREEN_HEIGHT - 280);
        pipes[i].passed = false;
    }

    // Cloud layers
    CloudLayer clouds[3] = {
        { 0,  10, (Color){255,255,255,40} },
        { 0,  20, (Color){255,255,255,60} },
        { 0,  35, (Color){255,255,255,80} }
    };

    int score = 0;
    bool gameOver = false;

    double accumulator = 0.0;
    double lastTime = GetTime();

    while (!WindowShouldClose())
    {
        // ---- TIME ----
        double currentTime = GetTime();
        double frameTime = currentTime - lastTime;
        lastTime = currentTime;

        if (frameTime > 0.25)
            frameTime = 0.25;

        accumulator += frameTime;

        // ---- INPUT ----
        if (!gameOver && IsKeyPressed(KEY_SPACE))
            birdVel = JUMP_FORCE;

        if (gameOver && IsKeyPressed(KEY_R))
        {
            birdPos.y = SCREEN_HEIGHT / 2.0f;
            birdPrevPos = birdPos;
            birdVel = 0;
            score = 0;
            gameOver = false;

            for (int i = 0; i < PIPE_COUNT; i++)
            {
                pipes[i].x = SCREEN_WIDTH + i * 300;
                pipes[i].prevX = pipes[i].x;
                pipes[i].gapY = 140 + rand() % (SCREEN_HEIGHT - 280);
                pipes[i].passed = false;
            }
        }

        // ---- FIXED UPDATE ----
        while (accumulator >= FIXED_DT)
        {
            birdPrevPos = birdPos;

            if (!gameOver)
            {
                birdVel += GRAVITY * FIXED_DT;
                birdPos.y += birdVel * FIXED_DT;

                for (int i = 0; i < PIPE_COUNT; i++)
                {
                    pipes[i].prevX = pipes[i].x;
                    pipes[i].x -= PIPE_SPEED * FIXED_DT;

                    if (pipes[i].x + PIPE_WIDTH < 0)
                    {
                        float maxX = 0.0f;
                        for (int j = 0; j < PIPE_COUNT; j++)
                            if (pipes[j].x > maxX)
                                maxX = pipes[j].x;

                        pipes[i].x = maxX + PIPE_SPACING;
                        pipes[i].prevX = pipes[i].x;
                        pipes[i].gapY = 140 + rand() % (SCREEN_HEIGHT - 280);
                        pipes[i].passed = false;
                    }


                    if (!pipes[i].passed && pipes[i].x + PIPE_WIDTH < birdPos.x)
                    {
                        score++;
                        pipes[i].passed = true;
                    }

                    Rectangle topPipe = {
                        pipes[i].x,
                        0,
                        PIPE_WIDTH,
                        pipes[i].gapY - GAP_HEIGHT / 2
                    };

                    Rectangle bottomPipe = {
                        pipes[i].x,
                        pipes[i].gapY + GAP_HEIGHT / 2,
                        PIPE_WIDTH,
                        SCREEN_HEIGHT
                    };

                    if (CheckCollisionCircleRec(birdPos, birdRadius, topPipe) ||
                        CheckCollisionCircleRec(birdPos, birdRadius, bottomPipe))
                        gameOver = true;
                }

                if (birdPos.y < 0 || birdPos.y > SCREEN_HEIGHT)
                    gameOver = true;
            }

            // Clouds update
            for (int i = 0; i < 3; i++)
            {
                clouds[i].offset += clouds[i].speed * FIXED_DT;
                if (clouds[i].offset > SCREEN_WIDTH)
                    clouds[i].offset -= SCREEN_WIDTH;
            }

            accumulator -= FIXED_DT;
        }

        float alpha = (float)(accumulator / FIXED_DT);

        // ---- DRAW ----
        BeginDrawing();

        // Sky
        DrawRectangleGradientV(
            0, 0,
            SCREEN_WIDTH, SCREEN_HEIGHT,
            (Color){ 90, 180, 255, 255 },
            (Color){ 180, 220, 255, 255 }
        );

        // Clouds (animated background)
        for (int l = 0; l < 3; l++)
        {
            for (int x = -100; x < SCREEN_WIDTH + 100; x += 200)
            {
                DrawRectangle(
                    x - (int)clouds[l].offset,
                    50 + l * 40,
                    140, 20,
                    clouds[l].color
                );
            }
        }

        // Pipes
        for (int i = 0; i < PIPE_COUNT; i++)
        {
            float interpX = pipes[i].prevX +
                (pipes[i].x - pipes[i].prevX) * alpha;

            Rectangle top = {
                interpX,
                0,
                PIPE_WIDTH,
                pipes[i].gapY - GAP_HEIGHT / 2
            };

            Rectangle bottom = {
                interpX,
                pipes[i].gapY + GAP_HEIGHT / 2,
                PIPE_WIDTH,
                SCREEN_HEIGHT
            };

            DrawPipeShadow(top);
            DrawPipeShadow(bottom);
            DrawPipe(top, GREEN);
            DrawPipe(bottom, GREEN);
        }

        // Bird (interpolated)
        Vector2 birdInterp = {
            birdPrevPos.x + (birdPos.x - birdPrevPos.x) * alpha,
            birdPrevPos.y + (birdPos.y - birdPrevPos.y) * alpha
        };

        DrawCircleV(birdInterp, birdRadius, YELLOW);

        DrawText(TextFormat("Score: %d", score), 20, 40, 30, DARKGRAY);
        DrawFPS(20, 10);

        if (gameOver)
        {
            DrawText("GAME OVER",
                SCREEN_WIDTH / 2 - 120,
                SCREEN_HEIGHT / 2 - 40,
                40, RED);

            DrawText("Press R to restart",
                SCREEN_WIDTH / 2 - 140,
                SCREEN_HEIGHT / 2 + 10,
                20, DARKGRAY);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
