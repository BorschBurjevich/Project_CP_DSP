/*
 * slicer.c
 */
#include "Lorandit_functions/modes/modes.h"
extern void sliser(int32_t *j10, int32_t *j12, int32_t *j13);
/*typedef struct
{
    int32_t band_now;
    int32_t band_next;
    int32_t slsr_cmd[376];
    int32_t *save_idx_band;
    int32_t num_words_slsr;

}sliser_;
sliser_ slsr;*/

/*int32_t _Pel_VarsFreq[10] = {0};
int32_t _Pel_CmdFreq[100] = {2,0,50,  0,0,  1,0,1,  3,0,100,-9,  4,0};
int32_t *_Pel_SaveIndexBand = &_Pel_CmdFreq[0];*/
//sliser(&_Pel_SaveIndexBand, _Pel_VarsFreq, _Pel_CmdFreq);
void sliser(int32_t *j10, int32_t *j12, int32_t *j13)
{
    int32_t tmp, *j11, j14 = 0, j15 = 0, j16 = 0;
    int32_t xr20, xr21;
    tmp = *j10;
    j11 = tmp;

    WhileSlises:
        j16 = *j11++;
        if(j16 == 0)
        {
            j14 = *j11++;
            j15 = *j12 + j14;
            memcpy(j10, &j11, sizeof(j11));
        }

        if(j16 == 1)
        {
            j14 = *j11++;
            xr20 = *j11++;
            xr21 = *j12 + j14;
            xr21 = xr21 + xr20;
            j12 += j14;
            memcpy(j12, &xr21, sizeof(xr21));
            goto WhileSlises;
        }

        if(j16 == 2)
        {
            j14 = *j11++;
            xr20 = *j11++;
            memcpy(j12, &xr20, sizeof(xr20));
            goto WhileSlises;
        }

        if(j16 == 3)
        {
            j14 = *j11++;
            xr20 = *j11++;
            j15 = *j11++;
            xr21 = *j12 + j14;
            if(xr21 < xr20)
            {
                j11 += j15;
                j14 = *j11;
            }
            goto WhileSlises;
        }

        if(j16 == 4)
        {
            j14 = *j11++;
            j11 = j13 + j14;
            goto WhileSlises;
        }
}


