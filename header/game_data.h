#pragma once
#include "typedef.h"
#include "parts_data.h"

#define MAX_GARAGE_CARS 8

typedef struct {
    char  name[64];
    int   hSprite;
    F_4   maxSpeed;
    F_4   acceleration;
    SI_4  gearCount;
    SI_4  price;
    SI_4  equippedParts[PARTS_CAT_COUNT];   // -1 = none
} PlayerCar;

typedef struct {
    SI_4      money;
    SI_4      carCount;
    SI_4      selectedCarIndex;
    PlayerCar cars[MAX_GARAGE_CARS];
    F_4       lastTime;
    SI_4      lastRank;
    F_4       bestTime;                     // -1.0f = no record
} GameData;

extern GameData g_gameData;

void InitGameData(void);
