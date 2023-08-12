#include "uart/uart.h"
extern UART_Handle  GPS;
extern UART_Handle  KOMPAS;
extern UART_Handle  GLON;

uint32_t uartTestInstance = 1;
int16_t verifyLoopback;



static void UART_initConfig(bool dmaMode)
{
    UART_HwAttrs uart_cfg;

    UART_socGetInitCfg(uartTestInstance, &uart_cfg);

#ifdef UART_DMA_ENABLE
    if (dmaMode == true)
    {
#if defined (SOC_AM65XX) || defined(SOC_J7)
        uart_cfg.edmaHandle = UartApp_udmaInit(&uart_cfg);
#else
        uart_cfg.edmaHandle = UartApp_edmaInit();
#endif
        uart_cfg.dmaMode    = TRUE;
    }
    else
#endif
    {
        uart_cfg.edmaHandle = NULL;
        uart_cfg.dmaMode    = FALSE;
    }
    uart_cfg.loopback   = FALSE;

    if(verifyLoopback)
        uart_cfg.loopback   = TRUE;
    /* Set the DMA enabled UART init configurations */
    UART_socSetInitCfg(uartTestInstance, &uart_cfg);
}

int init_uart()
{

    UART_Params  uartParams_Kompas;
    UART_Params  uartParams_GPS;
    UART_Params  uartParams_GLON;
    uint8_t testIdPrompt[1] = {0x5A};

    UART_init();

    /* UART SoC init configuration */
    UART_initConfig(false);

    /* Initialize the default configuration params. */
    UART_Params_init(&uartParams_Kompas);   // work right
    uartParams_Kompas.parityType = UART_PAR_NONE;
    uartTestInstance = UART_PORT1;
    KOMPAS = UART_open(uartTestInstance, &uartParams_Kompas);

    UART_Params_init(&uartParams_GPS); // work right
    uartParams_GPS.parityType = UART_PAR_NONE;
    uartTestInstance = UART_PORT2;
    GPS = UART_open(uartTestInstance, &uartParams_GPS);


    UART_Params_init(&uartParams_GLON);   // work right
    uartParams_GLON.parityType = UART_PAR_NONE;
    uartTestInstance = UART_PORT3;
    GLON = UART_open(uartTestInstance, &uartParams_GLON);
    GPIO_write(GPIO5_11,1);
    ////////////////////////////////
   /* while(1) // for test write/read
    {
           UART_write(KOMPAS, (void *)(uintptr_t)testIdPrompt, sizeof(testIdPrompt));
           udelay(100);
           UART_write(GPS, (void *)(uintptr_t)testIdPrompt, sizeof(testIdPrompt));
           udelay(100);
           UART_write(KOMPAS, (void *)(uintptr_t)testIdPrompt, sizeof(testIdPrompt));
           udelay(100);

    }*/
    ///////////////////////////////
    return 0;
}

void UARTx_mux(void)
{
        ////////////////////////ENABLE UART////////////////////////////////////////////
        HW_WR_REG32(0x4A009840, 0x00000002);//enable UART1 clocks(checked)
        HW_WR_REG32(0x4A0037E4, 0x000E0000);// uart1_txd
        HW_WR_REG32(0x4A0037E0, 0x000E0000);// uart1_rxd


        HW_WR_REG32(0x4A009850, 0x00000002);//enable UART3 clocks(checked)
        HW_WR_REG32(0x4A0037F8, 0x00060002);// uart3_rxd
        HW_WR_REG32(0x4A0037FC, 0x00060001);// uart3_txd


        HW_WR_REG32(0x4A009848, 0x00000002);//enable UART2 clocks(checked)
        HW_WR_REG32(0x4A0037F0, 0x00060004); //uart2_txd
        HW_WR_REG32(0x4A0037F4, 0x00060004); // uart2_rxd

}

