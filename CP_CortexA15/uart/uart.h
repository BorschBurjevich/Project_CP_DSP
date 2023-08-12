#include <ti/board/board.h>
#include <stdio.h>
#include <string.h>

#include <xdc/std.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

#include <ti/csl/csl_chip.h>

#include <ti/osal/osal.h>

#include <ti/drv/uart/UART.h>
#include <ti/drv/uart/UART_stdio.h>
#include <ti/drv/uart/src/UART_osal.h>
#include <ti/drv/uart/soc/UART_soc.h>

#include <ti/drv/gpio/GPIO.h>
#include <ti/drv/gpio/soc/GPIO_v1.h>
#include <ti/csl/soc.h>
#include <ti/drv/gpio/soc/GPIO_soc.h>

#include "gpio/gpio.h"
#include "delay/delay.h"

#define  UART_PORT1  (0U)
#define  UART_PORT2  (1U)
#define  UART_PORT3  (2U)

/* Define the UART test interface */
typedef struct UART_Tests_s
{
    bool     (*testFunc)(bool);
    bool     dmaMode;
    int16_t  testId;
    char     testDesc[80];

} UART_Tests;

int init_uart();
void UARTx_mux(void);
