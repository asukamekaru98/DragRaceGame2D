#pragma once

#include "../../header/car_data.h"

class CarEngine {
private:
	float fRPM_;
	float fSpeed_;
	int iGear_;
	float fBoostPressure_;
	CarData carData_;
	int iEquippedParts_[];

public:
	void Update(KeyInput input);
};