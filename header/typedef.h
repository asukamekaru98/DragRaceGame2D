#pragma once

typedef unsigned char		uchar;
typedef unsigned short		ushort;
typedef unsigned int		uint;
typedef signed long long	long2;
typedef unsigned long long	ulong2;

// 旧型エイリアス（既存ソースが使用中のため併存）
typedef char				SC_1;
typedef unsigned char		UC_1;
typedef signed short		SI_2;
typedef unsigned short		UI_2;
typedef signed int			SI_4;
typedef unsigned int		UI_4;
typedef signed long long	SI_8;
typedef unsigned long long	UI_8;
typedef float				F_4;
typedef double				D_8;

typedef enum {
	INIT_RESULT_ERROR		= -1,
	INIT_RESULT_SUCCESS		= 1,

}INIT_RESULT;


typedef void (*UpdateFunc)(void);
typedef void (*DrawFunc)(void);