#include "garage_input.h"
#include "../../DxLib/DxLib.h"

GarageInput::GarageInput()
	: bIsMenuUpTriggered_(false)
	, bIsMenuDownTriggered_(false)
	, bIsCarPrevTriggered_(false)
	, bIsCarNextTriggered_(false)
	, bIsDecideTriggered_(false)
{
}

void GarageInput::Update()
{
	RefreshKeyState();

	bIsMenuUpTriggered_   = IsTriggered(KEY_INPUT_UP);
	bIsMenuDownTriggered_ = IsTriggered(KEY_INPUT_DOWN);
	bIsCarPrevTriggered_  = IsTriggered(KEY_INPUT_LEFT);
	bIsCarNextTriggered_  = IsTriggered(KEY_INPUT_RIGHT);
	bIsDecideTriggered_   = IsTriggered(KEY_INPUT_Z) || IsTriggered(KEY_INPUT_RETURN);
}

bool GarageInput::IsMenuUpTriggered() const
{
	return bIsMenuUpTriggered_;
}

bool GarageInput::IsMenuDownTriggered() const
{
	return bIsMenuDownTriggered_;
}

bool GarageInput::IsCarPrevTriggered() const
{
	return bIsCarPrevTriggered_;
}

bool GarageInput::IsCarNextTriggered() const
{
	return bIsCarNextTriggered_;
}

bool GarageInput::IsDecideTriggered() const
{
	return bIsDecideTriggered_;
}
