#include "../DxLib/DxLib.h"
#include "../header/game_data.h"
#include <string.h>

GameData g_gameData;

void InitGameData(void) {
    memset(&g_gameData, 0, sizeof(g_gameData));

    g_gameData.money            = 3000;
    g_gameData.carCount         = 1;
    g_gameData.selectedCarIndex = 0;
    g_gameData.lastTime         = 0.0f;
    g_gameData.lastRank         = 0;
    g_gameData.bestTime         = -1.0f;

    PlayerCar* car = &g_gameData.cars[0];
    strcpy_s(car->name, sizeof(car->name), "STOCK CAR");
    car->hSprite      = -1;
    car->maxSpeed     = 180.0f;
    car->acceleration = 1.5f;
    car->gearCount    = 4;
    car->price        = 0;
    for (int i = 0; i < PARTS_CAT_COUNT; i++) {
        car->equippedParts[i] = -1;
    }
}
