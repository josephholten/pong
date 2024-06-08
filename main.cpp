#include <stdio.h>
#include <time.h>
#include "raylib.h"
#include "raymath.h"

int GetRandomSign() {
    return GetRandomValue(0, 1) ? 1 : -1;
}

int FloatSign(float x) {
    if (x > 0) return 1;
    if (x < 0) return -1;
    return 0;
}

// Custom logging function
void CustomLog(int logLevel, const char *text, va_list args)
{
    char timeStr[64] = { 0 };
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);

    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", tm_info);
    printf("[%s] ", timeStr);

    switch (logLevel)
    {
        case LOG_INFO: printf("[INFO] : "); break;
        case LOG_ERROR: printf("[ERROR]: "); break;
        case LOG_WARNING: printf("[WARN] : "); break;
        case LOG_DEBUG: printf("[DEBUG]: "); break;
        default: break;
    }

    vprintf(text, args);
    printf("\n");
}

void DrawLineExDashed(Vector2 startPos, Vector2 endPos, float thick, Color color, float dashLength, float spaceLength) {
    float totalLength = Vector2Length(Vector2Subtract(endPos, startPos));
    float numDashes = (totalLength + spaceLength) / (dashLength + spaceLength);

    Vector2 direction = Vector2Normalize(Vector2Subtract(endPos, startPos));
    Vector2 pos {startPos.x, startPos.y};
    for (int i = 0; i < numDashes; i++) {
        Vector2 dash_end = Vector2Add(pos, Vector2Scale(direction, dashLength));
        DrawLineEx(pos, dash_end, thick, color);
        pos = dash_end;
        Vector2 space_end = Vector2Add(pos, Vector2Scale(direction, spaceLength));
        // not draw
        pos = space_end;
    }
}

void DrawTextCentered(const char* text, Vector2 pos, int fontSize, Color color) {
    int text_width = MeasureText(text, fontSize);
    DrawText(text, pos.x-text_width/2., pos.y-fontSize/2., fontSize, color);
}

int main(int, char**){
    Vector2 screenSize = {800, 450};
    const Vector2 center = Vector2Scale(screenSize, 0.5f);
    float xMargin = 50;

    SetTraceLogCallback(CustomLog);
    SetTraceLogLevel(LOG_DEBUG);
    InitWindow(screenSize.x, screenSize.y, "pong");

    SetRandomSeed((unsigned int)time(NULL));

    SetTargetFPS(60);

    Vector2 playerSize = { .x = 20.0f, .y = 150.0f };
    Vector2 playerPos[2] = {
        { xMargin, screenSize.y/2 - playerSize.y/2},
        { screenSize.x - xMargin, screenSize.y/2 - playerSize.y/2 },
    };

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
            pos = Vector2Clamp(pos, {0,0}, Vector2Subtract(screenSize, playerSize));

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
                TraceLog(LOG_DEBUG, "new ball direction (%f %f)", ballDirection.x, ballDirection.y);

                hasHit = true;
                lastCollision = time(NULL);
            }
        }
        if (ballPosition.y + ballSize >= screenSize.y || ballPosition.y - ballSize <= 0)
            ballDirection.y *= -1.0f;

        if (ballPosition.x <= xMargin || ballPosition.x >= screenSize.x - xMargin) {
            ballAlive = false;
            hasHit = false;
        }

        BeginDrawing(); {
            ClearBackground(BLACK);
            float net_thickness = 5.0f;
            DrawLineExDashed({xMargin + playerSize.x/2, 0}, {xMargin + playerSize.x/2, screenSize.y}, net_thickness, WHITE, 20.0f, 10.0f);
            DrawLineExDashed({screenSize.x - xMargin + playerSize.x/2, 0}, {screenSize.x - xMargin + playerSize.x/2, screenSize.y}, net_thickness, WHITE, 20.0f, 10.0f);

            DrawLineExDashed({screenSize.x/2, 0}, {screenSize.x/2, screenSize.y}, net_thickness*1.5, WHITE, 20.0f*1.5, 10.0f*1.5);
            for (Vector2 pos : playerPos)
                DrawRectangleV(pos, playerSize, WHITE);
            if (ballAlive) {
                DrawCircleV(ballPosition, ballSize, WHITE);
            } else {
                int fontSize = 20;
                int margin = 10;
                const char* text = "Ball DEAD";
                int width = MeasureText(text, fontSize);
                DrawText("Ball DEAD", screenSize.x/2-width-net_thickness-margin, screenSize.y/2-fontSize/2, fontSize, WHITE);
            }
        }

        EndDrawing();
    }

    CloseWindow();
}
