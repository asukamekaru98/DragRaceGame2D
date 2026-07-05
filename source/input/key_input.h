#pragma once

#define KEY_INPUT_KEY_COUNT 256

class KeyInput {	// abstract
public:
	void Update() const;

private:
	bool bPrevKetState_[KEY_INPUT_KEY_COUNT];

protected:
	void RefreshKeyState() const;
	bool IsPressed(const int iKeyCode) const;
	bool IsTriggered(const int iKeyCode) const;
};