#include "raylib.h"
#include <vector>
// #include <string>
// #include <cstdint>
#include <iostream>
#include <filesystem>
#include <cstring>

// TODO: IMPLEMENT GAME LOGIC

#define MAX_SONG 32

typedef struct{
    int timeLeft;
    int increment;
} PlayerClock;

enum class GameState{
    GameScreen,
    MenuScreen,
    StartScreen  
};

static PlayerClock playerBlack, playerGray;
static std::vector<Music> song;
static std::vector<const char*> songList;
static std::vector<std::vector<int>> grid(8,std::vector<int>(8));
static std::pair<int,int> graveyard[5] = {{0,0},{0,0},{0,0},{0,0},{0,0}};
static std::vector<Vector2> pieceSize;
static Vector2 piecePos[8][8];
static Rectangle nextButtonsBounds{630,315,30,30};
static Rectangle prevButtonsBounds{60,315,30,30};
static Rectangle volumeUpBounds{630,375,30,30};
static Rectangle volumeDownBounds{60,375,30,30};
static Rectangle volumeRec[10];
static Rectangle blackTitleBox, grayTitleBox;
static Rectangle playGameBox;
static Rectangle menuBox;
static Rectangle gameTimeIncBox,gameTimeDecBox;
static Rectangle incTimeIncBox,incTimeDecBox;
static Vector2 textSoundSize;
static Vector2 textSoundPos;
static Rectangle musicButtonBounds;
static Vector2 overSize;
static Vector2 overPos;
static Vector2 bTurnSize;
static Vector2 bTurnPos;
static Vector2 gTurnSize;
static Vector2 gTurnPos;
static Vector2 bTimerSize;
static Vector2 bTimerPos;
static Vector2 gTimerSize;
static Vector2 gTimerPos;
static Vector2 resetSize;
static Vector2 resetPos;
static Vector2 startSize;
static Vector2 startPos;
static Vector2 menuSize;
static Vector2 menuPos;
static Vector2 choiceSize;
static Vector2 choicePos;
static Vector2 volumeSize;
// static Vector2 volumePos;
static float barLength;
static Vector2 vUpSize;
static Vector2 vUpPos;
static Vector2 vDownSize;
static Vector2 vDownPos;
static Vector2 nextSize;
static Vector2 nextPos;
static Vector2 prevSize;
static Vector2 prevPos;
static Vector2 blackTimerSize, blackTimerPos;
static Vector2 grayTimerSize, grayTimerPos;
static Vector2 turnTextSize, turnTextPos;
static Vector2 titleSize, titlePos;
static Vector2 enterSize, enterPos;
static Vector2 blackTitleSize, blackTitlePos;
static Vector2 grayTitleSize, grayTitlePos;
static Vector2 titleMenuSize, titleMenuPos;
static Vector2 timerMenuSize, timerMenuPos;
static Vector2 incrementSize, incrementPos;
static Vector2 playGameSize, playGamePos;
static Vector2 gameTimePos, incTimePos;
static Vector2 gameTimeSize, incTimeSize;
static Vector2 gameTimeIncPos,gameTimeDecPos;
static Vector2 incTimeIncPos,incTimeDecPos;
static std::vector<std::pair<Vector2,int>> bGravePos(16),gGravePos(16);
static const char* characters="CIXTU";
static char blackTimer[6];
static char grayTimer[6];
static char turnText[8];
static float timer, holdTimer, blinkTimer=0, pauseTime = 0;
static const int screenWidth = 720;
static const int screenHeight = 720;
static int cntL=0,cntR=0;
static int moveCount = 0;
static int curSong = 0;
static int masterVolume = 10;
static int x=-1,y=-1;
static int posX=-1,posY=-1;
static int gameTime=5,incTime=3;
static GameState state = GameState::StartScreen;
static bool runWindow = 1;
static bool clicked = 0;
static bool gameOver = 0;
static bool gameStart = 0;
static bool musicButtonActive = 0;
static bool menuBoxActive = 0;
static bool isHold=0;
static bool isBlinking=0;

static inline bool validGrid(int x,int y){
    return x>=0&&x<8&&y>=0&&y<8;
}

static inline void changeSong(bool nxt){
    StopMusicStream(song[curSong]);
    curSong=(curSong+(nxt?1:-1))%songList.size();
    PlayMusicStream(song[curSong]);
}

static inline void clearGame(){
    clicked=0;
    gameOver=0;
    gameStart=0;   
    cntL=0;
    cntR=0; 
    for(int i=0;i<5;i++) graveyard[i]={0,0};
    moveCount=0;
}

static inline void handleMusicBox(const Vector2 &mousePos){
    if(CheckCollisionPointRec(mousePos, nextButtonsBounds)){
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) changeSong(1);
        else if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
            if(!isHold){
                isHold=1;
                holdTimer=GetTime();
            }
            else if(GetTime()-holdTimer>=0.2){
                holdTimer=GetTime();
                changeSong(1);
            } 
        }
    }
    if(CheckCollisionPointRec(mousePos, prevButtonsBounds)){
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) changeSong(0);
        else if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
            if(!isHold){
                isHold=1;
                holdTimer=GetTime();
            }
            else if(GetTime()-holdTimer>=0.2){
                holdTimer=GetTime();
                changeSong(0);
            } 
        }
    }
    if(CheckCollisionPointRec(mousePos, volumeUpBounds)){
        if(masterVolume<10&&IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            masterVolume++;
            SetMasterVolume(masterVolume/10.f);
        }
        else if(masterVolume<10&&IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
            if(!isHold){
                isHold=1;
                holdTimer=GetTime();
            }
            else if(GetTime()-holdTimer>=0.2){
                holdTimer=GetTime();
                masterVolume++;
                SetMasterVolume(masterVolume/10.f);
            }
        }
    }
    if(CheckCollisionPointRec(mousePos, volumeDownBounds)){
        if(masterVolume>0&&IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            masterVolume--;
            SetMasterVolume(masterVolume/10.f);
        }
        else if(masterVolume>0&&IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
            if(!isHold){
                isHold=1;
                holdTimer=GetTime();
            }
            else if(GetTime()-holdTimer>=0.2){
                holdTimer=GetTime();
                masterVolume--;
                SetMasterVolume(masterVolume/10.f);
            }
        }
    }
}

static void drawMusicBox(){
    DrawTextEx(GetFontDefault(),"Music",textSoundPos,30,1,BLACK);
    if(musicButtonActive){
        DrawRectangle(120,285,480,150,BLACK);
        DrawRectangle(60,315,30,30,BLACK);
        DrawRectangle(630,315,30,30,BLACK);
        DrawRectangle(60,375,30,30,BLACK);
        DrawRectangle(630,375,30,30,BLACK);
        DrawText(TextFormat("Song: %s", songList[curSong]), 150, 315, 30, WHITE);
        DrawText("Volume:", 150, 405-volumeSize.y, 30, WHITE);
        for(int i=0;i<masterVolume;i++) DrawRectangleRec(volumeRec[i],WHITE);
        DrawTextEx(GetFontDefault(),">",nextPos,30,1,WHITE);
        DrawTextEx(GetFontDefault(),"<",prevPos,30,1,WHITE);
        DrawTextEx(GetFontDefault(),"-",vDownPos,30,1,WHITE);
        DrawTextEx(GetFontDefault(),"+",vUpPos,30,1,WHITE);
    }
}

static void init(){
    ClearBackground(RAYWHITE);
    playerBlack = {
        .timeLeft = gameTime*60,
        .increment = incTime
    };

    playerGray = {
        .timeLeft = gameTime*60,
        .increment = incTime
    };

    sprintf(blackTimer, "%02d:%02d", playerBlack.timeLeft/60, playerBlack.timeLeft%60);
    blackTimerSize=MeasureTextEx(GetFontDefault(),blackTimer,30,1);
    blackTimerPos={120-(blackTimerSize.x/2),90-(blackTimerSize.y/2)};
    sprintf(grayTimer, "%02d:%02d", playerGray.timeLeft/60, playerGray.timeLeft%60);
    grayTimerSize=MeasureTextEx(GetFontDefault(),grayTimer,30,1);
    grayTimerPos={600-(grayTimerSize.x/2),90-(grayTimerSize.y/2)};
    sprintf(turnText, "Turn %d", moveCount+1);
    turnTextSize=MeasureTextEx(GetFontDefault(),turnText,30,1);
    turnTextPos={360-(turnTextSize.x/2),30-(turnTextSize.y/2)};

    for(int i=2;i<6;i++){
        for(int j=0;j<8;j++){
            grid[i][j]=10;
            piecePos[i][j]={150.f+60*i,150.f+60*j};
        } 
    }
    for(int i=0;i<2;i++){
        grid[i][0]=0;
        grid[i][7]=0;
        piecePos[i][0]={150.f+60*i-pieceSize[0].x/2,150.f-pieceSize[0].y/2};
        piecePos[i][7]={150.f+60*i-pieceSize[0].x/2,150.f+60*7-pieceSize[0].y/2};
    }
    for(int i=6;i<8;i++){
        grid[i][0]=5;
        grid[i][7]=5;
        piecePos[i][0]={150.f+60*i-pieceSize[0].x/2,150.f-pieceSize[0].y/2};
        piecePos[i][7]={150.f+60*i-pieceSize[0].x/2,150.f+60*7-pieceSize[0].y/2};
    }
    for(int i=1;i<7;i++){
        grid[1][i]=1;
        grid[6][i]=6;
        piecePos[1][i]={150.f+60*1-pieceSize[1].x/2,150.f+60*i-pieceSize[1].y/2};
        piecePos[6][i]={150.f+60*6-pieceSize[1].x/2,150.f+60*i-pieceSize[1].y/2};
    }
    for(int j=1;j<8;j+=4){
        grid[0][j]=2;
        grid[0][j+1]=2;
        piecePos[0][j]={150.f-pieceSize[2].x/2,150.f+60*j-pieceSize[2].y/2};
        piecePos[0][j+1]={150.f-pieceSize[2].x/2,150.f+60*(j+1)-pieceSize[2].y/2};
    }
    for(int j=1;j<8;j+=4){
        grid[7][j]=7;
        grid[7][j+1]=7;
        piecePos[7][j]={150.f+60*7-pieceSize[2].x/2,150.f+60*j-pieceSize[2].y/2};
        piecePos[7][j+1]={150.f+60*7-pieceSize[2].x/2,150.f+60*(j+1)-pieceSize[2].y/2};
    }
    grid[0][3]=3;
    grid[0][4]=4;
    grid[7][3]=9;
    grid[7][4]=8;
    piecePos[0][3]={150.f-pieceSize[3].x/2,150.f+60*3-pieceSize[3].y/2};
    piecePos[0][4]={150.f-pieceSize[4].x/2,150.f+60*4-pieceSize[4].y/2};
    piecePos[7][3]={150.f+60*7-pieceSize[3].x/2,150.f+60*3-pieceSize[3].y/2};
    piecePos[7][4]={150.f+60*7-pieceSize[4].x/2,150.f+60*4-pieceSize[4].y/2};
}

static inline void draw(){
    if(clicked){
        int piece=grid[x][y]%5;
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
    DrawTextEx(GetFontDefault(),"Music",textSoundPos,30,1,BLACK);
    // DrawRectangle(120,120,480,480, WHITE);
    for(int i=120;i<=600;i+=60) DrawLine(i,120,i,600,BLACK);
    for(int i=120;i<=600;i+=60) DrawLine(120,i,600,i,BLACK);
    // for(int i=0;i<8;i++){
    //     for(int )
    // }
    for(int i=0;i<8;i++){
        for(int j=0;j<8;j++){
            if(grid[i][j]==10) continue;
            bool side=grid[i][j]/5;
            // std::cout<<grid[i][j]<<'\n';
            char temp[2]={characters[grid[i][j]%5], '\0'};
            DrawTextEx(GetFontDefault(),temp,piecePos[i][j],30,1,(clicked && x == i && y == j) ? (side ? GREEN : BLUE) : (side ? GRAY : BLACK));
        }
    }
    // Refactor how to draw the pieces graveyard
    for(int i=0;i<cntL;i++){
        char temp[2]={characters[bGravePos[i].second], '\0'};
        DrawTextEx(GetFontDefault(),temp,bGravePos[i].first,30,1,GRAY);
    }
    for(int i=0;i<cntR;i++){
        char temp[2]={characters[gGravePos[i].second], '\0'};
        DrawTextEx(GetFontDefault(),temp,gGravePos[i].first,30,1,BLACK);
    }
    if(gameOver){
        DrawTextEx(GetFontDefault(),"Game Over",overPos,30,1,BLACK);
        DrawRectangle(120,285,480,150,BLACK);
        DrawTextEx(GetFontDefault(),"Click anywhere to reset the game!",resetPos,24,1,WHITE);
    }
    else{
        bool side=moveCount%2;
        Color color=side?GRAY:BLACK;
        DrawTextEx(GetFontDefault(),turnText,turnTextPos,30,1,color);
        DrawTextEx(GetFontDefault(),side?"Gray's Turn":"Black's Turn",side?bTurnPos:gTurnPos,30,1,color);
        DrawTextEx(GetFontDefault(),"Black's Timer",bTimerPos,30,1,BLACK);
        DrawTextEx(GetFontDefault(),"Gray's Timer",gTimerPos,30,1,GRAY);
        DrawTextEx(GetFontDefault(),blackTimer,blackTimerPos,30,1,BLACK);
        DrawTextEx(GetFontDefault(),grayTimer,grayTimerPos,30,1,GRAY);
    }
    if(!gameStart){
        DrawRectangle(120,285,480,150,BLACK);
        DrawTextEx(GetFontDefault(),"Click anywhere to start the game!",startPos,24,1,WHITE);
    }
    if(musicButtonActive) drawMusicBox();
    if(menuBoxActive){
        DrawRectangle(120,285,480,150,BLACK);
        DrawTextEx(GetFontDefault(),"Would you like to go back to main menu?",menuPos,24,1,WHITE);
        DrawTextEx(GetFontDefault(),"YES (Enter) or NO (ESC)",choicePos,30,1,WHITE);
    }
}

static inline void update(){
    // std::cout<<GetMasterVolume()<<'\n';
    if(WindowShouldClose()) runWindow = 0;
    if(!musicButtonActive&&IsKeyPressed(KEY_ESCAPE)){
        menuBoxActive=!menuBoxActive;
        if(menuBoxActive) pauseTime=GetTime();
        else pauseTime=GetTime()-pauseTime;
    }
    if(gameStart&&!menuBoxActive&&!musicButtonActive&&!gameOver&&GetTime()-pauseTime-timer>=1){
        if(moveCount&1){
            playerGray.timeLeft--;
            sprintf(grayTimer, "%02d:%02d", playerGray.timeLeft/60, playerGray.timeLeft%60);
            grayTimerSize=MeasureTextEx(GetFontDefault(),grayTimer,30,1);
            grayTimerPos={600-(grayTimerSize.x/2),90-(grayTimerSize.y/2)};
        }
        else{
            playerBlack.timeLeft--;
            sprintf(blackTimer, "%02d:%02d", playerBlack.timeLeft/60, playerBlack.timeLeft%60);
            blackTimerSize=MeasureTextEx(GetFontDefault(),blackTimer,30,1);
            blackTimerPos={120-(blackTimerSize.x/2),90-(blackTimerSize.y/2)};
        }
        if(playerBlack.timeLeft==0||playerGray.timeLeft==0) gameOver=1;
        timer=GetTime();
        pauseTime=0;
    }
    if(menuBoxActive&&IsKeyPressed(KEY_ENTER)){
        menuBoxActive=0;
        clearGame();
        state=GameState::MenuScreen;
    }
    Vector2 mousePos = GetMousePosition();
    if(musicButtonActive) handleMusicBox(mousePos);
    if(isHold&&IsMouseButtonUp(MOUSE_LEFT_BUTTON)) isHold=0;
    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        if(!gameStart){
            gameStart=1;
            timer=GetTime();
        }
        else if(!menuBoxActive&&!musicButtonActive&&gameOver){
            clearGame();
            init();
        }
        else if(!menuBoxActive&&CheckCollisionPointRec(mousePos, musicButtonBounds)){
            musicButtonActive=!musicButtonActive;
            if(musicButtonActive) pauseTime=GetTime();
            else pauseTime=GetTime()-pauseTime;
        }
        else if(!musicButtonActive&&!menuBoxActive){
            int mouseX=(mousePos.x - 120)/60;
            int mouseY=(mousePos.y - 120)/60;
            if(validGrid(mouseX,mouseY)){
                int enemy=grid[mouseX][mouseY];
                // std::cout<<piece<<'\n';
                if(!clicked&&enemy!=10){
                    // std::cout<<move<<' '<<side<<'\n';
                    if((moveCount&1)==grid[mouseX][mouseY]/5){
                        clicked=1;
                        x=mouseX;
                        y=mouseY;    
                        posX=120+x*60;
                        posY=120+y*60;
                    }
                }
                else if(clicked){
                    int piece=grid[x][y]%5;
                    int difX=abs(mouseX-x),difY=abs(mouseY-y);
                    bool side=grid[x][y]/5,enemySide=enemy==10?!side:enemy/5;
                    bool good=0;
                    if(piece==1){
                        if(difX+difY==1&&enemySide!=side){
                            grid[mouseX][mouseY]=(side&&mouseX==0)?8:(!side&&mouseX==7)?3:grid[x][y]; 
                            good=1;
                        }
                    }
                    else if(piece==0){
                        if(difX==1&&difY==1&&enemySide!=side){
                            grid[mouseX][mouseY]=(side&&mouseX==0)?7:(!side&&mouseX==7)?2:grid[x][y]; 
                            good=1;
                        }
                    } 
                    else if(piece==2){
                        if(difX==difY&&enemySide!=side){
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
                        int mx=std::max(difX,difY),mn=std::min(difX,difY);
                        if(mx>0&&mn==0&&enemySide!=side){
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
                        if(difX<=1&&difY<=1&&enemySide!=side){
                            grid[mouseX][mouseY]=grid[x][y]; 
                            good=1;
                        }
                    }
                    if(good){
                        if(enemy%5==4){
                            gameOver=1;
                        }
                        if(enemy!=10){
                            enemy%=5;
                            if(side){
                                graveyard[enemy].second++;
                                cntR++;
                            }
                            else{
                                graveyard[enemy].first++;
                                cntL++;
                            }
                            int cntLL=0,cntRR=0;
                            for(int i=0;i<5;i++){
                                for(int l=0;l<graveyard[i].first;l++){
                                    bGravePos[cntLL]={{90-cntLL/8*60-(pieceSize[i].x/2),150.f+60*(cntLL%8)-(pieceSize[i].y/2)},i};
                                    cntLL++;
                                }
                                for(int r=0;r<graveyard[i].second;r++){
                                    gGravePos[cntRR]={{630+cntRR/8*60-(pieceSize[i].x/2),150.f+60*(cntRR%8)-(pieceSize[i].y/2)},i};
                                    cntRR++;
                                }
                            }
                        }
                        if(side){
                            playerGray.timeLeft+=playerGray.increment;
                            sprintf(grayTimer, "%02d:%02d", playerGray.timeLeft/60, playerGray.timeLeft%60);
                            grayTimerSize=MeasureTextEx(GetFontDefault(),grayTimer,30,1);
                            grayTimerPos={600-(grayTimerSize.x/2),90-(grayTimerSize.y/2)};
                        } 
                        else{
                            playerBlack.timeLeft+=playerBlack.increment;
                            sprintf(blackTimer, "%02d:%02d", playerBlack.timeLeft/60, playerBlack.timeLeft%60);
                            blackTimerSize=MeasureTextEx(GetFontDefault(),blackTimer,30,1);
                            blackTimerPos={120-(blackTimerSize.x/2),90-(blackTimerSize.y/2)};
                        } 
                        grid[x][y]=10;
                        piecePos[x][y]={150.f+60*x,150.f+60*y};
                        piecePos[mouseX][mouseY]={150.f+60*mouseX-pieceSize[grid[mouseX][mouseY]%5].x/2,150.f+60*mouseY-pieceSize[grid[mouseX][mouseY]%5].y/2};
                        moveCount++;
                        sprintf(turnText, "Turn %d", moveCount+1);
                        turnTextSize=MeasureTextEx(GetFontDefault(),turnText,30,1);
                        turnTextPos={360-(turnTextSize.x/2),30-(turnTextSize.y/2)};
                    }
                    clicked=0;
                    x=-1;
                    y=-1;
                    posX=-1;
                    posY=-1;
                }
            }
        }
    }
}

static void drawMenu(){
    // DrawTextEx(GetFontDefault(),"Press enter to start the game", {0,0}, 32, 1, BLACK);
    DrawTextEx(GetFontDefault(),"Catur Abjad", titleMenuPos, 64, 1, BLACK);
    DrawRectangleRec(menuBox,BLACK);
    DrawTextEx(GetFontDefault(),"Timer: ", timerMenuPos, 30, 1, WHITE);
    DrawTextEx(GetFontDefault(),std::to_string(gameTime).c_str(), gameTimePos, 30, 1, WHITE);
    DrawTextEx(GetFontDefault(),"Increment: ", incrementPos, 30, 1, WHITE);
    DrawTextEx(GetFontDefault(),std::to_string(incTime).c_str(), incTimePos, 30, 1, WHITE);
    DrawRectangleRec(gameTimeIncBox,WHITE);
    DrawRectangleRec(gameTimeDecBox,WHITE);
    DrawRectangleRec(incTimeIncBox,WHITE);
    DrawRectangleRec(incTimeDecBox,WHITE);
    DrawTextEx(GetFontDefault(),"-",gameTimeIncPos,30,1,BLACK);
    DrawTextEx(GetFontDefault(),"+",gameTimeDecPos,30,1,BLACK);
    DrawTextEx(GetFontDefault(),"-",incTimeIncPos,30,1,BLACK);
    DrawTextEx(GetFontDefault(),"+",incTimeDecPos,30,1,BLACK);
    DrawRectangleRec(playGameBox,WHITE);
    DrawTextEx(GetFontDefault(),"Play Game", playGamePos, 30, 1, BLACK);
    drawMusicBox();
}

static void updateMenu(){
    if(WindowShouldClose()) runWindow = 0;
    Vector2 mousePos = GetMousePosition();
    if(musicButtonActive) handleMusicBox(mousePos);
    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)&&CheckCollisionPointRec(mousePos, musicButtonBounds)) musicButtonActive=!musicButtonActive;
    if(isHold&&IsMouseButtonUp(MOUSE_LEFT_BUTTON)) isHold=0;
    if(!musicButtonActive){
        bool changed=false,gt=false;
        if(CheckCollisionPointRec(mousePos, incTimeIncBox)&&incTime>0){
            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
                incTime--;
                changed=1;
            }
            else if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
                if(!isHold){
                    isHold=1;
                    holdTimer=GetTime();
                }
                else if(GetTime()-holdTimer>=0.2){
                    incTime--;
                    holdTimer=GetTime();
                }
                changed=1;
            }
        }
        if(CheckCollisionPointRec(mousePos, incTimeDecBox)&&incTime<5){
            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
                incTime++;
                changed=1;
            }
            else if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
                if(!isHold){
                    isHold=1;
                    holdTimer=GetTime();
                }
                else if(GetTime()-holdTimer>=0.2){
                    incTime++;
                    holdTimer=GetTime();
                }
                changed=1;
            }
        }
        if(CheckCollisionPointRec(mousePos, gameTimeIncBox)&&gameTime>1){
            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
                gameTime--;
                changed=1;
                gt=1;
            }
            else if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
                if(!isHold){
                    isHold=1;
                    holdTimer=GetTime();
                }
                else if(GetTime()-holdTimer>=0.2){
                    gameTime--;
                    holdTimer=GetTime();
                }
                changed=1;
                gt=1;
            }
        }
        if(CheckCollisionPointRec(mousePos, gameTimeDecBox)&&gameTime<15){
            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
                gameTime++;
                changed=1;
                gt=1;
            }
            else if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
                if(!isHold){
                    isHold=1;
                    holdTimer=GetTime();
                }
                else if(GetTime()-holdTimer>=0.2){
                    gameTime++;
                    holdTimer=GetTime();
                }
                changed=1;
                gt=1;
            }
        }
        if(changed){
            if(gt) gameTimeSize=MeasureTextEx(GetFontDefault(),std::to_string(gameTime).c_str(),30,1);
            else incTimeSize=MeasureTextEx(GetFontDefault(),std::to_string(incTime).c_str(),30,1);
            gameTimeIncBox={gameTimePos.x+gameTimeSize.x+10,gameTimePos.y,30,30};
            gameTimeDecBox={gameTimeIncBox.x+gameTimeIncBox.width+10,gameTimePos.y,30,30};
            incTimeIncBox={incTimePos.x+incTimeSize.x+10,incTimePos.y,30,30};
            incTimeDecBox={incTimeIncBox.x+incTimeIncBox.width+10,incTimePos.y,30,30};
            gameTimeIncPos={gameTimeIncBox.x+gameTimeIncBox.width/2-vDownSize.x/2,gameTimePos.y};
            gameTimeDecPos={gameTimeDecBox.x+gameTimeDecBox.width/2-vUpSize.x/2,gameTimePos.y};
            incTimeIncPos={incTimeIncBox.x+incTimeIncBox.width/2-vDownSize.x/2,incTimePos.y};
            incTimeDecPos={incTimeDecBox.x+incTimeDecBox.width/2-vUpSize.x/2,incTimePos.y};
        }
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)&&CheckCollisionPointRec(mousePos, playGameBox)){
            init();
            state=GameState::GameScreen;
        } 
    }
}

static void drawStart(){
    DrawRectangleRec(blackTitleBox,BLACK);
    DrawRectangleRec(grayTitleBox,GRAY);
    DrawTextEx(GetFontDefault(),"U T X I C v", blackTitlePos, 24, 1, BLUE);
    DrawTextEx(GetFontDefault(),"s C I X T U", grayTitlePos, 24, 1, GREEN);
    DrawTextEx(GetFontDefault(),"Catur Abjad", titlePos, 90, 1, BLACK);
    if(GetTime()-blinkTimer>=0.5){
        blinkTimer=GetTime();
        isBlinking=!isBlinking;
    }
    if(isBlinking) DrawTextEx(GetFontDefault(),"Press enter to start game", enterPos, 32, 1, BLACK);
    drawMusicBox();
}

static void updateStart(){
    if(WindowShouldClose()) runWindow = 0;
    if(!musicButtonActive&&IsKeyPressed(KEY_ENTER)) state=GameState::MenuScreen;
    if(isHold&&IsMouseButtonUp(MOUSE_LEFT_BUTTON)) isHold=0;
    Vector2 mousePos = GetMousePosition();
    if(musicButtonActive) handleMusicBox(mousePos);
    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)&&CheckCollisionPointRec(mousePos, musicButtonBounds)) musicButtonActive=!musicButtonActive;
}

int main(){
    // init window
    InitWindow(screenWidth, screenHeight, "Catur Abjad");
    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(9216);
    SetExitKey(KEY_NULL);
    SetTargetFPS(30);
    SetMasterVolume(masterVolume/10.f);
    pieceSize={
        MeasureTextEx(GetFontDefault(),"C",30,1),
        MeasureTextEx(GetFontDefault(),"I",30,1),
        MeasureTextEx(GetFontDefault(),"X",30,1),
        MeasureTextEx(GetFontDefault(),"T",30,1),
        MeasureTextEx(GetFontDefault(),"U",30,1)
    };
    textSoundSize=MeasureTextEx(GetFontDefault(),"Music",30,1);
    textSoundPos={screenWidth/2-(textSoundSize.x/2),660-(textSoundSize.y/2)};
    musicButtonBounds={
        textSoundPos.x,
        textSoundPos.y,
        textSoundSize.x,
        textSoundSize.y
    };
    overSize=MeasureTextEx(GetFontDefault(),"Game Over",30,1);
    overPos={screenWidth/2-(overSize.x/2),60-(overSize.y/2)};
    bTurnSize=MeasureTextEx(GetFontDefault(),"Black's Turn",30,1);
    bTurnPos={screenWidth/2-(bTurnSize.x/2),90-(bTurnSize.y/2)};
    gTurnSize=MeasureTextEx(GetFontDefault(),"Gray's Turn",30,1);
    gTurnPos={screenWidth/2-(gTurnSize.x/2),90-(gTurnSize.y/2)};
    bTimerSize=MeasureTextEx(GetFontDefault(),"Black's Timer",30,1);
    bTimerPos={screenWidth/6-(bTimerSize.x/2),30-(bTimerSize.y/2)};
    gTimerSize=MeasureTextEx(GetFontDefault(),"Gray's Timer",30,1);
    gTimerPos={screenWidth*5/6-(gTimerSize.x/2),30-(gTimerSize.y/2)};
    resetSize=MeasureTextEx(GetFontDefault(),"Click anywhere to reset the game!",24,1);
    resetPos={screenWidth/2-(resetSize.x/2),screenHeight/2-(resetSize.y/2)};
    startSize=MeasureTextEx(GetFontDefault(),"Click anywhere to start the game!",24,1);
    startPos={screenWidth/2-(startSize.x/2),screenHeight/2-(startSize.y/2)};
    menuSize=MeasureTextEx(GetFontDefault(),"Would you like to go back to main menu?",24,1);
    menuPos={screenWidth/2-(menuSize.x/2),330-(menuSize.y/2)};
    choiceSize=MeasureTextEx(GetFontDefault(),"YES (Enter) or NO (ESC)",30,1);
    choicePos={screenWidth/2-(choiceSize.x/2),390-(choiceSize.y/2)};
    volumeSize=MeasureTextEx(GetFontDefault(), "Volume:", 30, 1);
    // volumePos={405-volumeSize.y, 30};
    barLength = (420-volumeSize.x)/11;
    vUpSize=MeasureTextEx(GetFontDefault(), "+", 30, 1);
    vUpPos={645-(vUpSize.x/2),390-(vUpSize.y/2)};
    vDownSize=MeasureTextEx(GetFontDefault(), "-", 30, 1);
    vDownPos={75-(vDownSize.x/2),390-(vDownSize.y/2)};
    nextSize=MeasureTextEx(GetFontDefault(), ">", 30, 1);
    nextPos={645-(nextSize.x/2),330-(nextSize.y/2)};
    prevSize=MeasureTextEx(GetFontDefault(), "<", 30, 1);
    prevPos={75-(prevSize.x/2),330-(prevSize.y/2)};
    titleSize=MeasureTextEx(GetFontDefault(),"Catur Abjad",90,1);
    titlePos={screenWidth/2-titleSize.x/2,screenHeight/2-titleSize.y};
    enterSize=MeasureTextEx(GetFontDefault(),"Press enter to start game",32,1);
    enterPos={screenWidth/2-enterSize.x/2,screenHeight/2};
    blackTitleSize=MeasureTextEx(GetFontDefault(),"U T X I C v",24,1);
    blackTitlePos={screenWidth/2-blackTitleSize.x,titlePos.y-blackTitleSize.y};
    grayTitleSize=MeasureTextEx(GetFontDefault(),"s C I X T U",24,1);
    grayTitlePos={blackTitlePos.x+blackTitleSize.x,blackTitlePos.y};
    blackTitleBox={blackTitlePos.x-10,blackTitlePos.y-5,blackTitleSize.x+10,blackTitleSize.y+10};
    grayTitleBox={grayTitlePos.x,grayTitlePos.y-5,grayTitleSize.x+10,grayTitleSize.y+10};
    titleMenuSize=MeasureTextEx(GetFontDefault(),"Catur Abjad",64,1);
    titleMenuPos={screenWidth/2-titleMenuSize.x/2,60};
    timerMenuSize=MeasureTextEx(GetFontDefault(),"Timer: ",30,1);
    gameTimeSize=MeasureTextEx(GetFontDefault(),std::to_string(gameTime).c_str(),30,1);
    incTimeSize=MeasureTextEx(GetFontDefault(),std::to_string(incTime).c_str(),30,1);
    for(int i=1;i<=10;i++){
        volumeRec[i-1]={150+volumeSize.x+i*barLength, 405-volumeSize.y, barLength, volumeSize.y};  
    }
    incrementSize=MeasureTextEx(GetFontDefault(),"Increment: ",30,1);
    playGameSize=MeasureTextEx(GetFontDefault(),"Play Game",30,1);
    float menuBoxSize=playGameSize.y+timerMenuSize.y+incrementSize.y+60;
    menuBox={titleMenuPos.x,screenHeight/2-menuBoxSize/2.f,titleMenuSize.x,menuBoxSize};
    timerMenuPos={menuBox.x+menuBox.width/10,menuBox.y+10};
    gameTimePos={timerMenuPos.x+timerMenuSize.x,timerMenuPos.y};
    incrementPos={timerMenuPos.x,timerMenuPos.y+timerMenuSize.y+10};
    incTimePos={incrementPos.x+incrementSize.x,incrementPos.y};
    playGameBox={screenWidth/2-playGameSize.x/2-10,incrementPos.y+incrementSize.y+10,playGameSize.x+20,playGameSize.y+20};
    playGamePos={playGameBox.x+10,playGameBox.y+10};
    gameTimeIncBox={gameTimePos.x+gameTimeSize.x+10,gameTimePos.y,30,30};
    gameTimeDecBox={gameTimeIncBox.x+gameTimeIncBox.width+10,gameTimePos.y,30,30};
    incTimeIncBox={incTimePos.x+incTimeSize.x+10,incTimePos.y,30,30};
    incTimeDecBox={incTimeIncBox.x+incTimeIncBox.width+10,incTimePos.y,30,30};
    gameTimeIncPos={gameTimeIncBox.x+gameTimeIncBox.width/2-vDownSize.x/2,gameTimePos.y};
    gameTimeDecPos={gameTimeDecBox.x+gameTimeDecBox.width/2-vUpSize.x/2,gameTimePos.y};
    incTimeIncPos={incTimeIncBox.x+incTimeIncBox.width/2-vDownSize.x/2,incTimePos.y};
    incTimeDecPos={incTimeDecBox.x+incTimeDecBox.width/2-vUpSize.x/2,incTimePos.y};
    song.reserve(MAX_SONG);
    songList.reserve(MAX_SONG);
    std::string musicPath="assets/music";
    for(const auto &entry:std::filesystem::directory_iterator(musicPath)){
        if(song.size()==MAX_SONG) break;
        if(entry.is_regular_file()){
            auto path=entry.path();
            std::string ext=path.extension().string();
            if(ext==".mp3"){
                Music newSong=LoadMusicStream(path.string().c_str());
                std::string filename=path.filename().string();
                char* songName=new char[filename.length()+1];
                std::strcpy(songName,filename.c_str());
                songList.push_back(songName);
                song.push_back(newSong);
            }
        }
    }
    if(!song.empty()) PlayMusicStream(song[curSong]);
    while(runWindow){
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawFPS(0,0);
        if(!song.empty()) UpdateMusicStream(song[curSong]);
        if(state==GameState::GameScreen){
            draw();
            update();
        }
        else if(state==GameState::MenuScreen){
            drawMenu();
            updateMenu();
        }
        else if(state==GameState::StartScreen){
            drawStart();
            updateStart();
        }
        EndDrawing();
    }
    for(const char* name:songList) delete[] name;
    for(Music &i:song) UnloadMusicStream(i);
    CloseAudioDevice();
    CloseWindow();
}