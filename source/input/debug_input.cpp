#include "debug_input.h"

void DebugInput::Update()
{

}

bool DebugInput::IsAccel() const
{
	return bIsAccel_;
}

bool DebugInput::IsGearUpTriggered() const
{
	return bIsGearUpTriggered_;
}

bool DebugInput::IsGearDownTriggered() const
{
	return bIsGearDownTriggered_;
}

bool DebugInput::IsRetireTriggered() const
{
	return bIsRetireTriggered_;
}