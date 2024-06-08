#include <stdio.h>
#include <time.h>
#include "raylib.h"
#include "raymath.h"

#include "util.h"

void DrawBorder(Vector2 screenSize, Vector2 margin, float thick, Color color) {
    Vector2 nw {margin.x, margin.y};
    Vector2 ne {screenSize.x-margin.x, margin.y};
    Vector2 sw {margin.x, screenSize.y-margin.y};
    Vector2 se {screenSize.x-margin.x, screenSize.y-margin.x};

    float dashLength = 3*thick;

    Vector2 horizontalLineOffset = {0, thick/2};
    Vector2 verticalLineOffset = {thick/2, 0};
    // top
    DrawLineEx(Vector2Add(nw, horizontalLineOffset), Vector2Add(ne, horizontalLineOffset), thick, color);
    // right
    DrawLineEx(Vector2Subtract(ne, verticalLineOffset), Vector2Subtract(se, verticalLineOffset), thick, color);
    // left
    DrawLineEx(Vector2Add(sw, verticalLineOffset), Vector2Add(nw, verticalLineOffset), thick, color);
}

int main(int, char**){
    Vector2 screenSize = {800, 450};
    const Vector2 center = Vector2Scale(screenSize, 0.5f);
    float textMargin = 5;
    float margin = 50;

    Color backgroundColor = BLACK;
    Color foregroundColor = WHITE;

    float fontSize = 20.f;
    float fontSpacing = 3.f;

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

    float borderThickness = 5.0f;

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
        playerPos = Vector2Clamp(playerPos, {textMargin + borderThickness, 0}, {screenSize.x-textMargin-borderThickness-playerSize.x, screenSize.y});

        // collisions with borders
        if (ballAlive) {
            ballPosition = Vector2Add(ballPosition, Vector2Scale(ballDirection, ballSpeed*delta));
            if (ballPosition.y < ballSize+textMargin+borderThickness)
                ballDirection.y *= -1.0f;
            else if (ballPosition.y + ballSize+textMargin+borderThickness > screenSize.y)
                ballAlive = false;
            else if (ballPosition.x + ballSize + textMargin + borderThickness > screenSize.x || ballPosition.x < ballSize + borderThickness + textMargin) {
                ballDirection.x *= -1.0f;
            }
        }

        // collisions with player
        if (ballAlive) {
            Rectangle playerRec = {playerPos.x, playerPos.y, playerSize.x, playerSize.y};
            if (CheckCollisionCircleRec(ballPosition, ballSize, playerRec)) {
                ballDirection.y *= -1.0f;
            }
        }

        ballPosition = Vector2Add(ballPosition, Vector2Scale(ballDirection, ballSpeed*delta));

        // movement
        BeginDrawing(); {
            ClearBackground(backgroundColor);

            // border
            DrawBorder(screenSize, {textMargin, textMargin}, borderThickness, foregroundColor);

            // player
            DrawRectangleV(playerPos, playerSize, foregroundColor);

            // ball
            DrawCircleV(ballPosition, ballSize, foregroundColor);

            if (!ballAlive) {
                const char* text = "Ball DEAD: press Left Mouse Btn to shoot ball";
                Vector2 textSize = MeasureTextEx(font, text, fontSize, fontSpacing);
                DrawTextEx(font, text, {screenSize.x/2 - textSize.x/2, screenSize.y - textSize.y - textMargin}, fontSize, fontSpacing, foregroundColor);
            }
        }

        EndDrawing();
    }

    CloseWindow();
}
