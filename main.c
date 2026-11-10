#include "raylib.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#define SCREEN_WIDTH 400
#define SCREEN_HEIGHT 600

#define GRAVITY 1000.0f
#define JUMP_FORCE -350.0f
#define PIPE_SPEED 220.0f
#define PIPE_WIDTH 60
#define GAP_SIZE 180

typedef struct {
    float x;
    float topHeight;
    float bottomY;
    bool passed;
    bool active;
} Pipe;

// Game State Variables
float birdY = 200.0f;
float birdVelocityY = 0.0f;
float birdRadius = 16.0f;

Pipe pipes[3];
float spawnTimer = 0.0f;

int score = 0;
bool gameOver = false;

void ResetGame(void) {
    birdY = 200.0f;
    birdVelocityY = 0.0f;
    score = 0;
    gameOver = false;
    spawnTimer = 1.5f; // Spawn a pipe quickly on start

    for (int i = 0; i < 3; i++) {
        pipes[i].active = false;
    }
}

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raylib Flappy Bird in C");
    SetTargetFPS(60);

    srand((unsigned int)time(NULL));
    ResetGame();

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        if (!gameOver) {
            // Flap / Jump input
            if (IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsKeyPressed(KEY_UP)) {
                birdVelocityY = JUMP_FORCE;
            }

            // Apply gravity
            birdVelocityY += GRAVITY * deltaTime;
            birdY += birdVelocityY * deltaTime;

            // Floor collision
            if (birdY + birdRadius >= SCREEN_HEIGHT - 60) {
                birdY = SCREEN_HEIGHT - 60 - birdRadius;
                gameOver = true;
            }
            // Ceiling collision
            if (birdY - birdRadius <= 0) {
                birdY = birdRadius;
                birdVelocityY = 0;
            }

            // Pipe spawning logic
            spawnTimer += deltaTime;
            if (spawnTimer >= 2.2f) {
                spawnTimer = 0.0f;
                for (int i = 0; i < 3; i++) {
                    if (!pipes[i].active) {
                        pipes[i].active = true;
                        pipes[i].x = (float)SCREEN_WIDTH;
                        pipes[i].topHeight = (float)(rand() % 180 + 80); // Safe random height
                        pipes[i].bottomY = pipes[i].topHeight + GAP_SIZE;
                        pipes[i].passed = false;
                        break;
                    }
                }
            }

            // Update pipes and check collisions
            for (int i = 0; i < 3; i++) {
                if (pipes[i].active) {
                    pipes[i].x -= PIPE_SPEED * deltaTime;

                    // Deactivate if off-screen
                    if (pipes[i].x + PIPE_WIDTH < 0) {
                        pipes[i].active = false;
                    }

                    // Score point if bird passes pipe
                    if (!pipes[i].passed && pipes[i].x + PIPE_WIDTH < 100) {
                        score++;
                        pipes[i].passed = true;
                    }

                    // Collision detection (Bird vs Top/Bottom Pipes)
                    Rectangle birdRect = { 100 - birdRadius, birdY - birdRadius, birdRadius * 2, birdRadius * 2 };
                    Rectangle topPipeRect = { pipes[i].x, 0, PIPE_WIDTH, pipes[i].topHeight };
                    Rectangle bottomPipeRect = { pipes[i].x, pipes[i].bottomY, PIPE_WIDTH, SCREEN_HEIGHT - pipes[i].bottomY };

                    if (CheckCollisionRecs(birdRect, topPipeRect) || CheckCollisionRecs(birdRect, bottomPipeRect)) {
                        gameOver = true;
                    }
                }
            }
        } else {
            // Restart on ENTER or click
            if (IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                ResetGame();
            }
        }

        // --- DRAWING ---
        BeginDrawing();
        ClearBackground((Color){ 112, 197, 206, 255 }); // Sky blue background

        // Draw Pipes
        for (int i = 0; i < 3; i++) {
            if (pipes[i].active) {
                // Top Pipe
                DrawRectangle((int)pipes[i].x, 0, PIPE_WIDTH, (int)pipes[i].topHeight, (Color){ 115, 190, 43, 255 });
                DrawRectangleLines((int)pipes[i].x, 0, PIPE_WIDTH, (int)pipes[i].topHeight, DARKGREEN);
                
                // Bottom Pipe
                DrawRectangle((int)pipes[i].x, (int)pipes[i].bottomY, PIPE_WIDTH, SCREEN_HEIGHT - (int)pipes[i].bottomY, (Color){ 115, 190, 43, 255 });
                DrawRectangleLines((int)pipes[i].x, (int)pipes[i].bottomY, PIPE_WIDTH, SCREEN_HEIGHT - (int)pipes[i].bottomY, DARKGREEN);
            }
        }

        // Draw Ground Floor
        DrawRectangle(0, SCREEN_HEIGHT - 60, SCREEN_WIDTH, 60, (Color){ 222, 216, 149, 255 });
        DrawLine(0, SCREEN_HEIGHT - 60, SCREEN_WIDTH, SCREEN_HEIGHT - 60, DARKGREEN);

        // Draw Bird
        DrawCircle(100, (int)birdY, birdRadius, YELLOW);
        DrawCircleLines(100, (int)birdY, birdRadius, ORANGE);
        DrawCircle(107, (int)birdY - 4, 3, BLACK); // Eye

        // Draw Score UI
        DrawText(TextFormat("%d", score), SCREEN_WIDTH / 2 - MeasureText(TextFormat("%d", score), 40) / 2, 50, 40, RAYWHITE);

        // Game Over Screen Overlay
        if (gameOver) {
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.4f));
            DrawText("GAME OVER", SCREEN_WIDTH / 2 - MeasureText("GAME OVER", 28) / 2, SCREEN_HEIGHT / 2 - 40, 28, RED);
            DrawText("Press ENTER to Restart", SCREEN_WIDTH / 2 - MeasureText("Press ENTER to Restart", 14) / 2, SCREEN_HEIGHT / 2 + 10, 14, RAYWHITE);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

