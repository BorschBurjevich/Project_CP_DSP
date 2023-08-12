
/*
 *  ======== MainDsp0.c ========
 */
#include <xdc/std.h>
#include <xdc/runtime/Diags.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Log.h>
#include <xdc/runtime/System.h>
#include <ti/ipc/Ipc.h>
#include <ti/ipc/MultiProc.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include "fft/fft.h"
#include <xdc/runtime/Timestamp.h>


#include "Bearing_USW/Header_Func_USW_5.h"
#include "Bearing_SW/FUNC_SW.h"
#include "defines.h"

#include <ti/mathlib/mathlib.h>
#include <ti/dsplib/dsplib.h>




extern float                w_1024[2048];
extern float                w_512[1024];
extern float                w_256[512];
extern float                ten[1024];
extern float                n_1024[2048];
extern float                n_512[1024];
extern float                n_256[512];

void taskFxn(UArg arg0, UArg arg1);

/*
 *  ======== main ========
 */
int main(int argc, char* argv[])
{
    Task_Params     tskPrms;
    Task_Params_init(&tskPrms);
    tskPrms.priority = 3;
    tskPrms.stackSize = 5000;
    Task_create (taskFxn, &tskPrms, NULL);
    BIOS_start();
}

/*
 *  ======== Server_taskFxn ========
 */

void taskFxn(UArg arg0, UArg arg1)
{
    int count;
    tw_gen (w_1024, 1024);
    tw_gen (w_512, 512);
    tw_gen (w_256, 256);

    for (count = 0; count < 512; count++)
    {
        n_256[count] = 256.0;
    }

    for (count = 0; count < 1024; count++)
    {
        ten[count]   = 10.0;
        n_512[count] = 512.0;
    }

    for (count = 0; count < 2048; count++)
    {
        n_1024[count] = 1024.0;
    }


    while(1)
    {
        if(word00 == SPECTRUM_POWER_DSP0)
        {
            spectrum_power();
            word00 = 0;
        }

        if(word00 == SPECTRUM_DSP0)
        {
            spectrum();
            word00 = 0;
        }

    }

}
