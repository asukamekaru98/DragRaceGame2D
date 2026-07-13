#include "dealer_input.h"
#include "../../DxLib/DxLib.h"

DealerInput::DealerInput()
	: bIsLeftTriggered_(false)
	, bIsRightTriggered_(false)
	, bIsDecideTriggered_(false)
	, bIsCancelTriggered_(false)
{
}

void DealerInput::Update()
{
	RefreshKeyState();

	bIsLeftTriggered_   = IsTriggered(KEY_INPUT_LEFT);
	bIsRightTriggered_  = IsTriggered(KEY_INPUT_RIGHT);
	bIsDecideTriggered_ = IsTriggered(KEY_INPUT_Z) || IsTriggered(KEY_INPUT_RETURN);
	bIsCancelTriggered_ = IsTriggered(KEY_INPUT_X) || IsTriggered(KEY_INPUT_ESCAPE);
}

bool DealerInput::IsLeftTriggered() const
{
	return bIsLeftTriggered_;
}

bool DealerInput::IsRightTriggered() const
{
	return bIsRightTriggered_;
}

bool DealerInput::IsDecideTriggered() const
{
	return bIsDecideTriggered_;
}

bool DealerInput::IsCancelTriggered() const
{
	return bIsCancelTriggered_;
}
