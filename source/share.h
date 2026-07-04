#pragma once
#include "typedef.h"

// �F��\���l�I�u�W�F�N�g�BDxLib �� GetColor ���B�����A
// �`��n�ɂ� Code() ���Ԃ��J���[�R�[�h�݂̂�n���B
class Color {
public:
	// constructor
	Color(uchar r, uchar g, uchar b);

	int Code() const;

	// ���F�̖��O�t���萔�����L�����o�Ƃ��Đ錾
	static const Color RED;
	static const Color GREEN;
	static const Color BLUE;
	static const Color YELLOW;
	static const Color MAGENTA;
	static const Color CYAN;
	static const Color WHITE;
	static const Color BLACK;

private:
	uchar r_, g_, b_;
};
