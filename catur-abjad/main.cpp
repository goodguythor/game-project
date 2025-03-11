#include "raylib.h"
#include <vector>
#include <string>
#include <iostream>

static std::vector<std::vector<int>> grid(8,std::vector<int>(8));
static std::vector<int> clickedPiece(2);
static const int screenWidth = 720;
static const int screenHeight = 720;
static bool runWindow = true;
static bool clicked = false;

static bool validGrid(int x,int y){
    return x>=0&&x<8&&y>=0&&y<8;
}

static void init(){
    for(int i=2;i<6;i++){
        for(int j=0;j<8;j++) grid[i][j]=10;
    }
    for(int i=0;i<2;i++){
        grid[i][0]=0;
        grid[i][7]=0;
    }
    for(int i=6;i<8;i++){
        grid[i][0]=5;
        grid[i][7]=5;
    }
    for(int i=1;i<7;i++){
        grid[1][i]=1;
        grid[6][i]=6;
    }
    for(int j=1;j<8;j+=4){
        grid[0][j]=2;
        grid[0][j+1]=2;
    }
    for(int j=1;j<8;j+=4){
        grid[7][j]=7;
        grid[7][j+1]=7;
    }
    grid[0][3]=3;
    grid[0][4]=4;
    grid[7][3]=9;
    grid[7][4]=8;
}

static void draw(){
    if(clicked){
        int x=clickedPiece[0],y=clickedPiece[1];
        int piece=grid[x][y]%5;
        int posX=120+x*60,posY=120+y*60;
        bool side=grid[x][y]/5;

        DrawRectangle(posX,posY,60,60,WHITE);
        if(piece==1){
            if(x>0&&grid[x-1][y]/5!=side) DrawRectangle(posX-60,posY,60,60,WHITE);
            else if(x<7&&grid[x+1][y]/5!=side) DrawRectangle(posX+60,posY,60,60,WHITE);
            else if(y>0&&grid[x][y-1]/5!=side) DrawRectangle(posX,posY-60,60,60,WHITE);
            else if(y<7&&grid[x][y+1]/5!=side) DrawRectangle(posX,posY+60,60,60,WHITE);
        }
        // else if(piece==1) 
        // else if(piece==3) 
        // else if(piece==4) 
        // else 
    }
    // DrawRectangle(120,120,480,480, WHITE);
    for(int i=120;i<=600;i+=60) DrawLine(i,120,i,600,BLACK);
    for(int i=120;i<=600;i+=60) DrawLine(120,i,600,i,BLACK);
    // for(int i=0;i<8;i++){
    //     for(int )
    // }
    for(int i=0;i<8;i++){
        for(int j=0;j<8;j++){
            if(grid[i][j]==10) continue;
            Vector2 textPos;
            Color color=clicked&&clickedPiece==std::vector<int>{i,j}?BLUE:BLACK;
            if(grid[i][j]/5) color=clicked&&clickedPiece==std::vector<int>{i,j}?GREEN:GRAY;
            char* c;
            int piece=grid[i][j]%5;
            float centerX=150+60*i, centerY=150+60*j;
            if(piece==0) c="C";
            else if(piece==1) c="I";
            else if(piece==2) c="X";
            else if(piece==3) c="T";
            else c="U";
            // std::cout<<grid[i][j]<<'\n';
            Vector2 textSize=MeasureTextEx(GetFontDefault(),c,30,1);
            textPos.x=centerX-(textSize.x/2);
            textPos.y=centerY-(textSize.y/2);
            DrawTextEx(GetFontDefault(),c,textPos,30,1,color);
        }
    }
}

static void update(){
    Vector2 mousePos = GetMousePosition();
    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        
        int mouseX=(mousePos.x - 120)/60;
        int mouseY=(mousePos.y - 120)/60;
        if(validGrid(mouseX,mouseY)){
            int piece=grid[mouseX][mouseY];
            if(!clicked&&piece>0){
                clicked=1;
                clickedPiece={mouseX,mouseY};
            }
            else if(clicked){
                clicked=0;
                clickedPiece.clear();
            }
            // if(piece==1){

            // }
            // else if(piece==2) 
            // else if(piece==3) 
            // else if(piece==4) 
            // else if(piece==5) 
        }
    }
}

int main(){
    // init window
    InitWindow(screenWidth, screenHeight, "Catur Abjad");
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);
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