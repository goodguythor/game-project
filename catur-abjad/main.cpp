#include "raylib.h"
#include <vector>
#include <string>
// #include <cstdint>
#include <iostream>
#include <filesystem>
#include <cstring>
#include <math.h>

// TODO:
// Refactor some shit like the pieces movements etc
// Add power ups such as Piece ressurection, Move 2 piece in 1 turn, Piece promotion ... 
// Add ability (shield, swap, freeze, ...)
// Add in game help/tutorial
// Add more song prolly
// Improve game rule
// Resize screen 
// Compress all file before shipment
// Add animation
#define MAX_SONG 32

enum class GameState{
    GameScreen,
    MenuScreen,
    StartScreen  
};

Texture2D atlas;
std::vector<std::vector<int>> grid(8,std::vector<int>(8));
std::vector<std::pair<Vector2,int>> bGravePos(16),gGravePos(16);
// Vector2 lastClicked;
const char* characters="CIXTU";
float timer, holdTimer, blinkTimer=0, pauseTime = 0;
const int screenWidth = 720;
const int screenHeight = 720;
int cntL=0,cntR=0;
int moveCount = 0;
int curSong = 0;
int x=-1,y=-1;
int posX=-1,posY=-1;
int gameTime=5,incTime=3;
int curBPower=0,curGPower=0;
GameState state = GameState::StartScreen;
bool runWindow = 1;
bool clicked = 0;
bool gameOver = 0;
bool gameStart = 0;
bool musicButtonActive = 0;
bool menuBoxActive = 0;
bool isHold=0;
bool isBlinking=0;
bool blackFreeze=0,grayFreeze=0;
bool blackShield=0,grayShield=0;

class GameText{
private:
    std::string text;
    Vector2 pos,size;
    int fontSize;
    Color color;
public:
    void Init(const std::string textInput,Vector2 position,const int fontSizeInput,const Color col,const std::pair<bool,bool> central, const std::pair<bool,bool> start){
        text=textInput;
        fontSize=fontSizeInput;
        color=col;
        pos=position;
        size=MeasureTextEx(GetFontDefault(),text.c_str(),fontSize,1);
        pos.x-=(central.first?size.x/2:(start.first?0:size.x));
        pos.y-=(central.second?size.y/2:(start.second?0:size.y));
    }
    void Draw(){
        DrawTextEx(GetFontDefault(),text.c_str(),pos,fontSize,1,color);
    }
    void Draw(bool side){
        float posX=side?510-size.x:210,posY=530;
        DrawTextEx(GetFontDefault(),text.c_str(),{posX,posY},fontSize,1,color);
    }
    void Update(const std::string &newText){
        text=newText;
        Vector2 newSize=MeasureTextEx(GetFontDefault(),text.c_str(),fontSize,1);
        pos.x-=(newSize.x-size.x)/2;
        pos.y-=(newSize.y-size.y)/2;
        size=newSize;
    }
    void Update(const std::string &newText,const Color col){
        text=newText;
        Vector2 newSize=MeasureTextEx(GetFontDefault(),text.c_str(),fontSize,1);
        pos.x-=(newSize.x-size.x)/2;
        pos.y-=(newSize.y-size.y)/2;
        size=newSize;
        color=col;
    }
    void Update(float x,float y){
        pos.x+=x;
        pos.y+=y;
    }
    inline void SetPosY(float y){
        pos.y=y;
    }
    inline float GetPosY(){
        return pos.y;
    }
    inline float GetPosX(){
        return pos.x;
    }
    inline float GetEndPosX(){
        return pos.x+size.x;
    }
    inline float GetEndPosY(){
        return pos.y+size.y;
    }
    inline float GetSizeX(){
        return size.x;
    }
    inline float GetSizeY(){
        return size.y;
    }
};

class PlayerClock{
private:
    char text[5];
    Vector2 pos,central;
    int timeLeft;
    int increment;
    Color color;
    bool finished;
public:
    void Init(const int time,const int inc,const Vector2 &cen,const Color col){
        timeLeft=time;
        increment=inc;
        color=col;  
        finished=0;
        central=cen;
        sprintf(text, "%02d:%02d", timeLeft/60, timeLeft%60);
        Vector2 size=MeasureTextEx(GetFontDefault(),text,30,1);
        pos={central.x-(size.x/2),central.y-(size.y/2)};
    }
    void Draw(){
        DrawTextEx(GetFontDefault(),text,pos,30,1,color);
    }
    void Update(bool inc){
        timeLeft+=inc?increment:-1;
        if(timeLeft==0) finished=1;
        sprintf(text, "%02d:%02d", timeLeft/60, timeLeft%60);
        Vector2 size=MeasureTextEx(GetFontDefault(),text,30,1);
        pos={central.x-(size.x/2),central.y-(size.y/2)};
    }
    inline bool IsFinished(){
        return finished;
    }
};

class Button{
private:
    GameText buttonText;
    Rectangle buttonBox;
    Color color;
    bool isRec;
public:
    void Init(const std::string textInput,const Vector2 position,const int fontSizeInput,const Color textColor,const Color col,const std::pair<bool,bool> central, const std::pair<bool,bool> start,const bool rec){
        buttonText.Init(textInput,position,fontSizeInput,textColor,central,start);
        color=col;
        isRec=rec;
        buttonBox={
            buttonText.GetPosX()-(isRec?10.f:0),
            buttonText.GetPosY()-(isRec?10.f:0),
            buttonText.GetSizeX()+(isRec?20.f:0),
            buttonText.GetSizeY()+(isRec?20.f:0)
        };
    }
    void Draw(){
        if(isRec) DrawRectangleRec(buttonBox,color);
        buttonText.Draw();
    }

    void Draw(bool side){
        float posX=side?520-buttonBox.width:200,posY=520;
        if(isRec) DrawRectangleRec({posX,posY,buttonBox.width,buttonBox.height},color);
        buttonText.Draw(side);
    }

    void Update(float x,float y){
        buttonText.Update(x,y);
        buttonBox={
            buttonText.GetPosX()-(isRec?10.f:0),
            buttonText.GetPosY()-(isRec?10.f:0),
            buttonText.GetSizeX()+(isRec?20.f:0),
            buttonText.GetSizeY()+(isRec?20.f:0)
        };
    }
    inline float GetPosX(){
        return buttonText.GetPosX()-(isRec?10.f:0);
    }
    inline float GetPosY(){
        return buttonText.GetPosY()-(isRec?10.f:0);
    }
    inline float GetEndPosX(){
        return buttonText.GetEndPosX()+(isRec?20.f:0);
    }
    inline float GetEndPosY(){
        return buttonText.GetEndPosY()+(isRec?20.f:0);
    }
    inline Rectangle GetButtonBound(){
        return buttonBox;
    }
};

class Volume{
private:
    GameText volumeText;
    Rectangle volumeRec[10];
    float barLength;
    float masterVolume;
public:
    void Init(const std::string textInput,const Vector2 position,const int fontSize,const Color col,const std::pair<bool,bool> central,const std::pair<bool,bool> start){
        volumeText.Init(textInput,position,fontSize,col,central,start);
        barLength=324.f/11.f;
        masterVolume=0.f;
        SetMasterVolume(0.f);
        for(int i=1;i<=10;i++){
            volumeRec[i-1]={
                150+volumeText.GetSizeX()+i*barLength, 
                405-volumeText.GetSizeY(), 
                barLength, 
                volumeText.GetSizeY()
            };
        } 
    }
    void Draw(){
        volumeText.Draw();
        for(int i=1;i<=masterVolume;i++) DrawRectangleRec(volumeRec[i-1],WHITE);
    }
    void Update(bool up){
        masterVolume+=up?1:-1;
        SetMasterVolume(masterVolume/10.f);
    }
    int GetMasterVolume(){
        return masterVolume;
    }
};

class Power{
private:
    Button descriptionBox;
    Button instruction;
    Rectangle powerBox;
    Vector2 pos;
    std::pair<int,int> last;
    int cooldown,currentCooldown;
    int duration,currentDuration;
    std::pair<bool,bool> usage;
    bool once;
    bool active;
    bool hover;
    bool clicked;
    bool usable;
public:
    void Init(const Rectangle box,const std::string textInput,const std::pair<bool,bool> u,Vector2 position,const int fontSizeInput,const int cd,const int dur,const Color textColor,const Color col,const std::pair<bool,bool> central, const std::pair<bool,bool> start,const bool rec){
        last={-1,-1};
        powerBox=box;
        pos=position;
        cooldown=cd;
        currentCooldown=0;
        duration=dur;
        currentDuration=0;
        active=0;
        hover=0;
        clicked=0;
        usage=u;
        usable=0;
        once=!u.second;
        position.y-=100;
        descriptionBox.Init(textInput,position,fontSizeInput,textColor,col,central,start,rec);
        instruction.Init(TextFormat("Click on %s pieces %s",usage.first?"your own":"enemy",usage.second?"twice":"once"),{screenWidth/2,630},10,textColor,col,{1,1},{0,0},1);
    }

    void Draw(bool game,bool side){
        float posX=pos.x,posY=pos.y;
        if(game){
            if(side) posX=450;
            else posX=210;
            posY=630;
        }
        if(clicked) instruction.Draw();
        if(hover) game?descriptionBox.Draw(side):descriptionBox.Draw();
        if(usable) DrawRectangleLinesEx({posX,posY,60,60},2,BLACK);
        DrawTextureRec(atlas,powerBox,{posX,posY},WHITE);
        if(currentCooldown>0) DrawTextEx(GetFontDefault(),std::to_string(currentCooldown).c_str(),{posX+30,posY+30},30,1,side?GREEN:BLUE);
    }   

    void UpdateCooldown(){
        currentCooldown--;
        if(currentCooldown==0) usable=1;
    }

    void UpdateDuration(){
        currentDuration++;
        // std::cout<<currentDuration<<' '<<duration<<'\n';
        if(duration==currentDuration){
            currentDuration=0;
            active=0;
        }
    }

    void Activate(int gridX,int gridY){
        if(once){
            std::pair cur={gridX,gridY};
            if(cur==last){
                SetOnce();
                clicked=0;
                last={-1,-1};
                return;
            }
            SetActive(1);
            SetUsable(0);
            clicked=0;
            currentCooldown=cooldown;
        }
        else{
            once=1;
        }
        last={gridX,gridY};
    }

    inline Rectangle GetBound(){
        return {pos.x,pos.y,60,60};
    }

    inline int GetCooldown(){
        return currentCooldown;
    }

    inline int GetDuration(){
        return currentDuration;
    }

    inline int GetLastX(){
        return last.first;
    }

    inline int GetLastY(){
        return last.second;
    }

    inline bool IsUsable(){
        return usable;
    }

    inline bool IsActive(){
        return active;
    }

    inline bool IsOnce(){
        return once;
    }

    inline bool IsHover(){
        return hover;
    }

    inline bool IsClicked(){
        return clicked;
    }

    inline bool IsOwn(){
        return usage.first;
    }

    inline bool IsTwice(){
        return usage.second;
    }

    inline void Reset(){
        currentCooldown=0;
        currentDuration=0;
        active=0;
        usable=1;
        hover=0;
        clicked=0;
        last={-1,-1};
        once=!usage.second;
    }

    inline void SetLast(){
        last={-1,-1};
    }

    inline void SetClicked(bool click){
        clicked=click;
    }

    inline void SetOnce(){
        once=!usage.second;
    }

    inline void SetUsable(bool use){
        usable=use;
    }

    inline void SetActive(bool act){
        if(!act){
            currentDuration=0;
        }
        active=act;
    }

    inline void SetHover(bool hov){
        hover=hov;
    }
};

PlayerClock playerBlack, playerGray;
std::vector<Music> song;
std::vector<std::string> songList;
std::pair<int,int> graveyard[5] = {{0,0},{0,0},{0,0},{0,0},{0,0}};
std::vector<Vector2> pieceSize;
Vector2 piecePos[8][8];
Volume volume;
std::vector<Power> blackPower(3),grayPower(3);
Button musicGameButton;
Button volumeUpButton;
Button volumeDownButton;
Button nextSongButton;
Button prevSongButton;
GameText gameOverText;
GameText turnText;
GameText turnCountText;
GameText blackTimerText;
GameText grayTimerText;
GameText resetGameText;
GameText startGameText;
GameText mainMenuText;
GameText choiceText;
GameText titleText;
GameText enterText;
Button blackTitleButton;
Button grayTitleButton;
Rectangle menuBox;
GameText timerMenuText;
GameText incMenuText;
Button playGameButton;
Button timeUpButton;
Button timeDownButton;
Button incUpButton;
Button incDownButton;
GameText songText;


bool validGrid(int x,int y){
    return x>=0&&x<8&&y>=0&&y<8;
}

void changeSong(bool nxt){
    StopMusicStream(song[curSong]);
    curSong=(curSong+(nxt?1:-1))%songList.size();
    songText.Update(TextFormat("Song: %s",songList[curSong].c_str()));
    PlayMusicStream(song[curSong]);
}

void clearGame(){
    clicked=0;
    pauseTime=0;
    gameOver=0;
    gameStart=0;   
    cntL=0;
    cntR=0; 
    for(int i=0;i<5;i++) graveyard[i]={0,0};
    moveCount=0;
    grayPower[curGPower].Reset();
    blackPower[curBPower].Reset();
    turnText.Update("Black's Turn",BLACK);
    turnCountText.Update("Turn 1",BLACK);
}

void handleMusicBox(const Vector2 &mousePos){
    int masterVolume=volume.GetMasterVolume();
    if(CheckCollisionPointRec(mousePos, nextSongButton.GetButtonBound())){
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
    if(CheckCollisionPointRec(mousePos, prevSongButton.GetButtonBound())){
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
    if(CheckCollisionPointRec(mousePos, volumeUpButton.GetButtonBound())){
        if(masterVolume<10&&IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            volume.Update(1);
        }
        else if(masterVolume<10&&IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
            if(!isHold){
                isHold=1;
                holdTimer=GetTime();
            }
            else if(GetTime()-holdTimer>=0.2){
                holdTimer=GetTime();
                volume.Update(1);
            }
        }
    }
    if(CheckCollisionPointRec(mousePos, volumeDownButton.GetButtonBound())){
        if(masterVolume>0&&IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            volume.Update(0);
        }
        else if(masterVolume>0&&IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
            if(!isHold){
                isHold=1;
                holdTimer=GetTime();
            }
            else if(GetTime()-holdTimer>=0.2){
                holdTimer=GetTime();
                volume.Update(0);
            }
        }
    }
}

void drawMusicBox(){
    musicGameButton.Draw();
    if(musicButtonActive){
        DrawRectangle(120,285,480,150,BLACK);
        songText.Draw();
        volume.Draw();
        nextSongButton.Draw();
        prevSongButton.Draw();
        volumeUpButton.Draw();
        volumeDownButton.Draw();
    }
}

void init(){
    ClearBackground(RAYWHITE);
    playerBlack.Init(gameTime*60,incTime,{120,90},BLACK);
    playerGray.Init(gameTime*60,incTime,{600,90},GRAY);
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

void draw(){
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
            char temp[2]={characters[(grid[i][j]+10)%5], '\0'};
            if(temp[0]!='C'&&temp[0]!='I'&&temp[0]!='X'&&temp[0]!='T'&&temp[0]!='U') std::cout<<i<<' '<<j<<' '<<grid[i][j]<<' '<<temp<<'\n';
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
        DrawRectangle(120,285,480,150,BLACK);
        gameOverText.Draw();
        resetGameText.Draw();
    }
    else{
        blackTimerText.Draw();
        turnCountText.Draw();
        grayTimerText.Draw();
        playerBlack.Draw();
        turnText.Draw();
        playerGray.Draw();
    }
    if(!gameStart){
        DrawRectangle(120,285,480,150,BLACK);
        startGameText.Draw();
    }
    drawMusicBox();
    if(menuBoxActive){
        DrawRectangle(120,285,480,150,BLACK);
        mainMenuText.Draw();
        choiceText.Draw();
    }
    blackPower[curBPower].Draw(1,0);
    grayPower[curGPower].Draw(1,1);
}

void update(){
    // std::cout<<GetMasterVolume()<<'\n';
    if(WindowShouldClose()) runWindow = 0;
    if(!musicButtonActive&&IsKeyPressed(KEY_ESCAPE)){
        menuBoxActive=!menuBoxActive;
        if(menuBoxActive) pauseTime=GetTime();
        else pauseTime=GetTime()-pauseTime;
    }
    if(gameStart&&!menuBoxActive&&!musicButtonActive&&!gameOver&&GetTime()-pauseTime-timer>=1){
        if(moveCount&1) playerGray.Update(0);
        else playerBlack.Update(0);
        if(playerBlack.IsFinished()||playerGray.IsFinished()) gameOver=1;
        timer=GetTime();
        pauseTime=0;
    }
    if(menuBoxActive&&IsKeyPressed(KEY_ENTER)){
        menuBoxActive=0;
        clearGame();
        state=GameState::MenuScreen;
    }
    Vector2 mousePos = GetMousePosition();
    if(CheckCollisionPointRec(mousePos,{210,630,60,60})){
        if(!(moveCount&1)&&blackPower[curBPower].IsUsable()&&IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            blackPower[curBPower].SetClicked(1);
        }
        blackPower[curBPower].SetHover(1);
    }
    else blackPower[curBPower].SetHover(0);
    if(CheckCollisionPointRec(mousePos,{450,630,60,60})){
        if((moveCount&1)&&grayPower[curGPower].IsUsable()&&IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            grayPower[curGPower].SetClicked(1);
        }
        grayPower[curGPower].SetHover(1);
    }
    else grayPower[curGPower].SetHover(0);
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
        else if(!menuBoxActive&&CheckCollisionPointRec(mousePos, musicGameButton.GetButtonBound())){
            musicButtonActive=!musicButtonActive;
            if(musicButtonActive) pauseTime=GetTime();
            else pauseTime=GetTime()-pauseTime;
        }
        else if(!musicButtonActive&&!menuBoxActive){
            int mouseX=std::floor(mousePos.x/60-2);
            int mouseY=std::floor(mousePos.y/60-2);
            // std::cout<<mousePos.x<<' '<<mousePos.y<<'\n';
            if(validGrid(mouseX,mouseY)){
                int enemy=grid[mouseX][mouseY];
                // std::cout<<piece<<'\n';
                if(blackPower[curBPower].IsClicked()){
                    if(enemy!=10&&blackPower[curBPower].IsOwn()&&(moveCount&1)==enemy/5){
                        blackPower[curBPower].Activate(mouseX,mouseY);
                    }
                    else if(enemy!=10&&!blackPower[curBPower].IsOwn()&&(moveCount&1)!=enemy/5){
                        blackPower[curBPower].Activate(mouseX,mouseY);
                    }
                    else{
                        blackPower[curBPower].SetClicked(0);
                        blackPower[curBPower].SetOnce();
                    }
                    if(blackPower[curBPower].IsActive()){
                        int lastX=blackPower[curBPower].GetLastX(),lastY=blackPower[curBPower].GetLastY();
                        if(curBPower==0){
                            grid[lastX][lastY]+=20;
                            blackShield=1;
                        }
                        else if(curBPower==1){
                            grid[lastX][lastY]+=30;
                            blackFreeze=1;
                        }
                        else if(curBPower==2){
                            std::swap(grid[lastX][lastY],grid[mouseX][mouseY]);
                        }
                        if(grayPower[curGPower].IsActive()){
                            grayPower[curGPower].UpdateCooldown();
                            grayPower[curGPower].UpdateDuration();
                            if(!grayPower[curGPower].IsActive()){
                                int dif=grayFreeze?30:20;
                                grid[grayPower[curGPower].GetLastX()][grayPower[curGPower].GetLastY()]-=dif;
                                // std::cout<<grid[grayPower[curGPower].GetLastX()][grayPower[curGPower].GetLastY()]<<'\n';
                                grayShield=0;
                                grayFreeze=0;
                                grayPower[curGPower].SetLast();
                            }
                        }
                        playerBlack.Update(1);
                        turnText.Update("Gray's Turn",GRAY);
                        moveCount++;
                        std::string newText=TextFormat("Turn %d",moveCount+1);
                        turnCountText.Update(newText,GRAY);
                    }
                }
                else if(grayPower[curGPower].IsClicked()){
                    if(enemy!=10&&grayPower[curGPower].IsOwn()&&(moveCount&1)==enemy/5){
                        grayPower[curGPower].Activate(mouseX,mouseY);
                    }
                    else if(enemy!=10&&!grayPower[curGPower].IsOwn()&&(moveCount&1)!=enemy/5){
                        grayPower[curGPower].Activate(mouseX,mouseY);
                    }
                    else{
                        grayPower[curGPower].SetClicked(0);
                        grayPower[curGPower].SetOnce();
                    }
                    if(grayPower[curGPower].IsActive()){
                        int lastX=grayPower[curGPower].GetLastX(),lastY=grayPower[curGPower].GetLastY();
                        if(curGPower==0){
                            grid[lastX][lastY]+=20;
                            grayShield=1;
                        }
                        else if(curGPower==1){
                            grid[lastX][lastY]+=30;
                            grayFreeze=1;
                        }
                        else if(curGPower==2){
                            std::swap(grid[lastX][lastY],grid[mouseX][mouseY]);
                        }
                        if(blackPower[curBPower].IsActive()){
                            blackPower[curBPower].UpdateCooldown();
                            blackPower[curBPower].UpdateDuration();
                            if(!blackPower[curBPower].IsActive()){
                                int dif=blackFreeze?30:20;
                                grid[blackPower[curBPower].GetLastX()][blackPower[curBPower].GetLastY()]-=dif;
                                // std::cout<<grid[blackPower[curBPower].GetLastX()][blackPower[curBPower].GetLastY()]<<'\n';
                                blackFreeze=0;
                                blackShield=0;
                                blackPower[curBPower].SetLast();
                            }
                        }
                        playerGray.Update(1);
                        turnText.Update("Black's Turn",BLACK);
                        moveCount++;
                        std::string newText=TextFormat("Turn %d",moveCount+1);
                        turnCountText.Update(newText,BLACK);
                    }
                }
                else if(!clicked&&enemy!=10){
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
                            blackPower[curBPower].UpdateCooldown();
                            blackPower[curBPower].UpdateDuration();
                            if(!blackPower[curBPower].IsActive()){
                                int dif=blackFreeze?30:20;
                                grid[blackPower[curBPower].GetLastX()][blackPower[curBPower].GetLastY()]-=dif;
                                // std::cout<<grid[blackPower[curBPower].GetLastX()][blackPower[curBPower].GetLastY()]<<'\n';
                                blackFreeze=0;
                                blackShield=0;
                                blackPower[curBPower].SetLast();
                            }
                            playerGray.Update(1);
                            turnText.Update("Black's Turn",BLACK);
                        }
                        else{
                            grayPower[curGPower].UpdateCooldown();
                            grayPower[curGPower].UpdateDuration();
                            if(!grayPower[curGPower].IsActive()){
                                int dif=grayFreeze?30:20;
                                grid[grayPower[curGPower].GetLastX()][grayPower[curGPower].GetLastY()]-=dif;
                                // std::cout<<grid[grayPower[curGPower].GetLastX()][grayPower[curGPower].GetLastY()]<<'\n';
                                grayShield=0;
                                grayFreeze=0;
                                grayPower[curGPower].SetLast();
                            }
                            playerBlack.Update(1);
                            turnText.Update("Gray's Turn",GRAY);
                        }
                        grid[x][y]=10;
                        piecePos[x][y]={150.f+60*x,150.f+60*y};
                        piecePos[mouseX][mouseY]={150.f+60*mouseX-pieceSize[grid[mouseX][mouseY]%5].x/2,150.f+60*mouseY-pieceSize[grid[mouseX][mouseY]%5].y/2};
                        moveCount++;
                        std::string newText=TextFormat("Turn %d",moveCount+1);
                        turnCountText.Update(newText,moveCount&1?GRAY:BLACK);
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

void drawMenu(){
    DrawRectangleRec(menuBox,BLACK);
    titleText.Draw();
    timerMenuText.Draw();
    incMenuText.Draw();
    timeUpButton.Draw();
    timeDownButton.Draw();
    incUpButton.Draw();
    incDownButton.Draw();
    playGameButton.Draw();
    drawMusicBox();
    for(auto &power:blackPower) power.Draw(0,0);
    for(auto &power:grayPower) power.Draw(0,1);
}

void updateMenu(){
    if(WindowShouldClose()) runWindow = 0;
    Vector2 mousePos = GetMousePosition();
    if(musicButtonActive) handleMusicBox(mousePos);
    for(int i=0;i<3;i++){
        if(CheckCollisionPointRec(mousePos,blackPower[i].GetBound())){
            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
                blackPower[curBPower].SetUsable(0);
                blackPower[i].SetUsable(1);
                curBPower=i;
            }
            blackPower[i].SetHover(1);
        }
        else blackPower[i].SetHover(0);
        if(CheckCollisionPointRec(mousePos,grayPower[i].GetBound())){
            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
                grayPower[curGPower].SetUsable(0);
                grayPower[i].SetUsable(1);
                curGPower=i;
            }
            grayPower[i].SetHover(1);
        }
        else grayPower[i].SetHover(0);
    }
    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)&&CheckCollisionPointRec(mousePos, musicGameButton.GetButtonBound())) musicButtonActive=!musicButtonActive;
    if(isHold&&IsMouseButtonUp(MOUSE_LEFT_BUTTON)) isHold=0;
    if(!musicButtonActive){
        bool changed=false,gt=false;
        if(CheckCollisionPointRec(mousePos,incDownButton.GetButtonBound())&&incTime>0){
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
        if(CheckCollisionPointRec(mousePos,incUpButton.GetButtonBound())&&incTime<5){
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
        if(CheckCollisionPointRec(mousePos,timeDownButton.GetButtonBound())&&gameTime>1){
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
        if(CheckCollisionPointRec(mousePos,timeUpButton.GetButtonBound())&&gameTime<15){
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
            if(gt){
                std::string temp="Timer: "+std::to_string(gameTime);
                float lastX=timerMenuText.GetPosX(),lastY=timerMenuText.GetEndPosY();
                timerMenuText.Update(temp);
                float difX=(timerMenuText.GetPosX()-lastX)/2,difY=(timerMenuText.GetEndPosY()-lastY)/2;
                timeUpButton.Update((timerMenuText.GetPosX()-lastX)/2,(timerMenuText.GetEndPosY()-lastY)/2);
                timeDownButton.Update((timerMenuText.GetPosX()-lastX)/2,(timerMenuText.GetEndPosY()-lastY)/2);
            }
            else{
                std::string temp="Increment: "+std::to_string(incTime);
                float lastX=incMenuText.GetPosX(),lastY=incMenuText.GetEndPosY();
                incMenuText.Update(temp);
                float difX=(incMenuText.GetPosX()-lastX)/2,difY=(incMenuText.GetEndPosY()-lastY)/2;
                incUpButton.Update(difX,difY);
                incDownButton.Update(difX,difY);
            }
        }
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)&&CheckCollisionPointRec(mousePos, playGameButton.GetButtonBound())){
            init();
            state=GameState::GameScreen;
        } 
    }
}

void drawStart(){
    blackTitleButton.Draw();
    grayTitleButton.Draw();
    titleText.Draw();
    if(GetTime()-blinkTimer>=0.5){
        blinkTimer=GetTime();
        isBlinking=!isBlinking;
    }
    if(isBlinking) enterText.Draw();
    drawMusicBox();
}

void updateStart(){
    if(WindowShouldClose()) runWindow = 0;
    if(!musicButtonActive&&IsKeyPressed(KEY_ENTER)){
        titleText.SetPosY(60);
        state=GameState::MenuScreen;
    }
    if(isHold&&IsMouseButtonUp(MOUSE_LEFT_BUTTON)) isHold=0;
    Vector2 mousePos = GetMousePosition();
    if(musicButtonActive) handleMusicBox(mousePos);
    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)&&CheckCollisionPointRec(mousePos,musicGameButton.GetButtonBound())) musicButtonActive=!musicButtonActive;
}

void initGame(){
    atlas=LoadTexture("assets/texture/game-texture.png");
    pieceSize={
        MeasureTextEx(GetFontDefault(),"C",30,1),
        MeasureTextEx(GetFontDefault(),"I",30,1),
        MeasureTextEx(GetFontDefault(),"X",30,1),
        MeasureTextEx(GetFontDefault(),"T",30,1),
        MeasureTextEx(GetFontDefault(),"U",30,1)
    };
    volume.Init("Volume:",{150,405},30,WHITE,{0,0},{1,0});
    musicGameButton.Init("Music",{screenWidth/2,660},30,BLACK,WHITE,{1,1},{0,0},0);
    volumeUpButton.Init("+",{645,390},30,WHITE,BLACK,{1,1},{0,0},1);
    volumeDownButton.Init("-",{75,390},30,WHITE,BLACK,{1,1},{0,0},1);
    nextSongButton.Init(">",{645,330},30,WHITE,BLACK,{1,1},{0,0},1);
    prevSongButton.Init("<",{75,330},30,WHITE,BLACK,{1,1},{0,0},1);
    gameOverText.Init("Game Over",{screenWidth/2,60},30,BLACK,{1,1},{0,0});
    turnText.Init("Black's Turn",{screenWidth/2,90},30,BLACK,{1,1},{0,0});
    turnCountText.Init("Turn 1",{screenWidth/2,30},30,BLACK,{1,1},{0,0});
    blackTimerText.Init("Black's Timer",{screenWidth/6,30},30,BLACK,{1,1},{0,0});
    grayTimerText.Init("Gray's Timer",{screenWidth*5/6,30},30,GRAY,{1,1},{0,0});
    resetGameText.Init("Click anywhere to reset the game!",{screenWidth/2,screenHeight/2},24,WHITE,{1,1},{0,0});
    startGameText.Init("Click anywhere to start the game!",{screenWidth/2,screenHeight/2},24,WHITE,{1,1},{0,0});
    mainMenuText.Init("Would you like to go back to main menu?",{screenWidth/2,330},24,WHITE,{1,1},{0,0});
    choiceText.Init("YES (Enter) or NO (ESC)",{screenWidth/2,390},30,WHITE,{1,1},{0,0});
    titleText.Init("Catur Abjad",{screenWidth/2,screenHeight/2},90,BLACK,{1,0},{0,0});
    enterText.Init("Press enter to start game",{screenWidth/2,screenHeight/2},24,BLACK,{1,0},{0,1});
    blackTitleButton.Init("U T X I C v",{screenWidth/2,titleText.GetPosY()-10},24,BLACK,BLUE,{0,0},{0,0},1);
    grayTitleButton.Init("s C I X T U",{screenWidth/2+20,blackTitleButton.GetPosY()+10},24,GRAY,GREEN,{0,0},{1,1},1);
    menuBox={
        titleText.GetPosX(),
        screenHeight/2-100,
        480,
        200
    };
    timerMenuText.Init("Timer: 5",{screenWidth/2,menuBox.y+30},30,WHITE,{0,0},{0,1});
    incMenuText.Init("Increment: 3",{screenWidth/2,timerMenuText.GetEndPosY()+30},30,WHITE,{0,0},{0,1});
    playGameButton.Init("Play Game",{screenWidth/2,incMenuText.GetEndPosY()+30},30,BLACK,WHITE,{1,0},{0,1},1);
    timeDownButton.Init("-",{timerMenuText.GetEndPosX()+20,timerMenuText.GetPosY()},30,BLACK,WHITE,{0,0},{1,1},1);
    timeUpButton.Init("+",{timeDownButton.GetEndPosX()+20,timerMenuText.GetPosY()},30,BLACK,WHITE,{0,0},{1,1},1);
    incDownButton.Init("-",{incMenuText.GetEndPosX()+20,incMenuText.GetPosY()},30,BLACK,WHITE,{0,0},{1,1},1);
    incUpButton.Init("+",{incDownButton.GetEndPosX()+20,incMenuText.GetPosY()},30,BLACK,WHITE,{0,0},{1,1},1);
    songText.Init("Song: ganyangffff.mp3",{screenWidth/2,315},30,WHITE,{1,0},{0,1});
    blackPower[0].Init(
        {0,0,60,60},
        "Make one piece immune to enemy capture for 1 turn\nCooldown: 10 turns",
        {1,0},
        {menuBox.x-70,menuBox.y},
        24,
        10,
        1,
        WHITE,
        BLACK,
        {0,0},
        {1,1},
        1
    );
    blackPower[0].SetUsable(1);
    grayPower[0].Init(
        {0,0,60,60},
        "Make one piece immune to enemy capture for 1 turn\nCooldown: 10 turns",
        {1,0},
        {menuBox.x+menuBox.width+10,menuBox.y},
        24,
        10,
        1,
        WHITE,
        BLACK,
        {0,0},
        {0,1},
        1
    );
    grayPower[0].SetUsable(1);
    blackPower[1].Init(
        {124,0,60,60},
        "Make one enemy piece unable to move for 1 turn\nCooldown: 10 turns",
        {0,0},
        {menuBox.x-70,menuBox.y+70},
        24,
        10,
        1,
        WHITE,
        BLACK,
        {0,0},
        {1,1},
        1
    );
    grayPower[1].Init(
        {124,0,60,60},
        "Make one enemy piece unable to move for 1 turn\nCooldown: 10 turns",
        {0,0},
        {menuBox.x+menuBox.width+10,menuBox.y+70},
        24,
        10,
        1,
        WHITE,
        BLACK,
        {0,0},
        {0,1},
        1
    );
    blackPower[2].Init(
        {186,0,60,60},
        "Swap the positions of your own two pieces\nCooldown: 10 turns",
        {1,1},
        {menuBox.x-70,menuBox.y+140},
        24,
        10,
        1,
        WHITE,
        BLACK,
        {0,0},
        {1,1},
        1
    );
    grayPower[2].Init(
        {186,0,60,60},
        "Swap the positions of your own two pieces\nCooldown: 10 turns",
        {1,1},
        {menuBox.x+menuBox.width+10,menuBox.y+140},
        24,
        10,
        1,
        WHITE,
        BLACK,
        {0,0},
        {0,1},
        1
    );

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
                songList.push_back(filename);
                song.push_back(newSong);
            }
        }
    }
    if(!song.empty()) PlayMusicStream(song[curSong]);
}

int main(){
    // init window
    InitWindow(screenWidth, screenHeight, "Catur Abjad");
    InitAudioDevice();
    SetAudioStreamBufferSizeDefault(9216);
    SetExitKey(KEY_NULL);
    SetTargetFPS(30);
    // BeginBlendMode(BLEND_ALPHA);
    // SetTextureFilter(atlas,TEXTURE_FILTER_ANISOTROPIC_16X);
    initGame();
    // ToggleFullscreen();
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
    UnloadTexture(atlas);
    for(Music &i:song) UnloadMusicStream(i);
    CloseAudioDevice();
    CloseWindow();
}