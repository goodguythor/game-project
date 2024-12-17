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

static const int screenWidth = 720;
static const int screenHeight = 480;
static const int squareSize = 24;
static const int maxLength = 480;

static std::vector<Food> powerUp{
    {
        YELLOW,
        Vector2{float(GetRandomValue(0,screenWidth/squareSize-1)*squareSize),float(GetRandomValue(0, (screenHeight-96)/squareSize-1)*squareSize + 96)}, 
        Vector2{squareSize, squareSize}, 
        false
    },
    {
        GRAY,
        Vector2{float(GetRandomValue(0,screenWidth/squareSize-1)*squareSize),float(GetRandomValue(0, (screenHeight-96)/squareSize-1)*squareSize + 96)}, 
        Vector2{squareSize, squareSize}, 
        false
    },
    {
        GREEN,
        Vector2{float(GetRandomValue(0,screenWidth/squareSize-1)*squareSize),float(GetRandomValue(0, (screenHeight-96)/squareSize-1)*squareSize + 96)}, 
        Vector2{squareSize, squareSize}, 
        false
    },
    {
        BROWN,
        Vector2{float(GetRandomValue(0,screenWidth/squareSize-1)*squareSize),float(GetRandomValue(0, (screenHeight-96)/squareSize-1)*squareSize + 96)}, 
        Vector2{squareSize, squareSize}, 
        false
    }
};
static Snake snake = {
    BLACK, 
    {Vector2{0,96}}, 
    Vector2{squareSize, squareSize}, 
    1, 
    1, 
    {0,0,0,1}
};
static Food food = {
    RED, 
    Vector2{float(GetRandomValue(0,screenWidth/squareSize-1)*squareSize),float(GetRandomValue(0, (screenHeight-96)/squareSize-1)*squareSize + 96)}, 
    Vector2{squareSize, squareSize}, 
    true
};
static std::vector<int> powerDuration{3,4,2,3};
static std::vector<std::vector<bool>> filled(31,std::vector<bool>(16,false));
// static double startTime = GetTime();
static double powerTime = GetTime();
static double powerEnd;
static int score = 0;
static int highScore = 0;
static int cur = -1;
static bool exitWindowRequested = false;
static bool pause = false;
static bool gameOver = false;
static bool powered = false;
static bool runWindow = true;

// TODO:
// 1. Breakdown init, update, draw game
// 2. Use frames instead of timer
// 4. Create obstacle
// 5. Reduce vision 
// 6. Laser beam 
// 7. Reduce length 
// 8. Can move faster/slower 
// 9. Immune to obstacle 
// 10. Food Frenzy


// TODO:
// use static and move all the init variable here
static void init(){
    snake.pos.reserve(maxLength);
    filled[snake.pos[0].x/squareSize][snake.pos[0].y/squareSize] = true;
    filled[food.pos.x/squareSize][food.pos.y/squareSize] = true;
    while(food.pos.x==0&&food.pos.y==96) food.pos = Vector2{float(GetRandomValue(0,screenWidth/squareSize-1)*squareSize),float(GetRandomValue(0, (screenHeight-96)/squareSize-1)*squareSize + 96)};
    // for(auto& power:powerUp){
    //     while(power.pos.x==0&&power.pos.y==96) power.pos = Vector2{float(GetRandomValue(0,screenWidth/squareSize-1)*squareSize),float(GetRandomValue(0, (screenHeight-96)/squareSize-1)*squareSize + 96)};
    // }
    InitWindow(screenWidth, screenHeight, "Ula Gendeng");
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);
}

// TODO:
// draw arena, food, snake, powerups
static void draw(){
    if(exitWindowRequested){
        DrawRectangle(0, 140, screenWidth, 200, BLACK);
        const char* exitText = "Are you sure you want to exit program? [Y/N]"; 
        int exitTextWidth = MeasureText(exitText, 30);
        int exitTextX = (screenWidth - exitTextWidth)/2;
        int exitTextY = (screenHeight - 30)/2;
        DrawText(exitText, exitTextX, exitTextY, 30, WHITE);
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
        // std::cout<<snake.pos[0].x<<' '<<snake.pos[0].y<<'\n';
        // std::cout<<filled[snake.pos[0].x][snake.pos[0].y]<<'\n';
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
        if(cur!=-1&&!powered) DrawRectangleV(powerUp[cur].pos, powerUp[cur].size, powerUp[cur].color);
    }
}

// TODO:
// update snake, food and powerups behaviour & movement. 
static void update(){
    if(exitWindowRequested){
        if(IsKeyPressed(KEY_Y)) runWindow = false;
        else if(IsKeyPressed(KEY_N)) exitWindowRequested = false;
    }
    else if(gameOver){
        if(IsKeyPressed(KEY_ENTER)){
            snake.pos = {Vector2{0,96}};
            snake.dir = {0,0,0,1};
            snake.speed = 1;
            snake.length = 1;
            food.active = false;
            if(cur!=-1){
                powerUp[cur].active = false;
                powerUp[cur].pos = Vector2{float(GetRandomValue(0,screenWidth/squareSize-1)*squareSize),float(GetRandomValue(0, (screenHeight-96)/squareSize-1)*squareSize + 96)};
                powerTime = GetTime();
                powered = false;
            }
            // startTime = GetTime();
            cur = -1;
            gameOver = false;
        }
    }
    else if(!pause){
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
        if(!food.active){
            filled[food.pos.x/squareSize][food.pos.y/squareSize] = false;
            food.pos = Vector2{float(GetRandomValue(0,screenWidth/squareSize-1)*squareSize),float(GetRandomValue(0, (screenHeight-96)/squareSize-1)*squareSize + 96)};
            while(filled[food.pos.x/squareSize][food.pos.y/squareSize]) food.pos = Vector2{float(GetRandomValue(0,screenWidth/squareSize-1)*squareSize),float(GetRandomValue(0, (screenHeight-96)/squareSize-1)*squareSize + 96)};
            filled[food.pos.x/squareSize][food.pos.y/squareSize] = true;
            food.active = true;
        }
        if(cur!=-1&&!powered){ 
            if(snake.pos[0].x==powerUp[cur].pos.x&&snake.pos[0].y==powerUp[cur].pos.y){
                if(cur==0) snake.speed = 5;
                else if(cur==1) snake.speed = 3;
                else if(cur==2) snake.speed = 2;
                else snake.speed = 2;
                powered = true;
                powerTime = GetTime();
            }
            if(GetTime()-powerEnd>=5){
                cur = -1;
                powerTime = GetTime();
            }
        }
        if(cur==-1&&GetTime()-powerTime>=5){
            int mx = powerUp.size()-1;
            cur = GetRandomValue(0,mx);
            powerUp[cur].active = true;
            powerEnd = GetTime();
        }
        if(powered&&GetTime()-powerTime>=powerDuration[cur]){
            snake.speed = 1;
            powerUp[cur].active = false;
            powerUp[cur].pos = Vector2{float(GetRandomValue(0,screenWidth/squareSize-1)*squareSize),float(GetRandomValue(0, (screenHeight-96)/squareSize-1)*squareSize + 96)};
            powerTime = GetTime();
            cur = -1;
            powered = false;
        }
        filled[snake.pos[snake.length-1].x/squareSize][snake.pos[snake.length-1].y/squareSize] = false;
        int dist = squareSize*snake.speed;
        for(int i=snake.length-1;i>0;i--) snake.pos[i] = snake.pos[i-1];
        if(snake.dir[0]) snake.pos[0].x-=dist;
        else if(snake.dir[1]) snake.pos[0].y-=dist;
        else if(snake.dir[2]) snake.pos[0].x+=dist;
        else snake.pos[0].y+=dist;
        if(snake.pos[0].x==food.pos.x&&snake.pos[0].y==food.pos.y){
            score++;
            food.active = false;
            snake.pos.push_back({snake.pos[snake.length-1].x,snake.pos[snake.length-1].y});
            snake.length++;
        }
        else if(snake.pos[0].x<0||snake.pos[0].x>screenWidth||snake.pos[0].y<96||snake.pos[0].y>screenHeight||filled[snake.pos[0].x/squareSize][snake.pos[0].y/squareSize]){
            gameOver = true;
            highScore = std::max(highScore, score);
            score = 0;
        }
        if(!gameOver) filled[snake.pos[0].x/squareSize][snake.pos[0].y/squareSize] = true;
    }
    if(WindowShouldClose()||IsKeyPressed(KEY_ESCAPE)) exitWindowRequested = true;
    if(IsKeyPressed(KEY_P)) pause = !pause;
}

int main(){
    init();
    while(runWindow){
        BeginDrawing();
        ClearBackground(RAYWHITE);
        update();
        draw();
        EndDrawing();
    }
    CloseWindow();
}