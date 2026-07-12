#include "debug_meter_input.h"

#ifdef _DEBUG

#include "../../DxLib/DxLib.h"

DebugMeterInput::DebugMeterInput()
	: bIsAccel_(false)
	, bIsGearUpTriggered_(false)
	, bIsGearDownTriggered_(false)
	, bIsRetireTriggered_(false)
{
}

void DebugMeterInput::Update()
{
	RefreshKeyState();

	// spec/detail_design_debug.md 5.2
	bIsAccel_ = IsPressed(KEY_INPUT_Z);
	bIsGearUpTriggered_ = IsTriggered(KEY_INPUT_RIGHT);
	bIsGearDownTriggered_ = IsTriggered(KEY_INPUT_LEFT);
	bIsRetireTriggered_ = IsTriggered(KEY_INPUT_ESCAPE);
	bIsResetTriggered_ = IsTriggered(KEY_INPUT_ESCAPE);
	bCarSelect1Triggered_ = IsTriggered(KEY_INPUT_1);
	bCarSelect2Triggered_ = IsTriggered(KEY_INPUT_2);
	bCarSelect3Triggered_ = IsTriggered(KEY_INPUT_3);
	bCarSelect4Triggered_ = IsTriggered(KEY_INPUT_4);
	bCarSelect5Triggered_ = IsTriggered(KEY_INPUT_5);
}

bool DebugMeterInput::IsAccel() const
{
	return bIsAccel_;
}

bool DebugMeterInput::IsGearUpTriggered() const
{
	return bIsGearUpTriggered_;
}

bool DebugMeterInput::IsGearDownTriggered() const
{
	return bIsGearDownTriggered_;
}

bool DebugMeterInput::IsRetireTriggered() const
{
	return bIsRetireTriggered_;
}

bool DebugMeterInput::IsResetTriggered() const
{
	return bIsRetireTriggered_;
}

bool DebugMeterInput::IsCarSelect1Triggered() const
{
	return bCarSelect1Triggered_;
}

bool DebugMeterInput::IsCarSelect2Triggered() const
{
	return bCarSelect2Triggered_;
}

bool DebugMeterInput::IsCarSelect3Triggered() const
{
	return bCarSelect3Triggered_;
}

bool DebugMeterInput::IsCarSelect4Triggered() const
{
	return bCarSelect4Triggered_;
}

bool DebugMeterInput::IsCarSelect5Triggered() const
{
	return bCarSelect5Triggered_;
}

#endif // _DEBUG