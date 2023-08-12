/**  
 * @file pcie_example_board.h
 *
 */

#ifndef _PCIE_SAMPLE_BOARD_H_
#define _PCIE_SAMPLE_BOARD_H_

#include <ti/osal/osal.h>

void PlatformUnlockMMR(void);
void PlatformPCIESS1ClockEnable(void);
void PlatformPCIESS1PllConfig(void);
void PlatformPCIESSSetPhyMode(void);
void PlatformPCIESS1Reset(void);
void PlatformPCIESS1CtrlConfig(void);
void PlatformPCIESS1PhyConfig(void);
void PlatformPCIESS2ClockEnable(void);
void PlatformPCIESS2CtrlConfig(void);
void PlatformPCIESS2Reset(void);
void PlatformPCIESS2PhyConfig(void);
SemaphoreP_Handle PlatformSetupMSIAndINTX (void *handle);
void PlatformPCIE_PERSTn_Reset(uint32_t);
void PlatformPCIE_GPIO_Init(void);

#endif

