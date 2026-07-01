#pragma once

typedef struct {
    const char* name;
    float       maxSpeed;       // km/h
    float       acceleration;   // 0.0 - 5.0
    int         gearCount;
    int         price;
} CarData;

extern const CarData    CAR_TABLE[];
extern const int        CAR_TABLE_COUNT;
