#include <edma_pcie/PCIeEDMA.h>


/**
 *  \brief   EDMA3 mem-to-mem data copy test case, using a DMA channel.
 *
 *  \param  hEdma       [IN]      EDMA handle
 *  \param  srcBuff     [IN]      Source buffer address
 *  \param  dstBuff     [IN]      Destination buffer address
 *  \param  acnt        [IN]      Number of bytes in an array
 *  \param  bcnt        [IN]      Number of arrays in a frame
 *  \param  ccnt        [IN]      Number of frames in a block
 *  \param  syncType    [IN]      Synchronization type (A/AB Sync)
 *
 *  \return  EDMA3_DRV_SOK or EDMA3_DRV Error Code
 */
EDMA3_DRV_Result edma3_test(EDMA3_DRV_Handle hEdma, unsigned int* srcBuff,
		unsigned int* dstBuff, unsigned int acnt, unsigned int bcnt,
		unsigned int ccnt, EDMA3_DRV_SyncType syncType, unsigned long* totalTime) {
	EDMA3_DRV_Result result = EDMA3_DRV_SOK;
	EDMA3_DRV_PaRAMRegs paramSet = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	uint32_t chId = 0;
	uint32_t tcc = 0;
	int i;
	unsigned int numenabled = 0;
	unsigned int BRCnt = 0;
	int srcbidx = 0, desbidx = 0;
	int srccidx = 0, descidx = 0;
	unsigned int* srcBuff1;
	unsigned int* dstBuff1;
	unsigned long startTime=0;

	srcBuff1 = (unsigned int*) GLOBAL_ADDR((signed char*)srcBuff);
	dstBuff1 = (unsigned int*) GLOBAL_ADDR((signed char*)dstBuff);

	//Check to make sure acnt,bcnt,ccnt are at least 1
	if (acnt==0) acnt=1;
	if (bcnt==0) bcnt=1;
	if (ccnt==0) ccnt=1;

#ifdef EDMA3_ENABLE_DCACHE
	/*
	 * Note: These functions are required if the buffer is in DDR.
	 * For other cases, where buffer is NOT in DDR, user
	 * may or may not require the below functions.
	 */
	/* Flush the Source Buffer */
	if (result == EDMA3_DRV_SOK) {
		result = Edma3_CacheFlush((unsigned int) srcBuff1, (acnt * bcnt * ccnt));
	}

	/* Invalidate the Destination Buffer */
	if (result == EDMA3_DRV_SOK) {
		result = Edma3_CacheInvalidate((unsigned int) dstBuff1, (acnt * bcnt * ccnt));
	}
#endif  /* EDMA3_ENABLE_DCACHE */

    /* Set B count reload as B count. */
    BRCnt = bcnt;

	/* set the SRC/DES Indexes */
    srcbidx = (int)acnt;
    desbidx = (int)acnt;
    if (syncType == EDMA3_DRV_SYNC_A)
        {
        /* A Sync Transfer Mode */
        srccidx = (int)acnt;
        descidx = (int)acnt;
        }
    else
        {
        /* AB Sync Transfer Mode */
        srccidx = ((int)acnt * (int)bcnt);
        descidx = ((int)acnt * (int)bcnt);
        }

	/* Setup for Channel 1*/
	tcc = EDMA3_DRV_TCC_ANY;
	chId = EDMA3_DRV_DMA_CHANNEL_ANY;

	/* Request any DMA channel and any TCC */
	if (result == EDMA3_DRV_SOK) {
		result = EDMA3_DRV_requestChannel(hEdma, &chId, &tcc,
				(EDMA3_RM_EventQueue) 0, &pcie_edma_cb_isr1, NULL);
	}

	if (result == EDMA3_DRV_SOK) {
		/* Fill the PaRAM Set with transfer specific information */

		paramSet.srcAddr = (unsigned int) (srcBuff1);
		paramSet.destAddr = (unsigned int) (dstBuff1);

		/**
		 * Be Careful !!!
		 * Valid values for SRCBIDX/DSTBIDX are between -32768 and 32767
		 * Valid values for SRCCIDX/DSTCIDX are between -32768 and 32767
		 */
		paramSet.srcBIdx = srcbidx;
		paramSet.destBIdx = desbidx;
		paramSet.srcCIdx = srccidx;
		paramSet.destCIdx = descidx;

		/**
		 * Be Careful !!!
		 * Valid values for ACNT/BCNT/CCNT are between 0 and 65535.
		 * ACNT/BCNT/CCNT must be greater than or equal to 1.
		 * Maximum number of bytes in an array (ACNT) is 65535 bytes
		 * Maximum number of arrays in a frame (BCNT) is 65535
		 * Maximum number of frames in a block (CCNT) is 65535
		 */
		paramSet.aCnt = acnt;
		paramSet.bCnt = bcnt;
		paramSet.cCnt = ccnt;

		/* For AB-synchronized transfers, BCNTRLD is not used. */
		paramSet.bCntReload = BRCnt;

		paramSet.linkAddr = 0xFFFFu;

		/* Src & Dest are in INCR modes */
		paramSet.opt &= 0xFFFFFFFCu;
		/* Program the TCC */
		paramSet.opt |= ((tcc << OPT_TCC_SHIFT)& OPT_TCC_MASK);

		/* Clear TCCMODE to make sure edma does not interrupt till finished */
		paramSet.opt |= (0 << OPT_TCCMOD_SHIFT);

		/* Enable Intermediate & Final transfer completion interrupt */
		paramSet.opt |= (1 << OPT_ITCINTEN_SHIFT);
		paramSet.opt |= (1 << OPT_TCINTEN_SHIFT);

		if (syncType == EDMA3_DRV_SYNC_A) {
			paramSet.opt &= 0xFFFFFFFBu;
		} else {
			/* AB Sync Transfer Mode */
			paramSet.opt |= (1 << OPT_SYNCDIM_SHIFT);
		}

		/* Now, write the PaRAM Set. */
		result = EDMA3_DRV_setPaRAM(hEdma, chId, &paramSet);
	}

	/*
	 * Since the transfer is going to happen in Manual mode of EDMA3
	 * operation, we have to 'Enable the Transfer' multiple times.
	 * Number of times depends upon the Mode (A/AB Sync)
	 * and the different counts.
	 */
	if (result == EDMA3_DRV_SOK) {
		/*Need to activate next param*/
		if (syncType == EDMA3_DRV_SYNC_A) {
			numenabled = bcnt * ccnt;
		} else {
			/* AB Sync Transfer Mode */
			numenabled = ccnt;
		}

	    *totalTime = 0;

		for (i = 0; i < numenabled; i++) {
			irqRaised1 = 0;
			startTime = EdmaReadTime(); //Start time as soon as transfer is enabled

			/*
			 * Now enable the transfer as many times as calculated above.
			 */

			result = EDMA3_DRV_enableTransfer(hEdma, chId,
					EDMA3_DRV_TRIG_MODE_MANUAL);

			if (result != EDMA3_DRV_SOK) {
				//PCIE_logPrintf("edma3_test: EDMA3_DRV_enableTransfer "
						//"Failed, error code: %d\r\n", result);
				break;
			}

			/**Wait for a transfer completion interrupt to occur and clear it**/
			while (!irqRaised1);

		    *totalTime += (EdmaReadTime() - startTime); //End time after transfer completion

			if (result != EDMA3_DRV_SOK) {
				//PCIE_logPrintf("edma3_test: EDMA3_DRV_waitAndClearTcc "
						//"Failed, error code: %d\r\n", result);
				break;

			}

			/* Check the status of the completed transfer */
			if (irqRaised1 < 0) {
				/* Some error occured, break from the FOR loop. */
				PCIE_logPrintf("\r\nedma3_test: Event Miss Occured!!!\r\n");

				/* Clear the error bits first */
				result = EDMA3_DRV_clearErrorBits(hEdma, chId);
				break;
			}
		}

	}

	/* Free the previously allocated channel. */
	result = EDMA3_DRV_freeChannel(hEdma, chId);
	if (result != EDMA3_DRV_SOK) {
		//PCIE_logPrintf("edma3_test: EDMA3_DRV_freeChannel() FAILED, "
				//"error code: %d\r\n", result);
	}

	return result;
}

