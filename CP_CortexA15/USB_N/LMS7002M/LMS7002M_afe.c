///
/// \file LMS7002M_afe.c
///
/// ADC and DAC config for the LMS7002M C driver.
///
/// \copyright
/// Copyright (c) 2015-2016 Fairwaves, Inc.
/// Copyright (c) 2015-2016 Rice University
/// SPDX-License-Identifier: Apache-2.0
/// http://www.apache.org/licenses/LICENSE-2.0
///

#include <stdlib.h>
#include "LMS7002M_impl.h"

void LMS7002M_afe_enable(LMS7002M_t *self, const LMS7002M_dir_t direction, const LMS7002M_chan_t channel, const bool enable)
{
    uint32_t answer = 0;
    //support using LMS_CHAB to set both ADCs or DACs
    if (channel == LMS_CHAB)
    {
        LMS7002M_afe_enable(self, direction, LMS_CHA, enable);
        LMS7002M_afe_enable(self, direction, LMS_CHB, enable);
        return;
    }

    //AFE is a global register space
    LMS7002M_set_mac_ch(self, LMS_CHAB);  //LMS_CHAB

    //always individual enables and both ADCs
    self->regs->reg_0x0082_en_g_afe = 1;
    self->regs->reg_0x0082_mode_interleave_afe = REG_0X0082_MODE_INTERLEAVE_AFE_INTERLEAVED;//REG_0X0082_MODE_INTERLEAVE_AFE_INTERLEAVED;
    self->regs->reg_0x0082_pd_afe = 0;

    /////////////Test RSSI//////////////////////////////////////////////////////////////
/*    self->regs->reg_0x0082_en_g_afe = 1;
    self->regs->reg_0x0082_mode_interleave_afe = REG_0X0082_MODE_INTERLEAVE_AFE_2ADCS;
    self->regs->reg_0x0082_pd_afe = 0;
    self->regs->reg_0x0082_pd_rx_afe1 = 0;
    self->regs->reg_0x0082_pd_rx_afe2 = 0;
    self->regs->reg_0x0082_mux_afe_1 = 3;
    self->regs->reg_0x0082_mux_afe_2 = 3;
    */
    ////////////////////////////////////////////////////////////////////////////////////
    //conditional power downs
    if (direction == LMS_TX && channel == LMS_CHA) self->regs->reg_0x0082_pd_tx_afe1 = enable?0:1;
    if (direction == LMS_TX && channel == LMS_CHB) self->regs->reg_0x0082_pd_tx_afe2 = enable?0:1;
    if (direction == LMS_RX && channel == LMS_CHA) self->regs->reg_0x0082_pd_rx_afe1 = enable?0:1;
    if (direction == LMS_RX && channel == LMS_CHB) self->regs->reg_0x0082_pd_rx_afe2 = enable?0:1;

   // LMS7002M_regs_spi_write(self, 0x0082);
   // answer = LMS7002M_regs_spi_read(self, 0x0082);

    LMS7002M_usb_write(0x0082,0x8001);      /// либо 0x800B
    answer = LMS7002M_usb_read(self,0x0082);

}
