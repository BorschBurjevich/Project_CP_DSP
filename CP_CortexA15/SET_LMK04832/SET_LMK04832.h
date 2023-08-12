/*
 * SET_LMK04832.h
 *
 *  Created on: 10 дек. 2020 г.
 *      Author: NK
 */


#ifndef SET_LMK04832_SET_LMK04832_H_
#define SET_LMK04832_SET_LMK04832_H_

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <stdint.h>
#include <math.h>
#include <ti/drv/spi/SPI.h>
#include <ti/drv/spi/soc/SPI_v1.h>
#include <ti/drv/spi/test/src/SPI_log.h>
#include <ti/drv/spi/test/src/SPI_test.h>
#include <ti/drv/spi/soc/SPI_soc.h>
#include <ti/drv/spi/src/SPI_osal.h>
#include <ti/drv/spi/MCSPI.h>


bool spi_LMK_write(SPI_Handle spiii,uint32_t data);
uint8_t spi_LMK_read(void *spiii, uint16_t addr);



uint32_t LMK_REG_TABLE[125] = {
       // #include <HexRegisterValues_new.txt>
        #include <LMK_REG_TABLE.txt>
      };
  
#endif /* SET_LMK04832_SET_LMK04832_H_ */
