/*
 * Demod.c
 *
 *  Created on: 2 θών. 2020 γ.
 *      Author: oleg
 */

#include <xdc/std.h>
#include <xdc/cfg/global.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
#include "delay/delay.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <math.h>

Void ReadFifo(Void);
Void DAC(Void);
int AM_Demod(Void);
int FM_Demod(Void);

float   DDC_data[4] = {-0.438970817148944, -1.04431289059841, 1.10760438712306, -0.132164586070945};

Void ReadFifo(Void)
{

}

Void DAC(Void)
{

}

int AM_Demod(Void)
{
    union
    {
        uint32_t v;
        int32_t i;
        float f;
        double d;
    } u;
     float fIm1, fRe1, fext, S;
     float koeff1 = 32768.0;
     float koeff2 = 255.0;
     uint32_t Im1, Re1;
     int voice;

      fext = DDC_data[0];
      Im1 = (uint32_t)fext;

      u.f = fext;

      Im1 = u.v >> 16;
      Im1 = Im1 | 0xffff0000;

      Re1 = u.v  & 0x0000ffff;
      Re1 =  Re1 | 0xffff0000;

      u.i = Im1;
      fIm1 = u.i;

      u.i = Re1;
      fRe1 = u.i;

      fIm1 = (1/koeff1)*fIm1;
      fRe1 = (1/koeff1)*fRe1;

      S = koeff2*sqrtf(fRe1*fRe1 + fIm1*fIm1);
      u.i = S;
      voice = u.i;

      return voice;
}

int FM_Demod(Void)
{
    union
     {
         uint32_t v;
         int32_t i;
         float f;
         double d;
     } u;
     float fIm1, fRe1, Im2, Re2, fext, S;
     float koeff1 = 32767.0;
     float koeff2 = 127.0;
     uint32_t Im1, Re1;
     int voice;


     fext = DDC_data[0];
     Im1 = (uint32_t)fext;

     u.f = fext;

     Im1 = u.v >> 16;
     Im1 = Im1 | 0xffff0000;

     Re1 = u.v  & 0x0000ffff;
     Re1 =  Re1 | 0xffff0000;

     u.i = Im1;
     fIm1 = u.i;

     u.i = Re1;
     fRe1 = u.i;

     fIm1 = (1/koeff1)*fIm1;
     fRe1 = (1/koeff1)*fRe1;

     Re2 = DDC_data[2];
     Im2 = DDC_data[3];

     S = (fIm1*Re2 - fRe1*Im2)/(fRe1*fRe1 +fIm1*fIm1);

     S = koeff2*S + koeff2;
     S = roundf(S);
     u.i = S;
     voice = abs(u.i);

     return voice;
}

