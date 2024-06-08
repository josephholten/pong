#include <stdio.h>
#include <time.h>
#include "raylib.h"
#include "raymath.h"

#include "util/util.h"

void DrawNet(Vector2 screenSize, Vector2 margin, float thick, Color color) {
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
    DrawLineExDashed(Vector2Subtract(ne, verticalLineOffset), Vector2Subtract(se, verticalLineOffset), thick, color, dashLength);
    // bottom
    DrawLineEx(Vector2Subtract(se, horizontalLineOffset), Vector2Subtract(sw, horizontalLineOffset), thick, color);
    // left
    DrawLineExDashed(Vector2Add(sw, verticalLineOffset), Vector2Add(nw, verticalLineOffset), thick, color, dashLength);
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
    InitWindow(screenSize.x, screenSize.y, "pong");

    Font font = GetFontDefault();
    if (!IsFontReady(font)) {
        TraceLog(LOG_ERROR, "font is not ready!");
        CloseWindow();
    }

    SetRandomSeed((unsigned int)time(NULL));

    SetTargetFPS(60);

    int goals[2] = {0};

    // pixels per second
    float playerSpeed = 200.0f;

    bool ballAlive = false;
    Vector2 ballPosition = Vector2Scale(screenSize, 0.5f);
    Vector2 ballDirection = {0.0f, 0.0f};
    float ballSpeedInitial = 200.0f;
    float ballSpeed = 400.0f;
    float ballSize = 10.0f;
    bool hasHit = false;

    time_t lastCollision = 0;

    float netThickness = 10.0f;

    Vector2 playerSize = { .x = 20.0f, .y = 150.0f };
    Vector2 playerPos[2] = {
        { margin + netThickness + textMargin, screenSize.y/2 - playerSize.y/2},
        { screenSize.x - margin - playerSize.x - netThickness - textMargin, screenSize.y/2 - playerSize.y/2 },
    };

    // close with ESC
    while(!WindowShouldClose()) {
        float delta = GetFrameTime();

        // movement
        float playerDirection[2] = {0.0f, 0.0f};
        if (IsKeyDown(KEY_W)) playerDirection[0] = -delta*playerSpeed;
        if (IsKeyDown(KEY_S)) playerDirection[0] = delta*playerSpeed;

        if (IsKeyDown(KEY_UP)) playerDirection[1] = -delta*playerSpeed;
        if (IsKeyDown(KEY_DOWN)) playerDirection[1] = delta*playerSpeed;

        playerPos[0].y += playerDirection[0];
        playerPos[1].y += playerDirection[1];

        for (Vector2& pos : playerPos)
            pos = Vector2Clamp(pos, {0,margin+netThickness}, {screenSize.x, screenSize.y-playerSize.y-margin-netThickness});

        if (IsKeyPressed(KEY_SPACE)) {
            if (!ballAlive) {
                ballAlive = true;
                ballDirection = Vector2Normalize({
                    GetRandomSign() * screenSize.x/2,
                    GetRandomSign() * (float) GetRandomValue(0, screenSize.y/2)
                });
                ballPosition = center;
            }
            // DEBUG
            else {
                ballAlive = false;
                hasHit = false;
            }
        }

        if (ballAlive) {
            float speed = (hasHit ? ballSpeed : ballSpeedInitial);
            ballPosition = Vector2Add(ballPosition,
                Vector2Scale(
                    ballDirection,
                    delta*speed
                )
            );
        } else {
            ballPosition = center;
        }

        // collisions
        for (int i = 0; i < 2; i++) {
            Vector2 pos = playerPos[i];
            float direction = playerDirection[i];
            Rectangle playerRec = {pos.x, pos.y, playerSize.x, playerSize.y};
            if (CheckCollisionCircleRec(ballPosition, ballSize, playerRec) && time(NULL) > lastCollision) {
                // flip ball direction on collision
                ballDirection.x *= -1.0f;
                ballDirection.y += direction*delta*2;
                ballDirection = Vector2Normalize(ballDirection);

                hasHit = true;
                lastCollision = time(NULL);
            }
        }
        if (ballPosition.y + ballSize + margin + netThickness >= screenSize.y || ballPosition.y <= ballSize + margin + netThickness)
            ballDirection.y *= -1.0f;

        if (ballPosition.x <= margin + netThickness + ballSize) {
            goals[1]++;
            ballAlive = false;
            hasHit = false;

        }
        if(ballPosition.x + margin + netThickness + ballSize >= screenSize.x) {
            goals[0]++;
            ballAlive = false;
            hasHit = false;
        }

        BeginDrawing(); {
            ClearBackground(backgroundColor);

            // lines
            DrawNet(screenSize, {margin, margin}, netThickness, foregroundColor);
            DrawLineExDashed({screenSize.x/2.f, margin}, {screenSize.x/2.f, screenSize.y-margin}, netThickness/2.0f, foregroundColor, 2.0f*netThickness);

            // player
            for (Vector2 pos : playerPos)
                DrawRectangleV(pos, playerSize, foregroundColor);

            // ball
            DrawCircleV(ballPosition, ballSize, foregroundColor);
            if (!ballAlive) {
                const char* ballDead = "ball DEAD: press space to start";
                Vector2 ballDeadSize = MeasureTextEx(font, ballDead, fontSize, fontSpacing);
                DrawTextEx(font, ballDead, {screenSize.x/2.f-ballDeadSize.x/2,textMargin}, fontSize, fontSpacing, WHITE);
                DrawCircleV(ballPosition, ballSize, foregroundColor);
            }

            // score
            char scoreBuf[2];
            Vector2 scoreTextSize;

            sprintf(scoreBuf, "%d", goals[0]);
            DrawTextEx(font, scoreBuf, {margin,textMargin}, fontSize, fontSpacing, WHITE);

            sprintf(scoreBuf, "%d", goals[1]);
            scoreTextSize = MeasureTextEx(font, scoreBuf, fontSize, fontSpacing);
            DrawTextEx(font, scoreBuf, {screenSize.x-margin-scoreTextSize.x,textMargin}, fontSize, fontSpacing, WHITE);
        }

        EndDrawing();
    }

    CloseWindow();
}
