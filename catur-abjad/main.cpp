#include "raylib.h"
#include <vector>
#include <string>
#include <iostream>

// TODO: IMPLEMENT GAME LOGIC

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
static std::vector<Music> song(2);
static std::vector<std::vector<int>> grid(8,std::vector<int>(8));
static std::vector<std::pair<int,int>> graveyard(5);
static std::vector<int> clickedPiece{-1,-1};
static std::vector<char*> songList{"Mafia Sholawat", "Ganyang Fufufafa"};
static Rectangle musicButtonBounds;
static Rectangle nextButtonsBounds{630,315,30,30};
static Rectangle prevButtonsBounds{60,315,30,30};
static Rectangle volumeUpBounds{630,375,30,30};
static Rectangle volumeDownBounds{60,375,30,30};
static float timer, pauseTime = 0;
static const int screenWidth = 720;
static const int screenHeight = 720;
static int moveCount = 0;
static int curSong = 0;
static int masterVolume = 10;
static GameState state = GameState::GameScreen;
static bool runWindow = 1;
static bool clicked = 0;
static bool gameOver = 0;
static bool gameStart = 0;
static bool musicButtonActive = 0;
static bool menuBoxActive = 0;

static bool validGrid(int x,int y){
    return x>=0&&x<8&&y>=0&&y<8;
}

static void drawText(const char* text, const Color color, float centerX, float centerY, int fontSize=30){
    Vector2 textPos;
    Vector2 textSize=MeasureTextEx(GetFontDefault(),text,fontSize,1);
    textPos.x=centerX-(textSize.x/2);
    textPos.y=centerY-(textSize.y/2);
    DrawTextEx(GetFontDefault(),text,textPos,fontSize,1,color);
}

static void init(){
    ClearBackground(RAYWHITE);
    SetMasterVolume(masterVolume/10.f);
    playerBlack = {
        .timeLeft = 5*60,
        .increment = 3
    };

    playerGray = {
        .timeLeft = 5*60,
        .increment = 3
    };

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
    float centerSoundX=360, centerSoundY=660;
    Vector2 textSoundPos;
    Vector2 textSoundSize=MeasureTextEx(GetFontDefault(),"Music",30,1);
    textSoundPos.x=centerSoundX-(textSoundSize.x/2);
    textSoundPos.y=centerSoundY-(textSoundSize.y/2);
    DrawTextEx(GetFontDefault(),"Music",textSoundPos,30,1,BLACK);
    musicButtonBounds={
        textSoundPos.x,
        textSoundPos.y,
        textSoundSize.x,
        textSoundSize.y
    };
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
    int cntL=0,cntR=0;
    for(int i=0;i<5;i++){
        auto [l,r]=graveyard[i];
        char* c;
        if(i==0) c="C";
        else if(i==1) c="I";
        else if(i==2) c="X";
        else if(i==3) c="T";
        else c="U";
        // std::cout<<grid[i][j]<<'\n';
        while(l--){
            float centerX=90-cntL/8*60, centerY=150+60*(cntL%8);
            drawText(c, GRAY, centerX, centerY);
            cntL++;
        }
        while(r--){
            float centerX=630+cntR/8*60, centerY=150+60*(cntR%8);
            drawText(c, BLACK, centerX, centerY);
            cntR++;
        }
    }
    if(gameOver){
        drawText("Game Over", BLACK, 360, 60);
        DrawRectangle(120,285,480,150,BLACK);
        drawText("Click anywhere to reset the game!", WHITE, 360, 360, 24);
    }
    else{
        const char* s=moveCount%2?"Gray's Turn":"Black's Turn";
        Color color=moveCount%2?GRAY:BLACK;
        drawText(TextFormat("Turn %d", moveCount+1), color, 360, 30);
        drawText(s, color, 360, 90);
        drawText("Black's Timer", BLACK, 120, 30);
        drawText("Gray's Timer", GRAY, 600, 30);
        int minuteBlack=playerBlack.timeLeft/60, secondBlack=playerBlack.timeLeft%60;
        int minuteGray=playerGray.timeLeft/60, secondGray=playerGray.timeLeft%60;
        drawText(TextFormat("%02d:%02d", minuteBlack, secondBlack), BLACK, 120, 90);
        drawText(TextFormat("%02d:%02d", minuteGray, secondGray), GRAY, 600, 90);
    }
    if(!gameStart){
        DrawRectangle(120,285,480,150,BLACK);
        drawText("Click anywhere to start the game!", WHITE, 360, 360, 24);
    }
    if(musicButtonActive){
        DrawRectangle(120,285,480,150,BLACK);
        DrawRectangle(60,315,30,30,BLACK);
        DrawRectangle(630,315,30,30,BLACK);
        DrawRectangle(60,375,30,30,BLACK);
        DrawRectangle(630,375,30,30,BLACK);
        DrawText(TextFormat("Song: %s", songList[curSong]), 150, 315, 30, WHITE);
        Vector2 textSize=MeasureTextEx(GetFontDefault(), "Volume: ", 30, 1);
        DrawText("Volume:", 150, 405-textSize.y, 30, WHITE);
        int barLength = (420-textSize.x)/11;
        for(int i=1;i<=masterVolume;i++){
            DrawRectangle(150+textSize.x+i*barLength, 405-textSize.y, barLength, textSize.y, WHITE);
        }
        drawText(">", WHITE, 645, 330);
        drawText("<", WHITE, 75, 330);
        drawText("-", WHITE, 75, 390);
        drawText("+", WHITE, 645, 390);
    }
    if(menuBoxActive){
        DrawRectangle(120,285,480,150,BLACK);
        drawText("Would you like to go back to main menu?", WHITE, 360, 330, 24);
        drawText("YES (Enter) or NO (ESC)", WHITE, 360, 390);
    }
}

static void update(){
    std::cout<<GetMasterVolume()<<'\n';
    UpdateMusicStream(song[curSong]);
    if(WindowShouldClose()) runWindow = 0;
    if(!musicButtonActive&&IsKeyPressed(KEY_ESCAPE)){
        menuBoxActive=!menuBoxActive;
        if(menuBoxActive) pauseTime=GetTime();
        else pauseTime=GetTime()-pauseTime;
    }
    Vector2 mousePos = GetMousePosition();
    if(gameStart&&!menuBoxActive&&!musicButtonActive&&!gameOver&&GetTime()-pauseTime-timer>=1){
        if(moveCount&1) playerGray.timeLeft--;
        else playerBlack.timeLeft--;
        if(playerBlack.timeLeft==0||playerGray.timeLeft==0) gameOver=1;
        timer=GetTime();
        pauseTime=0;
    }
    if(menuBoxActive&&IsKeyPressed(KEY_ENTER)){
        state=GameState::MenuScreen;
    }
    if(musicButtonActive&&IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
        if(CheckCollisionPointRec(mousePos, nextButtonsBounds)){
            StopMusicStream(song[curSong]);
            curSong=(curSong+1)%songList.size();
            PlayMusicStream(song[curSong]);
        }
        if(CheckCollisionPointRec(mousePos, prevButtonsBounds)){
            StopMusicStream(song[curSong]);
            curSong=(curSong-1)%songList.size();
            PlayMusicStream(song[curSong]);
        }
        if(CheckCollisionPointRec(mousePos, volumeUpBounds)){
            if(masterVolume<10) masterVolume++;
            SetMasterVolume(masterVolume/10.f);
        }
        if(CheckCollisionPointRec(mousePos, volumeDownBounds)){
            if(masterVolume>0) masterVolume--;
            SetMasterVolume(masterVolume/10.f);
        }
    }
    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        if(!gameStart){
            gameStart=1;
            timer=GetTime();
        }
        else if(!menuBoxActive&&!musicButtonActive&&gameOver){
            init();
            clicked=0;
            gameOver=0;
            gameStart=0;    
            graveyard=std::vector<std::pair<int,int>>(5);
            moveCount=0;
        }
        else if(!menuBoxActive&&CheckCollisionPointRec(mousePos, musicButtonBounds)){
            musicButtonActive=!musicButtonActive;
            if(musicButtonActive) pauseTime=GetTime();
            else pauseTime=GetTime()-pauseTime;
        }
        // else if(musicButtonActive){
        //     if(CheckCollisionPointRec(mousePos, nextButtonsBounds)){
        //         curSong=(curSong+1)%songList.size();
        //     }
        //     if(CheckCollisionPointRec(mousePos, prevButtonsBounds)){
        //         curSong=(curSong-1)%songList.size();
        //     }
        //     if(CheckCollisionPointRec(mousePos, volumeUpBounds)){
        //         if(masterVolume<10) masterVolume++;
        //         SetMasterVolume(masterVolume);
        //     }
        //     if(CheckCollisionPointRec(mousePos, volumeDownBounds)){
        //         if(masterVolume>0) masterVolume--;
        //         SetMasterVolume(masterVolume);
        //     }
        // }
        else if(!musicButtonActive&&!menuBoxActive){
            int mouseX=(mousePos.x - 120)/60;
            int mouseY=(mousePos.y - 120)/60;
            if(validGrid(mouseX,mouseY)){
                int piece=grid[mouseX][mouseY];
                // std::cout<<piece<<'\n';
                if(!clicked&&piece!=10){
                    bool side=grid[mouseX][mouseY]/5;
                    // std::cout<<move<<' '<<side<<'\n';
                    if((moveCount&1)==side){
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
                            grid[mouseX][mouseY]=(side&&mouseX==0)?8:(!side&&mouseX==7)?3:grid[x][y]; 
                            good=1;
                        }
                    }
                    else if(piece==0){
                        if(abs(mouseX-x)==1&&abs(mouseY-y)==1&&enemy/5!=side){
                            grid[mouseX][mouseY]=(side&&mouseX==0)?7:(!side&&mouseX==7)?2:grid[x][y]; 
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
                        if(enemy!=10){
                            enemy%=5;
                            if(side) graveyard[enemy].second++;
                            else graveyard[enemy].first++;
                        }
                        if(side) playerGray.timeLeft+=playerGray.increment;
                        else playerBlack.timeLeft+=playerBlack.increment;
                        grid[x][y]=10;
                        moveCount++;
                    }
                    clicked=0;
                    clickedPiece={-1,-1};
                }
            }
        }
    }
}

static void drawMenu(){

}

static void updateMenu(){

}

static void drawStart(){

}

static void updateStart(){

}

int main(){
    // init window
    InitWindow(screenWidth, screenHeight, "Catur Abjad");
    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(9216);
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);
    init();
    song[0]=LoadMusicStream("assets/music/mafiaslwt.mp3");
    song[1]=LoadMusicStream("assets/music/ganyangffff.mp3");
    PlayMusicStream(song[curSong]);
    while(runWindow){
        BeginDrawing();
        ClearBackground(RAYWHITE);
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
    for(auto &i:song) UnloadMusicStream(i);
    CloseAudioDevice();
    CloseWindow();
}