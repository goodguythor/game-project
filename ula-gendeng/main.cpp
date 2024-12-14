#include "raylib.h"
#include <iostream>

static const int screenWidth = 720;
static const int screenHeight = 480;

// static bool lose = false;
// static bool pause = false;

// void initGame(){
//     lose = false;
//     pause = false;
// }

int main(){
    InitWindow(screenWidth, screenHeight, "Ula Gendeng");
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);
    bool exitWindowRequested = false;
    while(!WindowShouldClose()){
        if(WindowShouldClose() || IsKeyPressed(KEY_ESCAPE)){
            exitWindowRequested = true;
        }
        BeginDrawing();
        ClearBackground(RAYWHITE);
        const char* titleText = "Ula Gendeng";
        int titleTextWidth = MeasureText(titleText, 48);
        int titleTextX = (screenWidth - titleTextWidth)/2;
        int titleTextY = 24;
        DrawText(titleText, titleTextX, titleTextY, 48, BLACK);
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
        EndDrawing();
    }
    CloseWindow();
}