#include "customize_input.h"
#include "../../DxLib/DxLib.h"

CustomizeInput::CustomizeInput()
	: bIsUpTriggered_(false)
	, bIsDownTriggered_(false)
	, bIsLeftTriggered_(false)
	, bIsRightTriggered_(false)
	, bIsDecideTriggered_(false)
	, bIsCancelTriggered_(false)
{
}

void CustomizeInput::Update()
{
	RefreshKeyState();

	bIsUpTriggered_     = IsTriggered(KEY_INPUT_UP);
	bIsDownTriggered_   = IsTriggered(KEY_INPUT_DOWN);
	bIsLeftTriggered_   = IsTriggered(KEY_INPUT_LEFT);
	bIsRightTriggered_  = IsTriggered(KEY_INPUT_RIGHT);
	bIsDecideTriggered_ = IsTriggered(KEY_INPUT_Z) || IsTriggered(KEY_INPUT_RETURN);
	bIsCancelTriggered_ = IsTriggered(KEY_INPUT_X) || IsTriggered(KEY_INPUT_ESCAPE);
}

bool CustomizeInput::IsUpTriggered() const
{
	return bIsUpTriggered_;
}

bool CustomizeInput::IsDownTriggered() const
{
	return bIsDownTriggered_;
}

bool CustomizeInput::IsLeftTriggered() const
{
	return bIsLeftTriggered_;
}

bool CustomizeInput::IsRightTriggered() const
{
	return bIsRightTriggered_;
}

bool CustomizeInput::IsDecideTriggered() const
{
	return bIsDecideTriggered_;
}

bool CustomizeInput::IsCancelTriggered() const
{
	return bIsCancelTriggered_;
}
