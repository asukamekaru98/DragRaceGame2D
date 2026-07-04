#include "car_data.h"

const CarData CAR_TABLE[] = {
    // name            maxSpeed  accel  gears  price
    { "STOCK COUPE",   180.0f,   1.5f,  4,      500 },
    { "SPORTS SEDAN",  220.0f,   2.5f,  5,     1500 },
    { "MUSCLE CAR",    260.0f,   3.5f,  5,     3000 },
    { "SUPER COUPE",   300.0f,   4.0f,  6,     6000 },
    { "HYPER GT",      340.0f,   5.0f,  6,    12000 },
};
const SI_4 CAR_TABLE_COUNT = (SI_4)(sizeof(CAR_TABLE) / sizeof(CAR_TABLE[0]));
