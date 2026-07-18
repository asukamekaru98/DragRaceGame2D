#pragma once

#ifdef _DEBUG

#include "key_input.h"

#define DEBUG_METER_CAR_SELECT_COUNT 5

// Key input for the debug car-meter screen (F2):
// throttle / gear shift / test-car select (1-5) / reset / back.
class DebugMeterInput :public KeyInput {
private:
	bool bIsAccel_;
	bool bIsGearUpTriggered_;
	bool bIsGearDownTriggered_;
	bool bIsRetireTriggered_;
	bool bIsResetTriggered_;
	int  iCarSelectTriggered_;	// 0..4 when a number key (1-5) is triggered, -1 otherwise

public:
	DebugMeterInput();
	void Update() override;
	bool IsAccel() const;
	bool IsGearUpTriggered() const;
	bool IsGearDownTriggered() const;
	bool IsRetireTriggered() const;
	bool IsResetTriggered() const;
	int  GetCarSelectTriggered() const;
};

#endif // _DEBUG
