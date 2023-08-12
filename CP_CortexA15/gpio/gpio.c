/* GPIO Header files */
#include <gpio/gpio.h>
#include <stdio.h>
#include <ti/drv/gpio/GPIO.h>
#include <ti/drv/gpio/soc/GPIO_v1.h>
#include <ti/csl/soc.h>
#include <ti/drv/gpio/soc/GPIO_soc.h>

/* GPIO Driver board specific pin configuration structure */
GPIO_PinConfig gpioPinConfigs[] = {

   /* Output pin : */
     GPIO_DEVICE_CONFIG(GPIO_GPIO1_27_PORT_NUM           , GPIO_GPIO1_27_PIN_NUM)         |    GPIO_CFG_OUTPUT,
     GPIO_DEVICE_CONFIG(GPIO_GPIO4_1_PORT_NUM            , GPIO_GPIO4_1_PIN_NUM)          |    GPIO_CFG_OUTPUT,
     GPIO_DEVICE_CONFIG(GPIO_GPIO4_6_PORT_NUM            , GPIO_GPIO4_6_PIN_NUM)          |    GPIO_CFG_OUTPUT,
     GPIO_DEVICE_CONFIG(GPIO_GPIO5_14_PORT_NUM           , GPIO_GPIO5_14_PIN_NUM)         |    GPIO_CFG_OUTPUT,
     GPIO_DEVICE_CONFIG(GPIO_GPIO5_9_PORT_NUM            , GPIO_GPIO5_9_PIN_NUM)          |    GPIO_CFG_OUTPUT,
     GPIO_DEVICE_CONFIG(GPIO_GPIO_VIN1_3_1_PORT_NUM      , GPIO_GPIO_VIN1_3_1_PIN_NUM)    |    GPIO_CFG_OUTPUT,
     GPIO_DEVICE_CONFIG(GPIO_GPIO8_2_PORT_NUM            , GPIO_GPIO8_2_PIN_NUM)          |    GPIO_CFG_OUTPUT,
     GPIO_DEVICE_CONFIG(GPIO_GPIO8_3_PORT_NUM            , GPIO_GPIO8_3_PIN_NUM)          |    GPIO_CFG_OUTPUT,
     GPIO_DEVICE_CONFIG(GPIO_GPIO8_4_PORT_NUM            , GPIO_GPIO8_4_PIN_NUM)          |    GPIO_CFG_OUTPUT,
     GPIO_DEVICE_CONFIG(GPIO_GPIO8_5_PORT_NUM            , GPIO_GPIO8_5_PIN_NUM)          |    GPIO_CFG_OUTPUT,
     GPIO_DEVICE_CONFIG(GPIO_GPIO8_6_PORT_NUM            , GPIO_GPIO8_6_PIN_NUM)          |    GPIO_CFG_OUTPUT,
     GPIO_DEVICE_CONFIG(GPIO_GPIO8_15_PORT_NUM           , GPIO_GPIO8_15_PIN_NUM)         |    GPIO_CFG_OUTPUT,
	 GPIO_DEVICE_CONFIG(GPIO_GPIO5_11_PORT_NUM           , GPIO_GPIO5_11_PIN_NUM)         |    GPIO_CFG_OUTPUT,
	 GPIO_DEVICE_CONFIG(GPIO_RESET_LMK_PORT_NUM          , GPIO_RESET_LMK_PIN_NUM)        |    GPIO_CFG_OUTPUT,
   //  GPIO_DEVICE_CONFIG(GPIO_GPIO5_17_PIN_U3_PORT_NUM    , GPIO_GPIO5_17_PIN_U3_PIN_NUM)  |GPIO_CFG_IN_INT_RISING|GPIO_CFG_INPUT,
   //  GPIO_DEVICE_CONFIG(GPIO_GPIO5_16_PIN_U4_PORT_NUM    , GPIO_GPIO5_16_PIN_U4_PIN_NUM)  |GPIO_CFG_IN_INT_RISING|GPIO_CFG_INPUT,

};

GPIO_CallbackFxn gpioCallbackFunctions[] = {
    NULL,
    NULL
};

/* GPIO Driver configuration structure */
GPIO_v1_Config GPIO_v1_config = {
    gpioPinConfigs,
    gpioCallbackFunctions,
    sizeof(gpioPinConfigs) / sizeof(GPIO_PinConfig),
    sizeof(gpioCallbackFunctions) / sizeof(GPIO_CallbackFxn),
    0x20,
};

uint32_t gpioBaseAddr;
uint32_t gpioPin;

int Board_crossbarInit(void)
{
    ////////  VIN1   //////////
                                                                               // Ball Number //
        HW_WR_REG32(0x4A0034DC, 0x00040000);//  CTRL_CORE_PAD_VIN1A_CLK0        AG8
        HW_WR_REG32(0x4A0034F0, 0x00040000);//  CTRL_CORE_PAD_VIN1A_VSYNC0      AF8
        HW_WR_REG32(0x4A0034EC, 0x00040000);//  CTRL_CORE_PAD_VIN1A_HSYNC0      AE9
        HW_WR_REG32(0x4A0034E4, 0x00040000);//  CTRL_CORE_PAD_VIN1A_DE0         AD9
        HW_WR_REG32(0x4A003550, 0x00040000);//  CTRL_CORE_PAD_VIN1A_D23         AD3
        HW_WR_REG32(0x4A00354C, 0x00040000);//  CTRL_CORE_PAD_VIN1A_D22         AD2
        HW_WR_REG32(0x4A003548, 0x00040000);//  CTRL_CORE_PAD_VIN1A_D21         AE6
        HW_WR_REG32(0x4A003544, 0x00040000);//  CTRL_CORE_PAD_VIN1A_D20         AE2
        HW_WR_REG32(0x4A003540, 0x00040000);//  CTRL_CORE_PAD_VIN1A_D19         AE1
        HW_WR_REG32(0x4A00353C, 0x00040000);//  CTRL_CORE_PAD_VIN1A_D18         AE5
        HW_WR_REG32(0x4A003538, 0x00040000);//  CTRL_CORE_PAD_VIN1A_D17         AE3
        HW_WR_REG32(0x4A003534, 0x00040000);//  CTRL_CORE_PAD_VIN1A_D16         AF1
        HW_WR_REG32(0x4A003530, 0x00040000);//  CTRL_CORE_PAD_VIN1A_D15         AF4
        HW_WR_REG32(0x4A00352C, 0x00040000);//  CTRL_CORE_PAD_VIN1A_D14         AF3
        HW_WR_REG32(0x4A003528, 0x00040000);//  CTRL_CORE_PAD_VIN1A_D13         AF6
        HW_WR_REG32(0x4A003524, 0x00040000);//  CTRL_CORE_PAD_VIN1A_D12         AF2
        HW_WR_REG32(0x4A003520, 0x00040000);//  CTRL_CORE_PAD_VIN1A_D11         AG5
        HW_WR_REG32(0x4A00351C, 0x00040000);//  CTRL_CORE_PAD_VIN1A_D10         AG3
        HW_WR_REG32(0x4A003518, 0x00040000);//  CTRL_CORE_PAD_VIN1A_D9          AG2
        HW_WR_REG32(0x4A003514, 0x00040000);//  CTRL_CORE_PAD_VIN1A_D8          AG4
        HW_WR_REG32(0x4A003510, 0x00040000);//  CTRL_CORE_PAD_VIN1A_D7          AH4
        HW_WR_REG32(0x4A00350C, 0x00040000);//  CTRL_CORE_PAD_VIN1A_D6          AG6
        HW_WR_REG32(0x4A003508, 0x00040000);//  CTRL_CORE_PAD_VIN1A_D5          AH5
        HW_WR_REG32(0x4A003504, 0x00040000);//  CTRL_CORE_PAD_VIN1A_D4          AH3
        HW_WR_REG32(0x4A003500, 0x00040000);//  CTRL_CORE_PAD_VIN1A_D3          AH6
        HW_WR_REG32(0x4A0034FC, 0x00040000);//  CTRL_CORE_PAD_VIN1A_D2          AG7
        HW_WR_REG32(0x4A0034F8, 0x00040000);//  CTRL_CORE_PAD_VIN1A_D1          AD8
        HW_WR_REG32(0x4A0034F4, 0x00040000);//  CTRL_CORE_PAD_VIN1A_D0          AE8
        //////////////////////////////////////////////////////////////////////////

                           /////// VIN2   //////

        HW_WR_REG32(0x4A003554, 0x00040000);//  CTRL_CORE_PAD_VIN2A_CLK0        E1
        HW_WR_REG32(0x4A003564, 0x00040000);//  CTRL_CORE_PAD_VIN2A_VSYNC0      G6
        HW_WR_REG32(0x4A003560, 0x00040000);//  CTRL_CORE_PAD_VIN2A_HSYNC0      G1
        HW_WR_REG32(0x4A00355C, 0x00040000);//  CTRL_CORE_PAD_VIN2A_FLD0        H7
        HW_WR_REG32(0x4A003558, 0x00040000);//  CTRL_CORE_PAD_VIN2A_DE0         G2
        HW_WR_REG32(0x4A0035C4, 0x00040000);//  CTRL_CORE_PAD_VIN2A_D23         A4
        HW_WR_REG32(0x4A0035C0, 0x00040000);//  CTRL_CORE_PAD_VIN2A_D22         B5
        HW_WR_REG32(0x4A0035BC, 0x00040000);//  CTRL_CORE_PAD_VIN2A_D21         B4
        HW_WR_REG32(0x4A0035B8, 0x00040000);//  CTRL_CORE_PAD_VIN2A_D20         B3
        HW_WR_REG32(0x4A0035B4, 0x00040000);//  CTRL_CORE_PAD_VIN2A_D19         A3
        HW_WR_REG32(0x4A0035B0, 0x00040000);//  CTRL_CORE_PAD_VIN2A_D18         C5
        HW_WR_REG32(0x4A0035AC, 0x00040000);//  CTRL_CORE_PAD_VIN2A_D17         D6
        HW_WR_REG32(0x4A0035A8, 0x00040000);//  CTRL_CORE_PAD_VIN2A_D16         B2
        HW_WR_REG32(0x4A0035A4, 0x00040000);//  CTRL_CORE_PAD_VIN2A_D15         C4
        HW_WR_REG32(0x4A0035A0, 0x00040000);//  CTRL_CORE_PAD_VIN2A_D14         C3
        HW_WR_REG32(0x4A00359C, 0x00040000);//  CTRL_CORE_PAD_VIN2A_D13         C2
        HW_WR_REG32(0x4A003598, 0x00040000);//  CTRL_CORE_PAD_VIN2A_D12         D5
        HW_WR_REG32(0x4A003594, 0x00040000);//  CTRL_CORE_PAD_VIN2A_D11         F6
        HW_WR_REG32(0x4A003590, 0x00040000);//  CTRL_CORE_PAD_VIN2A_D10         D3
        HW_WR_REG32(0x4A00358C, 0x00040000);//  CTRL_CORE_PAD_VIN2A_D9          E6
        HW_WR_REG32(0x4A003588, 0x00040000);//  CTRL_CORE_PAD_VIN2A_D8          F5
        HW_WR_REG32(0x4A003584, 0x00040000);//  CTRL_CORE_PAD_VIN2A_D7          E4
        HW_WR_REG32(0x4A003580, 0x00040000);//  CTRL_CORE_PAD_VIN2A_D6          C1
        HW_WR_REG32(0x4A00357C, 0x00040000);//  CTRL_CORE_PAD_VIN2A_D5          F4
        HW_WR_REG32(0x4A003578, 0x00040000);//  CTRL_CORE_PAD_VIN2A_D4          D2
        HW_WR_REG32(0x4A003574, 0x00040000);//  CTRL_CORE_PAD_VIN2A_D3          E2
        HW_WR_REG32(0x4A003570, 0x00040000);//  CTRL_CORE_PAD_VIN2A_D2          D1
        HW_WR_REG32(0x4A00356C, 0x00040000);//  CTRL_CORE_PAD_VIN2A_D1          F3
        HW_WR_REG32(0x4A003568, 0x00040000);//  CTRL_CORE_PAD_VIN2A_D0          F2

       //--------- Шина выбора каналов для вип порта
        HW_WR_REG32(0x4A0035E8, 0x0000000e);//канал 1
        HW_WR_REG32(0x4A0035EC, 0x0000000e);//канал 2
        HW_WR_REG32(0x4A0035F0, 0x0000000e);//канал 3
        HW_WR_REG32(0x4A0035F4, 0x0000000e);//канал 4

        //////////////////   GPIO 5_14   //////////////////////
        HW_WR_REG32(0x4A003728, 0x000A000E);//  CTRL_CORE_PAD_MCASP3_FSX          gpio5_14           F15
        //////////////////////////////////////////////////////////////////////////////
        /////////////////    GPIO 5_9    ////////////////////
        HW_WR_REG32(0x4A0036D0, 0x0004000E);//  CTRL_CORE_PAD_MCASP1_AXR7          gpio5_9           D12

        /////////////////    GPIO_VIN1_3_1    ////////////////////
        HW_WR_REG32(0x4A0034E8, 0x0004000E);//  CTRL_CORE_PAD_MCASP1_AXR7    gpio3_1           AF9

           //gpio_fpga lorandt

        HW_WR_REG32(0x4A0035E4, 0x0004000E);                                // gpio8_2           F10  //enable for vin1a
        HW_WR_REG32(0x4A003618, 0x0000000E);                                // gpio8_15          C7   //reset for Cyclone5
        ///////////////////                      //////////////////////////
        HW_WR_REG32(0x4A0036D8, 0x000C000E);                                // gpio5_11           A11 // UART KOMPAS_RE_DE
        HW_WR_REG32(0x4A003638,0x0004000E);
      ///////////////////////////////////////////////////////////

       /* SPI4 Clock Enable */
       HW_WR_REG32(0x4A009808, 0x00000002);//CM_CORE__L4PER_CM_L4PER_MCSPI4_CLKCTRL

       /* GPIO4 Clock Enable */
       HW_WR_REG32(0x4A009770, 0x00000102);//CM_CORE__L4PER_CM_L4PER_GPIO4_CLKCTRL

       /* SPI4_CLK */
       HW_WR_REG32((SOC_CORE_PAD_IO_REGISTERS_BASE + CTRL_CORE_PAD_GPMC_A8),
                   (CTRL_CORE_PAD_GPMC_A8_MUXMODE));

       /* SPI4_D1 */
       HW_WR_REG32((SOC_CORE_PAD_IO_REGISTERS_BASE + CTRL_CORE_PAD_GPMC_A9),
                   (CTRL_CORE_PAD_GPMC_A9_MUXMODE));

       /* SPI4_D0 */
       HW_WR_REG32((SOC_CORE_PAD_IO_REGISTERS_BASE + CTRL_CORE_PAD_GPMC_A10),
                   (CTRL_CORE_PAD_GPMC_A10_MUXMODE));

       /* SPI4_CS0 */
       HW_WR_REG32((SOC_CORE_PAD_IO_REGISTERS_BASE + CTRL_CORE_PAD_GPMC_A11),
                   (CTRL_CORE_PAD_GPMC_A11_MUXMODE));

       /* SPI4_CS1 */
       HW_WR_REG32((SOC_CORE_PAD_IO_REGISTERS_BASE + CTRL_CORE_PAD_GPMC_A12),
                     (CTRL_CORE_PAD_GPMC_A12_MUXMODE));

       HW_WR_REG32((SOC_CORE_PAD_IO_REGISTERS_BASE + CTRL_CORE_PAD_GPMC_A13),
                     (CTRL_CORE_PAD_GPMC_A13_MUXMODE));

       HW_WR_REG32((SOC_CORE_PAD_IO_REGISTERS_BASE + CTRL_CORE_PAD_GPMC_A14),
                     (CTRL_CORE_PAD_GPMC_A14_MUXMODE));


    return 0;
}
int init_gpio()
{
    GPIO_init();

    return 0;
}

