#pragma once

#include "key_input.h"

// Key input for the result screen
class ResultInput : public KeyInput {
private:
	bool bIsAnyInputPressed_;

public:
	ResultInput();
	void Update() override;
	bool IsAnyInputPressed() const;
};
