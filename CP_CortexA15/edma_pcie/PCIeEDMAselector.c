#include <edma_pcie/PCIeEDMA.h>
#include <xdc/std.h>
#include <stdio.h>
#include <string.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/BIOS.h>


int EDMA_debug = 0;
#ifdef DEBUG_MODE
    EDMA_debug = 1;
#endif

/* External Global Configuration Structure */
extern EDMA3_DRV_GblConfigParams sampleEdma3GblCfgParams[];

EDMA3_DRV_Result edma3MemToMemCpytest(
		EDMA3_DRV_Handle hEdma,
		EDMA3_Type EdmaType,
		unsigned int* src,
		unsigned int* dst,
		unsigned int acnt,
        unsigned int bcnt,
        unsigned int ccnt,
        EDMA3_DRV_SyncType syncType,
        unsigned long* totalTime);

EDMA3_DRV_Handle edmaInit(EDMA3_DRV_Handle hEdma) {
	EDMA3_DRV_Result edmaResult = EDMA3_DRV_SOK;

	if (sampleEdma3GblCfgParams[0].numRegions > 1) {
		/* For multi core test init and de-init only once per test
		 * for a core.
		 */
			hEdma = edma3init(0, &edmaResult);
			if (hEdma) {
				if(EDMA_debug) PCIE_logPrintf("edma3init() Passed\n");
			} else {
			    //PCIE_logPrintf("edma3init() Failed, error code: %d\n", edmaResult);
			}
	}
	return hEdma;

}


void edmaTransfer(EDMA3_DRV_Handle hEdma,
		          EDMA3_Type EdmaType,
		          unsigned int* src,
		          unsigned int* dst,
		          unsigned int acnt,
                  unsigned int bcnt,
                  unsigned int ccnt,
                  EDMA3_DRV_SyncType syncType,
                  unsigned long* totalTime) {


	EDMA3_DRV_Result edmaResult = EDMA3_DRV_SOK;

	if (edmaResult == EDMA3_DRV_SOK) {
		if(EDMA_debug) PCIE_logPrintf("\nStart -> EDMA3 Test memory to memory copy on Instance %d\n",0);

		edmaResult = edma3MemToMemCpytest(hEdma, EdmaType, src, dst,  acnt, bcnt, ccnt, syncType, totalTime);

		if (EDMA3_DRV_SOK != edmaResult) {
			/* Report EDMA Error */
			PCIE_logPrintf("edma3MemToMemCpytest() FAILED!\n");
			return;
		} else {
			if(EDMA_debug) PCIE_logPrintf("edma3MemToMemCpytest() Passed\n");
		}

		if(EDMA_debug) PCIE_logPrintf("\nEnd -> EDMA3 Test memory to memory copy\n\n");
	}

}

void edmaDeinit(EDMA3_DRV_Handle hEdma) {

	EDMA3_DRV_Result edmaResult = EDMA3_DRV_SOK;

	if (sampleEdma3GblCfgParams[0].numRegions == 1) {
		/* Single Region Config Do deinit */
		/* De-init EDMA3 */
		if (hEdma) {
			edmaResult = edma3deinit(0, hEdma);
			if (edmaResult != EDMA3_DRV_SOK) {
				//PCIE_logPrintf("edma3deinit() Failed, error code: %d\n", edmaResult);
			} else {
				if(EDMA_debug) PCIE_logPrintf("edma3deinit() Passed\n");
			}
		}
	}

	if (sampleEdma3GblCfgParams[0].numRegions > 1) {
			/* Multi core Do deinit */
			/* De-init EDMA3 */
			if (hEdma) {
				edmaResult = edma3deinit(0, hEdma);
				if (edmaResult != EDMA3_DRV_SOK) {
					//PCIE_logPrintf("edma3deinit() Failed, error code: %d\n",edmaResult);
				} else {
					if(EDMA_debug) PCIE_logPrintf("edma3deinit() Passed\n");
				}
			}
		}

	return;
}

/**
 *  \brief   Main sample test case which will call other EDMA3 test cases.
 *              If one wants to call Edma3 test cases, include this main
 *              test case only.
 *
 *  \return  EDMA3_DRV_SOK or EDMA3_DRV Error Code
 */
EDMA3_DRV_Result edma3MemToMemCpytest(
		EDMA3_DRV_Handle hEdma,
		EDMA3_Type EdmaType,
		unsigned int* src,
		unsigned int* dst,
		unsigned int acnt,
        unsigned int bcnt,
        unsigned int ccnt,
        EDMA3_DRV_SyncType syncType,
        unsigned long * totalTime) {
	EDMA3_DRV_Result result = EDMA3_DRV_SOK;

	if (hEdma == NULL) {
		result = EDMA3_DRV_E_INVALID_PARAM;
		return result;
	}

	switch (EdmaType) {

	case (EDMA3):
		/* Edma test without linking, async, incr mode */
		if (result == EDMA3_DRV_SOK) {
			result = edma3_test(hEdma, src, dst, acnt, bcnt, ccnt, syncType,totalTime);

			if (result == EDMA3_DRV_SOK) {
				if(EDMA_debug) PCIE_logPrintf("edma3_test (without linking) Passed\r\n");
			} else {
				PCIE_logPrintf("edma3_test (without linking) Failed\r\n");
			}
		}
		break;

	default:
		PCIE_logPrintf("EDMA type unknown.");
		break;
	}

	return result;
}
