/*
 * fft.h
 *
 *  Created on: 17 θών. 2020 γ.
 *      Author: Sergey_Trushin
 */
#include <xdc/std.h>
#include <xdc/runtime/Assert.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/Registry.h>
#include <math.h>
#include <stdio.h>
#include <ti/mathlib/mathlib.h>
#include <ti/dsplib/dsplib.h>
#include <time.h>
#include <string.h>
#include "defines.h"

/* package header files */
#include <ti/ipc/MessageQ.h>
#include <ti/ipc/MultiProc.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/family/c66/Cache.h>

#ifndef FFT_FFT_H_
#define FFT_FFT_H_



void tw_gen (float *w, int n);
void spectrum_power(void);
void spectrum(void);
void tw_gen (float *w, int n);

extern void spectrum_power_asm();
extern void memcpy_asm();
extern void butterfly();

#endif /* FFT_FFT_H_ */
