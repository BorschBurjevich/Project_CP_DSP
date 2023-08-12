#include "I2C.h"

I2C_Handle I2Cx_open(uint32_t port_num)
{
    I2C_Handle      handle;
    I2C_Params      i2cParams;

   // port_num -= 1;
    I2C_init();

    I2C_Params_init(&i2cParams);
    handle = I2C_open(port_num, &i2cParams);
    return(handle);
}

void I2Cx_close(I2C_Handle h)
{
    I2C_close((I2C_Handle)h);
}

uint16_t sensor_drv(I2C_Handle h, uint32_t slave_addr)
{
    I2C_Transaction i2cTransaction;
    //uint8_t txBuf[1] = {0x5a}; //отладочное значение
    uint8_t txBuf[1] = {0x00};
    uint8_t rxBuf[2] = {0x00, 0x00};
    int16_t status;
    uint16_t val;
    uint16_t temp;

    //Запись в датчик
    I2C_transactionInit(&i2cTransaction);
    //i2cTransaction.slaveAddress = slave_addr;
    i2cTransaction.writeBuf = (uint8_t *)&txBuf[0];
    i2cTransaction.writeCount = 1;
    i2cTransaction.readBuf = (uint8_t *)&rxBuf[0];
    i2cTransaction.readCount = 0;
    i2cTransaction.timeout   = 2000U;
    status = I2C_transfer(h, &i2cTransaction);

    mdelay(100);

    if(I2C_STS_SUCCESS == status)
    {
        //Чтение из датчика
        I2C_transactionInit(&i2cTransaction);
       // i2cTransaction.slaveAddress = slave_addr;
        i2cTransaction.writeBuf = (uint8_t *)&txBuf[0];
        i2cTransaction.writeCount = 0;
        i2cTransaction.readBuf = (uint8_t *)&rxBuf[0];
        i2cTransaction.readCount = 2;
        i2cTransaction.timeout   = 2000U;
        status = I2C_transfer(h, &i2cTransaction);

        if(I2C_STS_SUCCESS == status)
        {
            val = (rxBuf[0] << 4) | (rxBuf[1] >> 4);
            temp = (val * 625) / 10000;
        }
    }

    return(temp);
}


int16_t I2Cx_transfer(I2C_Handle h, uint32_t slave_addr, uint32_t data)
{
    I2C_Transaction i2cTransaction;
    int16_t status;
    uint8_t txBuf[6] = {0};
    uint8_t rxBuf[4] = {0};

    txBuf[0] = 0x42;
    txBuf[1] = 0x4;
    txBuf[2] = (uint8_t)(data >> 24);
    txBuf[3] = (uint8_t)(data >> 16);
    txBuf[4] = (uint8_t)(data >> 8);
    txBuf[5] = (uint8_t)(data >> 0);

    //Отправка сообщения
    I2C_transactionInit(&i2cTransaction);
    //i2cTransaction.slaveAddress = slave_addr;
    i2cTransaction.writeBuf = (uint8_t *)&txBuf[0];
    i2cTransaction.writeCount = 6;
    i2cTransaction.readBuf = (uint8_t *)&rxBuf;
    i2cTransaction.readCount = 0;
    i2cTransaction.timeout   = 10000U;
    status = I2C_transfer(h, &i2cTransaction);
    mdelay(1);

    //ожидание ответа
    if(I2C_STS_SUCCESS == status)
    {
        I2C_transactionInit(&i2cTransaction);
        //i2cTransaction.slaveAddress = slave_addr;
        i2cTransaction.writeBuf = (uint8_t *)&txBuf[0];
        i2cTransaction.writeCount = 0;
        i2cTransaction.readBuf = (uint8_t *)&rxBuf[0];
        i2cTransaction.readCount = 4;
        i2cTransaction.timeout   = 10000U;
        status = I2C_transfer(h, &i2cTransaction);
    }

    data = rxBuf[0] << 24 | rxBuf[1] << 16 | rxBuf[2] << 8 | rxBuf[0];
    return(data);
}


void I2Cx_mux(void)
{
    //// Скоммутирваоть на свою разводку при необходимости, на данный момент можно не использовать ////
//    HW_WR_REG32(0x4A0097A0, 0x00000002);//enable i2c1 clocks
//    HW_WR_REG32(0x4A0097B0, 0x00000002);//enable i2c3 clocks
//    HW_WR_REG32(0x4A0097B8, 0x00000002);//enable i2c4 clocks
//    HW_WR_REG32(0x4A005578, 0x00000002);//enable i2c5 clocks
//
//    HW_WR_REG32(0x4A003568, 0x0006000e);//gpio4_1  f2
//
//    HW_WR_REG32(0x4A003800, 0x00070000);//sda
//    HW_WR_REG32(0x4A003804, 0x00070000);//scl
//
//    HW_WR_REG32(0x4A0036A4, 0x0006000A);//gpio7_31      i2c3_sda
//    HW_WR_REG32(0x4A0036A8, 0x000E000A);//gpio7_30      i2c3_scl
//
//    HW_WR_REG32(0x4A003440, 0x00040007);//gpmc_a0      i2c4_sda
//    HW_WR_REG32(0x4A003444, 0x00040007);//gpmc_a1      i2c4_scl
//
//    HW_WR_REG32(0x4A0036B8, 0x000C000A);//mcasp1_axr1      i2c5_sda
//    HW_WR_REG32(0x4A003454, 0x00040007);//gpmc_a5      i2c5_scl
    ////////////////////////////////////////////////////////////////////
                        ///POU_N///
    HW_WR_REG32(0x4A0097A8, 0x00000002);//enable i2c2 clocks
    HW_WR_REG32(0x4A0097B0, 0x00000002);//enable i2c3 clocks

   // HW_WR_REG32(0x4A003568, 0x0006000e);//gpio4_1  f2

    HW_WR_REG32(0x4A003774, 0x00060002); //gpio6_10     i2c3_sda
    HW_WR_REG32(0x4A003778, 0x000E0002); //gpio6_11     i2c3_scl

    HW_WR_REG32(0x4A00380C, 0x00060000);//i2c2_scl      i2c2_scl
    HW_WR_REG32(0x4A003808, 0x00060000);//i2c2_sda      i2c2_sda



}
