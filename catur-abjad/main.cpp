#include "raylib.h"
#include <vector>
#include <string>
#include <iostream>

// TODO: IMPLEMENT GAME LOGIC

static std::vector<std::vector<int>> grid(8,std::vector<int>(8));
static std::vector<int> clickedPiece{-1,-1};
static const int screenWidth = 720;
static const int screenHeight = 720;
static int move = 0;
static bool runWindow = true;
static bool clicked = false;
static bool gameOver = false;

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
    if(gameOver){
        Vector2 textPos;
        Vector2 textSize=MeasureTextEx(GetFontDefault(),"Game Over",30,1);
        Color color=BLACK;
        float centerX=360,centerY=60;
        textPos.x=centerX-(textSize.x/2);
        textPos.y=centerY-(textSize.y/2);
        DrawTextEx(GetFontDefault(),"Game Over",textPos,30,1,color);
        // DrawText("Game Over",0,0,64,BLACK);
    }
    if(clicked){
        int x=clickedPiece[0],y=clickedPiece[1];
        int piece=grid[x][y]%5;
        int posX=120+x*60,posY=120+y*60;
        bool side=grid[x][y]/5;
        DrawRectangle(posX,posY,60,60,WHITE);
        if(piece==1){
            if(x>0&&grid[x-1][y]/5!=side) DrawRectangle(posX-60,posY,60,60,WHITE);
            if(x<7&&grid[x+1][y]/5!=side) DrawRectangle(posX+60,posY,60,60,WHITE);
            if(y>0&&grid[x][y-1]/5!=side) DrawRectangle(posX,posY-60,60,60,WHITE);
            if(y<7&&grid[x][y+1]/5!=side) DrawRectangle(posX,posY+60,60,60,WHITE);
        }
        else if(piece==0){
            if(x>0){
                if(y>0&&grid[x-1][y-1]/5!=side) DrawRectangle(posX-60,posY-60,60,60,WHITE);
                if(y<7&&grid[x-1][y+1]/5!=side) DrawRectangle(posX-60,posY+60,60,60,WHITE); 
            }
            if(x<7){
                if(y>0&&grid[x+1][y-1]/5!=side) DrawRectangle(posX+60,posY-60,60,60,WHITE);
                if(y<7&&grid[x+1][y+1]/5!=side) DrawRectangle(posX+60,posY+60,60,60,WHITE); 
            }
        } 
        else if(piece==2){
            if(x>0){
                for(int i=1;x-i>=0&&i<=y;i++){
                    int place=grid[x-i][y-i];
                    if(place/5!=side){
                        DrawRectangle(posX-60*i,posY-60*i,60,60,WHITE);
                        if(place!=10) break;
                    } 
                    else break;
                }
                for(int i=1;x-i>=0&&i<8-y;i++){
                    int place=grid[x-i][y+i];
                    if(place/5!=side){
                        DrawRectangle(posX-60*i,posY+60*i,60,60,WHITE);
                        if(place!=10) break;
                    } 
                    else break;
                } 
            }
            if(x<7){
                for(int i=1;x+i<8&&i<=y;i++){
                    int place=grid[x+i][y-i];
                    if(place/5!=side){
                        DrawRectangle(posX+60*i,posY-60*i,60,60,WHITE);
                        if(place!=10) break;
                    }
                    else break;
                }
                for(int i=1;x+i<8&&i<8-y;i++){
                    int place=grid[x+i][y+i];
                    if(place/5!=side){
                        DrawRectangle(posX+60*i,posY+60*i,60,60,WHITE);
                        if(place!=10) break;
                    }
                    else break;
                } 
            }
        } 
        else if(piece==3){
            for(int i=1;i<=y;i++){
                int place=grid[x][y-i];
                if(place/5!=side){
                    DrawRectangle(posX,posY-60*i,60,60,WHITE);
                    if(place!=10) break;
                }
                else break;
            }
            for(int i=1;i<8-y;i++){
                int place=grid[x][y+i];
                if(place/5!=side){
                    DrawRectangle(posX,posY+60*i,60,60,WHITE);
                    if(place!=10) break;
                }
                else break;
            }
            for(int i=1;i<=x;i++){
                int place=grid[x-i][y];
                if(place/5!=side){
                    DrawRectangle(posX-60*i,posY,60,60,WHITE);
                    if(place!=10) break;
                }
                else break;
            }
            for(int i=1;i<8-x;i++){
                int place=grid[x+i][y];
                if(place/5!=side){
                    DrawRectangle(posX+60*i,posY,60,60,WHITE);
                    if(place!=10) break;
                }
                else break;
            }
        }
        else{
            if(x>0){
                if(grid[x-1][y]/5!=side) DrawRectangle(posX-60,posY,60,60,WHITE);
                if(y>0&&grid[x-1][y-1]/5!=side) DrawRectangle(posX-60,posY-60,60,60,WHITE);
                if(y<7&&grid[x-1][y+1]/5!=side) DrawRectangle(posX-60,posY+60,60,60,WHITE); 
            }
            if(x<7){
                if(grid[x+1][y]/5!=side) DrawRectangle(posX+60,posY,60,60,WHITE);
                if(y>0&&grid[x+1][y-1]/5!=side) DrawRectangle(posX+60,posY-60,60,60,WHITE);
                if(y<7&&grid[x+1][y+1]/5!=side) DrawRectangle(posX+60,posY+60,60,60,WHITE); 
            }
            if(y>0&&grid[x][y-1]/5!=side) DrawRectangle(posX,posY-60,60,60,WHITE);
            if(y<7&&grid[x][y+1]/5!=side) DrawRectangle(posX,posY+60,60,60,WHITE);
        }
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
            Vector2 textPos;
            Vector2 textSize=MeasureTextEx(GetFontDefault(),c,30,1);
            textPos.x=centerX-(textSize.x/2);
            textPos.y=centerY-(textSize.y/2);
            DrawTextEx(GetFontDefault(),c,textPos,30,1,color);
        }
    }
}

static void update(){
    if(WindowShouldClose()||IsKeyPressed(KEY_ESCAPE)) runWindow = 0;
    Vector2 mousePos = GetMousePosition();
    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        if(gameOver){
            init();
            clicked=false;
            gameOver=false;
        }
        else{
            int mouseX=(mousePos.x - 120)/60;
            int mouseY=(mousePos.y - 120)/60;
            if(validGrid(mouseX,mouseY)){
                int piece=grid[mouseX][mouseY];
                // std::cout<<piece<<'\n';
                if(!clicked&&piece!=10){
                    bool side=grid[mouseX][mouseY]/5;
                    // std::cout<<move<<' '<<side<<'\n';
                    if((move&1)==side){
                        clicked=1;
                        clickedPiece={mouseX,mouseY};    
                    }
                }
                else if(clicked){
                    int x=clickedPiece[0],y=clickedPiece[1];
                    int piece=grid[x][y]%5,enemy=grid[mouseX][mouseY];;
                    bool side=grid[x][y]/5;
                    bool good=0;
                    if(piece==1){
                        if(abs(mouseX-x)+abs(mouseY-y)==1&&enemy/5!=side){
                            grid[mouseX][mouseY]=grid[x][y]; 
                            good=1;
                        }
                    }
                    else if(piece==0){
                        if(abs(mouseX-x)==1&&abs(mouseY-y)==1&&enemy/5!=side){
                            grid[mouseX][mouseY]=grid[x][y]; 
                            good=1;
                        }
                    } 
                    else if(piece==2){
                        int difX=abs(mouseX-x),difY=abs(mouseY-y);
                        if(difX==difY&&enemy/5!=side){
                            bool check=1;
                            for(int i=1;i<difX;i++){
                                if(mouseX>x){
                                    if(mouseY>y){
                                        if(grid[x+i][y+i]!=10){
                                            check=0;
                                            break;
                                        }
                                    }
                                    else{
                                        if(grid[x+i][y-i]!=10){
                                            check=0;
                                            break;
                                        }
                                    }
                                }
                                else{
                                    if(mouseY>y){
                                        if(grid[x-i][y+i]!=10){
                                            check=0;
                                            break;
                                        }
                                    }
                                    else{
                                        if(grid[x-i][y-i]!=10){
                                            check=0;
                                            break;
                                        }
                                    }
                                }
                            }
                            if(check){
                                grid[mouseX][mouseY]=grid[x][y]; 
                                good=1;
                            }
                        }
                    } 
                    else if(piece==3){
                        int difX=abs(mouseX-x),difY=abs(mouseY-y);
                        int mx=std::max(difX,difY),mn=std::min(difX,difY);
                        if(mx>0&&mn==0&&enemy/5!=side){
                            bool check=1;
                            for(int i=1;i<mx;i++){
                                if(mx==difX){
                                    if(mouseX>x){
                                        if(grid[x+i][y]!=10){
                                            check=0;
                                            break;
                                        }
                                    }
                                    else{
                                        if(grid[x-i][y]!=10){
                                            check=0;
                                            break;
                                        }
                                    }
                                }
                                else{
                                    if(mouseY<y){
                                        if(grid[x][y-i]!=10){
                                            check=0;
                                            break;
                                        }
                                    }
                                    else{
                                        if(grid[x][y+i]!=10){
                                            check=0;
                                            break;
                                        }
                                    }
                                }
                            }
                            if(check){
                                grid[mouseX][mouseY]=grid[x][y]; 
                                good=1;
                            }   
                        }
                    }
                    else{
                        if((abs(mouseX-x)<=1)&&(abs(mouseY-y)<=1)&&grid[mouseX][mouseY]/5!=side){
                            grid[mouseX][mouseY]=grid[x][y]; 
                            good=1;
                        }
                    }
                    if(good){
                        if(enemy%5==4){
                            gameOver=1;
                        }
                        grid[x][y]=10;
                        move++;
                    }
                    clicked=0;
                    clickedPiece={-1,-1};
                }
            }
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