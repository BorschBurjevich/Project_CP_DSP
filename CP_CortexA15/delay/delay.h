/*
 * delay.h
 *
 *  Created on: 11 рту. 2020 у.
 *      Author: Sergey_Trushin
 */

#include <stdio.h>
#include <math.h>
#include <xdc/std.h>
#include <ti/board/board.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/Timestamp.h>
#include <ti/sysbios/BIOS.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/knl/Clock.h>


void mdelay(uint32_t ms);
void udelay(uint32_t ms);
void ndelay(uint32_t ns);

