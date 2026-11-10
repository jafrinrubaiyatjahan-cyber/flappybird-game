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

[프로젝트 설명]
- 게임 이름: 플래피 버드 (Flappy Bird Clone)
- 사용 기술: C언어, Raylib 라이브러리
- GitHub 링크: [여기에 본인의 GitHub 저장소 링크 입력]
- 주요 기능: 스페이스바/마우스 클릭을 통한 새의 점프 및 중력 물리 구현, 랜덤 높이를 가진 상하단 파이프 장애물 생성 및 스크롤, 실시간 충돌 판정 및 점수 집계 시스템

[소감]
1. 모바일에서 큰 인기를 끌었던 플래피 버드 게임을 C언어와 Raylib를 활용해 직접 구현해 보니, 간단한 규칙 속에서도 정교한 물리 연산과 충돌 검출의 중요성을 느낄 수 있었습니다.
2. 파이프의 랜덤 높이 생성과 무한 스크롤 로직을 구현하는 과정에서 배열 관리와 상태 변수 제어 능력이 크게 향상되었습니다.
3. 내가 만든 게임을 직접 플레이해 보면서 프로그래밍에 대한 흥미와 성취감이 배가 되었고, 앞으로 더 발전된 게임 로직을 구현해 보고 싶다는 동기부여가 되었습니다.
