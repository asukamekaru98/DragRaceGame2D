#pragma once

#include "car_engine_data.h"

class GaugeBase {
public:
	virtual void Update(CarEngineData data) = 0;
	virtual void Draw() = 0;
	virtual void PlayOpening() = 0;
};