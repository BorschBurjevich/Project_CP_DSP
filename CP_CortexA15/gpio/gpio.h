#ifndef _GPIO_BOARD_H
#define _GPIO_BOARD_H

#include <ti/drv/gpio/GPIO.h>

/* Example/Board Header files */
#include <ti/board/board.h>

#define GPIO1   (*((unsigned int *) 0x4AE1013C))
#define GPIO2   (*((unsigned int *) 0x4805513C))
#define GPIO3   (*((unsigned int *) 0x4805713C))
#define GPIO4   (*((unsigned int *) 0x4805913C))
#define GPIO5   (*((unsigned int *) 0x4805B13C))
#define GPIO6   (*((unsigned int *) 0x4805D13C))
#define GPIO7   (*((unsigned int *) 0x4805113C))
#define GPIO8   (*((unsigned int *) 0x4805313C))

/* ON Board pins which are connected to GPIO pins. */
typedef enum GPIO_FPGA {
        GPIO1_27 = 0,
        GPIO4_1,
        GPIO4_6,
        GPIO5_14,              // для ethernet активный ноль
        GPIO5_9,
        GPIO_VIN1_3_1,
        GPIO8_2, // enable
        GPIO8_3,// выбор 1 канала
        GPIO8_4,// выбор 2 канала
        GPIO8_5,// выбор 3 канала
        GPIO8_6,// выбор 4 канала
        GPIO8_15,
	    GPIO5_11,//UART_KOMPAS_RE_DE
	    GPIO_RESET_LMK,
      //  GPIO5_17_PIN_U3,
      //  GPIO5_16_PIN_U4,

} GPIO_FPGA;

#define GPIO_PIN_VAL_LOW     (0U)
#define GPIO_PIN_VAL_HIGH    (1U)


#define GPIO_GPIO1_27_PORT_NUM          (0x1)
#define GPIO_GPIO1_27_PIN_NUM           (0x1B)
#define GPIO_GPIO4_1_PORT_NUM           (0x4)
#define GPIO_GPIO4_1_PIN_NUM            (0x1)
#define GPIO_GPIO4_6_PORT_NUM           (0x4)
#define GPIO_GPIO4_6_PIN_NUM            (0x6)
#define GPIO_GPIO5_14_PORT_NUM          (0x5)
#define GPIO_GPIO5_14_PIN_NUM           (0xE)
#define GPIO_GPIO5_9_PORT_NUM           (0x5)
#define GPIO_GPIO5_9_PIN_NUM            (0x9)
#define GPIO_GPIO_VIN1_3_1_PORT_NUM     (0x3)
#define GPIO_GPIO_VIN1_3_1_PIN_NUM      (0x1)
//#define GPIO_GPIO_VIN2_3_30_PORT_NUM    (0x3)
//#define GPIO_GPIO_VIN2_3_30_PIN_NUM     (0x1E)
#define GPIO_GPIO8_2_PORT_NUM           (0x8)
#define GPIO_GPIO8_2_PIN_NUM            (0x2)
#define GPIO_GPIO8_3_PORT_NUM           (8U)
#define GPIO_GPIO8_3_PIN_NUM            (3U)
#define GPIO_GPIO8_4_PORT_NUM           (8U)
#define GPIO_GPIO8_4_PIN_NUM            (4U)
#define GPIO_GPIO8_5_PORT_NUM           (8U)
#define GPIO_GPIO8_5_PIN_NUM            (5U)
#define GPIO_GPIO8_6_PORT_NUM           (8U)
#define GPIO_GPIO8_6_PIN_NUM            (6U)
#define GPIO_GPIO8_15_PORT_NUM          (8U)
#define GPIO_GPIO8_15_PIN_NUM           (15U)
#define GPIO_GPIO5_11_PORT_NUM          (5U)
#define GPIO_GPIO5_11_PIN_NUM           (11U)
#define GPIO_RESET_LMK_PORT_NUM         (8U)
#define GPIO_RESET_LMK_PIN_NUM          (23U)
//#define GPIO_GPIO5_17_PIN_U3_PORT_NUM   (5U)
//#define GPIO_GPIO5_17_PIN_U3_PIN_NUM    (17U)
//#define GPIO_GPIO5_16_PIN_U4_PORT_NUM   (5U)
//#define GPIO_GPIO5_16_PIN_U4_PIN_NUM    (16U)


#define CTRL_CORE_PAD_GPMC_WAIT0                    (0xD8U)     //RESET    AD1

#define CTRL_CORE_PAD_MCASP2_AXR6                   (0x31CU)    //RESET    AD2

#define CTRL_CORE_PAD_GPMC_WAIT0_MUXMODE            (0xE0000 | 0xE)//pull down

#define CTRL_CORE_PAD_MCASP2_AXR6_MUXMODE           (0x40000 | 0xE)


#define CTRL_CORE_PAD_GPMC_A8                       (0x60U)     //SPI4_CLK
#define CTRL_CORE_PAD_GPMC_A9                       (0x64U)     //SPI4_D1
#define CTRL_CORE_PAD_GPMC_A10                      (0x68U)     //SPI4_D0
#define CTRL_CORE_PAD_GPMC_A11                      (0x6CU)     //SPI4_CS0 AD1
#define CTRL_CORE_PAD_GPMC_A12                      (0x70U)     //SPI4_CS1 AD2
#define CTRL_CORE_PAD_GPMC_A13                      (0x74U)     //SPI4_CS2 AD3
#define CTRL_CORE_PAD_GPMC_A14                      (0x78U)     //SPI4_CS3 AD4


#define CTRL_CORE_PAD_GPMC_A8_MUXMODE               (0x40000 | 0x8)
#define CTRL_CORE_PAD_GPMC_A9_MUXMODE               (0x40000 | 0x8)
#define CTRL_CORE_PAD_GPMC_A10_MUXMODE              (0x40000 | 0x8)
#define CTRL_CORE_PAD_GPMC_A11_MUXMODE              (0x40000 | 0x8)
#define CTRL_CORE_PAD_GPMC_A12_MUXMODE              (0x40000 | 0x8)
#define CTRL_CORE_PAD_GPMC_A13_MUXMODE              (0x40000 | 0x8)
#define CTRL_CORE_PAD_GPMC_A14_MUXMODE              (0x40000 | 0x8)

#define CSL_MPU_CORE_PAD_IO_REGISTERS_REGS          (0x4a003400U)

#define SOC_CORE_PAD_IO_REGISTERS_BASE       (CSL_MPU_CORE_PAD_IO_REGISTERS_REGS)


int Board_crossbarInit(void);
int init_gpio(void);

#endif /* _GPIO_BOARD_H */
