/*
 * AD9208.c
 *
 */
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>


#include <AD9208_spi/AD9208.h>
#include <ti/drv/spi/SPI.h>
#include <ti/drv/spi/soc/SPI_soc.h>
#include "ad9208_reg.h"
#include "api_errors.h"

#define NO_OS_U16_MAX ((uint16_t)~0U)
#define NO_OS_S16_MAX ((int16_t) (NO_OS_U16_MAX>>1))

static jesd_serdes_pll_cfg ADI_REC_SERDES_PLL_CFG[] =
{
    {390, 781, 0xD},
    {781, 1562, 0x9},
    {1562, 3125, 0x5},
    {3125, 6250, 0x1},
    {6250, 12500, 0x0},
    {12500, 16000, 0x3}
};

static const chip_decimation_ratio_cfg ad9208_dcm_table[] =
{
    {1, 0x0}, {2, 0x1}, {3, 0x8}, {4, 0x2},
    {5, 0x5}, {6, 0x9}, {8, 0x3}, {10, 0x6},
    {12, 0xA}, {15, 0x7}, {16, 0x4}, {20, 0xD},
    {24, 0xB}, {30, 0xE}, {40, 0xF}, {48, 0xC}
};

typedef struct adi_dec_filt_data {
uint8_t dec_complex;
uint8_t dec_real;
uint8_t ctrl_reg_val;
uint8_t sel_reg_val;
} adi_dec_filt_data;

static const adi_dec_filt_data ADI_DEC_FILTER_COMPLEX_TBL[] =
{
    {2, 1, 0x3, 0x0},
    {3, 0, 0x7, 0x7},
    {4, 2, 0x0, 0x0},
    {6, 3, 0x4, 0x0},
    {8, 4, 0x1, 0x0},
    {10, 5, 0x7, 0x2},
    {12, 6, 0x5, 0x0},
    {15, 0, 0x7, 0x8},
    {16, 8, 0x2, 0x0},
    {20, 10, 0x7, 0x3},
    {24, 12, 0x6, 0x0},
    {40, 20, 0x7, 0x4},
    {48, 24, 0x7, 0x0}
};

static struct adi_reg_data ADI_REC_SERDES_INIT_TBL[] =
{
    {0x1228, 0x4F},
    {0x1228, 0x0F},
    {0x1222, 0x00},
    {0x1222, 0x04},
    {0x1222, 0x00},
    {0x1262, 0x08},
    {0x1262, 0x00}
};


static uint8_t api_revision[3] = { 1, 0, 1 };



void AD9208_jesd_syref_status_get(void *spi, uint8_t *hold_status, uint8_t *setup_status, uint8_t *phase_status)
{
    uint8_t tmp_reg;

    tmp_reg = spi1_read(spi, AD9208_SYSREF_STAT_0_REG);

    *hold_status = ((tmp_reg & 0xF0) >> 4);
    *setup_status = ((tmp_reg & 0x0F) >> 0);
    tmp_reg = spi1_read(spi, AD9208_SYSREF_STAT_1_REG);

    *phase_status = ((tmp_reg & 0xF) >> 0);
}

uint8_t ad9208_jesd_sysref_timestamp_set(void *spi, uint8_t timestamp_en, uint8_t control_bit, uint8_t delay)
{
    uint8_t tmp_reg, tmp_nibble;
    uint16_t tmp_addr;

    if ((timestamp_en > 0x1) || (control_bit > 2) || (delay > AD9208_SYSREF_TS_DELAY(ALL)))
        return API_ERROR_INVALID_PARAM;

    if (timestamp_en == 0x1)
    {
        spi1_write(spi, AD9208_CHIP_SYNC_MODE_REG, AD9208_SYNC_TS_ENABLE);

        switch (control_bit)
        {
            case 0:
                tmp_addr = AD9208_OP_MODE_CTRL_1_REG;
                tmp_nibble = 0;
                break;
            case 1:
                tmp_addr = AD9208_OP_MODE_CTRL_1_REG;
                tmp_nibble = 1;
                break;
            case 2:
                tmp_addr = AD9208_OP_MODE_CTRL_2_REG;
                tmp_nibble = 0;
                break;
            default:
                return API_ERROR_INVALID_PARAM;
        }

        tmp_reg = spi1_read(spi, tmp_addr);

        tmp_reg &= ((AD9208_OP_CONV_CTRL_BIT_SEL(ALL)) << (tmp_nibble * 4));
        tmp_reg |=  ((AD9208_OP_CONV_CTRL_BIT_SEL(AD9208_CB_SYSREF)) << (tmp_nibble * 4));
        spi1_write(spi, tmp_addr, tmp_reg);
        spi1_write(spi, AD9208_SYSREF_CTRL_3_REG, AD9208_SYSREF_TS_DELAY(delay));
    }

    return API_ERROR_OK;
}

uint8_t AD9208_jesd_syref_lmfc_offset_set(void *spi, uint8_t offset)
{
    uint8_t tmp_reg;

    if (offset > AD9208_JESD_LMFC_OFFSET(ALL))
        return API_ERROR_INVALID_PARAM;

    tmp_reg = spi1_read(spi, AD9208_JESD_LMFC_OFFSET_REG);

    tmp_reg &= AD9208_JESD_LMFC_OFFSET(ALL);
    tmp_reg |= AD9208_JESD_LMFC_OFFSET(offset);

    spi1_write(spi, AD9208_JESD_LMFC_OFFSET_REG, tmp_reg);

    return API_ERROR_OK;
}

uint8_t AD9208_jesd_syref_config_set(void *spi, uint8_t sysref_edge_sel, uint8_t clk_edge_sel, uint8_t neg_window_skew, uint8_t pos_window_skew)
{
    uint8_t tmp_reg;

    if ((sysref_edge_sel > 1) || (clk_edge_sel > 1) ||(neg_window_skew > 0x3) || (pos_window_skew > 3))
        return API_ERROR_INVALID_PARAM;

    tmp_reg = spi1_read(spi, AD9208_SYSREF_CTRL_0_REG);

    tmp_reg &= ~AD9208_SYSREF_TRANSITION_SEL(ALL);
    tmp_reg &= ~AD9208_SYSREF_CLK_EDGE_SEL(ALL);
    tmp_reg |= AD9208_SYSREF_CLK_EDGE_SEL(clk_edge_sel);
    tmp_reg |= AD9208_SYSREF_TRANSITION_SEL(sysref_edge_sel);

    spi1_write(spi, AD9208_SYSREF_CTRL_0_REG, tmp_reg);

    tmp_reg = AD9208_SYSREF_WIN_NEG(neg_window_skew) | AD9208_SYSREF_WIN_POS(pos_window_skew);
    spi1_write(spi, AD9208_SYSREF_CTRL_2_REG, tmp_reg);

    return API_ERROR_OK;
}

uint8_t AD9208_jesd_syref_mode_set(void *spi, jesd_sysref_mode_t mode, uint8_t sysref_count)
{
    uint8_t tmp_reg;

    if (mode >= SYSREF_MON)
        return API_ERROR_INVALID_PARAM;

    tmp_reg = spi1_read(spi, AD9208_SYSREF_CTRL_0_REG);

    tmp_reg &= ~AD9208_SYSREF_MODE_SEL(ALL);
    tmp_reg |= AD9208_SYSREF_MODE_SEL(mode);

    spi1_write(spi, AD9208_SYSREF_CTRL_0_REG, tmp_reg);

    tmp_reg = spi1_read(spi, AD9208_SYSREF_CTRL_1_REG);

    tmp_reg &= ~AD9208_SYSREF_NSHOT_IGNORE(ALL);
    if (mode == SYSREF_ONESHOT)
    {
        tmp_reg |= AD9208_SYSREF_NSHOT_IGNORE(sysref_count);
    }
    else
    {
        tmp_reg |= AD9208_SYSREF_NSHOT_IGNORE(0x0);
    }
    spi1_write(spi, AD9208_SYSREF_CTRL_1_REG, tmp_reg);

    return API_ERROR_OK;
}

uint8_t AD9208_jesd_set_if_config(void *spi, AD9208_t *hh, jesd_param_t jesd_param, uint64_t *lane_rate_kbps)
{
    int err;
    uint8_t tmp_reg, dcm, vco_cfg;
    uint64_t fout, slr, slr_mbps;

    err = check_jesd_params_range(jesd_param);
    if (err != API_ERROR_OK)
        return err;

    /*Determine DCM and Fout */
    AD9208_get_decimation(spi, &dcm);

    if (hh->adc_clk_freq_hz != 0)
    {
        fout = hh->adc_clk_freq_hz/ dcm;
    }
    else
    {
        /*printf("API:AD9208: JESD :INVALID: CLK FREQ \r\n"); */
        return API_ERROR_INVALID_PARAM;
    }

    /*Calculate Lane Rate */
    slr = jesd_param.jesd_M * jesd_param.jesd_NP * 10/8 * fout;
    slr /= jesd_param.jesd_L;
    slr_mbps = slr/ 1000000;

    if ((slr_mbps > LANE_RATE_MAX_MBPS) || (slr_mbps < LANE_RATE_MIN_MBPS))
    {
        /*printf("API:AD9208: JESD :INVALID: SLR :%lld \r\n", slr_mbps); */
        return API_ERROR_INVALID_PARAM;
    }

    /*CFG SERDES PLL for SLR */
    err = get_jesd_serdes_vco_cfg(slr_mbps, &vco_cfg);
    if (err != API_ERROR_OK)
    {
        return err;
    }
    tmp_reg = spi1_read(spi, AD9208_JESD_SERDES_PLL_CFG_REG);
    tmp_reg = AD9208_JESD_SLR_CTRL(vco_cfg);
    spi1_write(spi, AD9208_JESD_SERDES_PLL_CFG_REG, tmp_reg);
    /*Set NOF Converters */
    spi1_write(spi, AD9208_JESD_M_CFG_REG, (jesd_param.jesd_M - 1));

    /*Set Resolution and Sample and control bits */
    tmp_reg = (AD9208_JESD_CS(jesd_param.jesd_CS) | (AD9208_JESD_N(jesd_param.jesd_N - 1)));
    spi1_write(spi, AD9208_JESD_CS_N_CFG_REG, tmp_reg);

    /*Set NP */
    tmp_reg = spi1_read(spi, AD9208_JESD_SCV_NP_CFG_REG);

    tmp_reg &= ~(AD9208_JESD_NP(ALL));
    tmp_reg |= AD9208_JESD_NP(jesd_param.jesd_NP - 1);
    spi1_write(spi, AD9208_JESD_SCV_NP_CFG_REG, tmp_reg);

    /*Octets per Frame (F) and Frames per Multiframe (k) */
    spi1_write(spi, AD9208_JESD_F_CFG_REG,(AD9208_JESD_F(jesd_param.jesd_F - 1)));

    spi1_write(spi, AD9208_JESD_K_CFG_REG,(AD9208_JESD_K(jesd_param.jesd_K - 1)));

    /*Set Lanes */
    tmp_reg = spi1_read(spi, AD9208_JESD_L_SCR_CFG_REG);

    tmp_reg &= ~AD9208_JESD_LANES(ALL);
    tmp_reg |= AD9208_JESD_LANES(jesd_param.jesd_L - 1);
    spi1_write(spi, AD9208_JESD_L_SCR_CFG_REG, tmp_reg);

    if (lane_rate_kbps != NULL)
    *lane_rate_kbps = slr/ 1000;

    return API_ERROR_OK;
}

uint8_t get_jesd_serdes_vco_cfg(uint64_t slr_mbps, uint8_t *vco_cfg)
{
    /*Transport layer Parameter Ranges Table 22 */
    int i = 0x0;

    for (i = 0; i < ARRAY_SIZE(ADI_REC_SERDES_PLL_CFG); i++)
        {
            if ((slr_mbps >= ADI_REC_SERDES_PLL_CFG[i].slr_lwr_thres) &&  (slr_mbps < ADI_REC_SERDES_PLL_CFG[i].slr_upr_thres))
                {
                    *vco_cfg = ADI_REC_SERDES_PLL_CFG[i].vco_cfg;
                    return API_ERROR_OK;
                }
        }
    return API_ERROR_INVALID_PARAM;
}

uint8_t AD9208_jesd_subclass_set(void *spi, uint8_t subclass)
{
    uint8_t tmp_reg;

    if (subclass >= JESD_SUBCLASS_INVALID)
    {
        return API_ERROR_INVALID_PARAM;
    }

    tmp_reg = spi1_read(spi, AD9208_JESD_SCV_NP_CFG_REG);

    tmp_reg &= ~AD9208_JESD_SUBCLASS(ALL);
    tmp_reg |= AD9208_JESD_SUBCLASS(subclass);

    spi1_write(spi, AD9208_JESD_SCV_NP_CFG_REG, tmp_reg);

    return API_ERROR_OK;
}

uint8_t AD9208_jesd_enable_scrambler(void *spi, uint8_t en)
{
    uint8_t tmp_reg;

    tmp_reg = spi1_read(spi, AD9208_JESD_L_SCR_CFG_REG);

    tmp_reg &= ~AD9208_JESD_SCR_EN;
    tmp_reg |= en ? AD9208_JESD_SCR_EN : 0;
    spi1_write(spi, AD9208_JESD_L_SCR_CFG_REG, tmp_reg);

    return API_ERROR_OK;
}

uint8_t ad9208_jesd_set_lane_xbar(void *spi, uint8_t logical_lane, uint8_t physical_lane)
{
    uint8_t tmp_reg_val;
    uint16_t tmp_reg_addr;
    uint8_t tmp_nibble;

    if ((physical_lane > (LANE_MAX - 1)) || (logical_lane > LANE_MAX - 1))
        return API_ERROR_INVALID_PARAM;

    switch (physical_lane)
    {
        case 0:
        case 1:
            tmp_reg_addr = AD9208_JESD_XBAR_CFG_REG;
            break;
        case 2:
        case 3:
            tmp_reg_addr = AD9208_JESD_XBAR_CFG_REG + 1;
            break;
        case 4:
        case 5:
            tmp_reg_addr = AD9208_JESD_XBAR_CFG_REG + 3;
            break;
        case 6:
        case 7:
            tmp_reg_addr = AD9208_JESD_XBAR_CFG_REG + 4;
            break;
        default:
            return API_ERROR_INVALID_PARAM;
    }

    tmp_nibble = (physical_lane % 2) ? 1 : 0;

    tmp_reg_val = spi1_read(spi, tmp_reg_addr);

    if (tmp_nibble == 0) {
        tmp_reg_val &= (~AD9208_JESD_XBAR_LN_EVEN(ALL));
        tmp_reg_val |= AD9208_JESD_XBAR_LN_EVEN(logical_lane);
    } else {
        tmp_reg_val &= (~AD9208_JESD_XBAR_LN_ODD(ALL));
        tmp_reg_val |= AD9208_JESD_XBAR_LN_ODD(logical_lane);
    }
    spi1_write(spi, tmp_reg_addr, tmp_reg_val);

    return API_ERROR_OK;
}

uint8_t ad9208_jesd_get_lane_xbar(void *spi, uint8_t *phy_log_map)
{
    int i;
    uint8_t tmp_reg[AD9208_JESD_XBAR_CFG_REG_OFFSET];

    if (phy_log_map == NULL)
    {
        return API_ERROR_INVALID_PARAM;
    }

    for (i = 0; i < AD9208_JESD_XBAR_CFG_REG_OFFSET; i++)
    {
        tmp_reg[i] = spi1_read(spi, AD9208_JESD_XBAR_CFG_REG + i);
    }
    phy_log_map[0] = tmp_reg[0] & AD9208_JESD_XBAR_LN_EVEN(ALL);
    phy_log_map[1] = (tmp_reg[0] & AD9208_JESD_XBAR_LN_ODD(ALL)) >> 4;
    phy_log_map[2] = tmp_reg[1] & AD9208_JESD_XBAR_LN_EVEN(ALL);
    phy_log_map[3] = (tmp_reg[1] & AD9208_JESD_XBAR_LN_ODD(ALL)) >> 4;
    phy_log_map[4] = tmp_reg[3] & AD9208_JESD_XBAR_LN_EVEN(ALL);
    phy_log_map[5] = (tmp_reg[3] & AD9208_JESD_XBAR_LN_ODD(ALL)) >> 4;
    phy_log_map[6] = tmp_reg[4] & AD9208_JESD_XBAR_LN_EVEN(ALL);
    phy_log_map[7] = (tmp_reg[4] & AD9208_JESD_XBAR_LN_ODD(ALL)) >> 4;
    return API_ERROR_OK;
}

uint8_t AD9208_jesd_enable_link(void *spi, uint8_t en)
{
    uint8_t tmp_reg;

    if (en > 1)
    {
        return API_ERROR_INVALID_PARAM;
    }

    tmp_reg = spi1_read(spi, AD9208_JESD_LINK_CTRL1_REG);
    tmp_reg &= ~AD9208_JESD_LINK_PDN;
    tmp_reg |= en ? 0 : AD9208_JESD_LINK_PDN;
    spi1_write(spi, AD9208_JESD_LINK_CTRL1_REG, tmp_reg);

    if (en)
    {
        AD9208_register_write_tbl(spi, &ADI_REC_SERDES_INIT_TBL[0], ARRAY_SIZE(ADI_REC_SERDES_INIT_TBL));
    }

    return API_ERROR_OK;
}

uint8_t ad9208_jesd_get_cfg_param(void *spi, jesd_param_t *jesd_param)
{

    int i;
    uint8_t tmp_reg[AD9208_JESD_CFG_REG_OFFSET];

    if (jesd_param == NULL)
    {
        return API_ERROR_INVALID_PARAM;
    }

    /*Read All the JESD CFG Registers */
    for (i = 0; i < AD9208_JESD_CFG_REG_OFFSET; i++)
    {
        tmp_reg[i] = spi1_read(spi,(AD9208_JESD_L_SCR_CFG_REG + i));
    }
    jesd_param->jesd_L = (AD9208_JESD_LANES(tmp_reg[0]) + 1);
    jesd_param->jesd_F = (AD9208_JESD_F(tmp_reg[1]) + 1);
    jesd_param->jesd_K = (AD9208_JESD_K(tmp_reg[2]) + 1);
    jesd_param->jesd_M = (AD9208_JESD_M(tmp_reg[3]) + 1);
    jesd_param->jesd_CS = ((tmp_reg[4] & 0xC) >> 6);
    jesd_param->jesd_N = (AD9208_JESD_N(tmp_reg[4]) + 1);
    jesd_param->jesd_NP = (AD9208_JESD_NP(tmp_reg[4]) + 1);
    jesd_param->jesd_S = (AD9208_JESD_S(tmp_reg[5]));

    jesd_param->jesd_HD = (tmp_reg[6] & AD9208_JESD_HD) ? 1 : 0;
    jesd_param->jesd_CF = (AD9208_JESD_CF(tmp_reg[6]));

    /*Read All the JESD CFG Registers */
    for (i = 0; i < AD9208_JESD_ID_CFG_REG_OFFSET; i++)
    {
        tmp_reg[i] = spi1_read(spi, (AD9208_JESD_DID_CFG_REG + i));
    }

    jesd_param->jesd_DID = tmp_reg[0];
    jesd_param->jesd_BID = AD9208_JESD_BID(tmp_reg[1]);
    jesd_param->jesd_LID0 = AD9208_JESD_LID0(tmp_reg[2]);

    return API_ERROR_OK;
}

uint8_t check_jesd_params_range(jesd_param_t jesd_param)
{
    /*Transport layer Parameter Ranges Table 22 */
    if ((jesd_param.jesd_L != 1) && (jesd_param.jesd_L != 2) &&  (jesd_param.jesd_L != 4) && (jesd_param.jesd_L != 8))
    {
        /*printf("API:AD9208:Err: Invalid JESD L \r\n"); */
        return API_ERROR_INVALID_PARAM;
    }
    if ((jesd_param.jesd_M != 1) &&  (jesd_param.jesd_M != 2) && (jesd_param.jesd_M != 4) && (jesd_param.jesd_M != 8))
    {
        /*printf("API:AD9208:Err: Invalid JESD M \r\n"); */
        return API_ERROR_INVALID_PARAM;
    }
    if ((jesd_param.jesd_F != 1) &&  (jesd_param.jesd_F != 2) && (jesd_param.jesd_F != 4) && (jesd_param.jesd_F != 8))
    {
        /*printf("API:AD9208:Err: Invalid JESD F \r\n"); */
        return API_ERROR_INVALID_PARAM;
    }
    if ((jesd_param.jesd_N < N_MIN) || (jesd_param.jesd_N > N_MAX))
    {
        /*printf("API:AD9208:Err: Invalid JESD N \r\n"); */
        return API_ERROR_INVALID_PARAM;
    }
    if ((jesd_param.jesd_K < K_MIN) || (jesd_param.jesd_K > K_MAX) ||  (jesd_param.jesd_K % 4 != 0))
    {
        /*printf("API:AD9208:Err: Invalid JESD K \r\n"); */
        return API_ERROR_INVALID_PARAM;
    }
    if (jesd_param.jesd_CS > CS_MAX)
    {
        /*printf("API:AD9208:Err: Invalid JESD CS \r\n"); */
        return API_ERROR_INVALID_PARAM;
    }
    if (jesd_param.jesd_CF > CF_DEFAULT)
    {
        /*printf("API:AD9208:Err: Invalid JESD CF \r\n"); */
        return API_ERROR_INVALID_PARAM;
    }
    if ((jesd_param.jesd_NP != 8) && (jesd_param.jesd_NP != 16))
    {
        /*printf("API:AD9208:Err: Invalid JESD NP \r\n"); */
        return API_ERROR_INVALID_PARAM;
    }

    return API_ERROR_OK;
}

uint8_t AD9208_jesd_get_pll_status(void *spi, uint8_t *pll_status)
{
    uint8_t tmp_reg;

    tmp_reg = spi1_read(spi, AD9208_JESD_SERDES_PLL_REG);
    *pll_status = tmp_reg;

    return API_ERROR_OK;
}


uint8_t AD9208_adc_set_channel_select(void *spi, uint8_t ch)
{
    uint8_t tmp;
    if (ch > AD9208_ADC_CH_ALL)
    {
        return API_ERROR_INVALID_PARAM;
    }
    tmp = spi1_read(spi, AD9208_CH_INDEX_REG);
    spi1_write(spi, AD9208_CH_INDEX_REG, ch);
    tmp = spi1_read(spi, AD9208_CH_INDEX_REG);
    return API_ERROR_OK;
}

uint8_t AD9208_adc_get_channel_select(void *spi)
{
    uint8_t ch;
    ch = spi1_read(spi, AD9208_CH_INDEX_REG);
    return ch;
}

uint8_t AD9208_adc_set_ch_pdn_mode(void *spi, AD9208_pdn_mode_t mode)
{
    if (mode > AD9208_POWERDOWN)
    {
        return API_ERROR_INVALID_PARAM;
    }
    spi1_write(spi, AD9208_DEV_CFG_REG, AD9208_PDN_MODE(mode));

    return API_ERROR_OK;
}

uint8_t AD9208_set_input_clk_cfg(void *spi, AD9208_t *AD, uint64_t clk_freq_hz, uint8_t div)
{
    uint64_t fs_hz = 0x0;
    int tmp_reg;

    if ((clk_freq_hz > AD9208_IP_CLK_MAX_HZ) || (clk_freq_hz < AD9208_IP_CLK_MIN_HZ))
    {
        return API_ERROR_INVALID_PARAM;
    }

    if ((div != 1) && (div != 2) && (div != 4))
    {
        return API_ERROR_INVALID_PARAM;
    }

    fs_hz = clk_freq_hz/div;

    if ((fs_hz > AD9208_ADC_CLK_MAX_HZ) || (fs_hz < AD9208_ADC_CLK_MIN_HZ))
    {
        return API_ERROR_INVALID_PARAM;
    }

    tmp_reg = spi1_read(spi, AD9208_IP_CLK_CFG_REG);

    tmp_reg &= ~AD9208_IP_CLK_DIV(ALL);
    tmp_reg |= AD9208_IP_CLK_DIV(div - 1);
    spi1_write(spi, AD9208_IP_CLK_CFG_REG, tmp_reg);

    AD->adc_clk_freq_hz = fs_hz;
    return API_ERROR_OK;
}

uint64_t ad9208_get_adc_clk_freq(AD9208_t *AD, uint64_t adc_clk_freq_hz)
{
    adc_clk_freq_hz = AD->adc_clk_freq_hz;

    return (adc_clk_freq_hz);
}

uint8_t AD9208_adc_set_input_cfg(void *spi, signal_coupling_t analog_input_mode, uint8_t ext_vref, ad9208_adc_scale_range_t full_scale_range)
{
    int err;
    uint8_t tmp_reg;

    if ((analog_input_mode >= COUPLING_UNKNOWN) || (ext_vref > 1))
    {
        return API_ERROR_INVALID_PARAM;
    }

    if ((analog_input_mode == COUPLING_DC) && (ext_vref == 1))
    {
        return API_ERROR_INVALID_PARAM;
    }

    //Set Analog Input Mode Optimization
    tmp_reg = analog_input_mode ? AD9208_DC_COUPLE_EN(1) : AD9208_DC_COUPLE_EN(0);
    spi1_write(spi, AD9208_ANALOG_CFG_REG, tmp_reg);

    //ac_coupled mode
    if (analog_input_mode == COUPLING_AC)
    {
        //Turn OFF VCM Export Buffer
        err = AD9208_adc_set_vcm_export(spi, 0x0);
        if (err != API_ERROR_OK)
        {
            return err;
        }

        //Set Internal or External Vref
        tmp_reg = ext_vref ? AD9208_EXT_VREF_MODE : 0;
        spi1_write(spi, AD9208_VREF_CTRL_REG, tmp_reg);
    }

    else
    {
        //dc_coupled mode
        //Disable External Vref Mode
        spi1_write(spi, AD9208_VREF_CTRL_REG, 0x0);

        AD9208_adc_set_vcm_export(spi, 0x1);
    }

    err = AD9208_adc_set_input_scale(spi, full_scale_range);
    if (err != API_ERROR_OK)
    {
        return err;
    }
    return API_ERROR_OK;
}

uint8_t AD9208_adc_set_vcm_export(void *spi, uint8_t en)
{
    int tmp_reg;

    tmp_reg = spi1_read(spi, AD9208_EXT_VCM_CTRL_REG);

    if (en)
    {
        tmp_reg |= AD9208_EXT_VCM_BUFF;
    }
    else
    {
        tmp_reg &= ~AD9208_EXT_VCM_BUFF;
    }
    spi1_write(spi, AD9208_EXT_VCM_CTRL_REG, tmp_reg);

    return API_ERROR_OK;
}

uint8_t AD9208_adc_set_input_scale(void *spi, ad9208_adc_scale_range_t full_scale_range)
{
    int fs_val;

    switch (full_scale_range)
    {
        case AD9208_ADC_SCALE_2P04_VPP:
        {
            fs_val = 0;
            break;
        }

        case AD9208_ADC_SCALE_1P13_VPP:
        {
            fs_val = 0x8;
            break;
        }

        case AD9208_ADC_SCALE_1P25_VPP:
        {
            fs_val = 0x9;
            break;
        }

        case AD9208_ADC_SCALE_1P7_VPP:
        {
            fs_val = 0xD;
            break;
        }

        case AD9208_ADC_SCALE_1P81_VPP:
        {
            fs_val = 0xE;
            break;
        }

        case AD9208_ADC_SCALE_1P93_VPP:
        {
            fs_val = 0xF;
            break;
        }

        default:
            return API_ERROR_INVALID_PARAM;
    }

    spi1_write(spi, AD9208_FULL_SCALE_CFG_REG,AD9208_TRM_VREF(fs_val));

    return API_ERROR_OK;
}

uint8_t AD9208_adc_set_input_buffer_cfg(void *spi, ad9208_adc_buff_curr_t buff_curr_n, ad9208_adc_buff_curr_t buff_curr_p, ad9208_adc_buff_curr_t vcm_buff)
{
    uint8_t err;
    uint8_t tmp_reg;

    err = AD9208_check_buffer_current(buff_curr_n);
    if (err != API_ERROR_OK)
    {
        return err;
    }

    err = AD9208_check_buffer_current(buff_curr_p);
    if (err != API_ERROR_OK)
    {
        return err;
    }

    err = AD9208_check_buffer_current(vcm_buff);
    if (err != API_ERROR_OK)
    {
        return err;
    }

    //Optimize Buffer Settings for Common Mode Reference
    spi1_write(spi, AD9208_BUFF_CFG_P_REG, AD9208_BUFF_CTRL_P(buff_curr_p));
    spi1_write(spi, AD9208_BUFF_CFG_N_REG, AD9208_BUFF_CTRL_N(buff_curr_n));

    //Set Optimal VCM Buffer
    tmp_reg = spi1_read(spi, AD9208_EXT_VCM_CTRL_REG);

    tmp_reg &= ~AD9208_EXT_VCM_BUFF_CURR(ALL);
    tmp_reg |= AD9208_EXT_VCM_BUFF_CURR(vcm_buff);

    spi1_write(spi, AD9208_EXT_VCM_CTRL_REG, tmp_reg);

    return API_ERROR_OK;
}

uint8_t AD9208_check_buffer_current(ad9208_adc_buff_curr_t buff_curr)
{
    uint8_t err;

    switch (buff_curr)
   {
      case AD9208_ADC_BUFF_CURR_400_UA:
      case AD9208_ADC_BUFF_CURR_500_UA:
      case AD9208_ADC_BUFF_CURR_600_UA:
      case AD9208_ADC_BUFF_CURR_700_UA:
      case AD9208_ADC_BUFF_CURR_800_UA:
      case AD9208_ADC_BUFF_CURR_1000_UA:
          err = API_ERROR_OK;
          break;
      default:
          err = API_ERROR_INVALID_PARAM;
   }
    return err;
}

uint8_t ad9208_adc_set_dc_offset_filt_en(void *spi, uint8_t en)
{
    uint8_t tmp_reg;

    if (en > 1)
    {
        return API_ERROR_INVALID_PARAM;
    }

    tmp_reg = spi1_read(spi, AD9208_DC_OFFSET_CAL_CTRL);
    tmp_reg &= ~AD9208_DC_OFFSET_CAL_EN;
    tmp_reg |= (en) ? AD9208_DC_OFFSET_CAL_EN : 0;

    spi1_write(spi, AD9208_DC_OFFSET_CAL_CTRL, tmp_reg);
    return API_ERROR_OK;
}

uint8_t AD9208_adc_set_fc_ch_mode(void *spi, uint8_t fc_ch)
{
    uint8_t tmp_reg, op_mode;

    if ((fc_ch > AD9208_NOF_FC_MAX) || (fc_ch == 3))
    {
        return API_ERROR_INVALID_PARAM;
    }

    if (fc_ch == 4)
    {
        op_mode = 0x3;
    }
    else
    {
        op_mode = fc_ch;
    }

    tmp_reg = spi1_read(spi, AD9208_ADC_MODE_REG);

    tmp_reg &= ~AD9208_ADC_MODE(op_mode);
    tmp_reg |= AD9208_ADC_MODE(op_mode);
    ////

    //tmp_reg |=0x20; //Only real (I) selected; complex (Q) is ignored
    ////

    spi1_write(spi, AD9208_ADC_MODE_REG, tmp_reg);

    return API_ERROR_OK;
}

uint8_t AD9208_adc_set_dcm_mode(void *spi, uint8_t dcm)
{
    uint8_t err;
    uint8_t tmp_reg, dcm_mode;

    err = AD9208_get_decimation_cfg(dcm, &dcm_mode);
    if (err != API_ERROR_OK)
    {
        return err;
    }

    tmp_reg = spi1_read(spi, AD9208_ADC_DCM_REG);

    tmp_reg &= ~AD9208_ADC_DCM_RATE(ALL);
    tmp_reg |= dcm_mode;

    spi1_write(spi, AD9208_ADC_DCM_REG, tmp_reg);

    return API_ERROR_OK;
}

uint8_t AD9208_get_decimation_cfg(uint8_t dcm, uint8_t *cfg)
{
    uint32_t i;

    for (i = 0; i < ARRAY_SIZE(ad9208_dcm_table); i++)
    {
        if (dcm == ad9208_dcm_table[i].dcm)
        {
            *cfg = ad9208_dcm_table[i].dcm_cfg;
            return API_ERROR_OK;
        }
    }
    return API_ERROR_INVALID_PARAM;
}

uint8_t AD9208_adc_set_data_format(void *spi, ad9208_adc_data_frmt_t ip_data_frmt, ad9208_adc_data_frmt_t op_data_frmt)
{
    uint32_t offset;
    uint8_t tmp_reg, i;

    if ((ip_data_frmt > AD9208_DATA_FRMT_COMPLEX) || (op_data_frmt > AD9208_DATA_FRMT_COMPLEX))
    {
        return API_ERROR_INVALID_PARAM;
    }

    tmp_reg = spi1_read(spi, AD9208_ADC_MODE_REG);

    tmp_reg &= ~AD9208_ADC_Q_IGNORE;
    tmp_reg |= (op_data_frmt == AD9208_DATA_FRMT_COMPLEX) ? 0 : AD9208_ADC_Q_IGNORE;

    spi1_write(spi, AD9208_ADC_MODE_REG, tmp_reg);

    /*Consider dependancy on FC */
    for (i = 0; i < AD9208_NOF_FC_MAX; i++)
    {
        offset = (AD9208_DDCX_REG_OFFSET * i);

        tmp_reg = spi1_read(spi, AD9208_DDCX_CTRL0_REG + offset);

        tmp_reg &= ~(AD9208_DDCX_MIXER_SEL |  AD9208_DDCX_COMPLEX_TO_REAL);
        tmp_reg |= (ip_data_frmt == AD9208_DATA_FRMT_COMPLEX) ?  AD9208_DDCX_MIXER_SEL : 0;
        tmp_reg |= (op_data_frmt == AD9208_DATA_FRMT_COMPLEX) ?  0 : AD9208_DDCX_COMPLEX_TO_REAL;

        spi1_write(spi, AD9208_DDCX_CTRL0_REG + offset, tmp_reg);
    }
    return API_ERROR_OK;
}

uint8_t ad9208_adc_set_sync_update_mode_enable(void *spi, uint8_t en)
{
    uint8_t tmp_reg;

    if (en > 1)
    {
        return API_ERROR_INVALID_PARAM;
    }

    tmp_reg = spi1_read(spi, AD9208_DDC_SYNC_CTRL_REG);

    tmp_reg &= ~AD9208_DDC_UPDATE_MODE;
    tmp_reg |= (en) ? AD9208_DDC_UPDATE_MODE : 0;
    spi1_write(spi, AD9208_DDC_SYNC_CTRL_REG, tmp_reg);

    return API_ERROR_OK;
}

uint8_t ad9208_adc_set_ddc_ip_mux(void *spi, uint8_t ddc_ch, AD9208_adc_ch_t i_data_src, AD9208_adc_ch_t q_data_src)
{
    uint32_t offset;
    uint8_t tmp_reg;

    if ((ddc_ch >= AD9208_NOF_FC_MAX) || (i_data_src < AD9208_ADC_CH_A) || (i_data_src > AD9208_ADC_CH_B)
                                      || (q_data_src < AD9208_ADC_CH_A) || (q_data_src > AD9208_ADC_CH_B))
    {
        return API_ERROR_INVALID_PARAM;
    }

    offset = (AD9208_DDCX_REG_OFFSET * ddc_ch);
    tmp_reg = spi1_read(spi, AD9208_DDCX_DATA_SEL_REG + offset);

    tmp_reg &= ~(AD9208_DDCX_Q_IP_CHB_SEL | AD9208_DDCX_I_IP_CHB_SEL);
    tmp_reg |= (i_data_src == AD9208_ADC_CH_B) ? AD9208_DDCX_I_IP_CHB_SEL : 0;
    tmp_reg |= (q_data_src == AD9208_ADC_CH_B) ? AD9208_DDCX_Q_IP_CHB_SEL : 0;

    spi1_write(spi, AD9208_DDCX_DATA_SEL_REG + offset, tmp_reg);

    return API_ERROR_OK;
}

uint8_t AD9208_adc_set_ddc_gain(void *spi, uint8_t ddc_ch, uint8_t gain_db)
{
    uint32_t offset;
    uint8_t tmp_reg;

    if ((ddc_ch >= AD9208_NOF_FC_MAX) || ((gain_db != 0) && (gain_db != 6)))
    {
        return API_ERROR_INVALID_PARAM;
    }

    offset = (AD9208_DDCX_REG_OFFSET * ddc_ch);
    tmp_reg = spi1_read(spi, AD9208_DDCX_CTRL0_REG + offset);

    tmp_reg &= ~AD9208_DDCX_GAIN_SEL;
    tmp_reg |= (gain_db == 6) ? AD9208_DDCX_GAIN_SEL : 0;

    spi1_write(spi, AD9208_DDCX_CTRL0_REG + offset, tmp_reg);

    return API_ERROR_OK;
}

uint8_t AD9208_adc_set_ddc_dcm(void *spi, uint8_t ddc_ch, uint8_t dcm)
{
    int offset;
    uint8_t tmp_reg, filt_sel_reg_0, filt_sel_reg_1;
    ad9208_adc_data_frmt_t op_data_format = AD9208_DATA_FRMT_COMPLEX;

    if ((ddc_ch >= AD9208_NOF_FC_MAX) || (dcm > AD9208_ADC_DCM_MAX) || (dcm < AD9208_ADC_DCM_MIN))
    {
        return API_ERROR_INVALID_PARAM;
    }

    offset = (AD9208_DDCX_REG_OFFSET * ddc_ch);
    tmp_reg = spi1_read(spi, AD9208_DDCX_CTRL0_REG + offset);

    if (tmp_reg & AD9208_DDCX_COMPLEX_TO_REAL)
    op_data_format = AD9208_DATA_FRMT_REAL;

    AD9208_get_dec_filter_cfg(op_data_format, dcm, &filt_sel_reg_0, &filt_sel_reg_1);

    tmp_reg &= ~AD9208_DDCX_DCM_FILT_SEL_0(ALL);
    tmp_reg |= AD9208_DDCX_DCM_FILT_SEL_0(filt_sel_reg_0);

    spi1_write(spi, AD9208_DDCX_CTRL0_REG + offset, tmp_reg);

    tmp_reg &= ~AD9208_DDCX_DCM_FILT_SEL_1(ALL);
    tmp_reg |= AD9208_DDCX_DCM_FILT_SEL_1(filt_sel_reg_1);

    //tmp_reg = 0x0; //РјРѕРµ Р·Р°РЅС‡РµРЅРёРµ РїРѕРґСЃС‚Р°РІРёР» РІ СЃРѕРѕС‚РІРµС‚СЃС‚РІРёРё СЃ С‚Р°Р±Р»РёС†РµР№
    spi1_write(spi, AD9208_DDCX_DATA_SEL_REG + offset, tmp_reg);

    return API_ERROR_OK;
}

uint8_t AD9208_adc_set_ddc_nco_mode(void *spi, uint8_t ddc_ch, ad9208_adc_nco_mode_t mode)
{
    uint32_t offset;
    uint8_t tmp_reg;

    if ((ddc_ch >= AD9208_NOF_FC_MAX) || (mode >= AD9208_ADC_NCO_INVLD))
    {
        return API_ERROR_INVALID_PARAM;
    }
    offset = (AD9208_DDCX_REG_OFFSET * ddc_ch);
    tmp_reg = spi1_read(spi, AD9208_DDCX_CTRL0_REG + offset);

    tmp_reg &= ~AD9208_DDCX_NCO_IF_MODE(ALL);
    tmp_reg |= AD9208_DDCX_NCO_IF_MODE(mode);

    spi1_write(spi, AD9208_DDCX_CTRL0_REG + offset, tmp_reg);

    return API_ERROR_OK;
}

uint8_t AD9208_adc_set_ddc_nco(void *spi, AD9208_t *AD, uint8_t ddc_ch, const int64_t carrier_freq_hz)
{
    uint64_t tmp_carrier_freq;
    uint64_t tmp_adc_clk_freq;
    uint64_t ftw;
    uint64_t maw;
    uint64_t mbw;
    uint64_t M;
    uint64_t N;
    uint64_t pow_2_48 = 0x0001000000000000;

    if (!((carrier_freq_hz >= (int64_t) (0ll - AD->adc_clk_freq_hz / 2)) &&
                              (carrier_freq_hz < (int64_t) (AD->adc_clk_freq_hz / 2))))
    {
        return API_ERROR_INVALID_PARAM;
    }

    if (ddc_ch >= AD9208_NOF_FC_MAX)
    {
        return API_ERROR_INVALID_PARAM;
    }

    tmp_carrier_freq = carrier_freq_hz/1000000;
    tmp_adc_clk_freq = (AD->adc_clk_freq_hz)/1000000;

    M = tmp_carrier_freq % tmp_adc_clk_freq;
    N = tmp_adc_clk_freq;
    ftw = pow_2_48*M/N;
    //    maw = (pow_2_48*M) % N;
    //    mbw = N;
    maw = 0;
    mbw = 0;

    AD9208_adc_set_ddc_nco_ftw(spi, ddc_ch, ftw, maw, mbw);

    return API_ERROR_OK;
}
//-1 invalid parametr; 0 - всё ок.
int my_ad9208_adc_set_ddc_nco(void *spi, AD9208_t *AD, uint8_t ddc_ch,const int64_t carrier_freq_hz)
{
    uint64_t tmp_freq;
    int err;
    uint8_t is_pow2 = 0;

    if (spi == NULL)
        return -1;

    if (!carrier_freq_hz)
        return -1;

    if (!((carrier_freq_hz >= (int64_t) (0ll - AD->adc_clk_freq_hz / 2)) &&
          (carrier_freq_hz < (int64_t) (AD->adc_clk_freq_hz / 2))))
        return -1;

    if (ddc_ch >= AD9208_NOF_FC_MAX)
        return -1;

    tmp_freq = carrier_freq_hz;
    while (tmp_freq <= AD->adc_clk_freq_hz) {
        if ((tmp_freq) == AD->adc_clk_freq_hz) {
            /* It is power of 2 */
            is_pow2 = 1;
            break;
        }
        tmp_freq *= 2;
    }

    if (is_pow2 == 1) { /* Integer NCO mode */
        /*
         * As we are in Integer NCO mode it guranteed the
         *  value is integer power of 2
         */
        tmp_freq = do_div(AD->adc_clk_freq_hz, carrier_freq_hz);
        tmp_freq = do_div(ADI_POW2_48, tmp_freq);

        /* Write FTW */
        err = AD9208_adc_set_ddc_nco_ftw(spi, ddc_ch, tmp_freq, 0, 0);
        if (err != API_ERROR_OK)
            return -1;
    } else {        /* Programable Modulus Mode */
        int gcd;
        uint64_t ftw;
        uint64_t maw;
        uint64_t mbw;
        uint64_t M, N;

        uint64_t tmp_ah, tmp_al, /*tmp_bh, tmp_bl, tmp_fh, */ tmp_fl;

        gcd = adi_api_utils_gcd(carrier_freq_hz, AD->adc_clk_freq_hz);
        M = do_div(carrier_freq_hz, gcd);
        N = do_div(AD->adc_clk_freq_hz, gcd);

        if (M > NO_OS_S16_MAX) {
            uint64_t mask = U64MSB;
            int i = 0;

            while (((mask & M) == 0) && (mask != 1)) {
                mask >>= 1;
                i++;
            }
            ftw = do_div(M * ((uint64_t) 1u << i), N);
            ftw *= ((uint64_t) 1u << (48 - i));
        } else
            ftw = do_div(M * (ADI_POW2_48), N);

        adi_api_utils_mult_128(M, ADI_POW2_48, &tmp_ah, &tmp_al);
        adi_api_utils_mod_128(tmp_ah, tmp_al, N, &tmp_fl);

        maw = tmp_fl;
        mbw = N;

        gcd = adi_api_utils_gcd(maw, mbw);
        maw = do_div(maw, gcd);
        mbw = do_div(mbw, gcd);

        if ((maw > ADI_MAXUINT48) || (mbw > ADI_MAXUINT48))
            return API_ERROR_INVALID_PARAM; /*out of Range */

        err = AD9208_adc_set_ddc_nco_ftw(spi, ddc_ch, ftw, maw, mbw);
        if (err != API_ERROR_OK)
            return -1;
    }
    return API_ERROR_OK;
}

int adi_api_utils_gcd(int u, int v)
{
    int t;

    while (v != 0) {
        t = u;
        u = v;
        v = t % v;
    }
    return u < 0 ? -u : u;  /* abs(u) */
}

void adi_api_utils_mult_128(uint64_t a, uint64_t b, uint64_t *hi,uint64_t *lo)
{
    uint64_t ah = (a >> 32), al = a & 0xffffffff,
         bh = (b >> 32), bl = b & 0xffffffff,
         rh = (ah * bh), rl = (al * bl),
         rm1 = ah * bl, rm2 = al * bh,
         rm1h = rm1 >> 32, rm2h = rm2 >> 32,
         rm1l = rm1 & 0xffffffff, rm2l = rm2 & 0xffffffff,
         rmh = rm1h + rm2h, rml = rm1l + rm2l, c = ((rl >> 32) + rml) >> 32;

    rl = rl + (rml << 32);
    rh = rh + rmh + c;
    *lo = rl;
    *hi = rh;
}
void adi_api_utils_mod_128(uint64_t ah, uint64_t al, uint64_t div,
                  uint64_t *mod)
{
    uint64_t result = 0;
    uint64_t a;

    DIV_U64_REM(~0, div, &a);
    a += 1;

    DIV_U64_REM(ah, div, &ah);

    /*modular multiplication of (2^64*upper) % div */
    while (ah != 0) {
        if ((ah & 1) == 1) {
            result += a;
            if (result >= div)
                result -= div;
        }
        a <<= 1;
        if (a >= div)
            a -= div;
        ah >>= 1;
    }

    /* add up the 2 results and return the modulus */
    if (al > div)
        al -= div;

    DIV_U64_REM(al + result, div, mod);
}

uint64_t div64_u64_rem(uint64_t dividend, uint64_t divisor, uint64_t *remainder)
{
    *remainder = dividend % divisor;
    return dividend / divisor;
}
uint64_t do_div(uint64_t n, uint64_t base)
{
    uint64_t mod = 0;

    mod = n % base;
    n = n / base;

    return n;
}
uint8_t ad9208_adc_set_ddc_nco_reset(void *spi)
{
    uint8_t tmp_reg;

    tmp_reg = spi1_read(spi, AD9208_DDC_SYNC_CTRL_REG);
    spi1_write(spi, AD9208_DDC_SYNC_CTRL_REG, tmp_reg | AD9208_NCO_SOFT_RESET);

    spi1_write(spi, AD9208_DDC_SYNC_CTRL_REG, (tmp_reg & ~AD9208_NCO_SOFT_RESET));

    return API_ERROR_OK;
}

uint8_t ad9208_adc_set_clk_phase(void *spi, uint8_t phase_adj)
{
    if (phase_adj > AD9208_IP_CLK_PHASE_ADJ(ALL))
    {
        return API_ERROR_INVALID_PARAM;
    }

    spi1_write(spi, AD9208_IP_CLK_PHASE_ADJ_REG, phase_adj);

    return API_ERROR_OK;
}

uint8_t ad9208_adc_temp_sensor_set_enable(void *spi, uint8_t en)
{
    int err;
    uint8_t tmp_reg;

    if (en > 1)
    {
        return API_ERROR_INVALID_PARAM;
    }

    err = AD9208_adc_set_vcm_export(spi, 0x0);
    if (err != API_ERROR_OK)
    {
        return err;
    }
    /*Set Internal or External Vref */
    tmp_reg = (en == 1) ? AD9208_CENTRAL_DIODE_20X_EN : 0x0;
    spi1_write(spi, AD9208_TEMP_DIODE_CTRL_REG, tmp_reg);
    return API_ERROR_OK;
}

uint8_t AD9208_adc_set_ddc_nco_ftw(void *spi, uint8_t ddc_ch, uint64_t ftw, uint64_t mod_a, uint64_t mod_b)
{
    uint32_t offset;
    uint8_t tmp_reg;

    if (ddc_ch >= AD9208_NOF_FC_MAX)
    {
        return API_ERROR_INVALID_PARAM;
    }

    offset = (AD9208_DDCX_REG_OFFSET * ddc_ch);
    spi1_write(spi, (AD9208_DDCX_FTW0_REG + offset), ADI_GET_BYTE(ftw, 0));
    spi1_write(spi, (AD9208_DDCX_FTW1_REG + offset), ADI_GET_BYTE(ftw, 8));
    spi1_write(spi, (AD9208_DDCX_FTW2_REG + offset), ADI_GET_BYTE(ftw, 16));
    spi1_write(spi, (AD9208_DDCX_FTW3_REG + offset), ADI_GET_BYTE(ftw, 24));
    spi1_write(spi, (AD9208_DDCX_FTW4_REG + offset), ADI_GET_BYTE(ftw, 32));
    spi1_write(spi, (AD9208_DDCX_FTW5_REG + offset), ADI_GET_BYTE(ftw, 40));

    offset = (AD9208_DDCX_FRAC_REG_OFFSET * ddc_ch);
    if ((mod_a != 0) && (mod_b != 0))
        {
            spi1_write(spi, AD9208_DDCX_MAW0_REG + offset,ADI_GET_BYTE(mod_a, 0));
            spi1_write(spi, AD9208_DDCX_MAW1_REG + offset,ADI_GET_BYTE(mod_a, 8));
            spi1_write(spi, AD9208_DDCX_MAW2_REG + offset,ADI_GET_BYTE(mod_a, 16));
            spi1_write(spi, AD9208_DDCX_MAW3_REG + offset,ADI_GET_BYTE(mod_a, 24));
            spi1_write(spi, AD9208_DDCX_MAW4_REG + offset,ADI_GET_BYTE(mod_a, 32));
            spi1_write(spi, AD9208_DDCX_MAW5_REG + offset,ADI_GET_BYTE(mod_a, 40));
            spi1_write(spi, AD9208_DDCX_MBW0_REG + offset,ADI_GET_BYTE(mod_b, 0));
            spi1_write(spi, AD9208_DDCX_MBW1_REG + offset,ADI_GET_BYTE(mod_b, 8));
            spi1_write(spi, AD9208_DDCX_MBW2_REG + offset,ADI_GET_BYTE(mod_b, 16));
            spi1_write(spi, AD9208_DDCX_MBW3_REG + offset,ADI_GET_BYTE(mod_b, 24));
            spi1_write(spi, AD9208_DDCX_MBW4_REG + offset,ADI_GET_BYTE(mod_b, 32));
            spi1_write(spi, AD9208_DDCX_MBW5_REG + offset,ADI_GET_BYTE(mod_b, 40));
        }

    AD9208_is_sync_spi_update_enabled(spi, &tmp_reg);

    if (tmp_reg)
    {
        AD9208_register_chip_transfer(spi);
    }
    return API_ERROR_OK;
}

uint8_t AD9208_adc_set_ddc_nco_phase(void *spi, uint8_t ddc_ch, uint64_t po)
{
    uint32_t offset;

    if (ddc_ch >= AD9208_NOF_FC_MAX)
    return API_ERROR_INVALID_PARAM;

    offset = (AD9208_DDCX_REG_OFFSET * ddc_ch);

    spi1_write(spi, (AD9208_DDCX_PO0_REG + offset), ADI_GET_BYTE(po, 0));
    spi1_write(spi, (AD9208_DDCX_PO1_REG + offset), ADI_GET_BYTE(po, 8));
    spi1_write(spi, (AD9208_DDCX_PO2_REG + offset), ADI_GET_BYTE(po, 16));
    spi1_write(spi, (AD9208_DDCX_PO3_REG + offset), ADI_GET_BYTE(po, 24));
    spi1_write(spi, (AD9208_DDCX_PO4_REG + offset), ADI_GET_BYTE(po, 32));
    spi1_write(spi, (AD9208_DDCX_PO5_REG + offset), ADI_GET_BYTE(po, 40));

    return API_ERROR_OK;
}

uint8_t ad9208_adc_get_overange_status(void *spi, uint8_t status)
{
    status = spi1_read(spi, AD9208_OP_OVERANGE_STAT_REG);

    spi1_write(spi, AD9208_OP_OVERANGE_CLR_REG, status);
    return API_ERROR_OK;
}

uint8_t ad9208_adc_set_fd_thresholds(void *spi, uint16_t upper_dbfs, uint16_t lower_dbfs, uint16_t dwell_time)
{
    if ((upper_dbfs > FD_THRESHOLD_MAG_DBFS_MAX) || (lower_dbfs > FD_THRESHOLD_MAG_DBFS_MAX) || (dwell_time > FD_DWELL_CLK_CYCLES_MAX))
    {
        return API_ERROR_INVALID_PARAM;
    }

    spi1_write(spi, AD9208_FD_DWELL_LSB_REG, AD9208_FD_DWELL_LSB(dwell_time));
    spi1_write(spi, AD9208_FD_DWELL_MSB_REG, AD9208_FD_DWELL_MSB(dwell_time));
    spi1_write(spi, AD9208_FD_UT_LSB_REG, AD9208_FD_UT_LSB(upper_dbfs));
    spi1_write(spi, AD9208_FD_UT_MSB_REG, AD9208_FD_UT_MSB(upper_dbfs));
    spi1_write(spi, AD9208_FD_UT_LSB_REG, AD9208_FD_LT_LSB(lower_dbfs));
    spi1_write(spi, AD9208_FD_LT_MSB_REG, AD9208_FD_LT_MSB(lower_dbfs));

    return API_ERROR_OK;
}

void spi_configure(void *spi)
{
    spi1_write(spi, AD9208_IF_CFG_A_REG, 0x00);
    spi1_write(spi, AD9208_IF_CFG_B_REG, 0x80);
}

void AD9208_reset(void *spi)
{
    spi1_write(spi, AD9208_IF_CFG_A_REG, 0x81);
    spi1_write(spi, AD9208_IF_CFG_B_REG, 0x02);
}

uint8_t AD9208_get_chip_id(void *spi, adi_chip_id_t *chip_id)
{
    uint8_t tmp_reg;

    tmp_reg = spi1_read(spi, AD9208_CHIP_TYPE_REG);
    chip_id->chip_type = tmp_reg;

    tmp_reg = spi1_read(spi, AD9208_PROD_ID_MSB_REG);
    chip_id->prod_id = tmp_reg;
    chip_id->prod_id <<= 8;

    tmp_reg = spi1_read(spi, AD9208_PROD_ID_LSB_REG);
    chip_id->prod_id |= tmp_reg;

    tmp_reg = spi1_read(spi,AD9208_CHIP_GRADE_REG);
    chip_id->prod_grade = (tmp_reg >> 4);
    chip_id->dev_revision = (tmp_reg & 0x0F);

    return API_ERROR_OK;
}

uint8_t AD9208_set_pdn_pin_mode(void *spi, uint8_t pin_enable, AD9208_pdn_mode_t pin_mode)
{
    uint16_t tmp_reg;

    if ((pin_mode != AD9208_POWERDOWN) && (pin_mode != AD9208_STANDBY))
    {
        return API_ERROR_INVALID_PARAM;
    }

    if (pin_enable > 1)
    {
        return API_ERROR_INVALID_PARAM;
    }
    if (!pin_enable)
    {
        spi1_write(spi, AD9208_CHIP_PIN_CTRL0_REG, (uint8_t)AD9208_CHIP_PDN_PIN_DISABLE);
    }
    else
    {
        spi1_write(spi, AD9208_CHIP_PIN_CTRL0_REG,(uint8_t) ~AD9208_CHIP_PDN_PIN_DISABLE);

        tmp_reg = spi1_read(spi, AD9208_CHIP_PIN_CTRL1_REG);

        tmp_reg &= (~AD9208_CHIP_PDN_MODE(ALL));
        tmp_reg |= ((pin_mode == AD9208_STANDBY) ? 0x1 : 0x0);

        spi1_write(spi, AD9208_CHIP_PIN_CTRL1_REG, tmp_reg);
    }

    return API_ERROR_OK;
}

uint8_t AD9208_set_input_clk_duty_cycle_stabilizer(void *spi, uint8_t en)
{
    spi1_write(spi, AD9208_IP_CLK_DCS1_REG, en);
    spi1_write(spi, AD9208_IP_CLK_DCS2_REG, en);

    return API_ERROR_OK;
}

uint8_t ad9208_get_revision(uint8_t *rev_major, uint8_t *rev_minor, uint8_t *rev_rc)
{
    uint8_t err = API_ERROR_OK;

    if (rev_major != NULL)
    {
        *rev_major = api_revision[0];
    }
    else
    {
        err = API_ERROR_INVALID_PARAM;
    }
    if (rev_minor != NULL)
    {
        *rev_minor = api_revision[1];
    }
    else
    {
        err = API_ERROR_INVALID_PARAM;
    }
    if (rev_rc != NULL)
    {
        *rev_rc = api_revision[2];
    }
    else
    {
        err = API_ERROR_INVALID_PARAM;
    }

    return err;
}

uint8_t AD9208_get_dec_filter_cfg(ad9208_adc_data_frmt_t op_data_format, uint8_t dcm_rate, uint8_t *filt_sel_val_0, uint8_t *filt_sel_val_1)
{
    uint8_t i = 0;

    if (op_data_format == AD9208_DATA_FRMT_COMPLEX)
    {
        for (i = 0; i < ARRAY_SIZE(ADI_DEC_FILTER_COMPLEX_TBL); i++)
        {
            if (ADI_DEC_FILTER_COMPLEX_TBL[i].dec_complex == dcm_rate)
            {
                *filt_sel_val_0 = ADI_DEC_FILTER_COMPLEX_TBL[i].ctrl_reg_val;
                *filt_sel_val_1 = ADI_DEC_FILTER_COMPLEX_TBL[i].sel_reg_val;
                return API_ERROR_OK;
            }
        }
    }

    if (op_data_format == AD9208_DATA_FRMT_REAL)
    {
        for (i = 0; i < ARRAY_SIZE(ADI_DEC_FILTER_COMPLEX_TBL); i++)
        {
            if (ADI_DEC_FILTER_COMPLEX_TBL[i].dec_real == dcm_rate)
            {
                *filt_sel_val_0 = ADI_DEC_FILTER_COMPLEX_TBL[i].ctrl_reg_val;
                *filt_sel_val_1 = ADI_DEC_FILTER_COMPLEX_TBL[i].sel_reg_val;
                return API_ERROR_OK;
            }
        }
    }

    return API_ERROR_INVALID_PARAM;
}

uint8_t AD9208_is_sync_spi_update_enabled(void *spi, uint8_t *enabled)
{
    uint8_t tmp_reg;

    if (enabled == NULL)
    {
        return API_ERROR_INVALID_PARAM;
    }

    tmp_reg = spi1_read(spi, AD9208_DDC_SYNC_CTRL_REG);

    *enabled = (tmp_reg & AD9208_DDC_UPDATE_MODE) ? 0x1 : 0x0;

    return API_ERROR_OK;
}

uint8_t AD9208_register_chip_transfer(void *spi)
{
    spi1_write(spi, AD9208_CHIP_SPI_XFER_REG, AD9208_CHIP_TRIGGER_SPI_XFER);

    return API_ERROR_OK;
}

uint8_t AD9208_get_decimation(void *spi, uint8_t *dcm)
{
    int i;
    uint8_t tmp_reg, cfg;

    tmp_reg = spi1_read(spi, AD9208_ADC_DCM_REG);

    cfg = AD9208_ADC_DCM_RATE(tmp_reg);

    for (i = 0; i < ARRAY_SIZE(ad9208_dcm_table); i++)
    {
        if (cfg == ad9208_dcm_table[i].dcm_cfg)
        {
            *dcm = ad9208_dcm_table[i].dcm;
            return API_ERROR_OK;
        }
    }
    return API_ERROR_INVALID_PARAM;
}

uint8_t AD9208_register_write_tbl(void *spi, struct adi_reg_data * tbl, uint32_t count)
{
    uint16_t i = 0;

    if (tbl == NULL)
    return API_ERROR_INVALID_PARAM;

    for (i = 0; i < count; i++)
    {
        spi1_write(spi, tbl[i].reg, tbl[i].val);
    }

    return API_ERROR_OK;
}

void ad9208_init(void *spi)
{
    spi_configure(spi);
    spi1_write(spi, 0xA, 0xFB);
}

uint8_t ad9208_testmode_set(void *spi, unsigned int chan, unsigned int mode)
{
    int ret;

    ret = AD9208_adc_set_channel_select(spi, BIT(chan & 1));
    if (ret < 0)
    {
        return ret;
    }

    spi1_write(spi, AD9208_REG_TEST_MODE, mode);

    return AD9208_adc_set_channel_select(spi, AD9208_ADC_CH_ALL);
}
// setting_type = 0x0 - полоса 50 MHz, fd = 104.1 MHz; есть сигнал с ch0, ADC0;
//setting_type = 0x1 - полоса 100 МHz, fd = 104.1 MHz; есть слабый сигнал с обоих каналов.
//Предположительно забор данных только с одного ddc по 1lane.

uint8_t return_data[504];

int32_t ad9208_setup(void *spi, int8_t setting_type)
{
        AD9208_t AD;
        adi_chip_id_t chip_id;
        uint64_t sample_rate, lane_rate_kbps;
        uint8_t pll_stat, tmp;
        int timeout, ret;

        choose_config_ADC(setting_type, &AD);

       // ad9208_init(spi);
      //  AD9208_reset(spi);
        spi1_write(spi,0xA,0x01);
        spi1_write(spi, AD9208_IF_CFG_A_REG, 0x0);
        spi1_write(spi, AD9208_IF_CFG_B_REG, 0x0);
        mdelay(1);
        //AD9208_jesd_enable_link(spi, 0);
        //spi1_write(spi, 0x571, 0x01);// Disable FACI
        AD9208_get_chip_id(spi, &chip_id);
        printf("AD9208 chip_type (%X)\n", chip_id.chip_type);
        printf("AD9208 chip id (%X)\n", chip_id.prod_id);
        tmp = spi1_read(spi, 0x011b);
        printf("011b = (%X)\n", tmp);

        AD9208_adc_set_channel_select(spi, AD9208_ADC_CH_ALL);

        AD9208_adc_set_ch_pdn_mode(spi, AD9208_POWERUP);
        AD9208_set_pdn_pin_mode(spi, 0, AD9208_POWERUP);


        AD9208_set_input_clk_duty_cycle_stabilizer(spi, 0x3);
        sample_rate = AD.sampling_frequency_hz * AD.input_div;

        AD9208_set_input_clk_cfg(spi, &AD, sample_rate, AD.input_div);

        AD9208_adc_set_input_cfg(spi, COUPLING_AC, 0, AD9208_ADC_SCALE_1P13_VPP);

        AD9208_adc_set_input_buffer_cfg(spi, AD9208_ADC_BUFF_CURR_600_UA, AD9208_ADC_BUFF_CURR_600_UA, AD9208_ADC_BUFF_CURR_600_UA);

        AD9208_adc_set_fc_ch_mode(spi, AD.fc_ch);

        setting_DDC(spi,setting_type,&AD);

       // AD9208_jesd_enable_link(spi, 0);
        spi1_write(spi, 0x0040, 0x09);//gpio_A0, gpio_B0 is LMFC
        spi1_write(spi, 0x055A, 0x05);//control bit 2 is sysref
        spi1_write(spi, 0x01FF, 0x00);//JESD204 B sync mode
        spi1_write(spi, 0x0040, 0x36);//gpio_A0, gpio_B0 determined by 0x0041
        spi1_write(spi, 0x0041, 0x9);//gpio_A0 is SNTI (было до изменений)
        //spi1_write(spi, 0x0042, 0x8);//gpio_A1 is MNTO (дла теста)
        lane_rate_kbps = 3375000;
        AD9208_jesd_syref_mode_set(spi, AD.sysref_mode, AD.sysref_count);
        AD9208_jesd_syref_config_set(spi, AD.sysref_edge_sel, AD.sysref_clk_edge_sel, AD.sysref_neg_window_skew, AD.sysref_pos_window_skew);
        AD9208_jesd_set_if_config(spi, &AD, (jesd_param_t )AD.jesd_param, &lane_rate_kbps);
        AD9208_jesd_subclass_set(spi, AD.jesd_subclass);
        AD9208_jesd_enable_scrambler(spi, 0);

        //ad9208_adc_set_sync_update_mode_enable(spi, 1);// enable chip transfer
        tmp = spi1_read(spi, 0x300);//DDC NEXT SYNC
        spi1_write(spi, 0x300, tmp|0x3);//DDC NEXT SYNC

        timeout = 10;
        AD9208_jesd_enable_link(spi, 1);

        tmp = spi1_read(spi,0x0310);
        tmp = spi1_read(spi,0x0330);
        tmp = spi1_read(spi,0x0350);
        tmp = spi1_read(spi,0x0370);

        tmp = spi1_read(spi,0x0311);
        tmp = spi1_read(spi,0x0331);
        tmp = spi1_read(spi,0x0351);
        tmp = spi1_read(spi,0x0371);

        tmp = spi1_read(spi,0x056F);
        mdelay(10);
        tmp = spi1_read(spi,0x056F);
        timeout = 10;

        do {
            mdelay(10);
            ret = AD9208_jesd_get_pll_status(spi, &pll_stat);
            if (ret < 0)
            {
                printf("Failed to get pll status (%d)\n", ret);
            }
        } while (!(pll_stat & AD9208_JESD_PLL_LOCK_STAT) && timeout--);

        printf("AD9208 PLL %s\n", (pll_stat & AD9208_JESD_PLL_LOCK_STAT) ? "LOCKED" : "UNLOCKED");

}

int32_t ad9208_onechannel_setup(void *spi)
{
    AD9208_t AD;
    adi_chip_id_t chip_id;
    uint64_t sample_rate, lane_rate_kbps;
    uint8_t pll_stat;
    int timeout, i, ret;

    AD.sampling_frequency_hz = mhz_to_hz(2500.0); //2500000000 3100000000  //2750000000
    AD.input_div = 1;
    AD.fc_ch = 2; //Chip application mode
    AD.ddc_cnt = 2;// ddc

    AD.sysref_lmfc_offset = 0x0;
    AD.sysref_edge_sel = 0x0;
    AD.sysref_clk_edge_sel = 0x0;
    AD.sysref_neg_window_skew = 0x0;
    AD.sysref_pos_window_skew = 0x0;
    AD.sysref_mode = SYSREF_NONE;//SYSREF_ONESHOT;
    AD.sysref_count = 0x0;//0x7;

    for(i = 0; i < AD.ddc_cnt; i++)
    {
        AD.ddc[i].decimation = 12;       //12
        AD.ddc[i].nco_mode = AD9208_ADC_NCO_VIF;
        if(i == 1 || i == 3)
        {
            AD.ddc[i].carrier_freq_hz = mhz_to_hz(300.0)+AD.sampling_frequency_hz/(AD.ddc[i].decimation*4);
        }
        else
        {
            AD.ddc[i].carrier_freq_hz = mhz_to_hz(300.0);
        }
        AD.ddc[i].po = 0x0;
        AD.ddc[i].gain_db = 1;  //1
    }

    AD.jesd_param.jesd_L    = 2;
    AD.jesd_param.jesd_F    = 2;
    AD.jesd_param.jesd_K    = 32;
    AD.jesd_param.jesd_M    = 2;
    AD.jesd_param.jesd_CS   = 1;
    AD.jesd_param.jesd_N    = 14;
    AD.jesd_param.jesd_NP   = 16;
    AD.jesd_param.jesd_CF   = 0;
    AD.jesd_subclass = JESD_SUBCLASS_0;

    AD.powerdown_pin_en = true;
    AD.powerdown_mode = AD9208_POWERUP;


    ad9208_init(spi);
    AD9208_reset(spi);

    mdelay(1);
    AD9208_jesd_enable_link(spi, 0);

    //spi1_write(spi, 0x571, 0x01);// Disable FACI

    AD9208_get_chip_id(spi, &chip_id);
    printf("AD9208 chip_type (%X)\n", chip_id.chip_type);
    printf("AD9208 chip id (%X)\n", chip_id.prod_id);

    uint8_t tmp = spi1_read(spi, 0x011b);
    printf("011b = (%X)\n", tmp);

    AD9208_adc_set_ch_pdn_mode(spi, AD9208_POWERUP);
    AD9208_set_pdn_pin_mode(spi, 0, AD9208_POWERUP);
    spi1_write(spi, 0x0040, 0x09);//gpio_A0, gpio_B0 is LMFC

    spi1_write(spi, 0x055A, 0x05);//control bit 2 is sysref

    spi1_write(spi, 0x01FF, 0x00);//JESD204 B sync mode

    AD9208_adc_set_channel_select(spi, AD9208_ADC_CH_A);
    AD9208_set_input_clk_duty_cycle_stabilizer(spi, 0x3); // почему 0x03?

    tmp = spi1_read(spi, 0x0701);
    spi1_write(spi,0x0701,0x86);
    AD9208_adc_set_input_cfg(spi, COUPLING_AC, 0, AD9208_ADC_SCALE_1P13_VPP);
    AD9208_adc_set_input_buffer_cfg(spi, AD9208_ADC_BUFF_CURR_600_UA, AD9208_ADC_BUFF_CURR_600_UA, AD9208_ADC_BUFF_CURR_600_UA);

    sample_rate = AD.sampling_frequency_hz * AD.input_div;
    AD9208_set_input_clk_cfg(spi, &AD, sample_rate, AD.input_div);
    AD9208_adc_set_fc_ch_mode(spi, AD.fc_ch);           // задаёт chip application mode
    AD9208_adc_set_dcm_mode(spi,12);
    AD9208_adc_set_data_format(spi, AD9208_DATA_FRMT_REAL, AD9208_DATA_FRMT_REAL);

    for(i = 0; i < AD.ddc_cnt; i++)
    {
        AD9208_adc_set_ddc_gain(spi, i, AD.ddc[i].gain_db ? 6 : 0);
        AD9208_adc_set_ddc_dcm(spi, i, AD.ddc[i].decimation);
        if (AD.ddc_cnt == 1)
        {
            ad9208_adc_set_ddc_ip_mux(spi, i, AD9208_ADC_CH_A, AD9208_ADC_CH_A);
        }
        else
        {
            ad9208_adc_set_ddc_ip_mux(spi, i, AD9208_ADC_CH_A, AD9208_ADC_CH_A);
        }
        AD9208_adc_set_ddc_nco_mode(spi, i, AD.ddc[i].nco_mode);
        my_ad9208_adc_set_ddc_nco(spi, &AD,i,AD.ddc[i].carrier_freq_hz);
        //AD9208_adc_set_ddc_nco(spi, &AD, i,  AD.ddc[i].carrier_freq_hz);
        AD9208_adc_set_ddc_nco_phase(spi, i, AD.ddc[i].po);
    }

    spi1_write(spi, 0x0040, 0x36);//gpio_A0, gpio_B0 determined by 0x0041
    spi1_write(spi, 0x0042, 0x8);//gpio_A1 is MNTO

    AD9208_jesd_syref_mode_set(spi, AD.sysref_mode, AD.sysref_count);
    AD9208_jesd_syref_config_set(spi, AD.sysref_edge_sel, AD.sysref_clk_edge_sel, AD.sysref_neg_window_skew, AD.sysref_pos_window_skew);
    AD9208_jesd_set_if_config(spi, &AD, (jesd_param_t )AD.jesd_param, &lane_rate_kbps);
    AD9208_jesd_subclass_set(spi, AD.jesd_subclass);
    AD9208_jesd_enable_scrambler(spi, 0);
    tmp = spi1_read(spi, 0x300);//DDC NEXT SYNC
    spi1_write(spi, 0x300, tmp | 0x3);//DDC NEXT SYNC
    timeout = 10;
    AD9208_jesd_enable_link(spi, 1);

    timeout = 10;

    do {
        mdelay(10);
        ret = AD9208_jesd_get_pll_status(spi, &pll_stat);
        if (ret < 0)
        {
            printf("Failed to get pll status (%d)\n", ret);
        }
    } while (!(pll_stat & AD9208_JESD_PLL_LOCK_STAT) && timeout--);

    printf("AD9208 PLL %s\n", (pll_stat & AD9208_JESD_PLL_LOCK_STAT) ? "LOCKED" : "UNLOCKED");

    return 0;
}

int32_t FTW_send(void *spi, bool ftw_freq_sel)
{
    uint64_t tmp_carrier_freq;
    uint64_t tmp_adc_clk_freq;
    uint64_t ftw;
    uint64_t maw;
    uint64_t mbw;
    uint64_t M;
    uint64_t N;
    uint64_t pow_2_48 = 0x0001000000000000;
    tmp_adc_clk_freq = (2500000000)/1000000;
    N = tmp_adc_clk_freq;
    maw = 0;
    mbw = 0;

    if(ftw_freq_sel)
    {
        tmp_carrier_freq = 200000000/1000000;
    }
    else
    {
        tmp_carrier_freq = 100000000/1000000;
    }

    M = tmp_carrier_freq % tmp_adc_clk_freq;

    ftw = pow_2_48*M/N;

    AD9208_adc_set_ddc_nco_ftw(spi, 0, ftw, maw, mbw);//DDC 0
///////////////////////////////////////////////////////////////
//    tmp_carrier_freq = 200000000/1000000;//DDC1
//
//    M = tmp_carrier_freq % tmp_adc_clk_freq;
//
//    ftw = pow_2_48*M/N;
//
//    AD9208_adc_set_ddc_nco_ftw(spi, 1, ftw, maw, mbw);

    return 0;
}
// 0 - всё ок, 1 - ошибка
int8_t choose_config_ADC(uint8_t type_config, AD9208_t *AD)
{
    uint8_t num_ch;

    if (type_config == 0)
    {

        AD->sampling_frequency_hz =  mhz_to_hz(2500.0);//3100000000;
        AD->input_div = 1;
        AD->fc_ch = 2; //Chip application mode
        AD->ddc_cnt = 2;// ddc

        AD->sysref_lmfc_offset = 0x0;
        AD->sysref_edge_sel = 0x0;
        AD->sysref_clk_edge_sel = 0x0;
        AD->sysref_neg_window_skew = 0x0;
        AD->sysref_pos_window_skew = 0x0;
        AD->sysref_mode = SYSREF_NONE;//SYSREF_ONESHOT;
        AD->sysref_count = 0x0;//0x7;

        num_ch = 0;
        AD->ddc[num_ch].decimation = 12;
        AD->ddc[num_ch].nco_mode = AD9208_ADC_NCO_VIF;
        AD->ddc[num_ch].carrier_freq_hz = mhz_to_hz(300.0);
        AD->ddc[num_ch].po = 0x0;
        AD->ddc[num_ch].gain_db = 1;
        num_ch = 1;
        AD->ddc[num_ch].decimation = 12;
        AD->ddc[num_ch].nco_mode = AD9208_ADC_NCO_VIF;
        AD->ddc[num_ch].carrier_freq_hz = mhz_to_hz(326.0);
        AD->ddc[num_ch].po = 0x0;
        AD->ddc[num_ch].gain_db = 1;

        AD->jesd_param.jesd_L    = 2;
        AD->jesd_param.jesd_F    = 4;    // F = 2
        AD->jesd_param.jesd_K    = 32;
        AD->jesd_param.jesd_M    = 4;    // M = 2
        AD->jesd_param.jesd_CS   = 1;
        AD->jesd_param.jesd_N    = 14;
        AD->jesd_param.jesd_NP   = 16;
        AD->jesd_param.jesd_CF   = 0;
        AD->jesd_param.jesd_S    = 1;
        AD->jesd_param.jesd_HD   = 0;
        AD->jesd_subclass = JESD_SUBCLASS_0;

        AD->powerdown_pin_en = true;
        AD->powerdown_mode = AD9208_POWERUP;
        return 0;
    }
    else if (type_config == 0x1)
    {
       AD->sampling_frequency_hz =  mhz_to_hz(2500.0);//3100000000;
       AD->input_div = 1;
       AD->fc_ch = 2; //Chip application mode
       AD->ddc_cnt = 2;// ddc

       AD->sysref_lmfc_offset = 0x0;
       AD->sysref_edge_sel = 0x0;
       AD->sysref_clk_edge_sel = 0x0;
       AD->sysref_neg_window_skew = 0x0;
       AD->sysref_pos_window_skew = 0x0;
       AD->sysref_mode = SYSREF_NONE;//SYSREF_ONESHOT;
       AD->sysref_count = 0x0;//0x7;

       num_ch = 0;
       AD->ddc[num_ch].decimation = 24;
       AD->ddc[num_ch].nco_mode = AD9208_ADC_NCO_VIF;
       AD->ddc[num_ch].carrier_freq_hz = mhz_to_hz(300.0);
       AD->ddc[num_ch].po = 0x0;
       AD->ddc[num_ch].gain_db = 1;
       num_ch = 1;
       AD->ddc[num_ch].decimation = 24;
       AD->ddc[num_ch].nco_mode = AD9208_ADC_NCO_VIF;
       AD->ddc[num_ch].carrier_freq_hz = mhz_to_hz(300.0);
       AD->ddc[num_ch].po = 0x0;
       AD->ddc[num_ch].gain_db = 1;

       AD->jesd_param.jesd_L    = 2;
       AD->jesd_param.jesd_F    = 4;    // F = 2
       AD->jesd_param.jesd_K    = 32;
       AD->jesd_param.jesd_M    = 4;    // M = 2
       AD->jesd_param.jesd_CS   = 1;
       AD->jesd_param.jesd_N    = 14;
       AD->jesd_param.jesd_NP   = 16;
       AD->jesd_param.jesd_CF   = 0;
       AD->jesd_param.jesd_S    = 1;
       AD->jesd_param.jesd_HD   = 0;
       AD->jesd_subclass = JESD_SUBCLASS_0;

       AD->powerdown_pin_en = true;
       return 0;
    }
    if (type_config == 2)
    {

        AD->sampling_frequency_hz =  mhz_to_hz(2500.0);//3100000000;
        AD->input_div = 1;
        AD->fc_ch = 2; //Chip application mode
        AD->ddc_cnt = 2;// ddc

        AD->sysref_lmfc_offset = 0x0;
        AD->sysref_edge_sel = 0x0;
        AD->sysref_clk_edge_sel = 0x0;
        AD->sysref_neg_window_skew = 0x0;
        AD->sysref_pos_window_skew = 0x0;
        AD->sysref_mode = SYSREF_NONE;//SYSREF_ONESHOT;
        AD->sysref_count = 0x0;//0x7;

        num_ch = 0;
        AD->ddc[num_ch].decimation = 12;
        AD->ddc[num_ch].nco_mode = AD9208_ADC_NCO_VIF;
        AD->ddc[num_ch].carrier_freq_hz = mhz_to_hz(300.0);
        AD->ddc[num_ch].po = 0x0;
        AD->ddc[num_ch].gain_db = 1;
        num_ch = 1;
        AD->ddc[num_ch].decimation = 12;
        AD->ddc[num_ch].nco_mode = AD9208_ADC_NCO_VIF;
        AD->ddc[num_ch].carrier_freq_hz = mhz_to_hz(300.0); // 326MHz
        AD->ddc[num_ch].po = 0x0;
        AD->ddc[num_ch].gain_db = 0;    //1

        AD->jesd_param.jesd_L    = 2;
        AD->jesd_param.jesd_F    = 2;    // F = 2
        AD->jesd_param.jesd_K    = 32;
        AD->jesd_param.jesd_M    = 2;    // M = 2
        AD->jesd_param.jesd_CS   = 1;
        AD->jesd_param.jesd_N    = 14;
        AD->jesd_param.jesd_NP   = 16;
        AD->jesd_param.jesd_CF   = 0;
        AD->jesd_param.jesd_S    = 1;
        AD->jesd_param.jesd_HD   = 0;
        AD->jesd_subclass = JESD_SUBCLASS_0;

        AD->powerdown_pin_en = true;
        AD->powerdown_mode = AD9208_POWERUP;
        return 0;
    }

    else return -1;

    return 0;
}
// 0 - всё ок, -1 - ошибка
int8_t setting_DDC(void *spi,uint8_t type_config, AD9208_t *AD)
{
    uint8_t num_ch;
    if (type_config == 0x0)
    {
        AD9208_adc_set_dcm_mode(spi, 24);

        AD9208_adc_set_data_format(spi, AD9208_DATA_FRMT_REAL, AD9208_DATA_FRMT_REAL);


        num_ch = 0;
        AD9208_adc_set_ddc_gain(spi, num_ch, 0);
        //AD9208_adc_set_ddc_gain(spi, num_ch,  AD->ddc[num_ch].gain_db ? 6 : 0);
        AD9208_adc_set_ddc_dcm(spi, num_ch,  AD->ddc[num_ch].decimation);
        AD9208_adc_set_ddc_nco_mode(spi, num_ch,  AD->ddc[num_ch].nco_mode);
        my_ad9208_adc_set_ddc_nco(spi, AD,num_ch, AD->ddc[num_ch].carrier_freq_hz);
        AD9208_adc_set_ddc_nco_phase(spi, num_ch,  AD->ddc[num_ch].po);
        ad9208_adc_set_ddc_ip_mux(spi, num_ch, AD9208_ADC_CH_A, AD9208_ADC_CH_A);

        mdelay(10);
        num_ch = 1;

        AD9208_adc_set_ddc_gain(spi, num_ch, 0);
        //AD9208_adc_set_ddc_gain(spi, num_ch,  AD->ddc[num_ch].gain_db ? 6 : 0);
        AD9208_adc_set_ddc_dcm(spi, num_ch,  AD->ddc[num_ch].decimation);
        AD9208_adc_set_ddc_nco_mode(spi, num_ch,  AD->ddc[num_ch].nco_mode);
        my_ad9208_adc_set_ddc_nco(spi, AD,num_ch, AD->ddc[num_ch].carrier_freq_hz);
        //AD9208_adc_set_ddc_nco(spi, &AD, num_ch,   AD->ddc[num_ch].carrier_freq_hz);
        AD9208_adc_set_ddc_nco_phase(spi, num_ch,  AD->ddc[num_ch].po);
        ad9208_adc_set_ddc_ip_mux(spi, num_ch, AD9208_ADC_CH_A, AD9208_ADC_CH_A);

        mdelay(10);

    }
    else if (type_config == 0x1)
    {
        AD9208_adc_set_dcm_mode(spi, 24);

        AD9208_adc_set_data_format(spi, AD9208_DATA_FRMT_COMPLEX, AD9208_DATA_FRMT_COMPLEX);

        num_ch = 0;
        AD9208_adc_set_ddc_gain(spi, num_ch, 0);
        //AD9208_adc_set_ddc_gain(spi, num_ch,  AD->ddc[num_ch].gain_db ? 6 : 0);
        AD9208_adc_set_ddc_dcm(spi, num_ch,  AD->ddc[num_ch].decimation);
        AD9208_adc_set_ddc_nco_mode(spi, num_ch, AD->ddc[num_ch].nco_mode);
        my_ad9208_adc_set_ddc_nco(spi, AD,num_ch, AD->ddc[num_ch].carrier_freq_hz);
        AD9208_adc_set_ddc_nco_phase(spi, num_ch,  AD->ddc[num_ch].po);
        ad9208_adc_set_ddc_ip_mux(spi, num_ch, AD9208_ADC_CH_A, AD9208_ADC_CH_B);
        mdelay(10);
        num_ch = 1;

        AD9208_adc_set_ddc_gain(spi, num_ch, 0);
        //AD9208_adc_set_ddc_gain(spi, num_ch,  AD->ddc[num_ch].gain_db ? 6 : 0);
        AD9208_adc_set_ddc_dcm(spi, num_ch,  AD->ddc[num_ch].decimation);
        AD9208_adc_set_ddc_nco_mode(spi, num_ch, AD->ddc[num_ch].nco_mode);
        my_ad9208_adc_set_ddc_nco(spi, AD,num_ch, AD->ddc[num_ch].carrier_freq_hz);
        //AD9208_adc_set_ddc_nco(spi, &AD, num_ch,   AD->ddc[num_ch].carrier_freq_hz);
        AD9208_adc_set_ddc_nco_phase(spi, num_ch,  AD->ddc[num_ch].po);
        ad9208_adc_set_ddc_ip_mux(spi, num_ch, AD9208_ADC_CH_A, AD9208_ADC_CH_B);


    }
    else if (type_config == 0x2)
    {
        AD9208_adc_set_dcm_mode(spi, 12);

        AD9208_adc_set_data_format(spi, AD9208_DATA_FRMT_REAL, AD9208_DATA_FRMT_REAL);


        num_ch = 0;
        AD9208_adc_set_ddc_gain(spi, num_ch, 0);
        //AD9208_adc_set_ddc_gain(spi, num_ch,  AD->ddc[num_ch].gain_db ? 6 : 0);
        AD9208_adc_set_ddc_dcm(spi, num_ch,  AD->ddc[num_ch].decimation);
        AD9208_adc_set_ddc_nco_mode(spi, num_ch,  AD->ddc[num_ch].nco_mode);
        my_ad9208_adc_set_ddc_nco(spi, AD,num_ch, AD->ddc[num_ch].carrier_freq_hz);
        AD9208_adc_set_ddc_nco_phase(spi, num_ch,  AD->ddc[num_ch].po);
        ad9208_adc_set_ddc_ip_mux(spi, num_ch, AD9208_ADC_CH_A, AD9208_ADC_CH_A);

        mdelay(10);
        num_ch = 1;

        AD9208_adc_set_ddc_gain(spi, num_ch, 0);
        //AD9208_adc_set_ddc_gain(spi, num_ch,  AD->ddc[num_ch].gain_db ? 6 : 0);
        AD9208_adc_set_ddc_dcm(spi, num_ch,  AD->ddc[num_ch].decimation);
        AD9208_adc_set_ddc_nco_mode(spi, num_ch,  AD->ddc[num_ch].nco_mode);
        my_ad9208_adc_set_ddc_nco(spi, AD,num_ch, AD->ddc[num_ch].carrier_freq_hz);
        //AD9208_adc_set_ddc_nco(spi, &AD, num_ch,   AD->ddc[num_ch].carrier_freq_hz);
        AD9208_adc_set_ddc_nco_phase(spi, num_ch,  AD->ddc[num_ch].po);
        ad9208_adc_set_ddc_ip_mux(spi, num_ch, AD9208_ADC_CH_B, AD9208_ADC_CH_B);

        mdelay(10);

    }
    else return -1;
    return 0;
}
