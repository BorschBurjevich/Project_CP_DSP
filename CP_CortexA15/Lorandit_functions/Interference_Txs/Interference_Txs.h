/*
 * Interference_Txs.h
 *
 *  Created on: 2 окт. 2020 г.
 *      Author: Sergey_Trushin
 */

#ifndef LORANDIT_FUNCTIONS_INTERFERENCE_TXS_INTERFERENCE_TXS_H_
#define LORANDIT_FUNCTIONS_INTERFERENCE_TXS_INTERFERENCE_TXS_H_

#include "Lorandit_functions/Cod_com.h"
#include "gpio/gpio.h"

typedef enum{
    ms20  = 0x00,
    ms5   = 0x01,
    us250 = 0x03,
    ms1   = 0x02,
}supr_duration;

typedef enum{
    carry = 0x00,
    chmsh1= 0x01,
    chmsh2= 0x02,
    chmn1 = 0x03,
    chmn8 = 0x04,
    fmn1  = 0x05,
    fmn2  = 0x06,
    lchm  = 0x08,
    qcarry = 0x09,
    wifip = 0x0a,
    dmrp  = 0x0b,
    tetrap = 0x0c,
    gps = 0x0f,
}modulation_type;

typedef struct
{
    modulation_type type[4];
    uint32_t        freq[4];
    uint32_t        param1[4];
    uint32_t        param2[4];
}interf_matrix;


#endif /* LORANDIT_FUNCTIONS_INTERFERENCE_TXS_INTERFERENCE_TXS_H_ */
