#include "SPI/SPI_header.h"
#include "gpio/gpio.h"
uint8_t rx_buf[4];

/*num_spi = 1 - MCSPI1
 * num_spi = 2 - MCSPI2 CS0
 * num_spi = 3 - MCSPI2 CS1
 * num_spi = 4 - MCSPI3*/

MCSPI_Handle MCSPI_Master_Open(uint32_t portNum,uint8_t frm_size, uint32_t bit_rate, uint8_t num_spi)
{
    MCSPI_Params          mcspiParams1;    /* MCSPI params structure */ // for MCSPI1 AD1,AD2,AD3
    MCSPI_Params          mcspiParams2_0;  /* MCSPI params structure */ // for MCSPI2 CS0
    MCSPI_Params          mcspiParams2_3;  /* MCSPI params structure */ // for MCSPI2 CS3
    MCSPI_Params          mcspiParams3;    /* MCSPI params structure */ // for MCSPI3

    SPI_v1_HWAttrs        mcspi_cfg1;
    SPI_v1_HWAttrs        mcspi_cfg2_0;
    SPI_v1_HWAttrs        mcspi_cfg2_3;
    SPI_v1_HWAttrs        mcspi_cfg3;

    MCSPI_init();

 /////////////////////////////////////////////////////////////
    //              Настройка под MCSPI1               //
    if (num_spi == 1)
    {
        MCSPI_Params_init(&mcspiParams1);
        mcspiParams1.frameFormat  = SPI_POL1_PHA1;
        mcspiParams1.bitRate      = bit_rate;
        mcspiParams1.dataSize     = 8*frm_size;

        SPI_socGetInitCfg(portNum, &mcspi_cfg1);
        mcspi_cfg1.enableIntr = false;
        mcspi_cfg1.chMode     = multi ;
        SPI_socSetInitCfg(portNum, &mcspi_cfg1);
    }
     /////////////////////////////////////////////////////////////
     //              Настройка под MCSPI2 CS0              //
    else if (num_spi == 2)
    {
        MCSPI_Params_init(&mcspiParams2_0);
        mcspiParams2_0.frameFormat  = SPI_POL1_PHA1;
        mcspiParams2_0.bitRate      = bit_rate;
        mcspiParams2_0.dataSize     = 8*frm_size;

        SPI_socGetInitCfg(portNum, &mcspi_cfg2_0);
        mcspi_cfg2_0.enableIntr = false;
        mcspi_cfg2_0.chMode     = multi ;
        SPI_socSetInitCfg(portNum, &mcspi_cfg2_0);
    }
//////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////
         //              Настройка под MCSPI2 CS3              //
    else if (num_spi == 3)
    {
        MCSPI_Params_init(&mcspiParams2_3);
        mcspiParams2_3.frameFormat  = SPI_POL1_PHA1;
        mcspiParams2_3.bitRate      = bit_rate;
        mcspiParams2_3.dataSize     = 8*frm_size;

        SPI_socGetInitCfg(portNum, &mcspi_cfg2_3);
        mcspi_cfg2_3.enableIntr = false;
        mcspi_cfg2_3.chMode     = multi ;
        SPI_socSetInitCfg(portNum, &mcspi_cfg2_3);
    }

    /////////////////////////////////////////////////////////////
       //              Настройка под MCSPI3              //
    else if (num_spi == 4)
    {
        MCSPI_Params_init(&mcspiParams3);
        mcspiParams3.frameFormat  = SPI_POL1_PHA1;
        mcspiParams3.bitRate      = bit_rate;
        mcspiParams3.dataSize     = 8*frm_size;

        SPI_socGetInitCfg(portNum, &mcspi_cfg3);
        mcspi_cfg3.enableIntr = false;
        mcspi_cfg3.chMode     = multi ;
        SPI_socSetInitCfg(portNum, &mcspi_cfg3);
    }



    if (portNum ==  0)
    {
        AD9208_MCSPI1_CS0 = (MCSPI_Handle)MCSPI_open(portNum,CS_0, &mcspiParams1);
        AD9208_MCSPI1_CS1 = (MCSPI_Handle)MCSPI_open(portNum,CS_1, &mcspiParams1);
        AD9208_MCSPI1_CS2 = (MCSPI_Handle)MCSPI_open(portNum,CS_2, &mcspiParams1);
    }
    if ((portNum ==  1)&&(num_spi == 2))
    {
        MCSPI2_LVDS_CS0 = (MCSPI_Handle)MCSPI_open(portNum,CS_0, &mcspiParams2_0);

    }
    if ((portNum ==  1)&&(num_spi == 3))
    {

        MCSPI2_LVDS_CS3 = (MCSPI_Handle)MCSPI_open(portNum,CS_3, &mcspiParams2_3);
    }

    if (portNum ==  2)
    {
        MCSPI3_CS0 = (MCSPI_Handle)MCSPI_open(portNum,CS_0, &mcspiParams3);
        MCSPI3_CS1 = (MCSPI_Handle)MCSPI_open(portNum,CS_1, &mcspiParams3);
        MCSPI3_CS2 = (MCSPI_Handle)MCSPI_open(portNum,CS_2, &mcspiParams3);
        MCSPI3_CS3 = (MCSPI_Handle)MCSPI_open(portNum,CS_2, &mcspiParams3);
    }


    return 0;
}



SPI_Handle SPI_Master_Open(uint32_t portNum, uint32_t bit_rate, uint8_t frame_size)
{
    SPI_Params          spiParams;  /* SPI params structure */
     /* SPI handle */
    SPI_v1_HWAttrs      spi_cfg;

    SPI_Handle  handle;

    SPI_init();
    SPI_Params_init(&spiParams);
    spiParams.frameFormat  = SPI_POL1_PHA1;
    spiParams.bitRate      = bit_rate;
    spiParams.dataSize     = 8*frame_size;

    SPI_socGetInitCfg(portNum, &spi_cfg);
    spi_cfg.enableIntr = false;
    spi_cfg.chMode     = multi;
    SPI_socSetInitCfg(portNum, &spi_cfg);


    handle = SPI_open(portNum, &spiParams);
    return (handle);
}


void SPI_Close(SPI_Handle handle)
{
    SPI_close(handle);
}

uint32_t spi_master_xfer(void *spi, uint8_t *data, uint32_t n)
{
    bool retval;
    SPI_Transaction      transaction;

    transaction.count = n;
    transaction.txBuf = &data;
    transaction.rxBuf = NULL;
    retval = SPI_transfer((SPI_Handle)spi, &transaction);
    return (retval);
}


void SPI_callback(SPI_Handle handle, SPI_Transaction *transaction)
{
    ;;
}
bool spi1_write(void *spi, uint16_t addr, uint8_t data)
{
    bool            retVal;
    uint8_t         txBuf[3] = {0};
    SPI_Transaction      transaction;
    /* Initiate transfer */

    txBuf[2] = (uint8_t)((addr & 0xff00) >> 8);
    txBuf[1] = (uint8_t)(addr & 0x00ff);
    txBuf[0] = data;


    spi1_cs0 = spi1_cs0 & 0xE7FFFFFF;
    spi1_cs1 = spi1_cs1 & 0xE7FFFFFF;
    spi1_cs2 = spi1_cs2 & 0xE7FFFFFF;


    transaction.count = 1;
    transaction.txBuf = &txBuf[0];
    transaction.rxBuf = NULL;

    retVal = MCSPI_transfer((MCSPI_Handle)spi, &transaction);
    udelay(150);
    return (retVal);
}

uint8_t spi1_read(void *spi, uint16_t addr)
{
    uint8_t         txBuf[3] = {0};
    uint8_t         rxBuf[3] = {0};
    SPI_Transaction      transaction;
    /* Initiate transfer */
    addr = 0x8000 | addr;

    txBuf[2] = (uint8_t)((addr & 0xff00) >> 8);
    txBuf[1] = (uint8_t)(addr & 0x00ff);
    txBuf[0] = 0x00;

    spi1_cs0 = spi1_cs0 & 0xE7FFFFFF;
    spi1_cs1 = spi1_cs1 & 0xE7FFFFFF;
    spi1_cs2 = spi1_cs2 & 0xE7FFFFFF;

    transaction.count = 1;
    transaction.txBuf = &txBuf[0];
    transaction.rxBuf = &rxBuf[0];

    MCSPI_transfer((MCSPI_Handle)spi, &transaction);
    udelay(150);
    return (rxBuf[0]);
}

bool spi3_write(void *spi,uint32_t cmd_in)
{
    bool            retVal;
    uint8_t         txBuf[4] = {0};
    SPI_Transaction transaction;
    memcpy(&txBuf[0],&cmd_in,4);
    /* Initiate transfer */
    spi3_cs0  = spi3_cs0  & 0xE7FFFFFF;
    spi3_cs1  = spi3_cs1  & 0xE7FFFFFF;
    spi3_cs2  = spi3_cs2  & 0xE7FFFFFF;
    spi3_cs3  = spi3_cs3  & 0xE7FFFFFF;
    transaction.count = 1;
    transaction.txBuf = &txBuf[0];
    transaction.rxBuf = NULL;

    retVal = MCSPI_transfer((MCSPI_Handle)spi, &transaction);
    udelay(50);
    return (retVal);
}

bool spi3FPGA_write(MCSPI_Handle pointer_spi,uint8_t* buf_trans ,int count_tr)
{
    uint8_t         txBuf[10];
    uint8_t         rxBuf[10];
    bool            retVal;
    SPI_Transaction transaction;

    memset(&txBuf[0],0, sizeof(txBuf[0])*10);
    memset(&rxBuf[0],0, sizeof(rxBuf[0])*10);

    txBuf[0]  = buf_trans[0];
    txBuf[1]  = buf_trans[1];
    txBuf[2]  = buf_trans[2];
    txBuf[3]  = buf_trans[3];
    txBuf[4]  = buf_trans[4];
    txBuf[5]  = buf_trans[5];
    txBuf[6]  = buf_trans[6];
    txBuf[7]  = buf_trans[7];
    txBuf[8]  = buf_trans[8];
    txBuf[9]  = buf_trans[9];
    txBuf[10] = buf_trans[10];

    /* Initiate transfer */
    spi3_cs0  = spi3_cs0  & 0xE7FFFFFF;
    spi3_cs1  = spi3_cs1  & 0xE7FFFFFF;
    spi3_cs2  = spi3_cs2  & 0xE7FFFFFF;
    spi3_cs3  = spi3_cs3  & 0xE7FFFFFF;

    transaction.count = count_tr;
    transaction.txBuf = &txBuf[0];
    transaction.rxBuf = NULL;

    retVal = MCSPI_transfer((MCSPI_Handle)pointer_spi, &transaction);
    udelay(50);
    return (retVal);

}

uint8_t spi3_read(void *spi,uint32_t cmd_in)
{

    uint8_t         txBuf[4] = {0};
    uint8_t         rxBuf[4] = {0};
    SPI_Transaction transaction;
    memcpy(&txBuf[0],&cmd_in,4);
    /* Initiate transfer */
    spi3_cs0  = spi3_cs0  & 0xE7FFFFFF;
    spi3_cs1  = spi3_cs1  & 0xE7FFFFFF;
    spi3_cs2  = spi3_cs2  & 0xE7FFFFFF;
    spi3_cs3  = spi3_cs3  & 0xE7FFFFFF;

    transaction.count = 1;
    transaction.txBuf = &txBuf[0];
    transaction.rxBuf = &rxBuf[0];

    MCSPI_transfer((MCSPI_Handle)spi, &transaction);
    udelay(50);
    return (rxBuf[0]);
}


bool spi2_LVDS_write(MCSPI_Handle pointer_spi,uint16_t data, int count_tr)
{
    bool            retVal;
    uint8_t         txBuf[2] = {0};


    SPI_Transaction transaction;

    txBuf[1] = (uint8_t)((data & 0xff00) >> 8);
    txBuf[0] = (uint8_t)(data & 0x00ff);

    spi2_cs0  = spi2_cs0  & 0xE7FFFFFF;
    spi2_cs3  = spi2_cs3  & 0xE7FFFFFF;

    transaction.count = count_tr;
    transaction.txBuf = &txBuf[0];
    transaction.rxBuf = NULL;

    retVal = MCSPI_transfer((MCSPI_Handle)pointer_spi, &transaction);
    udelay(50);
    return (retVal);

}

uint8_t  spi2_LVDS_read(MCSPI_Handle pointer_spi,int count_tr)
{

    uint8_t         txBuf[4] = {0};
    rxSPI_Buf[0] = 0;
    rxSPI_Buf[1] = 0;
    rxSPI_Buf[2] = 0;
    rxSPI_Buf[3] = 0;

    SPI_Transaction transaction;

    spi2_cs0  = spi2_cs0  & 0xE7FFFFFF;
    spi2_cs3  = spi2_cs3  & 0xE7FFFFFF;

    transaction.count = count_tr;
    transaction.txBuf = &txBuf[0];
    transaction.rxBuf = &rxSPI_Buf[0];

    MCSPI_transfer((MCSPI_Handle)pointer_spi, &transaction);
    udelay(50);
    return (rxSPI_Buf[0]);
}

void MCSPI_Board_crossbarInit()
{
        HW_WR_REG32(0x4A009808, 0x00000002);// SPI4 Clock Enable
        HW_WR_REG32(0x4A009800, 0x00000002);// SPI3 Clock Enable
        HW_WR_REG32(0x4A0097F8, 0x00000002);// SPI2 Clock Enable
        HW_WR_REG32(0x4A0097F0, 0x00000002);// SPI1 Clock Enable


        // init spi1                                                        // Ball Number //
        HW_WR_REG32(0x4A0037B0, 0x00040000);//  CTRL_CORE_PAD_SPI1_CS0          A24
        HW_WR_REG32(0x4A0037B4, 0x00040000);//  CTRL_CORE_PAD_SPI1_CS1          A22
        HW_WR_REG32(0x4A0037B8, 0x00040000);//  CTRL_CORE_PAD_SPI1_CS2          B21
        HW_WR_REG32(0x4A0037AC, 0x00040000);//  CTRL_CORE_PAD_SPI1_D0           B25
        HW_WR_REG32(0x4A0037A8, 0x00040000);//  CTRL_CORE_PAD_SPI1_D1           F16
        HW_WR_REG32(0x4A0037A4, 0x00040000);//  CTRL_CORE_PAD_SPI1_SCLK         A25

        // init spi2
        HW_WR_REG32(0x4A0037C0, 0x00060000);//  CTRL_CORE_PAD_SPI2_SCLK         A26
        HW_WR_REG32(0x4A0037C8, 0x00060000);//  CTRL_CORE_PAD_SPI2_D0           G17
        HW_WR_REG32(0x4A0037CC, 0x00060000);//  CTRL_CORE_PAD_SPI2_CS0          B24
        HW_WR_REG32(0x4A0037C4, 0x00060000);//  CTRL_CORE_PAD_SPI2_D1           B22
        HW_WR_REG32(0x4A0037BC, 0x00060003);//  CTRL_CORE_PAD_SPI1_CS3          B20

        // init spi3
        HW_WR_REG32(0x4A003638, 0x00060008);//  CTRL_CORE_PAD_VOUT1_D23         A10  (CS3)
        HW_WR_REG32(0x4A0035DC, 0x00060008);//  CTRL_CORE_PAD_VOUT1_D0          F11  (CS2)
        HW_WR_REG32(0x4A0035D0, 0x00060008);//  CTRL_CORE_PAD_VOUT1_FLD         B11  (CS1)
        HW_WR_REG32(0x4A0035C8, 0x00060008);//  CTRL_CORE_PAD_VOUT1_CLK         D11  (CS0)
        HW_WR_REG32(0x4A0035D4, 0x00060008);//  CTRL_CORE_PAD_VOUT1_HSYNC       C11  (D0)
        HW_WR_REG32(0x4A0035CC, 0x00060008);//  CTRL_CORE_PAD_VOUT1_DE          B10  (D1)
        HW_WR_REG32(0x4A0035D8, 0x00060008);//  CTRL_CORE_PAD_VOUT1_VSYNC       E11  (SCLK)


        // init spi4

        HW_WR_REG32(0x4A003750, 0x00060002);//  CTRL_CORE_PAD_MCASP5_AXR1       AA4
        HW_WR_REG32(0x4A003748, 0x00060002);//  CTRL_CORE_PAD_MCASP5_FSX        AB9
        HW_WR_REG32(0x4A00374C, 0x00060002);//  CTRL_CORE_PAD_MCASP5_AXR0       AB3
        HW_WR_REG32(0x4A003744, 0x00060002);//  CTRL_CORE_PAD_MCASP5_ACLKX      AA3


      /*  /////////////////////////////////////////////////////////
        // init spi2
            HW_WR_REG32(0x4A0037C0, 0x00060000);//  CTRL_CORE_PAD_SPI2_SCLK         A26
            HW_WR_REG32(0x4A0037C8, 0x00060000);//  CTRL_CORE_PAD_SPI2_D0           G17
            HW_WR_REG32(0x4A0037CC, 0x00060000);//  CTRL_CORE_PAD_SPI2_CS0          B24
            HW_WR_REG32(0x4A0037C4, 0x00060000);//  CTRL_CORE_PAD_SPI2_D1           B22
          //  HW_WR_REG32(0x4A0037BC, 0x00060003);//  CTRL_CORE_PAD_SPI1_CS3          B20
    */

}
