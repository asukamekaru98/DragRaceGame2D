#pragma once
#include "typedef.h"

typedef struct {
    const char* name;
    F_4         maxSpeed;       // km/h
    F_4         acceleration;   // 0.0 - 5.0
    SI_4        gearCount;
    SI_4        price;
} CarData;

extern const CarData CAR_TABLE[];
extern const SI_4    CAR_TABLE_COUNT;
