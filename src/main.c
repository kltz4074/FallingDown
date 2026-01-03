#include <raylib.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

#define SCREEN_WIDTH   800
#define SCREEN_HEIGHT  450

#define FIXED_DT      (1.0f / 60.0f)

#define GRAVITY        1200.0f
#define JUMP_FORCE    -420.0f

#define PIPE_SPEED     220.0f
#define PIPE_WIDTH      90
#define GAP_HEIGHT     160
#define PIPE_COUNT       3
#define PIPE_SPACING   300

#define ROUNDNESS     0.35f
#define SEGMENTS        14
#define SHADOW_OFFSET   8

#define DAY_CYCLE_DURATION 60.0f

typedef struct {
    float x;
    float prevX;
    float gapY;
    bool passed;
} Pipe;

typedef struct {
    float offset;
    float speed;
    Color color;
    float y;
    float scale;
} CloudLayer;


void DrawPipe(Rectangle r, Color color)
{
    DrawRectangleRounded(r, ROUNDNESS, SEGMENTS, color);
}

void DrawPipeShadow(Rectangle r)
{
    Rectangle s = {
        r.x + SHADOW_OFFSET,
        r.y + SHADOW_OFFSET,
        r.width,
        r.height
    };
    DrawRectangleRounded(s, ROUNDNESS, SEGMENTS, (Color){0,0,0,80});
}

int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Flappy Bird");
    SetTargetFPS(120);
    srand((unsigned)time(NULL));

    // Bird
    Vector2 birdPos  = {200, SCREEN_HEIGHT/2};
    Vector2 birdPrev = birdPos;
    float birdVel = 0.0f;
    float birdR   = 18.0f;

    Pipe pipes[PIPE_COUNT];
    for (int i = 0; i < PIPE_COUNT; i++)
    {
        pipes[i].x     = SCREEN_WIDTH + i * PIPE_SPACING;
        pipes[i].gapY  = 140 + rand() % (SCREEN_HEIGHT - 280);
        pipes[i].passed = false;
    }
    CloudLayer clouds[3] = {
        {0, 10, {255,255,255,50},  70, 1.0f},
        {0, 20, {255,255,255,70}, 110, 1.2f},
        {0, 35, {255,255,255,90}, 150, 1.4f}
    };

    Color skyTop[4] = {
        { 90,180,255,255},
        {255,140, 80,255},
        { 20, 30, 70,255},
        {120,180,255,255}
    };

    Color skyBot[4] = {
        {180,220,255,255},
        {255,190,120,255},
        { 60, 70,120,255},
        {200,230,255,255}
    };

    Color pipeColors[4] = {
        { 20, 200,  20, 255},  // Day
        {180, 140,  40, 255},  // Evening 
        { 40, 120,  90, 255},  // Night
        {120, 200, 140, 255}   // Morning
    };

    Color textColorDay   = DARKGRAY;        
    Color textColorNight = (Color){200, 200, 230, 255}; 

    float dayTimer = 0.0f;

    int score = 0;
    bool gameOver = false;
    bool started  = false;

    double acc  = 0.0;
    double last = GetTime();

    while (!WindowShouldClose())
    {
        double now = GetTime();
        double dt  = now - last;
        last = now;

        if (dt > 0.25) dt = 0.25;
        acc += dt;

        if (!started && IsKeyPressed(KEY_SPACE))
            started = true;

        if (started && !gameOver && IsKeyPressed(KEY_SPACE))
            birdVel = JUMP_FORCE;

        if (gameOver && IsKeyPressed(KEY_R))
        {
            birdPos.y = SCREEN_HEIGHT/2;
            birdVel   = 0;
            score     = 0;
            gameOver  = false;
            started   = false;

            for (int i = 0; i < PIPE_COUNT; i++)
            {
                pipes[i].x     = SCREEN_WIDTH + i * PIPE_SPACING;
                pipes[i].prevX = pipes[i].x;
                pipes[i].gapY  = 140 + rand() % (SCREEN_HEIGHT - 280);
                pipes[i].passed = false;
            }
        }

        while (acc >= FIXED_DT)
        {
            birdPrev = birdPos;

            dayTimer += FIXED_DT;
            if (dayTimer >= DAY_CYCLE_DURATION)
                dayTimer -= DAY_CYCLE_DURATION;

            if (started && !gameOver)
            {
                birdVel += GRAVITY * FIXED_DT;
                birdPos.y += birdVel * FIXED_DT;

                for (int i = 0; i < PIPE_COUNT; i++)
                {
                    pipes[i].prevX = pipes[i].x;
                    pipes[i].x -= PIPE_SPEED * FIXED_DT;

                    if (pipes[i].x + PIPE_WIDTH < 0)
                    {
                        float maxX = pipes[0].x;
                        for (int j = 1; j < PIPE_COUNT; j++)
                            if (pipes[j].x > maxX) maxX = pipes[j].x;

                        pipes[i].x     = maxX + PIPE_SPACING;
                        pipes[i].prevX = pipes[i].x;
                        pipes[i].gapY  = 140 + rand() % (SCREEN_HEIGHT - 280);
                        pipes[i].passed = false;
                    }

                    if (!pipes[i].passed && pipes[i].x + PIPE_WIDTH < birdPos.x)
                    {
                        score++;
                        pipes[i].passed = true;
                    }

                    Rectangle t = {pipes[i].x, 0, PIPE_WIDTH,
                                    pipes[i].gapY - GAP_HEIGHT/2};
                    Rectangle b = {pipes[i].x,
                                    pipes[i].gapY + GAP_HEIGHT/2,
                                    PIPE_WIDTH, SCREEN_HEIGHT};

                    if (CheckCollisionCircleRec(birdPos, birdR, t) ||
                        CheckCollisionCircleRec(birdPos, birdR, b))
                        gameOver = true;
                }

                if (birdPos.y < 0 || birdPos.y > SCREEN_HEIGHT)
                    gameOver = true;
            }

            // Clouds
            for (int l = 0; l < 3; l++)
            {
                clouds[l].offset += clouds[l].speed * FIXED_DT;

                float step = 180.0f;
                if (clouds[l].offset > step) clouds[l].offset -= step;

                for (float x = -step; x < SCREEN_WIDTH + step; x += step)
                {
                    float cx = x - clouds[l].offset;
                    float y  = clouds[l].y;
                    float s  = clouds[l].scale;
                    Color c  = clouds[l].color;

                    DrawCircleV((Vector2){cx, y},           18*s, c);
                    DrawCircleV((Vector2){cx+20*s, y-8*s},  22*s, c);
                    DrawCircleV((Vector2){cx+45*s, y},      18*s, c);
                }
            }


            acc -= FIXED_DT;
        }

        float alpha = acc / FIXED_DT;

        // Day/Night
        float phaseLen = DAY_CYCLE_DURATION / 4.0f;
        int   p  = (int)(dayTimer / phaseLen);
        int   np = (p + 1) % 4;
        float pa = (dayTimer - p * phaseLen) / phaseLen;

        Color top = ColorLerp(skyTop[p], skyTop[np], pa);
        Color bot = ColorLerp(skyBot[p], skyBot[np], pa);
        Color pipeColor = ColorLerp(pipeColors[p], pipeColors[np], pa);

        float t = dayTimer / DAY_CYCLE_DURATION;
        float night = sinf(t * 2.0f * PI - PI / 2.0f);
        if (night < 0) night = 0;
        if (night > 1) night = 1;

        Color textColor = ColorLerp(textColorDay, textColorNight, night);

        if (night > 0.0f)
        {
            Color fogColor = (Color){60, 70, 90, (unsigned char)(120 * night)};
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, fogColor);
        }

        BeginDrawing();

        DrawRectangleGradientV(0,0,SCREEN_WIDTH,SCREEN_HEIGHT, top, bot);

        DrawRectangle(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,
            (Color){10,20,40,(unsigned char)(140 * night)});

        // Clouds
        for (int l = 0; l < 3; l++)
        {
            for (int x = -200; x < SCREEN_WIDTH + 200; x += 180)
            {
                float cx = x - clouds[l].offset;
                float y  = clouds[l].y;
                float s  = clouds[l].scale;
                Color c  = clouds[l].color;

                DrawCircleV((Vector2){cx, y},           18*s, c);
                DrawCircleV((Vector2){cx+20*s, y-8*s},  22*s, c);
                DrawCircleV((Vector2){cx+45*s, y},      18*s, c);
            }
        }

        // Pipes
        for (int i = 0; i < PIPE_COUNT; i++)
        {
            float ix = pipes[i].prevX +
                       (pipes[i].x - pipes[i].prevX) * alpha;

            Rectangle t = {ix, 0, PIPE_WIDTH,
                            pipes[i].gapY - GAP_HEIGHT/2};
            Rectangle b = {ix,
                            pipes[i].gapY + GAP_HEIGHT/2,
                            PIPE_WIDTH, SCREEN_HEIGHT};

            DrawPipeShadow(t);
            DrawPipeShadow(b);
            DrawPipe(t, pipeColor);
            DrawPipe(b, pipeColor);
        }

        Vector2 bi = {
            birdPrev.x + (birdPos.x - birdPrev.x) * alpha,
            birdPrev.y + (birdPos.y - birdPrev.y) * alpha
        };

        // count text
        DrawCircleV(bi, birdR, YELLOW);

        DrawText(TextFormat("Score: %d", score), 20, 40, 30, textColor);

        if (!started)
            DrawText("PRESS SPACE TO START",
                    SCREEN_WIDTH/2-170, SCREEN_HEIGHT/2-20,
                    30, textColor);

        if (gameOver)
        {
            DrawText("GAME OVER",
                    SCREEN_WIDTH/2-120, SCREEN_HEIGHT/2-40,
                    40, RED); // красный оставляем всегда
            DrawText("Press R to restart",
                    SCREEN_WIDTH/2-140, SCREEN_HEIGHT/2+10,
                    20, textColor);
        }


        EndDrawing();
    }

    CloseWindow();
    return 0;
}
