#include "raylib.h"
#include <vector>
#include <string>
#include <iostream>

typedef struct Snake{
    Color color;
    std::vector<Vector2> pos;
    Vector2 size;
    int speed, length;
    std::vector<bool> dir;
} Snake;

typedef struct Food{
    Color color;
    Vector2 pos, size;
    bool active;
} Food;

typedef struct Power{
    
} Power;

const int screenWidth = 720;
const int screenHeight = 480;
const int squareSize = 24;
const int maxLength = 480;

// TODO:
// 4. Create obstacle
// 5. Reduce vision 
// 6. Laser beam 
// 7. Reduce length 
// 8. Can move faster/slower 
// 9. Immune to obstacle 
// 10. Food Frenzy

int main(){
    Snake snake = {
        BLACK, 
        {Vector2{0,96}}, 
        Vector2{squareSize, squareSize}, 
        2, 
        1, 
        {0,0,0,1}
    };
    Food food = {
        RED, 
        Vector2{float(GetRandomValue(0,screenWidth/squareSize-1)*squareSize),float(GetRandomValue(0, (screenHeight-96)/squareSize-1)*squareSize + 96)}, 
        Vector2{squareSize, squareSize}, 
        true
    };
    double startTime = GetTime();
    int score = 0;
    int highScore = 0;
    bool exitWindowRequested = false;
    bool pause = false;
    bool gameOver = false;
    snake.pos.reserve(maxLength);
    while(food.pos.x==0&&food.pos.y==96) Vector2{float(GetRandomValue(0,screenWidth/squareSize-1)*squareSize),float(GetRandomValue(0, (screenHeight-96)/squareSize-1)*squareSize + 96)};
    InitWindow(screenWidth, screenHeight, "Ula Gendeng");
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);
    while(!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(RAYWHITE);
        if(WindowShouldClose()||IsKeyPressed(KEY_ESCAPE)) exitWindowRequested = true;
        if(IsKeyPressed(KEY_P)) pause = !pause;
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
            if(IsKeyPressed(KEY_ENTER)){
                snake.pos = {Vector2{0,96}};
                snake.dir = {0,0,0,1};
                snake.speed = 2;
                snake.length = 1;
                food.active = false;
                gameOver = false;
            } 
        }
        else{
            DrawRectangle(0, 96, screenWidth, 384, WHITE);
            const char* titleText = "Ula Gendeng";
            const char* scoreText = TextFormat("SCORE: %i", score);
            const char* highScoreText = TextFormat("HI-SCORE: %i", highScore);
            int titleTextWidth = MeasureText(titleText, 48);
            int titleTextX = (screenWidth - titleTextWidth)/2;
            int titleTextY = 24;
            int scoreTextWidth = MeasureText(scoreText, 30);
            int scoreTextX = 0;
            int scoreTextY = 33; 
            int highScoreTextWidth = MeasureText(highScoreText, 30);
            int highScoreTextX = screenWidth - highScoreTextWidth;
            int highScoreTextY = 33;
            DrawText(titleText, titleTextX, titleTextY, 48, BLACK);
            DrawText(scoreText, scoreTextX, scoreTextY, 30, MAROON);
            DrawText(highScoreText, highScoreTextX, highScoreTextY, 30, BLUE);
            for(int i=0;i<snake.length;i++) DrawRectangleV(snake.pos[i], snake.size, snake.color);
            if(food.active) DrawRectangleV(food.pos, food.size, food.color);
            else{
                food.pos = Vector2{float(GetRandomValue(0,screenWidth/squareSize-1)*squareSize),float(GetRandomValue(0, (screenHeight-96)/squareSize-1)*squareSize + 96)};
                for(auto filled:snake.pos){
                    while(food.pos.x==filled.x&&food.pos.y==filled.y) Vector2{float(GetRandomValue(0,screenWidth/squareSize-1)*squareSize),float(GetRandomValue(0, (screenHeight-96)/squareSize-1)*squareSize + 96)};
                }
                food.active = true;
            }
            if(snake.pos[0].x==food.pos.x&&snake.pos[0].y==food.pos.y){
                score++;
                food.active = false;
                snake.length++;
                snake.pos.push_back({0,0});
            }
            for(int i=snake.length-1;i>0;i--) snake.pos[i]=snake.pos[i-1];
            if(GetTime()-startTime>=(1/snake.speed)){
                if(snake.dir[0]) snake.pos[0].x-=squareSize;
                else if(snake.dir[1]) snake.pos[0].y-=squareSize;
                else if(snake.dir[2]) snake.pos[0].x+=squareSize;
                else snake.pos[0].y+=squareSize;
                startTime=GetTime();
            }
            if(snake.pos[0].x<0||snake.pos[0].x>720||snake.pos[0].y<96||snake.pos[0].y>480){
                gameOver = true;
                highScore = std::max(highScore, score);
                score = 0;
            }
        }
        EndDrawing();
    }
    CloseWindow();
}