#include "delay.h"

void mdelay(uint32_t ms) // мили
{
    xdc_runtime_Types_FreqHz cpuFreq;
    uint32_t t1, t2, f;
    uint64_t dt;

    if(ms != 0)
    {
        t1 = Timestamp_get32();
        BIOS_getCpuFreq(&cpuFreq);
        f = (uint32_t)cpuFreq.lo;

        //перевод времени в такты в соответсвтии с частотой
        if(f == 1000000000)
        {
            ms *= 1000000;
        }
        if(f == 1500000000)
        {
            ms *= 1500000;
        }

        do
        {
            t2 = Timestamp_get32();
            if(t2 < t1)
            {
                dt = 0xffffffff + t2 - t1;
            }
            else
            {
                dt = t2 - t1;
            }
        }while(dt < ms);
    }
}

void udelay(uint32_t us) // микро
{
    xdc_runtime_Types_FreqHz cpuFreq;
    uint32_t t1, t2, f;
    uint64_t dt;

    if(us != 0)
    {
        t1 = Timestamp_get32();
        BIOS_getCpuFreq(&cpuFreq);
        f = (uint32_t)cpuFreq.lo;

        //перевод времени в такты в соответсвтии с частотой
        if(f == 1000000000)
        {
            us *= 1000;
        }
        if(f == 1500000000)
        {
            us *= 1500;
        }

        do
        {
            t2 = Timestamp_get32();
            if(t2 < t1)
            {
                dt = 0xffffffff + t2 - t1;
            }
            else
            {
                dt = t2 - t1;
            }
        }while(dt < us);
    }
}

void ndelay(uint32_t ns)
{
    ;;
}

