#include "raylib.h"
#include "raymath.h"
#include <cmath>

const int cellSize = 32;
const int mapW = 16, mapH = 16;
const char map[] = "0000222222220000"\
                   "1              0"\
                   "1     111111   0"\
                   "1     0        0"\
                   "0     0  1110000"\
                   "0     3        0"\
                   "0   10000      0"\
                   "0   3   11100  0"\
                   "5   4   0      0"\
                   "5   4   1  00000"\
                   "0       1      0"\
                   "2       1      0"\
                   "0       0      0"\
                   "0 0000000      0"\
                   "0              0"\
                   "0002222222200000";
int mapGetNum(int i, int j) { return map[i+j*mapW] - '0'; }
bool mapIsEmpty(int i, int j) { return map[i+j*mapW] == ' '; }
const int rayCount = mapW * cellSize;

int main() {
    ///// INIT /////
    InitWindow(1024, 512, "TinyRAYCaster");
    Texture2D texWalls = LoadTexture("../walltext.png");
    
    Vector2 playerPos = { 3.456, 2.345 };
    int playerAngle = 0;
    float playerFoV = 60;
    Vector2 rayEnds[rayCount];
    float rayLength[rayCount];
    int rayWallType[rayCount];
    int rayTexCoord[rayCount];
    
    ///// LOOP /////
    SetTargetFPS(60);
    while(!WindowShouldClose()){
        ///// UPDATE /////
        // Change angle
        if(IsKeyDown(KEY_D)) playerAngle = (playerAngle + 2) % 360;
        if(IsKeyDown(KEY_A)) playerAngle = (playerAngle - 2) % 360;
        if(IsKeyDown(KEY_W)) playerPos = {
            playerPos.x + 0.05f * cos(playerAngle * DEG2RAD),
            playerPos.y + 0.05f * sin(playerAngle * DEG2RAD)
        };
        if(IsKeyDown(KEY_S)) playerPos = {
            playerPos.x - 0.05f * cos(playerAngle * DEG2RAD),
            playerPos.y - 0.05f * sin(playerAngle * DEG2RAD)
        };
        
        // Raycasting
        for(int i = 0; i < rayCount; i++){
            float angle = playerAngle - playerFoV / 2 + playerFoV * i / rayCount;
            for (rayLength[i] = 0; rayLength[i] < 20; rayLength[i] += 0.01f) { 
                rayEnds[i].x = playerPos.x + rayLength[i] * cos(angle * DEG2RAD);
                rayEnds[i].y = playerPos.y + rayLength[i] * sin(angle * DEG2RAD);
                if (!mapIsEmpty(rayEnds[i].x, rayEnds[i].y)){
                    rayWallType[i] = mapGetNum((int)rayEnds[i].x, (int)rayEnds[i].y);
                    float hitx = rayEnds[i].x - floor(rayEnds[i].x + 0.5f); 
                    float hity = rayEnds[i].y - floor(rayEnds[i].y + 0.5f); 
                    int tex = (std::abs(hity) > std::abs(hitx)) ? hity * 64 : hitx * 64; // v/h
                    rayTexCoord[i] = (tex < 0) ? tex + 64 : tex; // Fix negative tex
                    break;
                }
            }
        }
        
        ///// DRAW /////
        BeginDrawing();
            ClearBackground(BLACK);
            
            // draw Map
            for(int i = 0; i < mapW; i++)
                for(int j = 0; j < mapH; j++)
                    if(!mapIsEmpty(i, j))
                        DrawRectangle(i * cellSize, j * cellSize, cellSize, cellSize, WHITE);
            
            // draw player
            DrawCircleV(Vector2Scale(playerPos, cellSize), 5.f, GREEN);
            
            // draw rays
            for(int i = 0; i < rayCount; i++){
                DrawLineV(Vector2Scale(playerPos, cellSize), Vector2Scale(rayEnds[i], cellSize), RED);
                // draw 3Dcolumns
                int columnHeight = 512 / (rayLength[i] * cos((playerAngle - playerFoV / 2 + playerFoV * i / rayCount - playerAngle) * DEG2RAD));
                DrawTexturePro(texWalls
                             , { rayWallType[i]*64.f + rayTexCoord[i], 0.f, 1.f, 64.f }
                             , { 512.f + i, 256.f - columnHeight / 2, 1.f, (float)columnHeight }
                             , { 0.f, 0.f }, 0.f, WHITE);
            }
            
            // DrawFPS(10, 10);
        EndDrawing();
    }
    
    ///// SHUT /////
    UnloadTexture(texWalls);
    CloseWindow();

    return 0;
}