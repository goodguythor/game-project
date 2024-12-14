#include "raylib.h"
#include <vector>
#include <string>
#include <iostream>

typedef struct Snake{
    std::vector<Vector2> pos;
    Vector2 size;
    std::vector<bool> dir;
    int speed, length;
    Color color;
} Snake;

typedef struct Food{
    Vector2 pos, size;
    Color color;
    bool active;
} Food;

const int screenWidth = 720;
const int screenHeight = 480;
const int squareSize = 48;
const int maxLength = 100;

// TODO:
// 3. Create food 
// 4. Create obstacle
// 5. Reduce vision 
// 6. Laser beam 
// 7. Reduce length 
// 8. Can move faster/slower 
// 9. Immune to obstacle 
// 10. Food Frenzy

int main(){
    InitWindow(screenWidth, screenHeight, "Ula Gendeng");
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);
    double startTime = GetTime();
    int score = 0;
    int highScore = 1000;
    bool exitWindowRequested = false;
    bool pause = false;
    bool gameOver = false;
    Snake snake = {{Vector2{0,96},Vector2{0,144}}, Vector2{squareSize, squareSize}, {0,0,0,1}, 2, 2, BLACK};
    snake.pos.reserve(maxLength);
    while(!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(RAYWHITE);
        if(WindowShouldClose()||IsKeyPressed(KEY_ESCAPE)) exitWindowRequested = true;
        if(IsKeyPressed(KEY_P)) pause = !pause;
        if(IsKeyPressed(KEY_SPACE)) gameOver = !gameOver;
        if(IsKeyPressed(KEY_LEFT)||IsKeyPressed(KEY_A)){
            if(snake.dir[1]||snake.dir[3]) snake.dir[0]=true;
            if(snake.dir[1]) snake.dir[1]=false;
            else if(snake.dir[3]) snake.dir[3]=false;
        }
        if(IsKeyPressed(KEY_UP)||IsKeyPressed(KEY_W)){
            if(snake.dir[0]||snake.dir[2]) snake.dir[1]=true;
            if(snake.dir[0]) snake.dir[0]=false;
            else if(snake.dir[2]) snake.dir[2]=false;
        }
        if(IsKeyPressed(KEY_RIGHT)||IsKeyPressed(KEY_D)){
            if(snake.dir[1]||snake.dir[3]) snake.dir[2]=true;
            if(snake.dir[1]) snake.dir[1]=false;
            else if(snake.dir[3]) snake.dir[3]=false;
        }
        if(IsKeyPressed(KEY_DOWN)||IsKeyPressed(KEY_S)){
            if(snake.dir[0]||snake.dir[2]) snake.dir[3]=true;
            if(snake.dir[0]) snake.dir[0]=false;
            else if(snake.dir[2]) snake.dir[2]=false;
        }
        if(exitWindowRequested){
            DrawRectangle(0, 140, screenWidth, 200, BLACK);
            const char* exitText = "Are you sure you want to exit program? [Y/N]"; 
            int exitTextWidth = MeasureText(exitText, 30);
            int exitTextX = (screenWidth - exitTextWidth)/2;
            int exitTextY = (screenHeight - 30)/2;
            DrawText(exitText, exitTextX, exitTextY, 30, WHITE);
            if(IsKeyPressed(KEY_Y)) break;
            else if(IsKeyPressed(KEY_N)) exitWindowRequested = false;
        }
        else if(pause){
            DrawRectangle(0, 140, screenWidth, 200, BLACK);
            const char* exitText = "PAUSED"; 
            int exitTextWidth = MeasureText(exitText, 30);
            int exitTextX = (screenWidth - exitTextWidth)/2;
            int exitTextY = (screenHeight - 30)/2;
            DrawText(exitText, exitTextX, exitTextY, 30, WHITE);
        }
        else if(gameOver){
            DrawRectangle(0, 140, screenWidth, 200, BLACK);
            const char* gameOverText = "Game Over";
            const char* scoreText = TextFormat("SCORE: %i", score);
            const char* highScoreText = TextFormat("HI-SCORE: %i", highScore);
            int gameOverTextWidth = MeasureText(gameOverText, 48);
            int gameOverTextX = (screenWidth - gameOverTextWidth)/2;
            int gameOverTextY = 24;
            int scoreTextWidth = MeasureText(scoreText, 30);
            int scoreTextX = (screenWidth - scoreTextWidth)/2;
            int scoreTextY = (screenHeight - 60)/2; 
            int highScoreTextWidth = MeasureText(highScoreText, 30);
            int highScoreTextX = (screenWidth - highScoreTextWidth)/2;
            int highScoreTextY = screenHeight/2;
            DrawText(gameOverText, gameOverTextX, gameOverTextY, 48, RED);
            DrawText(scoreText, scoreTextX, scoreTextY, 30, MAROON);
            DrawText(highScoreText, highScoreTextX, highScoreTextY, 30, RAYWHITE);
        }
        else{
            DrawRectangle(0, 96, screenWidth, 384, WHITE);
            const char* titleText = "Ula Gendeng";
            int titleTextWidth = MeasureText(titleText, 48);
            int titleTextX = (screenWidth - titleTextWidth)/2;
            int titleTextY = 24;
            DrawText(titleText, titleTextX, titleTextY, 48, BLACK);
            for(int i=0;i<snake.length;i++) DrawRectangleV(snake.pos[i], snake.size, snake.color);
            for(int i=snake.length-1;i>0;i--) snake.pos[i]=snake.pos[i-1];
            if(GetTime()-startTime>=(1/snake.speed)){
                if(snake.dir[0]) snake.pos[0].x-=squareSize;
                else if(snake.dir[1]) snake.pos[0].y-=squareSize;
                else if(snake.dir[2]) snake.pos[0].x+=squareSize;
                else snake.pos[0].y+=squareSize;
                startTime=GetTime();
            } 
        }
        EndDrawing();
    }
    CloseWindow();
}