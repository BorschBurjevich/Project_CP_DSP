/*
 * Shared.c
 *
 *  Created on: 10 сент. 2020 г.
 *
 */

#include "Shared.h"
float  rxDSPBuf0[2048] __attribute__((aligned(4), section(".buffers"))) = {0};
float  rxDSPBuf1[2048] __attribute__((aligned(4), section(".buffers"))) = {0};

float  txDSPBuf0[2048] __attribute__((aligned(4), section(".buffers")))  = {0};
float  txDSPBuf1[2048] __attribute__((aligned(4), section(".buffers")))  = {0};
float  txDSPBuf2[2048] __attribute__((aligned(4), section(".buffers")))  = {0};
float  txDSPBuf3[2048] __attribute__((aligned(4), section(".buffers")))  = {0};
float  txDSPBuf4[2048] __attribute__((aligned(4), section(".buffers")))  = {0};
float  txDSPBuf5[2048] __attribute__((aligned(4), section(".buffers")))  = {0};

float  BearingBuf00[25000] __attribute__((aligned(4), section(".bss:variables")))  = {0};
float  BearingBuf01[25000] __attribute__((aligned(4), section(".bss:variables")))  = {0};
float  BearingBuf02[25000] __attribute__((aligned(4), section(".bss:variables")))  = {0};
float  BearingBuf03[25000] __attribute__((aligned(4), section(".bss:variables")))  = {0};
float  BearingBuf04[25000] __attribute__((aligned(4), section(".bss:variables")))  = {0};
// рамзер BearingBuf опеределяется как ((1024 точки) * 2  * 4 * 3

float  BearingBuf10[25000] __attribute__((aligned(4), section(".bss:variables")))  = {0};
float  BearingBuf11[25000] __attribute__((aligned(4), section(".bss:variables")))  = {0};
float  BearingBuf12[25000] __attribute__((aligned(4), section(".bss:variables")))  = {0};
float  BearingBuf13[25000] __attribute__((aligned(4), section(".bss:variables")))  = {0};
float  BearingBuf14[25000] __attribute__((aligned(4), section(".bss:variables")))  = {0};


extern uint8_t flag_aerial;
void sh_mem_send_time_seq(calculation_mode mode, uint32_t *buf_in, uint32_t *buf_out, uint32_t N_dots)
{
    if ((mode == SPECTRUM_POWER_DSP0) || (mode == SPECTRUM_DSP0))
    {
        //адрес входного массива
        word01 = (uint32_t)buf_in;
        Cache_inv (buf_in, sizeof(float)*2*N_dots, 0x7fff, TRUE);

        //адрес выходного
        word02 = (uint32_t)buf_out;
        Cache_inv (buf_out, sizeof(float)*N_dots, 0x7fff, TRUE);

        //число точек ффт
        word03 = N_dots;

        //выбор режима
        word00 = mode;
    }

    else if ((mode == SPECTRUM_POWER_DSP1) || (mode == SPECTRUM_DSP1))
    {
        //адрес входного массива
        word11 = (uint32_t)buf_in;
        Cache_inv (buf_in, sizeof(float)*2*N_dots, 0x7fff, TRUE);

        //адрес выходного
        word12 = (uint32_t)buf_out;
        Cache_inv (buf_out, sizeof(float)*N_dots, 0x7fff, TRUE);

        //число точек ффт
        word13 = N_dots;

        //выбор режима
        word10 = mode;
    }
}

void sh_mem_bearing(calculation_mode mode, uint32_t N_dots)
{
    if(mode == BEARING_DSP0)
    {
        if (flag_aerial == 1)                       // выбор алгоритма в зависимости от диапазона частот
        {
            word00 = (uint32_t)BEARING_SW_ANT_1_DSP0;

        }

        else if (flag_aerial == 2 || flag_aerial == 3)
        {
            word00 = (uint32_t)BEARING_USW_ANT_2_3_DSP0;

        }

        //входные массивы
        word01 = (uint32_t)BearingBuf00;
        word02 = (uint32_t)BearingBuf01;
        word03 = (uint32_t)BearingBuf02;
        word04 = (uint32_t)BearingBuf03;
        word05 = (uint32_t)BearingBuf04;

        Cache_inv (&BearingBuf00[0], sizeof(float)*N_dots, 0x7fff, TRUE);
        Cache_inv (&BearingBuf01[0], sizeof(float)*N_dots, 0x7fff, TRUE);
        Cache_inv (&BearingBuf02[0], sizeof(float)*N_dots, 0x7fff, TRUE);
        Cache_inv (&BearingBuf03[0], sizeof(float)*N_dots, 0x7fff, TRUE);
        Cache_inv (&BearingBuf04[0], sizeof(float)*N_dots, 0x7fff, TRUE);

        //указатель на выходную структуру
        word08 = (uint32_t)(&rxDSPBuf0[0]);
        Cache_inv (&(rxDSPBuf0[0]), sizeof(float)*2048, 0x7fff, TRUE);

        word09 = N_dots;


    }

    else if(mode == BEARING_DSP1)
    {

        if (flag_aerial == 1)
        {
           word10 = (uint32_t)BEARING_SW_ANT_1_DSP1;

        }

        else if (flag_aerial == 2 || flag_aerial == 3)
        {
           word10 = (uint32_t)BEARING_USW_ANT_2_3_DSP1;

        }

        //входные массивы
        word11 = (uint32_t)BearingBuf10;
        word12 = (uint32_t)BearingBuf11;
        word13 = (uint32_t)BearingBuf12;
        word14 = (uint32_t)BearingBuf13;
        word15 = (uint32_t)BearingBuf14;

        Cache_inv (&BearingBuf10[0], sizeof(float)*N_dots, 0x7fff, TRUE);
        Cache_inv (&BearingBuf11[0], sizeof(float)*N_dots, 0x7fff, TRUE);
        Cache_inv (&BearingBuf12[0], sizeof(float)*N_dots, 0x7fff, TRUE);
        Cache_inv (&BearingBuf13[0], sizeof(float)*N_dots, 0x7fff, TRUE);
        Cache_inv (&BearingBuf14[0], sizeof(float)*N_dots, 0x7fff, TRUE);

        //указатель на выходную структуру
        word18 = (uint32_t)(&rxDSPBuf1[0]);
        Cache_inv (&(rxDSPBuf1[0]), sizeof(float)*2048, 0x7fff, TRUE);

        word19 = N_dots;


    }

}

void sh_mem_recieve(core_ core)
{
    if(core == DSP0_)
    {
        while(word00 != 0)
        {
            ;;
        }
    }
    else if(core == DSP1_)
    {
        while(word10 != 0)
        {
            ;;
        }
    }
}

