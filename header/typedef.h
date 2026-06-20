#pragma once

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
typedef enum				ENUM;

/*
typedef enum {
	OP_FAILURE,
	OP_SUCCESS,


}OPENING_WINDOW;
*/
typedef enum {
	INIT_RESULT_ERROR		= -1,
	INIT_RESULT_SUCCESS		= 1,

}INIT_RESULT;

/*
typedef enum{


}EXCEPTION;
*/

typedef void (*UpdateFunc)(void);
typedef void (*DrawFunc)(void);