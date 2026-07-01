#pragma once

#include "car_data.h"
#include "car_engine_data.h"
#include "typedef.h"

class CarEngine {
private:
	float fRPM_;
	float fSpeed_;
	int iGear_;
	float fBoostPressure_;
	CarData carData_;
	int iEquippedParts_[];

public:
	void Update(/*KeyInput input*/);
	CarEngineData GetData();

};