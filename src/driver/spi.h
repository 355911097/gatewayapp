



#ifndef __SPI_H_
#define __SPI_H_


#include "bsp.h"
#include "stm32f2xx_spi.h"














void bsp_spi1_init(void);
u8 spi1_exchange_byte(u8 ch);


#endif
