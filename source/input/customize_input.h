#pragma once

#include "key_input.h"

// Key input for the customize screen
class CustomizeInput : public KeyInput {
private:
	bool bIsUpTriggered_;
	bool bIsDownTriggered_;
	bool bIsLeftTriggered_;
	bool bIsRightTriggered_;
	bool bIsDecideTriggered_;
	bool bIsCancelTriggered_;

public:
	CustomizeInput();
	void Update() override;
	bool IsUpTriggered() const;
	bool IsDownTriggered() const;
	bool IsLeftTriggered() const;
	bool IsRightTriggered() const;
	bool IsDecideTriggered() const;
	bool IsCancelTriggered() const;
};
