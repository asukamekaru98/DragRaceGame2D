#pragma once
#include "typedef.h"
#include "parts_data.h"

#define MAX_GARAGE_CARS 8

typedef struct {
    char  name[64];
    int   hSprite;
    float maxSpeed;
    float acceleration;
    int   gearCount;
    int   price;
    int   equippedParts[PARTS_CAT_COUNT];   // -1 = none
} PlayerCar;

typedef struct {
    int       money;
    int       carCount;
    int       selectedCarIndex;
    PlayerCar cars[MAX_GARAGE_CARS];
    float     lastTime;
    int       lastRank;
    float     bestTime;                     // -1.0f = no record
} GameData;

extern GameData g_gameData;

void InitGameData(void);
