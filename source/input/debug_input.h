#pragma once

#ifdef _DEBUG

#include "key_input.h"

class DebugInput :public KeyInput {
private:
	bool bIsAccel_;
	bool bIsGearUpTriggered_;
	bool bIsGearDownTriggered_;
	bool bIsRetireTriggered_;

public:
	DebugInput();
	void Update() override;
	bool IsAccel() const;
	bool IsGearUpTriggered() const;
	bool IsGearDownTriggered() const;
	bool IsRetireTriggered() const;
};

#endif // _DEBUG
