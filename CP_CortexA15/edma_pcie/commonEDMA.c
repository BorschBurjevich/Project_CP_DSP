#include <edma_pcie/PCIeEDMA.h>


/* Flag variable to check transfer completion on channel 1 */
volatile short irqRaised1 = 0;

/* Callback functions are used to handle interrupts. This is currently not enabled in this exmaple code.
 * To see how the callback function is used, refer to the EDMA LLD example project.
 */

/* Callback function 1 */
void pcie_edma_cb_isr1 (uint32_t tcc, EDMA3_RM_TccStatus status,
                        void *appData)
    {
    (void)tcc;
    (void)appData;

    switch (status)
        {
        case EDMA3_RM_XFER_COMPLETE:
            /* Transfer completed successfully */
            irqRaised1 = 1;
            break;
        case EDMA3_RM_E_CC_DMA_EVT_MISS:
            /* Transfer resulted in DMA event miss error. */
            irqRaised1 = -1;
            break;
        case EDMA3_RM_E_CC_QDMA_EVT_MISS:
            /* Transfer resulted in QDMA event miss error. */
            irqRaised1 = -2;
            break;
        default:
            break;
        }
    }

