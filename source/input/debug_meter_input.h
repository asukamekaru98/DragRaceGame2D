#pragma once

#ifdef _DEBUG

#include "key_input.h"

class DebugMeterInput :public KeyInput {
private:
	bool bIsAccel_;
	bool bIsGearUpTriggered_;
	bool bIsGearDownTriggered_;
	bool bIsRetireTriggered_;
	bool bIsResetTriggered_;
	bool bCarSelect1Triggered_;
	bool bCarSelect2Triggered_;
	bool bCarSelect3Triggered_;
	bool bCarSelect4Triggered_;
	bool bCarSelect5Triggered_;

public:
	DebugMeterInput();
	void Update() override;
	bool IsAccel() const;
	bool IsGearUpTriggered() const;
	bool IsGearDownTriggered() const;
	bool IsRetireTriggered() const;
	bool IsResetTriggered() const;
	bool IsCarSelect1Triggered() const;
	bool IsCarSelect2Triggered() const;
	bool IsCarSelect3Triggered() const;
	bool IsCarSelect4Triggered() const;
	bool IsCarSelect5Triggered() const;
};

#endif // _DEBUG
