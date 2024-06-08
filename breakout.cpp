#include <stdio.h>
#include <time.h>
#include "raylib.h"
#include "raymath.h"

#include "util.h"

int main(int, char**){
    Vector2 screenSize = {800, 450};
    const Vector2 center = Vector2Scale(screenSize, 0.5f);
    float textMargin = 5;
    float margin = 50;

    Color backgroundColor = BLACK;
    Color foregroundColor = WHITE;

    // float fontSize = 20.f;
    // float fontSpacing = 3.f;

    SetTraceLogCallback(CustomLog);
    SetTraceLogLevel(LOG_DEBUG);
    InitWindow(screenSize.x, screenSize.y, "breakout");

    Font font = GetFontDefault();
    if (!IsFontReady(font)) {
        TraceLog(LOG_ERROR, "font is not ready!");
        CloseWindow();
    }

    SetRandomSeed((unsigned int)time(NULL));

    SetTargetFPS(60);

    // int score = 0;

    bool ballAlive = false;
    Vector2 ballPosition = Vector2Scale(screenSize, 0.5f);
    Vector2 ballDirection = {0.0f, 0.0f};
    float ballSpeedInitial = 200.0f;
    float ballSpeed = ballSpeedInitial;
    float ballSize = 10.0f;

    Vector2 playerSize = { .x = 200.0f, .y = 20.0f };
    Vector2 playerPos = { center.x - playerSize.x/2, screenSize.y - playerSize.y - margin};

    // close with ESC
    while(!WindowShouldClose()) {
        float delta = GetFrameTime();

        if (!ballAlive) {
            ballPosition = {playerPos.x + playerSize.x/2, playerPos.y - ballSize};

            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                ballAlive = true;
                ballDirection = Vector2Normalize({(float)GetRandomValue(0-ballPosition.x, screenSize.x-ballPosition.x), -ballPosition.y});
            }
        }

        playerPos.x = GetMousePosition().x - playerSize.x/2;

        if (ballAlive) {
            ballPosition = Vector2Add(ballPosition, Vector2Scale(ballDirection, ballSpeed*delta));
        }

        // movement
        BeginDrawing(); {
            ClearBackground(backgroundColor);

            // player
            DrawRectangleV(playerPos, playerSize, foregroundColor);

            // ball
            DrawCircleV(ballPosition, ballSize, foregroundColor);
        }

        EndDrawing();
    }

    CloseWindow();
}
