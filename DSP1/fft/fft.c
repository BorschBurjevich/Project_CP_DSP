 /*
 * fft.c
 *
 */
#include "fft.h"
#include <string.h>

//объ€вление и выравнивание переменных
#pragma DATA_SECTION (x_sa, ".internal_data");
#pragma DATA_ALIGN(x_sa, 4);
float   x_sa[2048];

#pragma DATA_SECTION (y_sa, ".internal_data");
#pragma DATA_ALIGN(y_sa, 4);
float   y_sa[2048];

#pragma DATA_SECTION (w_1024, ".internal_data");
#pragma DATA_ALIGN(w_1024, 4);
float   w_1024 [2048];

#pragma DATA_SECTION (w_512, ".internal_data");
#pragma DATA_ALIGN(w_512, 4);
float   w_512 [1024];

#pragma DATA_SECTION (w_256, ".internal_data");
#pragma DATA_ALIGN(w_256, 4);
float   w_256 [512];

#pragma DATA_SECTION (n_1024, ".internal_data");
#pragma DATA_ALIGN(n_1024, 4);
float   n_1024 [2048];

#pragma DATA_SECTION (n_512, ".internal_data");
#pragma DATA_ALIGN(n_512, 4);
float   n_512 [1024];

#pragma DATA_SECTION (n_256, ".internal_data");
#pragma DATA_ALIGN(n_256, 4);
float   n_256 [512];

#pragma DATA_SECTION (ten, ".internal_data");
#pragma DATA_ALIGN(ten, 4);
float   ten [1024];


#pragma DATA_SECTION (brev, ".internal_data");
unsigned char brev[64] = {
    0x0, 0x20, 0x10, 0x30, 0x8, 0x28, 0x18, 0x38,
    0x4, 0x24, 0x14, 0x34, 0xc, 0x2c, 0x1c, 0x3c,
    0x2, 0x22, 0x12, 0x32, 0xa, 0x2a, 0x1a, 0x3a,
    0x6, 0x26, 0x16, 0x36, 0xe, 0x2e, 0x1e, 0x3e,
    0x1, 0x21, 0x11, 0x31, 0x9, 0x29, 0x19, 0x39,
    0x5, 0x25, 0x15, 0x35, 0xd, 0x2d, 0x1d, 0x3d,
    0x3, 0x23, 0x13, 0x33, 0xb, 0x2b, 0x1b, 0x3b,
    0x7, 0x27, 0x17, 0x37, 0xf, 0x2f, 0x1f, 0x3f
};

extern float window_1024[];
extern float window_512[];
extern float window_256[];



/**************************************
 ****************** функции************
 **************************************
 **************************************/
void spectrum_power()
{
    uint32_t n = word13;   // N fft
    Cache_inv ((float *)word11, sizeof(float)*2*n, 0x7fff, TRUE);//вынуть из кеша // word11 - data in
    switch(n)
    {
        case 256:
        {
            DSPF_sp_vecmul((float *)word11,window_256,x_sa, 2*n);//наложение окна блакман
            DSPF_sp_fftSPxSP(n, x_sa, w_256, y_sa, brev, 4, 0, n);//fft
            divsp_v(y_sa, n_256, y_sa, 2*n);//нормировка на число точек
            butterfly(y_sa,x_sa, n);//бабочка
            spectrum_power_asm(x_sa,y_sa, n);//спектр мощнности
            log10sp_v(y_sa, y_sa, n);//log10(sp)
            DSPF_sp_vecmul(y_sa,ten,(float *) word12, n);//10log10(sp) // word12 - data out
            Cache_wb ((float *)word12, sizeof(float)*2*n, 0x7fff, TRUE);//вынуть из кеша
            break;
        };

        case 512:
        {
            DSPF_sp_vecmul((float *)word11,window_512,x_sa, 2*n);
            DSPF_sp_fftSPxSP(n, x_sa, w_512, y_sa, brev, 2, 0, n);
            divsp_v(y_sa, n_512, y_sa, 2*n);//нормировка на число точек
            butterfly(y_sa,x_sa,n);//бабочка
            spectrum_power_asm(x_sa,y_sa, n);//спектр мощнности
            log10sp_v(y_sa, y_sa, n);//log10(sp)
            DSPF_sp_vecmul(y_sa,ten,(float *) word12, n);//10log10(sp)
            Cache_wb ((float *)word12, sizeof(float)*2*n, 0x7fff, TRUE);//вынуть из кеша
            break;
        };

        case 1024:
        {
            DSPF_sp_vecmul((float *)word11, window_1024,x_sa, 2*n);
            DSPF_sp_fftSPxSP(n, x_sa, w_1024, y_sa, brev, 4, 0, n);
            divsp_v(y_sa, n_1024, y_sa, 2*n);//нормировка на число точек
            butterfly(y_sa,x_sa, n);//бабочка
            spectrum_power_asm(x_sa,y_sa, n);//спектр мощнности
            log10sp_v(y_sa, y_sa, n);//log10(sp)
            DSPF_sp_vecmul(y_sa,ten,(float *) word12, n);//10log10(sp)
            Cache_wb ((float *)word12, sizeof(float)*2*n, 0x7fff, TRUE);//вынуть из кеша
            break;
        };
    }
}

void spectrum()
{
    /*uint32_t t_start, t_stop, t_overhead;
    t_start = _itoll(TSCH, TSCL);
    t_stop = _itoll(TSCH, TSCL);
    t_overhead = t_stop - t_start;*/
    uint32_t     n = word13;
    Cache_inv ((float *)word11, sizeof(float)*2*n, 0x7fff, TRUE);//вынуть из кеша
    switch(n)
    {
        case 256:
        {
            DSPF_sp_vecmul((float *)word11,window_256,x_sa, 2*n);//наложение окна блакмана
            DSPF_sp_fftSPxSP(n, x_sa, w_256, y_sa, brev, 4, 0, n);//fft
            divsp_v(y_sa, n_256, y_sa, 2*n);//нормировка на число точек
            butterfly(y_sa,(float *)word12, n);//бабочка
            Cache_wb ((float *)word12, sizeof(float)*2*n, 0x7fff, TRUE);//вынуть из кеша
            break;
        };

        case 512:
        {
            DSPF_sp_vecmul((float *)word11,window_512,x_sa, 2*n);
            DSPF_sp_fftSPxSP(n, x_sa, w_512, y_sa, brev, 2, 0, n);
            divsp_v(y_sa, n_512, y_sa, 2*n);
            butterfly(y_sa,(float *)word12, n);
            Cache_wb ((float *)word12, sizeof(float)*2*n, 0x7fff, TRUE);
            break;
        };

        case 1024:
        {
            DSPF_sp_vecmul((float *)word11,window_1024,x_sa, 2*n);
            DSPF_sp_fftSPxSP(n, x_sa, w_1024, y_sa, brev, 4, 0, n);
            divsp_v(y_sa, n_1024, y_sa, 2*n);
            butterfly(y_sa,(float *)word12, n);
            Cache_wb ((float *)word12, sizeof(float)*2*n, 0x7fff, TRUE);
            break;
        };
    }
}

void tw_gen (float *w, int n)
{
    int i, j, k;
    const double PI = 3.141592654;

    for (j = 1, k = 0; j <= n >> 2; j = j << 2)
    {
        for (i = 0; i < n >> 2; i += j)
        {

            w[k]     = (float) sinsp_i (2 * PI * i / n);
            w[k + 1] = (float) cossp_i (2 * PI * i / n);
            w[k + 2] = (float) sinsp_i (4 * PI * i / n);
            w[k + 3] = (float) cossp_i (4 * PI * i / n);
            w[k + 4] = (float) sinsp_i (6 * PI * i / n);
            w[k + 5] = (float) cossp_i (6 * PI * i / n);
            k += 6;
        }
    }
}
