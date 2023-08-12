///
/// \file LMS7002M_lml.c
///
/// Lime-light config for the LMS7002M C driver.
///
/// \copyright
/// Copyright (c) 2014-2016 Fairwaves, Inc.
/// Copyright (c) 2014-2016 Rice University
/// SPDX-License-Identifier: Apache-2.0
/// http://www.apache.org/licenses/LICENSE-2.0
///

#include <stdlib.h>
#include "LMS7002M_impl.h"

void LMS7002M_set_spi_mode(LMS7002M_t *self, const int numWires)
{
    //LML is in global register space
    LMS7002M_set_mac_ch(self, LMS_CHAB); //

    if (numWires == 3) self->regs->reg_0x0021_spimode = REG_0X0021_SPIMODE_3WIRE;
    if (numWires == 4) self->regs->reg_0x0021_spimode = REG_0X0021_SPIMODE_4WIRE;

    LMS7002M_regs_spi_write(self, 0x0021);

}

void LMS7002M_reset(LMS7002M_t *self)
{
    //LML is in global register space
    LMS7002M_set_mac_ch(self, LMS_CHAB);
    LMS7002M_usb_write(0x0020,0x1);
   // LMS7002M_spi_write(self, 0x0020, 0x1);
    LMS7002M_regs_spi_write(self, 0x0020);
    LMS7002M_regs_spi_write(self, 0x002E);//must write



}

void LMS7002M_reset_lml_fifo(LMS7002M_t *self, const LMS7002M_dir_t direction)
{
    //LML is in global register space
    LMS7002M_set_mac_ch(self, LMS_CHAB);

    //put into reset
    if (direction == LMS_RX)
    {
        self->regs->reg_0x0020_srst_rxfifo = 0;
        self->regs->reg_0x0020_lrst_rx_a = 0;
        self->regs->reg_0x0020_lrst_rx_b = 0;
    }
    if (direction == LMS_TX)
    {
        self->regs->reg_0x0020_srst_txfifo = 0;
        self->regs->reg_0x0020_lrst_tx_a = 0;
        self->regs->reg_0x0020_lrst_tx_b = 0;
    }
    LMS7002M_regs_spi_write(self, 0x0020);

    //clear resets
    if (direction == LMS_RX)
    {
        self->regs->reg_0x0020_srst_rxfifo = 1;
        self->regs->reg_0x0020_lrst_rx_a = 1;
        self->regs->reg_0x0020_lrst_rx_b = 1;
    }
    if (direction == LMS_TX)
    {
        self->regs->reg_0x0020_srst_txfifo = 1;
        self->regs->reg_0x0020_lrst_tx_a = 1;
        self->regs->reg_0x0020_lrst_tx_b = 1;
    }
    LMS7002M_regs_spi_write(self, 0x0020);
}

void LMS7002M_power_on(LMS7002M_t *self)
{
    int answer;
    //LML is in global register space
    LMS7002M_set_mac_ch(self, LMS_CHAB);//LMS_CHAB

    self->regs->reg_0x0020_rxen_a = 1;//
    self->regs->reg_0x0020_rxen_b = 1; //reg_0x0020_rxen_b = 1
    self->regs->reg_0x0020_txen_a = 1; //
    self->regs->reg_0x0020_txen_b = 1; //
    LMS7002M_regs_spi_write(self, 0x0020);
    answer = LMS7002M_regs_spi_read(self, 0x0020);
    LMS7002M_ldo_enable(self, false, LMS7002M_LDO_ALL);
}

void LMS7002M_power_down(LMS7002M_t *self)
{
    //LML is in global register space
    LMS7002M_set_mac_ch(self, LMS_CHAB);

    self->regs->reg_0x0020_rxen_a = 0;
    self->regs->reg_0x0020_rxen_b = 0;
    self->regs->reg_0x0020_txen_a = 0;
    self->regs->reg_0x0020_txen_b = 0;
    LMS7002M_regs_spi_write(self, 0x0020);

    LMS7002M_ldo_enable(self, false, LMS7002M_LDO_ALL);
    LMS7002M_afe_enable(self, LMS_TX, LMS_CHA, false);
    LMS7002M_afe_enable(self, LMS_TX, LMS_CHB, false);
    LMS7002M_afe_enable(self, LMS_RX, LMS_CHA, false);
    LMS7002M_afe_enable(self, LMS_RX, LMS_CHB, false);
    LMS7002M_rxtsp_enable(self, LMS_CHAB, false);
    LMS7002M_txtsp_enable(self, LMS_CHAB, false);
    LMS7002M_rbb_enable(self, LMS_CHAB, false);
    LMS7002M_tbb_enable(self, LMS_CHAB, false);
    LMS7002M_rfe_enable(self, LMS_CHAB, false);
    LMS7002M_trf_enable(self, LMS_CHAB, false);
    LMS7002M_sxx_enable(self, LMS_RX, false);
    LMS7002M_sxx_enable(self, LMS_TX, false);
}

void LMS7002M_configure_lml_port_TRXIQ(LMS7002M_t *self, const LMS7002M_port_t portNo, const LMS7002M_dir_t direction, const int mclkDiv)
{
    //LML is in global register space
    LMS7002M_set_mac_ch(self, LMS_CHA);

    //set TRXIQ on both ports
    if (portNo == LMS_PORT1)
    {
        self->regs->reg_0x0023_lml1_mode = REG_0X0023_LML1_MODE_TRXIQ;
        self->regs->reg_0x0023_lml1_rxntxiq = (direction==LMS_TX)?
            REG_0X0023_LML1_RXNTXIQ_RXIQ:REG_0X0023_LML1_RXNTXIQ_TXIQ; //WARNING: TX/RX perspective swap
    }
    if (portNo == LMS_PORT2)
    {
        self->regs->reg_0x0023_lml2_mode = REG_0X0023_LML2_MODE_TRXIQ;
        self->regs->reg_0x0023_lml2_rxntxiq = (direction==LMS_TX)?
            REG_0X0023_LML2_RXNTXIQ_RXIQ:REG_0X0023_LML2_RXNTXIQ_TXIQ; //WARNING: TX/RX perspective swap
    }

    //automatic directions based on mode above
    self->regs->reg_0x0023_enabledirctr1 = 0;
    self->regs->reg_0x0023_enabledirctr2 = 0;
 //   self->regs->reg_0x0023_lml1_fidm = 1;
 //   self->regs->reg_0x0023_lml2_fidm = 1;


    //set the FIFO rd and wr clock muxes based on direction
    if (direction == LMS_TX)
    {
        self->regs->reg_0x002a_txrdclk_mux = REG_0X002A_TXRDCLK_MUX_TXTSPCLK;
        self->regs->reg_0x002a_txwrclk_mux = (portNo==LMS_PORT1)?REG_0X002A_TXWRCLK_MUX_FCLK1:REG_0X002A_TXWRCLK_MUX_FCLK2;
    }
    if (direction == LMS_RX)
    {
        self->regs->reg_0x002a_rxrdclk_mux = (portNo==LMS_PORT1)?REG_0X002A_RXRDCLK_MUX_MCLK1:REG_0X002A_RXRDCLK_MUX_MCLK2;
        self->regs->reg_0x002a_rxwrclk_mux = REG_0X002A_RXWRCLK_MUX_RXTSPCLK;
    }

    //data stream muxes
    if (direction == LMS_TX)
    {
        self->regs->reg_0x002a_tx_mux = (portNo==LMS_PORT1)?REG_0X002A_TX_MUX_PORT1:REG_0X002A_TX_MUX_PORT2;
    }
    if (direction == LMS_RX)
    {
        self->regs->reg_0x002a_rx_mux = REG_0X002A_RX_MUX_RXTSP;
    }

    //clock mux (outputs to mclk pin)
    if (portNo == LMS_PORT1)
    {
        self->regs->reg_0x002b_mclk1src = (direction==LMS_TX)?
            ((mclkDiv==1)?REG_0X002B_MCLK1SRC_TXTSPCLKA:REG_0X002B_MCLK1SRC_TXTSPCLKA_DIV):
            ((mclkDiv==1)?REG_0X002B_MCLK1SRC_RXTSPCLKA:REG_0X002B_MCLK1SRC_RXTSPCLKA_DIV);
    }
    if (portNo == LMS_PORT2)
    {
        self->regs->reg_0x002b_mclk2src = (direction==LMS_TX)?
            ((mclkDiv==1)?REG_0X002B_MCLK2SRC_TXTSPCLKA:REG_0X002B_MCLK2SRC_TXTSPCLKA_DIV):
            ((mclkDiv==1)?REG_0X002B_MCLK2SRC_RXTSPCLKA:REG_0X002B_MCLK2SRC_RXTSPCLKA_DIV);
    }

    //clock divider (outputs to mclk pin)
    if (direction == LMS_TX)
    {
        self->regs->reg_0x002b_txdiven = (mclkDiv > 1)?1:0;
        self->regs->reg_0x002c_txtspclk_div = (mclkDiv/2)-1;
    }
    if (direction == LMS_RX)
    {
        self->regs->reg_0x002b_rxdiven = (mclkDiv > 1)?1:0;
        self->regs->reg_0x002c_rxtspclk_div = (mclkDiv/2)-1;
    }
//    self->regs->reg_0x002b_mclk1_dly = REG_0X002B_MCLK1_DLY_3;
//    self->regs->reg_0x002b_mclk2_dly = REG_0X002B_MCLK2_DLY_3;

    LMS7002M_regs_spi_write(self, 0x0023);
//    LMS7002M_regs_spi_read(self, 0x0023);
    LMS7002M_regs_spi_write(self, 0x002A);
//    LMS7002M_regs_spi_read(self, 0x002A);
    LMS7002M_regs_spi_write(self, 0x002B);
    LMS7002M_regs_spi_write(self, 0x002C);
}


void LMS7002M_configure_lml_port(LMS7002M_t *self, const LMS7002M_port_t portNo, const LMS7002M_dir_t direction, const int mclkDiv)
{
    int answer = 0;
    //LML is in global register space
    LMS7002M_set_mac_ch(self, LMS_CHA);

    //set TRXIQ on both ports
    if (portNo == LMS_PORT1)
    {
        self->regs->reg_0x0023_lml1_mode = REG_0X0023_LML1_MODE_TRXIQ;
        self->regs->reg_0x0023_lml1_rxntxiq = (direction==LMS_TX)?
            REG_0X0023_LML1_RXNTXIQ_RXIQ:REG_0X0023_LML1_RXNTXIQ_TXIQ; //WARNING: TX/RX perspective swap
    }
    if (portNo == LMS_PORT2)
    {
        self->regs->reg_0x0023_lml2_mode = REG_0X0023_LML2_MODE_TRXIQ;
        self->regs->reg_0x0023_lml2_rxntxiq = (direction==LMS_TX)?
            REG_0X0023_LML2_RXNTXIQ_RXIQ:REG_0X0023_LML2_RXNTXIQ_TXIQ; //WARNING: TX/RX perspective swap
    }

    //automatic directions based on mode above
    self->regs->reg_0x0023_enabledirctr1 = 0;
    self->regs->reg_0x0023_enabledirctr2 = 0;
//    self->regs->reg_0x0023_lml1_fidm = 1;
//    self->regs->reg_0x0023_lml2_fidm = 1;


    //set the FIFO rd and wr clock muxes based on direction
    if (direction == LMS_TX)
    {
        self->regs->reg_0x002a_txrdclk_mux = REG_0X002A_TXRDCLK_MUX_TXTSPCLK;
        self->regs->reg_0x002a_txwrclk_mux = (portNo==LMS_PORT1)?REG_0X002A_TXWRCLK_MUX_FCLK1:REG_0X002A_TXWRCLK_MUX_FCLK2;
    }
    if (direction == LMS_RX)
    {
        self->regs->reg_0x002a_rxrdclk_mux = (portNo==LMS_PORT1)?REG_0X002A_RXRDCLK_MUX_MCLK1:REG_0X002A_RXRDCLK_MUX_MCLK2;
        self->regs->reg_0x002a_rxwrclk_mux = REG_0X002A_RXWRCLK_MUX_RXTSPCLK;
    }

    //data stream muxes
    if (direction == LMS_TX)
    {
        self->regs->reg_0x002a_tx_mux = (portNo==LMS_PORT1)?REG_0X002A_TX_MUX_PORT1:REG_0X002A_TX_MUX_PORT2;
    }
    if (direction == LMS_RX)
    {
        self->regs->reg_0x002a_rx_mux = REG_0X002A_RX_MUX_RXTSP;
    }

    //clock mux (outputs to mclk pin)
    if (portNo == LMS_PORT1)
    {
        self->regs->reg_0x002b_mclk1src = (direction==LMS_TX)?
            ((mclkDiv==1)?REG_0X002B_MCLK1SRC_TXTSPCLKA:REG_0X002B_MCLK1SRC_TXTSPCLKA_DIV):
            ((mclkDiv==1)?REG_0X002B_MCLK1SRC_RXTSPCLKA:REG_0X002B_MCLK1SRC_RXTSPCLKA_DIV);
    }
    if (portNo == LMS_PORT2)
    {
        self->regs->reg_0x002b_mclk2src = (direction==LMS_TX)?
            ((mclkDiv==1)?REG_0X002B_MCLK2SRC_TXTSPCLKA:REG_0X002B_MCLK2SRC_TXTSPCLKA_DIV):
            ((mclkDiv==1)?REG_0X002B_MCLK2SRC_RXTSPCLKA:REG_0X002B_MCLK2SRC_RXTSPCLKA_DIV);
    }

    //clock divider (outputs to mclk pin)
    if (direction == LMS_TX)
    {
        self->regs->reg_0x002b_txdiven = (mclkDiv > 1)?1:0;
        self->regs->reg_0x002c_txtspclk_div = (mclkDiv/2)-1;
    }
    if (direction == LMS_RX)
    {
        self->regs->reg_0x002b_rxdiven = (mclkDiv > 1)?1:0;
        self->regs->reg_0x002c_rxtspclk_div = (mclkDiv/2)-1;
    }
//    self->regs->reg_0x002b_mclk1_dly = REG_0X002B_MCLK1_DLY_3;
//    self->regs->reg_0x002b_mclk2_dly = REG_0X002B_MCLK2_DLY_3;

    //LMS7002M_regs_spi_write(self, 0x0023);
    //answer = LMS7002M_regs_spi_read(self, 0x0023);

  /*  LMS7002M_usb_write(0x0023,0x5551);      // при включении Jesd данные не защёдкиваются
    answer = LMS7002M_usb_read(self,0x0023);
    LMS7002M_regs_spi_write(self, 0x002A);
    answer = LMS7002M_regs_spi_read(self, 0x002A);
    LMS7002M_regs_spi_write(self, 0x002B);
    answer =  LMS7002M_regs_spi_read(self, 0x002B);
    LMS7002M_regs_spi_write(self, 0x002C);
    answer = LMS7002M_regs_spi_read(self, 0x002C);*/
    LMS7002M_usb_write(0x0020,0xFFFD); // channel A access, soft reset inactive another is default
    LMS7002M_usb_write(0x0021,0x0E9F); // default
    LMS7002M_usb_write(0x0022,0x47DF); // SISO DDR MODE on for PORT2, TRIQPULSE off, others are in default.
    LMS7002M_usb_write(0x0023,0x5559); // Jesd mode enabled for PORT1,2
    LMS7002M_usb_write(0x0024,0xE4E4); // позиция отчётов для PoRT1
    LMS7002M_usb_write(0x0025,0x0101); // количество циклов для ожидания после burst_start/burst_stop, Jesd207 mode Port1 , BB2RF
    LMS7002M_usb_write(0x0026,0x0505); // --/-- RF2BB
    LMS7002M_usb_write(0x0027,0xE4E4); // позиция отчётов для PoRT2
    LMS7002M_usb_write(0x0028,0x0A00); // количество циклов для ожидания после burst_start/burst_stop, Jesd207 mode Port2 , BB2RF
    LMS7002M_usb_write(0x0029,0x0A05); //  --/-- RF2BB
    LMS7002M_usb_write(0x002A,0x000e); //0x000e//0x0086- RX_FIFO write clock - default, RX_FIFO read clock - MCLK2(default),TXWRCLK_MUX - FCLK1(default),TX_FIFO read clock - default, RX_FIFO data - default
    //LMS7002M_usb_write(0x002b,0x0010);




  /*  answer = LMS7002M_usb_read(self,0x0023);
    LMS7002M_usb_write(0x002A,0x000a);
    answer = LMS7002M_usb_read(self, 0x002A);*/
    LMS7002M_usb_write(0x002B,0x0011); // RX clock divider enabled, others default
    answer = LMS7002M_usb_read(self, 0x002B);
    LMS7002M_regs_spi_write(self, 0x002C);
    answer = LMS7002M_regs_spi_read(self, 0x002C);

    LMS7002M_usb_write(0x0081,0x0002); // Enables direct control XBUF
    LMS7002M_usb_write(0x0124,0x0004); // calib

}
void my_LMS7002M_configure_lml_port(LMS7002M_t *self, const LMS7002M_port_t portNo, const LMS7002M_dir_t direction, const int mclkDiv)
{
    uint32_t get_data = 0;
    //LML is in global register space
    LMS7002M_set_mac_ch(self, LMS_CHA);

    //LMS7002M_usb_write(0x0020,0xFFFD);   // 0xffd1
   // get_data = LMS7002M_usb_read(self,0x0020);

    //LMS7002M_usb_write(0x0021,0x0E9F);
    //get_data = LMS7002M_usb_read(self,0x0021);
    LMS7002M_usb_write(0x0022,0x57DF);      //  включен SISODDR mode
    get_data = LMS7002M_usb_read(self,0x0022);
    LMS7002M_usb_write(0x0023,0x5558);
    get_data = LMS7002M_usb_read(self,0x0023);
    LMS7002M_usb_write(0x0024,0x44e4);      // позиция отчётов в Port1
    get_data = LMS7002M_usb_read(self,0x0024);
   // LMS7002M_usb_write(0x0025,0x0202);
 //   get_data = LMS7002M_usb_read(self,0x0025);
  //  LMS7002M_usb_write(0x0026,0x0100);
  //  get_data = LMS7002M_usb_read(self,0x0026);
    LMS7002M_usb_write(0x0027,0x44E4);      // позиция отчётов в Port2
 //   get_data = LMS7002M_usb_read(self,0x0027);
  //  LMS7002M_usb_write(0x0028,0x0202);
  //  get_data = LMS7002M_usb_read(self,0x0028);
   // LMS7002M_usb_write(0x0029,0x0100);
   // get_data = LMS7002M_usb_read(self,0x0029);
    LMS7002M_usb_write(0x002a,0x0006);//0x008a - last //0x018e      // 0x008e(RXRDCLK_MUX - FCLK2)        //0x0086 (Port1 - Data transmit port to TSP block)
    get_data =  LMS7002M_usb_read(self,0x002a);
    LMS7002M_usb_write(0x002b,0x0311);     //0x003C- есть частота, нужен делитель MCLK_CLOCK_SOURCE  важный регистр от него зависит наличие частот MCLK   // 0x0010
    get_data =  LMS7002M_usb_read(self,0x002b);
    LMS7002M_usb_write(0x002c,0xff00);      // 0x0007 0xFFFF
    get_data = LMS7002M_usb_read(self,0x002c);

   // LMS7002M_usb_write(0x002d,0x0000);
   // get_data = LMS7002M_usb_read(self,0x002d);

   // LMS7002M_usb_write(0x002e,0x0000);
   // get_data = LMS7002M_usb_read(self,0x002e);


  //  LMS7002M_usb_write(0x0081,0x0000);
  //  get_data = LMS7002M_usb_read(self,0x0081);
   /* self->regs->reg_0x0023_lml2_mode = 0x1;
    self->regs->reg_0x0023_lml2_rxntxiq = 0x0;
    self->regs->reg_0x0023_lml1_mode = 0x1;
    self->regs->reg_0x0023_lml1_rxntxiq = 0x0;
    self->regs->reg_0x0023_lml1_fidm = 0x0;
    self->regs->reg_0x0023_lml2_fidm = 0x0;
    self->regs->reg_0x0023_mod_en = 0x1;
    self->regs->reg_0x0023_enabledir1 = 0x1;
    self->regs->reg_0x0023_enabledirctr1 = 0x0;
    self->regs->reg_0x0023_enabledir2 = 0x1;
    self->regs->reg_0x0023_enabledirctr2 = 0x0;
    self->regs->reg_0x0023_diqdir1 = 0x0;
    self->regs->reg_0x0023_diqdirctr1 = 0x0;
    self->regs->reg_0x0023_diqdir2 = 0x1;
    self->regs->reg_0x0023_diqdirctr2 = 0x0;*/

    ////////////////////////////////////////////////
    /*self->regs->reg_0x0023_lml2_mode = 0x1;
    self->regs->reg_0x0023_lml2_rxntxiq = 0x1;
    self->regs->reg_0x0023_lml1_mode = 0x1;
    self->regs->reg_0x0023_lml1_rxntxiq = 0x0;
    self->regs->reg_0x0023_lml1_fidm = 0x0;
    self->regs->reg_0x0023_lml2_fidm = 0x0;
    self->regs->reg_0x0023_mod_en = 0x1;
    self->regs->reg_0x0023_enabledir1 = 0x1;
    self->regs->reg_0x0023_enabledirctr1 = 0x1;
    self->regs->reg_0x0023_enabledir2 = 0x0;
    self->regs->reg_0x0023_enabledirctr2 = 0x1;
    self->regs->reg_0x0023_diqdir1 = 0x1;
    self->regs->reg_0x0023_diqdirctr1 = 0x1;
    self->regs->reg_0x0023_diqdir2 = 0x0;
    self->regs->reg_0x0023_diqdirctr2 = 0x1;*/


  /*  LMS7002M_regs_spi_write(self, 0x0023);
    get_data =  LMS7002M_regs_spi_read(self, 0x0023);
    //////////////////////////////////////////////
    self->regs->reg_0x0022_mclk1_pe = 0x1;
    self->regs->reg_0x0022_fclk1_pe = 0x1;
    self->regs->reg_0x0022_txnrx1_pe = 0x1;
    self->regs->reg_0x0022_txnrx1_pe = 0x1;
    self->regs->reg_0x0022_iq_sel_en_1_pe = 0x1;
    self->regs->reg_0x0022_diq1_pe = 0x1;
    self->regs->reg_0x0022_diq1_ds = 0x0;
    self->regs->reg_0x0022_mclk2_pe = 0x1;
    self->regs->reg_0x0022_fclk2_pe = 0x1;
    self->regs->reg_0x0022_txnrx2_pe = 0x1;
    self->regs->reg_0x0022_iq_sel_en_2_pe = 0x1;
    self->regs->reg_0x0022_diq2_pe = 0x1;
    self->regs->reg_0x0022_diq2_ds = 0x0;
    self->regs->reg_0x0022_LML1_SISODDR = 0x1;
    self->regs->reg_0x0022_LML1_TRXIQPULSE = 0x0;
    self->regs->reg_0x0022_LML2_SISODDR = 0x1;
    self->regs->reg_0x0022_LML2_TRXIQPULSE = 0x0;
    LMS7002M_regs_spi_write(self, 0x0022);
    get_data =  LMS7002M_regs_spi_read(self, 0x0022);
    //////////////////////////////////////////////
    self->regs->reg_0x002a_rxwrclk_mux = 0x2;
    self->regs->reg_0x002a_rxrdclk_mux = 0x2;
    self->regs->reg_0x002a_txwrclk_mux = 0x00;
    self->regs->reg_0x002a_txrdclk_mux = 0x3;
    self->regs->reg_0x002a_tx_mux = 0x00;
    self->regs->reg_0x002a_rx_mux = 0x00;
    self->regs->reg_0x002a_FCLK1_DLY = 0x00;
    self->regs->reg_0x002a_FCLK2_DLY = 0x00;
    LMS7002M_regs_spi_write(self, 0x002a);
    get_data = LMS7002M_regs_spi_read(self, 0x002a);
    ////////////////////////////////////////////////
    self->regs->reg_0x002b_rxdiven = (mclkDiv > 1)?1:0;
   // self->regs->reg_0x002c_txtspclk_div = 0;
    self->regs->reg_0x002c_rxtspclk_div = (mclkDiv/2)-1;
   // self->regs->reg_0x002c_txtspclk_div = 0;
    self->regs->reg_0x002b_txdiven = 0x0;
    self->regs->reg_0x002b_mclk1src = 0x3;
    self->regs->reg_0x002b_mclk2src = 0x3;
    self->regs->reg_0x002b_mclk1_inv = 0x0;
    self->regs->reg_0x002b_mclk2_inv = 0x0;
    self->regs->reg_0x002b_mclk1_dly = 0x0;
    self->regs->reg_0x002b_mclk2_dly = 0x0;
    self->regs->reg_0x002b_fclk1_inv = 0x0;
    self->regs->reg_0x002b_fclk2_inv = 0x0;
    LMS7002M_regs_spi_write(self, 0x002b);
    get_data = LMS7002M_regs_spi_read(self, 0x002b);
    LMS7002M_regs_spi_write(self, 0x002C);
    get_data = LMS7002M_regs_spi_read(self, 0x002c);
    //////////////////////////////////////////////////
    self->regs->reg_0x0025_lml1_BB2RF_PST = 2;       // ждать 2 такта перед стопом отправки данных
    self->regs->reg_0x0025_lml1_BB2RF_PRE = 2;       // ждать 2 такта перед стопом отправки данных
    //////////////////////////////////////////////////////////////////////////////////
    self->regs->reg_0x0028_lml2_BB2RF_PST = 2;
    self->regs->reg_0x0028_lml2_BB2RF_PRE = 2;
    //////////////////////////////////////////////////////////////////////////////////
    self->regs->reg_0x0026_lml1_RF2BB_PST = 1;
    self->regs->reg_0x0026_lml1_RF2BB_PRE = 0;
    //////////////////////////////////////////////////////////////////////////////////
    self->regs->reg_0x0029_lml2_RF2BB_PST = 1;
    self->regs->reg_0x0029_lml2_RF2BB_PRE = 0;
    //////////////////////////////////////////////////////////////////////////////////
    LMS7002M_regs_spi_write(self, 0x0025);
    LMS7002M_regs_spi_write(self, 0x0028);
    LMS7002M_regs_spi_write(self, 0x0026);
    LMS7002M_regs_spi_write(self, 0x0029);*/



   /* if (portNo == LMS_PORT2)
    {
        self->regs->reg_0x0023_lml2_mode = REG_0X0023_LML2_MODE_JESD207;
        self->regs->reg_0x0023_lml2_rxntxiq = (direction==LMS_TX)?
            REG_0X0023_LML2_RXNTXIQ_RXIQ:REG_0X0023_LML2_RXNTXIQ_TXIQ; //WARNING: TX/RX perspective swap

    }

    if (portNo == LMS_PORT1)
    {
       self->regs->reg_0x0023_lml1_mode = REG_0X0023_LML2_MODE_JESD207;
       self->regs->reg_0x0023_lml1_rxntxiq = (direction==LMS_TX)?
       REG_0X0023_LML1_RXNTXIQ_RXIQ:REG_0X0023_LML1_RXNTXIQ_RXIQ; //WARNING: TX/RX perspective swap

    }*/

   /* //automatic directions based on mode above  // хз для чего используется
    self->regs->reg_0x0023_enabledirctr1 = 0;
    self->regs->reg_0x0023_enabledirctr2 = 0;*/

    //set the FIFO rd and wr clock muxes based on direction
  /*  if (direction == LMS_RX)
    {
        self->regs->reg_0x002a_rxrdclk_mux = (portNo==LMS_PORT1)?REG_0X002A_RXRDCLK_MUX_MCLK1:REG_0X002A_RXRDCLK_MUX_FCLK1;//REG_0X002A_RXRDCLK_MUX_MCLK2
        self->regs->reg_0x002a_rxwrclk_mux = REG_0X002A_RXWRCLK_MUX_RXTSPCLK;
    }

    //data stream muxes
    if (direction == LMS_RX)
    {
        self->regs->reg_0x002a_rx_mux = REG_0X002A_RX_MUX_RXTSP;
    }*/


    //clock mux (outputs to mclk pin)

  //  if (portNo == LMS_PORT2)
  //  {
  //      self->regs->reg_0x002b_mclk2src = (direction==LMS_TX)?
  //          ((mclkDiv==1)?REG_0X002B_MCLK2SRC_TXTSPCLKA:REG_0X002B_MCLK2SRC_TXTSPCLKA_DIV):
  //          ((mclkDiv==1)?REG_0X002B_MCLK2SRC_RXTSPCLKA:REG_0X002B_MCLK2SRC_RXTSPCLKA /*REG_0X002B_MCLK2SRC_RXTSPCLKA_DIV*/);
  //  }

  //  if (portNo == LMS_PORT1)
  //  {
  //      self->regs->reg_0x002b_mclk1src = (direction==LMS_TX)?
  //      ((mclkDiv==1)?REG_0X002B_MCLK1SRC_TXTSPCLKA:REG_0X002B_MCLK1SRC_TXTSPCLKA_DIV):
  //      ((mclkDiv==1)?REG_0X002B_MCLK1SRC_RXTSPCLKA:REG_0X002B_MCLK1SRC_RXTSPCLKA/*REG_0X002B_MCLK1SRC_RXTSPCLKA_DIV*/);
  //  }

   /* if (direction == LMS_RX)
    {
        self->regs->reg_0x002b_rxdiven = (mclkDiv > 1)?1:0;
        self->regs->reg_0x002c_rxtspclk_div = (mclkDiv/2)-1;
    }*/
//    self->regs->reg_0x002b_mclk1_dly = REG_0X002B_MCLK1_DLY_3;
//    self->regs->reg_0x002b_mclk2_dly = REG_0X002B_MCLK2_DLY_3;
    ///////////////////////////////////////////////////////////////////////////////////
   /* self->regs->reg_0x0022_LML2_TRXIQPULSE = 0;         // TRXIQPULSE mode off
    self->regs->reg_0x0022_LML2_SISODDR = 1;            // SISODDR mode off (default)
    self->regs->reg_0x0022_LML2_SISODDR = 1;
    self->regs->reg_0x0022_LML1_TRXIQPULSE = 0;
    self->regs->reg_0x0022_LML1_SISODDR = 1;
    self->regs->reg_0x0022_LML1_SISODDR = 1;*/
 /*   //////////////////////////////////////////////////////////////////////////////////
    self->regs->reg_0x0025_lml1_BB2RF_PST = 2;       // ждать 2 такта перед стопом отправки данных
    self->regs->reg_0x0025_lml1_BB2RF_PRE = 2;       // ждать 2 такта перед стопом отправки данных
    //////////////////////////////////////////////////////////////////////////////////
    self->regs->reg_0x0028_lml2_BB2RF_PST = 2;
    self->regs->reg_0x0028_lml2_BB2RF_PRE = 2;
    //////////////////////////////////////////////////////////////////////////////////
    self->regs->reg_0x0026_lml1_RF2BB_PST = 1;
    self->regs->reg_0x0026_lml1_RF2BB_PRE = 0;
    //////////////////////////////////////////////////////////////////////////////////
    self->regs->reg_0x0029_lml2_RF2BB_PST = 1;
    self->regs->reg_0x0029_lml2_RF2BB_PRE = 0;
    //////////////////////////////////////////////////////////////////////////////////*/
 /*   LMS7002M_regs_spi_write(self, 0x0023);
//    LMS7002M_regs_spi_read(self, 0x0023);
    LMS7002M_regs_spi_write(self, 0x002A);
//    LMS7002M_regs_spi_read(self, 0x002A);
    LMS7002M_regs_spi_write(self, 0x002B);
    LMS7002M_regs_spi_write(self, 0x002C);          // возможно потребуется изменить значениа в этом регистре. Лежит значение делителя = 256 для MCLK_DIV(RXTSPCLKA_DIV)

    LMS7002M_regs_spi_write(self, 0x0022);
    LMS7002M_regs_spi_write(self, 0x0025);
    LMS7002M_regs_spi_write(self, 0x0028);
    LMS7002M_regs_spi_write(self, 0x0026);
    LMS7002M_regs_spi_write(self, 0x0029);*/
}
void LMS7002M_invert_fclk(LMS7002M_t *self, const bool invert)
{
    //LML is in global register space
    LMS7002M_set_mac_ch(self, LMS_CHAB);

    self->regs->reg_0x002b_fclk1_inv = invert?1:0;
    self->regs->reg_0x002b_fclk2_inv = invert?1:0;
    LMS7002M_regs_spi_write(self, 0x002B);
}

void LMS7002M_invert_mclk(LMS7002M_t *self, const bool invert)
{
    //LML is in global register space
    LMS7002M_set_mac_ch(self, LMS_CHA);

    self->regs->reg_0x002b_mclk1_inv = invert?1:0;
    self->regs->reg_0x002b_mclk2_inv = invert?1:0;
    LMS7002M_regs_spi_write(self, 0x002B);
    LMS7002M_regs_spi_read(self, 0x002B);
}
void LMS7002M_setup_digital_loopback(LMS7002M_t *self)
{
    //LML is in global register space
    LMS7002M_set_mac_ch(self, LMS_CHAB);

    self->regs->reg_0x002a_rx_mux = REG_0X002A_RX_MUX_RXTSP;
    //self->regs->reg_0x002a_rx_mux = REG_0X002A_RX_MUX_TXFIFO;
    //self->regs->reg_0x002a_rx_mux = REG_0X002A_RX_MUX_LFSR_;
    if (self->regs->reg_0x002a_txwrclk_mux == REG_0X002A_TXWRCLK_MUX_FCLK1)
    {
        self->regs->reg_0x002a_rxwrclk_mux = REG_0X002A_RXWRCLK_MUX_FCLK1;
    }
    if (self->regs->reg_0x002a_txwrclk_mux == REG_0X002A_TXWRCLK_MUX_FCLK2)
    {
        self->regs->reg_0x002a_rxwrclk_mux = REG_0X002A_RXWRCLK_MUX_FCLK2;
    }
    LMS7002M_regs_spi_write(self, 0x002A);
}

void LMS7002M_set_mac_ch(LMS7002M_t *self, const LMS7002M_chan_t channel)
{
    //pick the register map and setting based on channel
    int newValue = 0;
    LMS7002M_regs_t *regs = NULL;
    switch (channel)
    {
    case LMS_CHA:
        newValue = REG_0X0020_MAC_CHA;
        regs = &self->_regs[0];
        break;

    case LMS_CHB:
        newValue = REG_0X0020_MAC_CHB;
        regs = &self->_regs[1];
        break;

    //specifying CHAB is a convenience to set both channels
    //however, we have to pick a single register shadow
    //we recommended to not use CHAB in most cases
    case LMS_CHAB:
        newValue = REG_0X0020_MAC_CHAB;
        regs = &self->_regs[0];
        break;
    }

    //manually pick the first shadow bank for this setting
    self->regs = self->_regs;
    if (self->regs->reg_0x0020_mac != newValue)
    {
        self->regs->reg_0x0020_mac = newValue;
        LMS7002M_regs_spi_write(self, 0x0020);
    }
    newValue = LMS7002M_regs_spi_read(self, 0x0020);

    //now select the user's desired shadow bank for further calls
    self->regs = regs;
}

void LMS7002M_set_mac_dir(LMS7002M_t *self, const LMS7002M_dir_t direction)
{
    switch (direction)
    {
    case LMS_RX: LMS7002M_set_mac_ch(self, LMS_CHA); break;
    case LMS_TX: LMS7002M_set_mac_ch(self, LMS_CHB); break;
    }
}

static inline int __lms7002m_diq_index(const int search, const int positions[4])
{
    for (size_t i = 0; i < 4; i++)
    {
        if (search == positions[i]) return i;
    }
    return 0; //dont care
}

void LMS7002M_set_diq_mux(LMS7002M_t *self, const LMS7002M_dir_t direction, const int positions[4])
{
    //LML is in global register space
    LMS7002M_set_mac_ch(self, LMS_CHAB);

    //set the same config on both ports, only one is used as per port config

    if (direction == LMS_TX)
    {
        self->regs->reg_0x0024_lml1_bqp = __lms7002m_diq_index(LMS7002M_LML_BQ, positions);
        self->regs->reg_0x0024_lml1_bip = __lms7002m_diq_index(LMS7002M_LML_BI, positions);
        self->regs->reg_0x0024_lml1_aqp = __lms7002m_diq_index(LMS7002M_LML_AQ, positions);
        self->regs->reg_0x0024_lml1_aip = __lms7002m_diq_index(LMS7002M_LML_AI, positions);
        self->regs->reg_0x0027_lml2_bqp = __lms7002m_diq_index(LMS7002M_LML_BQ, positions);
        self->regs->reg_0x0027_lml2_bip = __lms7002m_diq_index(LMS7002M_LML_BI, positions);
        self->regs->reg_0x0027_lml2_aqp = __lms7002m_diq_index(LMS7002M_LML_AQ, positions);
        self->regs->reg_0x0027_lml2_aip = __lms7002m_diq_index(LMS7002M_LML_AI, positions);
    }

    if (direction == LMS_RX)
    {
        self->regs->reg_0x0024_lml1_s3s = positions[3];
        self->regs->reg_0x0024_lml1_s2s = positions[2];
        self->regs->reg_0x0024_lml1_s1s = positions[1];
        self->regs->reg_0x0024_lml1_s0s = positions[0];
        self->regs->reg_0x0027_lml2_s3s = positions[3];
        self->regs->reg_0x0027_lml2_s2s = positions[2];
        self->regs->reg_0x0027_lml2_s1s = positions[1];
        self->regs->reg_0x0027_lml2_s0s = positions[0];
    }

    LMS7002M_regs_spi_write(self, 0x0024);
    LMS7002M_regs_spi_write(self, 0x0027);
}
