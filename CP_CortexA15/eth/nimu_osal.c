
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>


/* Resource manager for QMSS, PA, CPPI */

/* NIMU */
//#include "ti/transport/ndk/nimu/src/v2/nimu_eth.h"

/* XDC/BIOS includes */
#include <xdc/std.h>
#include <xdc/runtime/IHeap.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Memory.h>
#include <xdc/runtime/Error.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

#include <ti/csl/tistdtypes.h>
#include <ti/csl/cslr_device.h>
/* CSL CHIP, SEM Functional layer includes */
#include <ti/csl/csl_chip.h>


void Osal_TaskSleep(uint32_t sleepTime)
{
    Task_sleep(sleepTime);
}
void Osal_TaskCreate(void* pCbFn)
{

    Task_Params     taskParams;
    Error_Block eb;

    Error_init(&eb);

    Task_Params_init(&taskParams);
    taskParams.priority = 10;
    taskParams.instance->name = "EmacPollPkt";
    Task_create((ti_sysbios_knl_Task_FuncPtr)pCbFn, &taskParams, &eb);
}

void Osal_TaskCreate_v2(void* pCbFn, uint32_t arg)
{

    Task_Params     taskParams;
    Error_Block eb;

    Error_init(&eb);

    Task_Params_init(&taskParams);
    taskParams.arg0 = arg;
    taskParams.priority = 10;
    taskParams.instance->name = "EmacPollPkt";
    Task_create((ti_sysbios_knl_Task_FuncPtr)pCbFn, &taskParams, &eb);
}
/**
 *  @b Description
 *  @n
 *      The function is used to allocate a memory block of the specified size.
 *
 *  @param[in]  num_bytes
 *      Number of bytes to be allocated.
 *
 *  @retval
 *      Allocated block address
 */
void* Osal_malloc
(
    uint32_t  num_bytes
)
{
    Error_Block     errorBlock;
    Void*           ptr;

    /* Allocate memory */
    ptr = Memory_alloc(NULL, num_bytes, 4, &errorBlock);
    if (ptr == NULL)
    {
        return NULL;
    }

    /* Return the allocated memory block. */
    return ptr;
}

/**
 *  @b Description
 *  @n
 *      The function is used to clean up a specific memory block
 *
 *  @param[in]  ptr
 *      Pointer to the memory block to be cleaned up.
 *  @param[in]  num_bytes
 *      Size of the memory block being cleaned up.
 *
 *  @retval
 *      Not Applicable
 */
void
Osal_free
(
    void*       ptr,
    uint32_t      num_bytes
)
{
    Memory_free(NULL, ptr, num_bytes);
}
