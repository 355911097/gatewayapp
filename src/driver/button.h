


#ifndef __BUTTON_H_
#define __BUTTON_H_
#include  "bsp.h"
#include <stm32f2xx.h>







typedef enum
{
	BUTTON_NULL,
	BUTTON_UP,
	BUTTON_DOWN,
	BUTTON_CLICK,
	BUTTON_LONG_PRESS
		
}button_state_e;


#define BUTTON_VALID_COUNTER 		10




#endif
