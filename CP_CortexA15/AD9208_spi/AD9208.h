#include <ctype.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <limits.h>
#include <stdio.h>
#include <ti/drv/spi/SPI.h>
#include <ti/drv/spi/MCSPI.h>
#include "ad9208_reg.h"
#include "delay/delay.h"
#include "SPI/SPI_header.h"

typedef uint32_t AD9208_STATUS;


#define SUCCESS                                 0
#define ARRAY_SIZE(arr)                         (sizeof(arr) / sizeof((arr)[0]))
#define min(x, y)                               (((x) < (y)) ? (x) : (y))
#define min_t(type, x, y)                       (type)min((type)(x), (type)(y))
#define max(x, y)                               (((x) > (y)) ? (x) : (y))
#define max_t(type, x, y)                       (type)max((type)(x), (type)(y))
#define clamp(val, min_val, max_val)            (max(min((val), (max_val)), (min_val)))
#define clamp_t(type, val, min_val, max_val)    (type)clamp((type)(val), (type)(min_val), (type)(max_val))
#define DIV_ROUND_UP(x, y)                      (((x) + (y) - 1) / (y))
#define DIV_ROUND_CLOSEST(x, divisor)           (((x) + (divisor) / 2) / (divisor))
#define BIT(x)                                  (1 << (x))
#define CLK_IGNORE_UNUSED                       BIT(3)
#define CLK_GET_RATE_NOCACHE                    BIT(6)
#define ADI_POW2_48 ((uint64_t)1u << 48)
#define ADI_MAXUINT48 (ADI_POW2_48 - 1)
#define ADI_POW2_32 ((uint64_t)1u << 32)
#define ADI_MAXUINT32 (ADI_POW2_32 - 1)
#define ADI_MAXUINT24 (0xFFFFFF)
#define ADI_GET_BYTE(w, p) (uint8_t)(((w) >> (p)) & 0xFF)
#define ALL -1
#define DIV_U64_REM(x, y, r) div64_u64_rem(x, y, r)
#define DIV_U64(x, y) div_u64(x, y)
#define DIV_S64(x, y) div_s64(x, y)


#define LOWER_16(A) ((A) & 0xFFFF)
#define UPPER_16(A) (((A) >> 16) & 0xFFFF)
#define LOWER_32(A) ((A) & (uint32_t) 0xFFFFFFFF)
#define MHZ_TO_HZ(x) (((uint64_t)x)*1000*1000)
#define MS_TO_US(x) ((x)*1000)
#define U64MSB 0x8000000000000000ull


#define AD9208_DEV_ID    0x0001
#define AD9208_PASS                       (0U)
#define AD9208_FAIL                       (-1U)
#define AD9208_TIMEOUT                    (-2U)
#define AD9208_INVALID_PARAM              (-3U)

#define AD9208_IP_CLK_MIN_HZ 2500000000ULL
#define AD9208_IP_CLK_MAX_HZ 6000000000ULL
#define AD9208_ADC_CLK_MIN_HZ 2500000000ULL
#define AD9208_ADC_CLK_MAX_HZ 3100000000ULL
#define HW_RESET_PERIOD_US 5000
#define SPI_RESET_PERIOD_US 5000

#define AD9208_CHIP_TYPE    0x03
#define AD9208_CHIP_ID      0xDF


#define AD9208_NOF_FC_MAX 4
#define AD9208_ADC_DCM_NONE 1
#define AD9208_ADC_DCM_MIN 1
#define AD9208_ADC_DCM_MAX 48
#define NCO_RESET_PERIOD_US 0

#define AD9208_FULL_BANDWIDTH_MODE 0

#define CTRL_CORE_PAD_GPMC_A8_MUXMODE               (0x40000 | 0x8)
#define CTRL_CORE_PAD_GPMC_A9_MUXMODE               (0x40000 | 0x8)
#define CTRL_CORE_PAD_GPMC_A10_MUXMODE              (0x40000 | 0x8)
#define CTRL_CORE_PAD_GPMC_A11_MUXMODE              (0x40000 | 0x8)
#define CTRL_CORE_PAD_GPMC_A12_MUXMODE              (0x40000 | 0x8)

#define CTRL_CORE_PAD_GPMC_WAIT0                    (0xD8U)     //RESET    AD1
#define CTRL_CORE_PAD_MCASP2_AXR6                   (0x31CU)    //RESET    AD2
#define CTRL_CORE_PAD_GPMC_WAIT0_MUXMODE            (0xE0000 | 0xE)//pull down
#define CTRL_CORE_PAD_MCASP2_AXR6_MUXMODE           (0x40000 | 0xE)
#define CSL_MPU_CORE_PAD_IO_REGISTERS_REGS          (0x4a003400U)
#define SOC_CORE_PAD_IO_REGISTERS_BASE       (CSL_MPU_CORE_PAD_IO_REGISTERS_REGS)

#define K_MIN 4
#define K_MAX 32
#define LANE_MIN 1
#define LANE_MAX 8
#define CS_MAX 3
#define N_MIN 7
#define N_MAX 16
#define CF_DEFAULT 0
#define LANE_RATE_MIN_MBPS 390
#define LANE_RATE_MAX_MBPS 16000

/** Enumerates all available Jesd Subclass Modes */
typedef enum {
    JESD_SUBCLASS_0, /**< JESD SUBCLASS 0*/
    JESD_SUBCLASS_1, /**< JESD SUBCLASS 1 */
    JESD_SUBCLASS_INVALID
} jesd_subclass_t;


struct AD9208_regs_struct{
    uint16_t reg_0x0003_Chip_type;
    uint16_t reg_0x0004_Chip_ID_LSB;
    uint16_t reg_0x0005_Chip_ID_MSB;
    uint16_t reg_0x0006_Chip_grade;
    uint16_t chip_type;
    uint16_t prod_id;
    uint16_t prod_grade;
    uint16_t dev_revision;
};

typedef struct AD9208_regs_struct AD9208_regs_t;

struct AD9208_ddc{
    uint32_t decimation;
    uint32_t nco_mode;
    uint64_t carrier_freq_hz;
    uint64_t po;
    bool gain_db;
};

typedef struct AD9208_ddc AD9208_ddc_t;


/** JESD Interface Parameters. */
typedef struct
{
    uint8_t jesd_L; /**< JESD Lane Param L. */
    uint8_t jesd_F; /**< JESD Octet Param F. */
    uint8_t jesd_M; /**< JESD Converter Param M. */
    uint8_t jesd_S; /**< JESD No of Sample Param S. */

    uint8_t jesd_HD; /** JESD High Density Param HD.*/
    uint8_t jesd_K; /**< JESD multiframe Param K. */
    uint8_t jesd_N; /**< JESD Converter Resolution Param N. */
    uint8_t jesd_NP; /**< JESD Bit Packing Sample NP. */
    uint8_t jesd_CF; /**< JESD Param CF. */
    uint8_t jesd_CS; /**< JESD Param CS. */

    uint8_t jesd_DID; /**< JESD Device ID Param DID. */
    uint8_t jesd_BID; /**< JESD Bank ID. Param BID */
    uint8_t jesd_LID0; /**< JESD Lane ID for Lane 0 Param LIDO*/
    uint8_t jesd_JESDV; /**< JESD Version */
} jesd_param_t;

typedef struct
{
    uint64_t adc_clk_freq_hz;   /**< ADC Clock Frequency in Hz. Valid range 2.5GHz to 3.1 GHz */
    bool duty_cycle_stabilizer_en;
    uint64_t sampling_frequency_hz;
    uint32_t input_div;
    bool analog_input_mode;
    bool ext_vref_en;
    uint8_t current_scale;
    uint32_t buff_curr_n;
    uint32_t buff_curr_p;
    uint32_t fc_ch;
    uint32_t ddc_cnt;
    AD9208_ddc_t ddc[4];
    bool ddc_output_format_real_en;
    bool ddc_input_format_real_en;
    uint32_t sysref_lmfc_offset;
    bool sysref_edge_sel;
    bool sysref_clk_edge_sel;
    uint32_t sysref_neg_window_skew;
    uint32_t sysref_pos_window_skew;
    uint32_t sysref_mode;
    uint32_t sysref_count;
    jesd_param_t jesd_param;
    uint32_t jesd_subclass;

    bool powerdown_pin_en;
    uint32_t powerdown_mode;
    uint32_t test_mode_ch0;
    uint32_t test_mode_ch1;

}AD9208_t;

/** ADI Device Identification Data */
typedef struct {
    /** Chip Type Code */
    uint8_t chip_type;
    /** Product ID Code */
    uint16_t prod_id;
    /** Product Grade Code*/
    uint8_t prod_grade;
    /** Device Revision */
    uint8_t dev_revision;
} adi_chip_id_t;

//struct AD9208_struct AD9208_t;

typedef enum {
    AD9208_ADC_CH_NONE = 0x0,   /**< No ADC Channel*/
    AD9208_ADC_CH_A,        /**< ADC Channel A */
    AD9208_ADC_CH_B,        /**< ADC Channel B*/
    AD9208_ADC_CH_ALL       /**< ALL ADC Channels*/
} AD9208_adc_ch_t;

//! The opaque instance of the LMS7002M instance

typedef enum {
    AD9208_DATA_FRMT_REAL = 0x0,  /**< Real Data*/
    AD9208_DATA_FRMT_COMPLEX      /**< Complex Data */
} ad9208_adc_data_frmt_t;

typedef struct {
uint8_t dcm;
uint8_t dcm_cfg;
} chip_decimation_ratio_cfg;

typedef struct
{
    uint64_t slr_lwr_thres;
    uint64_t slr_upr_thres;
    uint8_t vco_cfg;
} jesd_serdes_pll_cfg;

typedef enum {
    AD9208_CB_LOW = 0,
    AD9208_CB_OVR_RANGE = 1,
    AD9208_CB_SIGNAL_MON = 2,
    AD9208_CB_FAST_DETECT = 3,
    AD9208_CB_SYSREF = 5,
} ad9208_control_bit_sel;

/** Enumerates SYSREF Synchronization Mode */
typedef enum {
    SYSREF_NONE = 0x0, /**< No SYSREF Support */
    SYSREF_CONT, /**< ONE-SHOT SYSREF */
    SYSREF_ONESHOT, /**< Continuous Sysref Synchronisation */
    SYSREF_MON, /**< SYSREF monitor Mode */
    SYSREF_MODE_INVLD
} jesd_sysref_mode_t;

/** Enumerates AD9208 Powerdown Modes*/
typedef enum {
    AD9208_POWERUP = 0x0,   /**< Normal Operational Powerup*/
    AD9208_STANDBY = 0x2,   /**< Standby Mode Powerup */
    AD9208_POWERDOWN = 0x3  /**< Full Powerdown Mode*/
} AD9208_pdn_mode_t;

/** Enumerate coupling mode */
typedef enum {
    COUPLING_AC = 0, /**AC COUPLED SIGNAL */
    COUPLING_DC, /**DC SIGNAL */
    COUPLING_UNKNOWN /**UNKNOWN/UNDEFINED COUPLING */
} signal_coupling_t;

typedef enum {
    AD9208_ADC_SCALE_2P04_VPP = 0,  /**< 2.04 Vpp Differential*/
    AD9208_ADC_SCALE_1P13_VPP,  /**< 1.13 Vpp Differential */
    AD9208_ADC_SCALE_1P25_VPP,  /**< 1.25 Vpp Differential */
    AD9208_ADC_SCALE_1P7_VPP,   /**< 1.70 Vpp Differential*/
    AD9208_ADC_SCALE_1P81_VPP,  /**< 1.81 Vpp Differential*/
    AD9208_ADC_SCALE_1P93_VPP   /**< 1.93 Vpp Differential*/
} ad9208_adc_scale_range_t;

/** Enumerates ADC Input Buffer Currents */
typedef enum {
    AD9208_ADC_BUFF_CURR_400_UA = 0x4,   /**< Buffer Current set to 400 uA*/
    AD9208_ADC_BUFF_CURR_500_UA = 0x9,   /**< Buffer Current set to 500 uA*/
    AD9208_ADC_BUFF_CURR_600_UA = 0x1E,  /**< Buffer Current set to 600 uA*/
    AD9208_ADC_BUFF_CURR_700_UA = 0x23,  /**< Buffer Current set to 700 uA*/
    AD9208_ADC_BUFF_CURR_800_UA = 0x28,  /**< Buffer Current set to 800 uA*/
    AD9208_ADC_BUFF_CURR_1000_UA = 0x32, /**< Buffer Current set to 1000 uA*/
} ad9208_adc_buff_curr_t;

/** Enumerates ADC NCO Modes*/
typedef enum {
    AD9208_ADC_NCO_VIF = 0,   /**< Variable IF Mode*/
    AD9208_ADC_NCO_ZIF = 1,   /**< Zero IF Mode */
    AD9208_ADC_NCO_TEST = 3,  /**< Test Mode*/
    AD9208_ADC_NCO_INVLD = 4  /**< Invalid NCO Mode*/
} ad9208_adc_nco_mode_t;

/** API register access structure */
struct adi_reg_data {
    /** Register address */
    uint16_t reg;
    /** Register value */
    uint8_t val;
};

static inline unsigned long int mhz_to_hz(float mhz){
    return((unsigned long int)((double)mhz*1000000.0));
}

void ad9208_init(void *spi);
bool spi_write(void *spi, uint16_t addr, uint8_t data);
uint8_t spi_read(void *spi, uint16_t addr);
uint8_t AD9208_register_write_tbl(void *spi, struct adi_reg_data * tbl, uint32_t count);
uint8_t ad9208_get_revision(uint8_t *rev_major, uint8_t *rev_minor, uint8_t *rev_rc);

void AD9208_jesd_syref_status_get(void *spi, uint8_t *hold_status, uint8_t *setup_status, uint8_t *phase_status);
uint8_t AD9208_jesd_syref_lmfc_offset_set(void *spi, uint8_t offset);
uint8_t AD9208_jesd_syref_config_set(void *spi, uint8_t sysref_edge_sel, uint8_t clk_edge_sel, uint8_t neg_window_skew, uint8_t pos_window_skew);
uint8_t AD9208_jesd_syref_mode_set(void *spi, jesd_sysref_mode_t mode, uint8_t sysref_count);
uint8_t check_jesd_params_range(jesd_param_t jesd_param);
uint8_t AD9208_jesd_set_if_config(void *spi, AD9208_t *hh, jesd_param_t jesd_param, uint64_t *lane_rate_kbps);
uint8_t ad9208_jesd_get_cfg_param(void *spi, jesd_param_t *jesd_param);
uint8_t get_jesd_serdes_vco_cfg(uint64_t slr_mbps, uint8_t *vco_cfg);
uint8_t AD9208_jesd_subclass_set(void *spi, uint8_t subclass);
uint8_t AD9208_jesd_enable_scrambler(void *spi, uint8_t en);
uint8_t AD9208_jesd_enable_link(void *spi, uint8_t en);
uint8_t ad9208_jesd_set_lane_xbar(void *spi, uint8_t logical_lane, uint8_t physical_lane);
uint8_t ad9208_jesd_get_lane_xbar(void *spi, uint8_t *phy_log_map);
uint8_t AD9208_jesd_get_pll_status(void *spi, uint8_t *pll_status);
uint8_t ad9208_jesd_sysref_timestamp_set(void *spi, uint8_t timestamp_en, uint8_t control_bit, uint8_t delay);


uint8_t AD9208_adc_set_channel_select(void *spi, uint8_t ch);
uint8_t AD9208_adc_get_channel_select(void *spi);
uint8_t AD9208_adc_set_ch_pdn_mode(void *spi, AD9208_pdn_mode_t mode);
uint64_t ad9208_get_adc_clk_freq(AD9208_t *AD, uint64_t adc_clk_freq_hz);
uint8_t AD9208_adc_set_input_cfg(void *spi, signal_coupling_t analog_input_mode, uint8_t ext_vref, ad9208_adc_scale_range_t full_scale_range);
uint8_t AD9208_adc_set_vcm_export(void *spi, uint8_t en);
uint8_t AD9208_adc_set_input_scale(void *spi, ad9208_adc_scale_range_t full_scale_range);
uint8_t AD9208_adc_set_input_buffer_cfg(void *spi, ad9208_adc_buff_curr_t buff_curr_n, ad9208_adc_buff_curr_t buff_curr_p, ad9208_adc_buff_curr_t vcm_buff);
uint8_t AD9208_check_buffer_current(ad9208_adc_buff_curr_t buff_curr);
uint8_t AD9208_adc_set_fc_ch_mode(void *spi, uint8_t fc_ch);
uint8_t ad9208_adc_set_dc_offset_filt_en(void *spi, uint8_t en);
uint8_t AD9208_adc_set_dcm_mode(void *spi, uint8_t dcm);
uint8_t AD9208_adc_set_data_format(void *spi, ad9208_adc_data_frmt_t ip_data_frmt, ad9208_adc_data_frmt_t op_data_frmt);
uint8_t AD9208_adc_set_ddc_gain(void *spi, uint8_t ddc_ch, uint8_t gain_db);
uint8_t ad9208_adc_set_sync_update_mode_enable(void *spi, uint8_t en);
uint8_t ad9208_adc_set_ddc_ip_mux(void *spi, uint8_t ddc_ch, AD9208_adc_ch_t i_data_src, AD9208_adc_ch_t q_data_src);
uint8_t AD9208_adc_set_ddc_dcm(void *spi, uint8_t ddc_ch, uint8_t dcm);
uint8_t AD9208_adc_set_ddc_nco_mode(void *spi, uint8_t ddc_ch, ad9208_adc_nco_mode_t mode);
uint8_t AD9208_adc_set_ddc_nco(void *spi, AD9208_t *AD, uint8_t ddc_ch, const int64_t carrier_freq_hz);
int my_ad9208_adc_set_ddc_nco(void *spi, AD9208_t *AD, uint8_t ddc_ch,const int64_t carrier_freq_hz);
int adi_api_utils_gcd(int u, int v);
void adi_api_utils_mult_128(uint64_t a, uint64_t b, uint64_t *hi,uint64_t *lo);
uint64_t do_div(uint64_t n, uint64_t base);
uint64_t div64_u64_rem(uint64_t dividend, uint64_t divisor, uint64_t *remainder);
void adi_api_utils_mod_128(uint64_t ah, uint64_t al, uint64_t div,uint64_t *mod);
uint8_t AD9208_adc_set_ddc_nco_ftw(void *spi, uint8_t ddc_ch, uint64_t ftw, uint64_t mod_a, uint64_t mod_b);
uint8_t ad9208_adc_set_ddc_nco_reset(void *spi);
uint8_t ad9208_adc_set_clk_phase(void *spi, uint8_t phase_adj);
uint8_t AD9208_adc_set_ddc_nco_phase(void *spi, uint8_t ddc_ch, uint64_t po);
uint8_t ad9208_adc_get_overange_status(void *spi, uint8_t status);
uint8_t ad9208_adc_set_fd_thresholds(void *spi, uint16_t upper_dbfs, uint16_t lower_dbfs, uint16_t dwell_time);
uint8_t ad9208_adc_temp_sensor_set_enable(void *spi, uint8_t en);



uint8_t AD9208_get_chip_id(void *spi, adi_chip_id_t *chip_id);
void AD9208_reset(void *spi);
uint8_t AD9208_set_pdn_pin_mode(void *spi, uint8_t pin_enable, AD9208_pdn_mode_t pin_mode);
void spi_configure(void *spi);
uint8_t AD9208_set_input_clk_cfg(void *spi, AD9208_t *AD, uint64_t clk_freq_hz, uint8_t div);
uint8_t AD9208_set_input_clk_duty_cycle_stabilizer(void *spi, uint8_t en);
uint8_t AD9208_get_decimation_cfg(uint8_t dcm, uint8_t *cfg);
uint8_t AD9208_get_dec_filter_cfg(ad9208_adc_data_frmt_t op_data_format, uint8_t dcm_rate, uint8_t *filt_sel_val_0, uint8_t *filt_sel_val_1);
uint8_t AD9208_is_sync_spi_update_enabled(void *spi, uint8_t *enabled);
uint8_t AD9208_register_chip_transfer(void *spi);
void AD9208_regs_set(AD9208_regs_t *regs, const int addr, const int value);
uint8_t AD9208_get_decimation(void *spi, uint8_t *dcm);

int32_t ad9208_setup(void *spi, int8_t setting_type);
int32_t ad9208_onechannel_setup(void *spi);
int32_t FTW_send(void *spi, bool ftw_freq_sel);
void AD9208_reg_setup(void *spi);
uint8_t ad9208_testmode_set(void *spi, unsigned int chan, unsigned int mode);
int8_t choose_config_ADC(uint8_t type_config, AD9208_t *AD);
int8_t setting_DDC(void *spi,uint8_t type_config, AD9208_t *AD);
