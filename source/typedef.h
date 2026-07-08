#pragma once
#include <string>

typedef unsigned char		uchar;
typedef unsigned short		ushort;
typedef unsigned int		uint;
typedef signed long long	long2;
typedef unsigned long long	ulong2;

typedef std::string			str;

typedef enum {
	INIT_RESULT_ERROR		= -1,
	INIT_RESULT_SUCCESS		= 1,

}INIT_RESULT;


typedef void (*UpdateFunc)(void);
typedef void (*DrawFunc)(void);