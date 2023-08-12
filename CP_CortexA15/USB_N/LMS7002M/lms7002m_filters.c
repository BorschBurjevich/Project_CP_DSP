#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include <USB_N/LMS7002M/LMS7002M_logger.h>
#include <USB_N/LMS7002M/LMS7002M.h>
#include "delay/delay.h"

#define LMS7002M_Rx 0
#define LMS7002M_Tx 1

#define VERBOSE 1

#define MSB_LSB(m, l) (m << 4 | l)

uint16_t RSSIDelayCounter = 1; // MCU timer delay between RSSI measurements
enum
{
    SEARCH_SUCCESS = 0,
    SEARCH_NEED_TO_DECREASE,
    SEARCH_NEED_TO_INCREASE
};

enum VCO_ID
{
    VCO_CGEN = 0,
    VCO_SXR,
    VCO_SXT
};

enum MCU_ERROR_CODES
{
    MCU_NO_ERROR = 0,
    MCU_ERROR,
    MCU_CGEN_TUNE_FAILED,
    MCU_SXR_TUNE_FAILED,
    MCU_SXT_TUNE_FAILED,
    MCU_LOOPBACK_SIGNAL_WEAK,
    MCU_INVALID_RX_PATH,
    MCU_INVALID_TX_BAND,
    MCU_RX_LPF_OUT_OF_RANGE,
    MCU_RX_INVALID_TIA,
    MCU_TX_LPF_OUT_OF_RANGE,
    MCU_PROCEDURE_DISABLED,
    MCU_R_CTL_LPF_LIMIT_REACHED,
    MCU_CFB_TIA_RFE_LIMIT_REACHED,
    MCU_RCAL_LPF_LIMIT_REACHED,

    MCU_ERROR_CODES_COUNT
};

typedef struct
{
    const uint16_t* addr;
    const uint16_t* val;
    const uint16_t* mask;
    const uint8_t cnt;
    const uint16_t* wrOnlyAddr;
    const uint16_t* wrOnlyData;
    const uint8_t wrOnlyAddrCnt;
    const uint8_t wrOnlyDataCnt;
} RegisterBatch;

uint8_t SetFrequencyCGEN(LMS7002M_t *self, float freq);
extern void Modify_SPI_Reg_bits (LMS7002M_t *self, unsigned short SPI_reg_addr, unsigned char MSB_bit, unsigned char LSB_bit, unsigned short new_bits_data);
extern unsigned short Get_SPI_Reg_bits (LMS7002M_t *self, unsigned short SPI_reg_addr, unsigned char MSB_bit, unsigned char LSB_bit);
float GetFrequencyCGEN(LMS7002M_t *self);
extern double GetRate (LMS7002M_t *self, bool tx, double rf_rate_Hz);
float RefClk = 20.00e6; //board reference clock
uint32_t GetRSSI(LMS7002M_t *self);

uint16_t pow2(const uint8_t power)
{
    return 1 << power;
}
//rx lpf range limits
static const float RxLPF_RF_LimitLow = 0.100e6;
static const float RxLPF_RF_LimitHigh = 130e6;

//tx lpf range limits
static const float TxLPF_RF_LimitLow = 5e6;
static const float TxLPF_RF_LimitLowMid = 40e6;
static const float TxLPF_RF_LimitMidHigh = 50e6;
static const float TxLPF_RF_LimitHigh = 130e6;

float GetReferenceClk_TSP_MHz(LMS7002M_t *self, bool tx)
{
    const float cgenFreq = GetFrequencyCGEN(self);
    const float clklfreq = cgenFreq/pow2(Get_SPI_Reg_bits(self, 0x0089, 12, 11));
    if(Get_SPI_Reg_bits(self, 0x0086, 11, 11) == 0)
        return tx ? clklfreq : cgenFreq/4;
    else
        return tx ? cgenFreq : clklfreq/4;
}
static int16_t clamp(int16_t value, int16_t minBound, int16_t maxBound)
{
    if(value < minBound)
        return minBound;
    if(value > maxBound)
        return maxBound;
    return value;
}

uint8_t GetValueOf_c_ctl_pga_rbb(uint8_t g_pga_rbb)
{
    if(g_pga_rbb < 21)
        return 1;
    if(g_pga_rbb < 13)
        return 2;
    if(g_pga_rbb < 8)
        return 3;
    return 0;
}

void UpdateRSSIDelay(LMS7002M_t *self)
{
    const uint16_t sampleCount = (2 << 7) << Get_SPI_Reg_bits(self, 0x040A, 2, 0);
    uint8_t decimation = Get_SPI_Reg_bits(self, 0x0403, 14, 12);
    if(decimation < 6)
        decimation = (2 << decimation);
    else
        decimation = 1; //bypass
    {
    float waitTime = sampleCount/((GetReferenceClk_TSP_MHz(self, false)/2) / decimation );
    RSSIDelayCounter = (0xFFFF) - (uint16_t)(waitTime*RefClk/12);
    }
}

void SetDefaultsSX(LMS7002M_t *self)
{
    const uint16_t SXAddr[]=    {0x011C, 0x011D, 0x011E, 0x011F, 0x0121, 0x0122, 0x0123};
    const uint16_t SXdefVals[]= {0xAD43, 0x0400, 0x0780, 0x3640, 0x3404, 0x033F, 0x067B};

    uint8_t i;
    for(i=sizeof(SXAddr)/sizeof(uint16_t); i; --i)
        LMS7002M_spi_write(self, SXAddr[i-1], SXdefVals[i-1]);
    //keep 0x0120[7:0]ICT_VCO bias value intact
    Modify_SPI_Reg_bits(self, 0x0120, 15, 8, 0xB9FF);
}

static uint8_t ConfigCGEN_ForLPF_IF(LMS7002M_t *self, float IF_Hz)
{
    uint8_t cgenMultiplier = clamp(IF_Hz*20 / 20.0e6/*46.08e6*/ + 0.5, 2, 13);
    return SetFrequencyCGEN(self, 20.0e6/*46.08e6*/ * cgenMultiplier/* + 10e6*/);
}

void FlipRisingEdge(LMS7002M_t *self, const uint16_t addr, unsigned char MSB_bit, unsigned char LSB_bit)
{
    Modify_SPI_Reg_bits(self, addr, MSB_bit, LSB_bit, 0);
    Modify_SPI_Reg_bits(self, addr, MSB_bit, LSB_bit, 1);
}

void WriteMaskedRegs(LMS7002M_t *self, const RegisterBatch * regs)
{
    uint8_t i;
    uint8_t index;
    for(i=regs->cnt; i; --i)
    {
        index = i-1;
        LMS7002M_usb_write(regs->addr[index], ( LMS7002M_spi_read(self,regs->addr[index]) & ~regs->mask[index] ) | regs->val[index]);
       // LMS7002M_spi_write(self, regs->addr[index], ( LMS7002M_spi_read(self,regs->addr[index]) & ~regs->mask[index] ) | regs->val[index]);
    }
    for(i=regs->wrOnlyAddrCnt; i; --i)
    {
        index = i-1;
        LMS7002M_usb_write(regs->wrOnlyAddr[index], i > regs->wrOnlyDataCnt ? 0 : regs->wrOnlyData[index]);
       // LMS7002M_spi_write(self, regs->wrOnlyAddr[index], i > regs->wrOnlyDataCnt ? 0 : regs->wrOnlyData[index]);
    }
}

uint32_t GetRSSI(LMS7002M_t *self)
{
    uint32_t rssi;
//#ifdef __cplusplus
//    int waitTime = 1000000.0*(0xFFFF - RSSIDelayCounter)*12/RefClk;
//    std::this_thread::sleep_for(std::chrono::microseconds(waitTime));
//#else
//    TR0 = 0; //stop timer 0
//    TH0 = (RSSIDelayCounter >> 8);
//    TL0 = (RSSIDelayCounter & 0xFF);
//    TF0 = 0; // clear overflow
//    TR0 = 1; //start timer 0
//    while( !TF0 ); // wait for timer overflow
//#endif
    udelay(10);
    FlipRisingEdge(self, 0x0400, 15, 15);
    rssi = LMS7002M_spi_read(self, 0x040F);
    return (rssi << 2 | LMS7002M_spi_read(self, 0x040E) & 0x3);
}

static uint8_t ReadCMP(LMS7002M_t *self, const bool SX)
{
//#ifdef __cplusplus
//    std::this_thread::sleep_for(std::chrono::microseconds(100));
//#else
//    TR0 = 0; //stop timer 0
//    TH0 = (gComparatorDelayCounter >> 8);
//    TL0 = (gComparatorDelayCounter & 0xFF);
//    TF0 = 0; // clear overflow
//    TR0 = 1; //start timer 0
//    while( !TF0 ); // wait for timer overflow
//#endif
    return (uint8_t)Get_SPI_Reg_bits(self, SX ? 0x0123 : 0x008C, 13, 12);
}

void SetNCOFrequency(LMS7002M_t *self, const bool tx, const float freq, uint8_t index)
{
    const uint16_t addr = (tx ? 0x0242 : 0x0442)+index*2;
    const uint32_t fcw = (uint32_t)((freq/GetReferenceClk_TSP_MHz(self, tx))*4294967296.0);
    LMS7002M_spi_write(self, addr, (fcw >> 16)); //NCO frequency control word register MSB part.
    LMS7002M_spi_write(self,addr+1, fcw); //NCO frequency control word register LSB part.
 }

void LoadDC_REG_TX_IQ(LMS7002M_t *self)
{
    LMS7002M_spi_write(self, 0x020C, 0x7FFF);
    FlipRisingEdge(self, 0x0200, 5, 5);
    LMS7002M_spi_write(self, 0x020C, 0x8000);
    FlipRisingEdge(self, 0x0200, 6, 6);
}

uint8_t TuneVCO(LMS7002M_t *self, bool SX) // 0-cgen, 1-SXR, 2-SXT
{
    typedef struct
    {
        uint8_t high;
        uint8_t low;
        uint8_t hasLock;
    } CSWInteval;

    uint16_t addrCSW_VCO;
    uint8_t msb;
    uint8_t lsb;

    CSWInteval cswSearch[2];
    cswSearch[0].high = 0; //search interval lowest value
    cswSearch[0].low = 127;
    cswSearch[1].high = 128;
    cswSearch[1].low = 255;
    cswSearch[1].hasLock = cswSearch[0].hasLock = false;

    if(SX)
    {
        addrCSW_VCO = 0x0121;
        msb = 10;
        lsb = 3; //CSW msb lsb
        //assuming the active channel is already correct
        Modify_SPI_Reg_bits(self, 0x011C, 2, 1, 0); //activate VCO and comparator
    }
    else //CGEN
    {
        addrCSW_VCO = 0x008B;
        msb = 8;
        lsb = 1; //CSW msb lsb
        Modify_SPI_Reg_bits(self, 0x0086, 2, 1, 0); //activate VCO and comparator
    }

    //check if lock is within VCO range
    Modify_SPI_Reg_bits(self, addrCSW_VCO, msb, lsb, 0);
    if(ReadCMP(self, SX) == 3) //VCO too high
        return MCU_ERROR;
    Modify_SPI_Reg_bits(self, addrCSW_VCO, msb, lsb, 255);
    if(ReadCMP(self, SX) == 0) //VCO too low
        return MCU_ERROR;

    //search intervals [0-127][128-255]
    {
        uint8_t t;
        for(t=0; t<2; ++t)
        {
            uint8_t mask;
            for(mask = (1 << 6); mask; mask >>= 1)
            {
                uint8_t cmpValue;
                cswSearch[t].high |= mask; // CSW_VCO<i>=1
                Modify_SPI_Reg_bits(self, addrCSW_VCO, msb, lsb, cswSearch[t].high);
                cmpValue = ReadCMP(self, SX);
                if(cmpValue == 0x03) // reduce CSW
                    cswSearch[t].high ^= mask; // CSW_VCO<i>=0
                else if(cmpValue == 0x02 && cswSearch[t].high <= cswSearch[t].low)
                {
                    cswSearch[t].hasLock = true;
                    cswSearch[t].low = cswSearch[t].high;
                }
            }
            for(; cswSearch[t].low; --cswSearch[t].low)
            {
                Modify_SPI_Reg_bits(self, addrCSW_VCO, msb, lsb, cswSearch[t].low);
                if(ReadCMP(self, SX) != 0x2)
                {
                    if(cswSearch[t].low < cswSearch[t].high)
                        ++cswSearch[t].low;
                    break;
                }
            }
        }
    }
    {
        uint8_t cswValue;
        //compare which interval is wider
        {
            if(cswSearch[1].hasLock && (cswSearch[1].high-cswSearch[1].low >= cswSearch[0].high-cswSearch[0].low))
                cswValue = cswSearch[1].low +((cswSearch[1].high-cswSearch[1].low) >> 1);
            else
                cswValue = cswSearch[0].low +((cswSearch[0].high-cswSearch[0].low) >> 1);
        }

        Modify_SPI_Reg_bits(self, addrCSW_VCO, msb, lsb, cswValue);
        if(ReadCMP(self, SX) != 0x2) //just in case high-low==1, if low fails, check if high locks
            Modify_SPI_Reg_bits(self, addrCSW_VCO, msb, lsb, ++cswValue);
    }
    if(ReadCMP(self, SX) == 0x2)
        return MCU_NO_ERROR;
    return MCU_ERROR;
}
uint8_t SetFrequencySX(LMS7002M_t *self, const bool tx, const float freq_Hz)
{
    int16_t tuneScore[3] = {255, 255, 255}; // best is closest to 0
    const uint16_t macBck = LMS7002M_spi_read(self, 0x0020);
    bool canDeliverFrequency = false;

    Modify_SPI_Reg_bits(self, 0x0020, 1, 0, tx?2:1);
    //find required VCO frequency
    {
        float VCOfreq;
        float temp;
        {
            uint8_t div_loch;
            for (div_loch = 7; div_loch; --div_loch)
            {
                VCOfreq = pow2(div_loch) * freq_Hz;
                if ((VCOfreq >= 3800e6) && (VCOfreq <= 7714e6))
                    break;
            }
            Modify_SPI_Reg_bits(self, 0x011F, 8, 6, div_loch-1);
        }
        {
            uint32_t fractionalPart;
            const uint8_t enDiv2 = (VCOfreq > 5500e6) ? 1 : 0;
            Modify_SPI_Reg_bits(self, 0x011C, 10, 10, enDiv2); //EN_DIV2_DIVPROG
            temp = VCOfreq / (RefClk * (1 + enDiv2));
            fractionalPart = (uint32_t)((temp - (uint32_t)(temp)) * 1048576);

            Modify_SPI_Reg_bits(self, 0x011E, 13, 4, (uint16_t)(temp - 4)); //INT_SDM
            Modify_SPI_Reg_bits(self, 0x011E, 3, 0, (fractionalPart >> 16)); //FRAC_SDM[19:16]
            LMS7002M_spi_write(self, 0x011D, fractionalPart & 0xFFFF); //FRAC_SDM[15:0]
        }
    }
    while(1)
    {
        uint8_t sel_vco;
        uint8_t bestvco = 0;
        for (sel_vco = 0; sel_vco < 3; ++sel_vco)
        {
            Modify_SPI_Reg_bits(self, 0x0121, 2, 1, sel_vco);
            if( TuneVCO(self, 1) == MCU_NO_ERROR)
            {
                tuneScore[sel_vco] = Get_SPI_Reg_bits(self, 0x0121, 10, 3)-128;
                canDeliverFrequency = true;
            }
            if(abs(tuneScore[sel_vco]) < abs(tuneScore[bestvco]))
                bestvco = sel_vco;
        }
        if(canDeliverFrequency)
        {
            Modify_SPI_Reg_bits(self, 0x0121, 2, 1, bestvco);
            Modify_SPI_Reg_bits(self, 0x0121, 10, 3, tuneScore[bestvco] + 128);
            break;
        }
        {
            uint16_t bias = Get_SPI_Reg_bits(self, 0x0120, 7, 0);
            if(bias == 255)
                break;
            Modify_SPI_Reg_bits(self, 0x0120, 7, 0, bias + 32 > 255 ? 255 : bias + 32);
        }
    }
    LMS7002M_spi_write(self, 0x0020, macBck);
    if (canDeliverFrequency == false)
        return tx ? MCU_SXT_TUNE_FAILED : MCU_SXR_TUNE_FAILED;
    return MCU_NO_ERROR;
}

static uint8_t RxFilterSearch(LMS7002M_t *self, const uint16_t addr, const uint8_t MSB_bit, const uint8_t LSB_bit, const uint16_t rssi_3dB, const uint16_t stepLimit)
{
    const bool doDecrement = GetRSSI(self) < rssi_3dB;
    int16_t value = Get_SPI_Reg_bits(self, addr, MSB_bit, LSB_bit);
    uint8_t msblsb = MSB_LSB(MSB_bit, LSB_bit);
    const uint16_t maxValue = pow2((msblsb>>4)-(msblsb&0xF)+1)-1;
    uint16_t stepSize = 1;
    while(1)
    {
        stepSize <<= 1;
        if(doDecrement)
            value -= stepSize;
        else
            value += stepSize;
        value = clamp(value, 0, maxValue);
        Modify_SPI_Reg_bits(self, addr, MSB_bit, LSB_bit , value);
        if(doDecrement != (GetRSSI(self) < rssi_3dB))
            break;
        if(stepSize >= stepLimit)
            return doDecrement ? SEARCH_NEED_TO_INCREASE : SEARCH_NEED_TO_DECREASE;
    }
    while(stepSize > 1)
    {
        stepSize /= 2;
        if(GetRSSI(self) >= rssi_3dB)
            value += stepSize;
        else
            value -= stepSize;
        value = clamp(value, 0, maxValue);
        Modify_SPI_Reg_bits(self, addr, MSB_bit, LSB_bit, value);
    }
    return 0;
}

void EnableMIMOBuffersIfNecessary(LMS7002M_t *self)
{
//modifications when calibrating channel B
    uint16_t x0020val = LMS7002M_spi_read(self, 0x0020);
    if( (x0020val&0x3) == 2)
    {
        Modify_SPI_Reg_bits(self, 0x0020, 1, 0, 1);
        Modify_SPI_Reg_bits(self, 0x010D, 0, 0, 1);
        Modify_SPI_Reg_bits(self, 0x0100, 14, 14, 1);
        LMS7002M_spi_write(self, 0x0020, x0020val);
    }
}
void EnableChannelPowerControls(LMS7002M_t *self)
{
    uint16_t afe = LMS7002M_spi_read(self, 0x0082);
    uint16_t value = LMS7002M_spi_read(self, 0x0020);
    if((value & 3) == 1)
    {
        value = value | 0x0014;
        afe &= ~0x14;
    }
    else
    {
        value = value | 0x0028;
        afe &= ~0x0A;
    }
    LMS7002M_spi_write(self, 0x0020, value);
    LMS7002M_spi_write(self, 0x0082, afe);
}
uint8_t TuneRxFilterSetup(LMS7002M_t *self, const float rx_lpf_IF)
{
    uint8_t status;
    const uint16_t ch = LMS7002M_spi_read(self, 0x0020);
    uint8_t g_tia_rfe = Get_SPI_Reg_bits(self, 0x0113, 1, 0);
    uint8_t g_pga_rbb = Get_SPI_Reg_bits(self, 0x0119, 4, 0);

    if(RxLPF_RF_LimitLow/2 > rx_lpf_IF || rx_lpf_IF > RxLPF_RF_LimitHigh/2)
        return MCU_RX_LPF_OUT_OF_RANGE;
#define BATCH_RX_SETUP 1
#if BATCH_RX_SETUP
    {
        const uint16_t RxFilterSetupAddr[] = {0x0085,0x010D,0x0113,0x0114, 0x0084, 0x008B};
        const uint16_t RxFilterSetupData[] = {0x0001,0x0100,0x0004,0x0010, 0x0400, 0x2100};
        const uint16_t RxFilterSetupMask[] = {0x0007,0x0188,0x003C,0x001F, 0xF83F, 0xC1FF};
        const uint16_t RxFilterSetupWrOnlyAddr[] = {0x0082,0x0086,0x0087,0x0088,0x0089,0x008A,0x008C,0x0100,0x0101,0x0102,0x0103,0x0104,0x0105,0x0106,0x0107,0x0108,0x0109,0x010A,0x010C,0x0115,0x0116,0x0117,0x0118,0x0119,0x011A,0x0200,0x0201,0x0202,0x0203,0x0204,0x0205,0x0206,0x0207,0x0208,0x0209,0x0240,0x0400,0x0401,0x0402,0x0403,0x0404,0x0405,0x0406,0x0407,0x0408,0x0409,0x040A,0x040C,0x0440, 0x0081};
        const uint16_t RxFilterSetupWrOnlyData[] = {0x8003,0x4901,0x0400,0x0780,0x0020,0x0514,0x067B,0x3409,0x6001,0x3180,0x0612,0x0088,0x0007,0x318C,0x318C,0x0426,0x61C1,0x104C,0x88C5,0x0009,0x8180,0x280C,0x018C,0x528B,0x2E02,0x008D,0x07FF,0x07FF,0x0000,0x0000,0x0000,0x0000,0x0000,0x2070,0x0000,0x0020,0x0081,0x07FF,0x07FF,0x7000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x1001,0x2038,0x0020};
        const RegisterBatch batch = {
            RxFilterSetupAddr, RxFilterSetupData, RxFilterSetupMask, sizeof(RxFilterSetupAddr)/sizeof(uint16_t),
            RxFilterSetupWrOnlyAddr, RxFilterSetupWrOnlyData, sizeof(RxFilterSetupWrOnlyAddr)/sizeof(uint16_t), sizeof(RxFilterSetupWrOnlyData)/sizeof(uint16_t)};
        WriteMaskedRegs(self, &batch);
    }
#else
    const uint8_t ict_vco = Get_SPI_Reg_bits(ICT_VCO_CGEN);
    BeginBatch("RxFilterSetup");

    SetDefaults(SECTION_RFE);
    Modify_SPI_Reg_bits(SEL_PATH_RFE, 2);
    Modify_SPI_Reg_bits(G_RXLOOPB_RFE, 1);
    Modify_SPI_Reg_bits(PD_RLOOPB_2_RFE, 0);
    Modify_SPI_Reg_bits(EN_INSHSW_LB2_RFE, 0);
    Modify_SPI_Reg_bits(PD_MXLOBUF_RFE, 0);
    Modify_SPI_Reg_bits(PD_QGEN_RFE, 0);
    Modify_SPI_Reg_bits(RFB_TIA_RFE, 16);

    //RBB
    SetDefaults(SECTION_RBB);
    Modify_SPI_Reg_bits(ICT_PGA_OUT_RBB, 20);
    Modify_SPI_Reg_bits(ICT_PGA_IN_RBB, 20);

    //TRF
    SetDefaults(SECTION_TRF);
    Modify_SPI_Reg_bits(L_LOOPB_TXPAD_TRF, 0);
    Modify_SPI_Reg_bits(EN_LOOPB_TXPAD_TRF, 1);
    Modify_SPI_Reg_bits(SEL_BAND1_TRF, 0);
    Modify_SPI_Reg_bits(SEL_BAND2_TRF, 1);

    //TBB
    SetDefaults(SECTION_TBB);
    Modify_SPI_Reg_bits(CG_IAMP_TBB, 1);
    Modify_SPI_Reg_bits(ICT_IAMP_FRP_TBB, 1);
    Modify_SPI_Reg_bits(ICT_IAMP_GG_FRP_TBB, 6);

    //AFE
    SetDefaults(SECTION_AFE);
    Modify_SPI_Reg_bits(PD_RX_AFE2, 0);

    //LDO
    //do nothing
    //XBUF
    Modify_SPI_Reg_bits(PD_XBUF_RX, 0);
    Modify_SPI_Reg_bits(PD_XBUF_TX, 0);
    Modify_SPI_Reg_bits(EN_G_XBUF, 1);

    //TxTSP
    SetDefaults(SECTION_TxTSP);
    SetDefaults(SECTION_TxNCO);
    Modify_SPI_Reg_bits(TSGMODE_TXTSP, 1);
    Modify_SPI_Reg_bits(INSEL_TXTSP, 1);
    Modify_SPI_Reg_bits(CMIX_SC_TXTSP, 1);
    Modify_SPI_Reg_bits(GFIR3_BYP_TXTSP, 1);
    Modify_SPI_Reg_bits(GFIR2_BYP_TXTSP, 1);
    Modify_SPI_Reg_bits(GFIR1_BYP_TXTSP, 1);

    //RxTSP
    SetDefaults(SECTION_RxTSP);
    SetDefaults(SECTION_RxNCO);
    Modify_SPI_Reg_bits(AGC_MODE_RXTSP, 1);
    Modify_SPI_Reg_bits(CMIX_BYP_RXTSP, 0);
    Modify_SPI_Reg_bits(GFIR3_BYP_RXTSP, 1);
    Modify_SPI_Reg_bits(GFIR2_BYP_RXTSP, 1);
    Modify_SPI_Reg_bits(GFIR1_BYP_RXTSP, 1);
    Modify_SPI_Reg_bits(AGC_AVG_RXTSP, 1);
    Modify_SPI_Reg_bits(HBD_OVR_RXTSP, 4);
    Modify_SPI_Reg_bits(CMIX_GAIN_RXTSP, 0);
    Modify_SPI_Reg_bits(CMIX_SC_RXTSP, 1);

    //CGEN
    SetDefaults(SECTION_CGEN);
    Modify_SPI_Reg_bits(ICT_VCO_CGEN, ict_vco);
    EndBatch();

    //BIAS
/*
    {
        const uint8_t rp_calib_bias = Get_SPI_Reg_bits(RP_CALIB_BIAS);
        SetDefaults(SECTION_BIAS);
        Modify_SPI_Reg_bits(RP_CALIB_BIAS, rp_calib_bias);
    }
*/
#endif
    Modify_SPI_Reg_bits(self, 0x0113, 1, 0, g_tia_rfe);
    if(g_pga_rbb == 31)
        Modify_SPI_Reg_bits(self, 0x0119, 4, 0, 22);
    else
        Modify_SPI_Reg_bits(self, 0x0119, 4, 0, g_pga_rbb);

    status = ConfigCGEN_ForLPF_IF(self, rx_lpf_IF);
    if(status != MCU_NO_ERROR)
        return status;

    double Clk_TSP = GetReferenceClk_TSP_MHz(self, 0);
    printf("%d - Actual Clk_TSP freq %f MHz\n", 0, Clk_TSP/1e6);

    double sampleRate = GetRate (self, 0, 20.0e6);
    printf("%d - sampleRate %f MHz\n", 0, sampleRate/1e6);

    //SXR
    Modify_SPI_Reg_bits(self, 0x0020, 1, 0, 1);
//    SetDefaultsSX(self);
    status = SetFrequencySX(self, LMS7002M_Rx, 105.7e6/*539.9e6*/);
    if(status != MCU_NO_ERROR)
        return status;

    //SXT
    Modify_SPI_Reg_bits(self, 0x0020, 1, 0, 2);
//    SetDefaultsSX(self);
    status = SetFrequencySX(self, LMS7002M_Tx, 105.7e6/*550e6*/);
    if(status != MCU_NO_ERROR)
        return status;

    LMS7002M_spi_write(self, 0x0020, ch);
    //LimeLight & PAD
    //do nothing

    if(rx_lpf_IF <= 54e6)
    {
        LMS7002M_spi_write(self, 0x0112, 1); //CFB_TIA_RFE=1, CCOMP_TIA_RFE=0
        Modify_SPI_Reg_bits(self, 0x0114, 8, 5, 15);
    }
    else
    {
        int16_t cfb_tia_rfe;
        int8_t ccomp_tia_rfe;
        if(g_tia_rfe > 1)
        {
            cfb_tia_rfe = (int16_t)( 1680e6/rx_lpf_IF - 10);
            ccomp_tia_rfe = cfb_tia_rfe/100;
        }
        else if(g_tia_rfe == 1)
        {
            cfb_tia_rfe = (int16_t)( 5400e6/rx_lpf_IF - 15);
            ccomp_tia_rfe = cfb_tia_rfe/100 + 1;
        }
        else
            return MCU_RX_INVALID_TIA;
        LMS7002M_spi_write(self, 0x0112, (clamp(ccomp_tia_rfe, 0, 15)<<8) | clamp(cfb_tia_rfe, 0, 4095));

        Modify_SPI_Reg_bits(self, 0x0114, 8, 5, clamp(15-cfb_tia_rfe/100, 0, 15));
    }
    {
        const int8_t rcc_ctl_pga_rbb = (430 * pow(0.65, g_pga_rbb/10) - 110.35)/20.45 + 16;
        LMS7002M_spi_write(self, 0x011A, rcc_ctl_pga_rbb<<9 | GetValueOf_c_ctl_pga_rbb(g_pga_rbb));
    }

    if(rx_lpf_IF < 18e6)
    {
        Modify_SPI_Reg_bits(self, 0x0115, 3, 2, 2); //PD_LPFL_RBB=0, PD_LPFH_RBB=1
        Modify_SPI_Reg_bits(self, 0x0118, 15, 13, 0);
        {
            const float freqIF = rx_lpf_IF*1.3;
            int16_t c_ctl_lpfl_rbb = clamp(2160e6/freqIF - 103, 0, 2047);

            uint8_t rcc_ctl_lpfl_rbb = 5;
            if(freqIF < 15e6)
                rcc_ctl_lpfl_rbb = 4;
            if(freqIF < 10e6)
                rcc_ctl_lpfl_rbb = 3;
            if(freqIF < 5e6)
                rcc_ctl_lpfl_rbb = 2;
            if(freqIF < 3e6)
                rcc_ctl_lpfl_rbb = 1;
            if(freqIF < 1.4e6)
                rcc_ctl_lpfl_rbb = 0;
            LMS7002M_spi_write(self, 0x0117, rcc_ctl_lpfl_rbb<<11 | c_ctl_lpfl_rbb);
        }
    }
    else if(rx_lpf_IF <= 54e6)
    {
        Modify_SPI_Reg_bits(self, 0x0115, 3, 2, 1); //PD_LPFL_RBB=1, PD_LPFH_RBB=0
        Modify_SPI_Reg_bits(self, 0x0118, 15, 13, 1);
        {
            const float lpfIF_adjusted = rx_lpf_IF*1.3;
            uint8_t c_ctl_lpfh_rbb = clamp( 6000e6/lpfIF_adjusted - 50, 0, 255);
            uint8_t rcc_ctl_lpfh_rbb = clamp( lpfIF_adjusted/10e6 - 3, 0, 7);
            Modify_SPI_Reg_bits(self, 0x0116, 10, 0, (rcc_ctl_lpfh_rbb<<8) | c_ctl_lpfh_rbb);
        }
    }
    else // rx_lpf_IF > 54e6
    {
        Modify_SPI_Reg_bits(self, 0x0115, 3, 2, 3); //PD_LPFL_RBB=1, PD_LPFH_RBB=1
        Modify_SPI_Reg_bits(self, 0x0118, 15, 13, 2);
    }

//    EnableMIMOBuffersIfNecessary(self);
//    EnableChannelPowerControls(self);
    return MCU_NO_ERROR;
}

typedef struct
{
    uint16_t addr;
    uint8_t msb;
    uint8_t lsb;
    int8_t step;
    uint8_t limit;
    uint8_t rssiShouldIncrease;
} ReachRSSIparams;

static bool ChangeUntilReachRSSI(LMS7002M_t *self, const ReachRSSIparams * param, uint16_t rssiTarget)
{
    uint16_t rssi;
    int8_t gain = Get_SPI_Reg_bits(self, param->addr, param->msb, param->lsb);
    while(((rssi = GetRSSI(self)) < rssiTarget) == param->rssiShouldIncrease)
    {
        gain += param->step;
        if(param->step > 0 && gain > param->limit)
            break;
        else if(param->step < 0 && gain < param->limit)
            break;
        Modify_SPI_Reg_bits(self, param->addr, param->msb, param->lsb, gain);
    }
    return (rssi > rssiTarget) == param->rssiShouldIncrease;
}

uint8_t TuneRxFilter(LMS7002M_t *self,const float rx_lpf_freq_RF)
{
    uint16_t rssi_3dB ;
    uint8_t status = 0;
    //calculate intermediate frequency
    float rx_lpf_IF;
    if(Get_SPI_Reg_bits(self, 0x0113, 1, 0) == 1 && rx_lpf_freq_RF<4e6)
        rx_lpf_IF = 2e6;
    else
        rx_lpf_IF = rx_lpf_freq_RF/2;
//    SaveChipState(0);

    status = TuneRxFilterSetup(self, rx_lpf_IF);
    if(status != MCU_NO_ERROR)
        goto RxFilterSearchEndStage;
    UpdateRSSIDelay(self);
    {
    const ReachRSSIparams paramTX = {0x0108, 15, 10, 2, 30, true};
    const ReachRSSIparams paramRX = {0x0113, 5, 2, 2, 14, true};
    ChangeUntilReachRSSI(self, &paramRX, 0x4700);
    ChangeUntilReachRSSI(self, &paramTX, 0x2700);
    }

    rssi_3dB = GetRSSI(self) * 0.7071 * pow(10, (-0.0018 * rx_lpf_IF/1e6)/20);

    if(rx_lpf_IF <= 54e6)
    {
        uint16_t Caddr, Crange;
        uint8_t MSB, LSB;
        int8_t Rstep;

        status = SetFrequencySX(self, LMS7002M_Rx, 105.7e6);

        if(status != MCU_NO_ERROR)
            goto RxFilterSearchEndStage;

        if(rx_lpf_IF < 18e6) //LPFL
        {
            Caddr = 0x0117; //C_CTL_LPFL_RBB
            MSB = 10;
            LSB = 0;
            Rstep = 2;
            Crange = 2048;
        }
        else //LPFH
        {
            Caddr = 0x0116; //C_CTL_LPFH_RBB
            MSB = 7;
            LSB = 0;
            Rstep = 1;
            Crange = 256;
        }
        { // Find RC
        uint8_t searchStatus;
        int8_t r_ctl_lpf = Get_SPI_Reg_bits(self, 0x0116, 15, 11);
        while((searchStatus = RxFilterSearch(self, Caddr, MSB, LSB, rssi_3dB, Crange)) != SEARCH_SUCCESS)
        {
            r_ctl_lpf += (searchStatus == SEARCH_NEED_TO_INCREASE) ? Rstep : -Rstep;
            if(r_ctl_lpf < 0 || 31 < r_ctl_lpf)
                break;
            Modify_SPI_Reg_bits(self, 0x0116, 15, 11, r_ctl_lpf);
        }
        if(searchStatus != SEARCH_SUCCESS)
        {
            status = MCU_R_CTL_LPF_LIMIT_REACHED;
            goto RxFilterSearchEndStage;
        }
        }

        status = SetFrequencySX(self, LMS7002M_Rx, 105.7e6);
        if(status != MCU_NO_ERROR)
            goto RxFilterSearchEndStage;

        {
            uint16_t cfb_tia_rfe;
            uint8_t g_tia_rfe = Get_SPI_Reg_bits(self, 0x0113, 1, 0);
            if(g_tia_rfe == 3 || g_tia_rfe == 2)
                cfb_tia_rfe = (int)( 1680e6 / (rx_lpf_IF * 0.72) - 10);
            else if(g_tia_rfe == 1)
                cfb_tia_rfe = (int)( 5400e6 / (rx_lpf_IF * 0.72) - 15);
            else
            {
                status = MCU_RX_INVALID_TIA;
                goto RxFilterSearchEndStage;
            }
            cfb_tia_rfe = clamp(cfb_tia_rfe, 0, 4095);
            Modify_SPI_Reg_bits(self, 0x0112, 11, 0, cfb_tia_rfe);

            {
                uint8_t ccomp_tia_rfe = cfb_tia_rfe / 100;
                if(g_tia_rfe == 1)
                    ccomp_tia_rfe += 1;
                Modify_SPI_Reg_bits(self, 0x0112, 15, 12, clamp(ccomp_tia_rfe, 0, 15));
            }
            Modify_SPI_Reg_bits(self, 0x0114, 8, 5, clamp(15 - cfb_tia_rfe/100, 0, 15));
        }
    }
    else //if(rx_lpf_IF > 54e6)
    {
        status = SetFrequencySX(self, LMS7002M_Rx, 105.7e6/*539.9e6*/ - rx_lpf_IF);
        if(status != 0)
            goto RxFilterSearchEndStage;
    }
    //START TIA
    if(RxFilterSearch(self, 0x0112, 11, 0, rssi_3dB, 2048) != SEARCH_SUCCESS)
    {
        status = MCU_CFB_TIA_RFE_LIMIT_REACHED;
        goto RxFilterSearchEndStage;
    }
    //END TIA

    {
    //Restore settings
    uint16_t ccomp_cfb_tia_rfe = LMS7002M_spi_read(self, 0x0112);
    uint16_t rcc_c_ctl_lpfl_rbb = LMS7002M_spi_read(self, 0x0117);
    uint16_t rcc_c_ctl_pga_rbb = LMS7002M_spi_read(self, 0x011A);
    uint16_t rcc_c_ctl_lpfh_rbb = LMS7002M_spi_read(self, 0x0116) & 0x07FF;
    uint8_t pd_lpfhl = Get_SPI_Reg_bits(self, 0x0115, 3, 2);
    uint8_t input_ctl_pga_rbb = Get_SPI_Reg_bits(self, 0x0118, 15, 13);
    uint8_t rcomp_tia_rfe = Get_SPI_Reg_bits(self, 0x0114, 8, 5);
RxFilterSearchEndStage:
//    SaveChipState(1);
    if(status != MCU_NO_ERROR)
        return status;
    LMS7002M_spi_write(self, 0x0112, ccomp_cfb_tia_rfe);
    LMS7002M_spi_write(self, 0x0117, rcc_c_ctl_lpfl_rbb);
    LMS7002M_spi_write(self, 0x011A, rcc_c_ctl_pga_rbb);
    LMS7002M_spi_write(self, 0x0116, (16 << 11) | rcc_c_ctl_lpfh_rbb);
    LMS7002M_spi_write(self, 0x0118, input_ctl_pga_rbb << 13 | 0x018C);
    LMS7002M_spi_write(self, 0x0114, rcomp_tia_rfe << 5 | 16);
    Modify_SPI_Reg_bits(self, 0x0119, 14, 5, (20 << 5) | 20);
    Modify_SPI_Reg_bits(self, 0x0115, 3, 2, pd_lpfhl);
    }
    return MCU_NO_ERROR;
}

static void SetTxLPF_PDs(LMS7002M_t *self, float lpf_IF)
{
    uint16_t filterPDs = LMS7002M_spi_read(self, 0x0105) & ~0x0016;
    if(lpf_IF <= TxLPF_RF_LimitLowMid/2)
    {
        filterPDs |= 0x10;
        Modify_SPI_Reg_bits(self, 0x010B, 0, 0, 1);
    }
    else
        filterPDs |= 0x06;
    LMS7002M_spi_write(self,0x0105, filterPDs);
}

uint8_t TuneTxFilterSetup(LMS7002M_t *self, const float tx_lpf_IF)
{
    uint8_t status;
    const uint16_t reg0020 = LMS7002M_spi_read(self, 0x0020);

#define BATCH_TX_SETUP 1
#if BATCH_TX_SETUP
    {
        const uint16_t TxFilterSetupAddr[] = {0x0082,0x0085,0x0100,0x010C,0x010D, 0x0084, 0x008B};
        const uint16_t TxFilterSetupData[] = {0x8001,0x0001,0x0000,0x0000,0x001E, 0x0400, 0x2100};
        const uint16_t TxFilterSetupMask[] = {0x1FFF,0x0007,0x0001,0x0001,0x001E, 0xF83F, 0xC1FF};
        const uint16_t TxFilterSetupWrOnlyAddr[] = {0x0086,0x0087,0x0088,0x0089,0x008A,0x008C,0x0105,0x0106,0x0107,0x0108,0x0109,0x010A,0x0115,0x0116,0x0117,0x0118,0x0119,0x011A,0x0200,0x0201,0x0202,0x0203,0x0204,0x0205,0x0206,0x0207,0x0208,0x0240,0x0241,0x0400,0x0401,0x0402,0x0403,0x0404,0x0405,0x0406,0x0407,0x0408,0x0409,0x040A,0x040C,0x0440,0x0441,0x0081};
        const uint16_t TxFilterSetupWrOnlyData[] = {0x4901,0x0400,0x0780,0x0020,0x0514,0x067B,0x3007,0x318C,0x318C,0x058C,0x61C1,0x104C,0x000D,0x8180,0x280C,0x618C,0x528C,0x2E02,0x008D,0x07FF,0x07FF,0x0000,0x0000,0x0000,0x0000,0x0000,0x0070,0x0020,0x0000,0x0081,0x07FF,0x07FF,0x7000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x1001,0x2038,0x0020,0x0000};
        const RegisterBatch batch = {
            TxFilterSetupAddr, TxFilterSetupData, TxFilterSetupMask, sizeof(TxFilterSetupAddr)/sizeof(uint16_t),
            TxFilterSetupWrOnlyAddr, TxFilterSetupWrOnlyData, sizeof(TxFilterSetupWrOnlyAddr)/sizeof(uint16_t), sizeof(TxFilterSetupWrOnlyData)/sizeof(uint16_t)};
        WriteMaskedRegs(self, &batch);
    }
#else
    const uint8_t ict_vco = Get_SPI_Reg_bits(ICT_VCO_CGEN);
    BeginBatch("TxFilterSetup");
    //RFE
    Modify_SPI_Reg_bits(EN_G_RFE, 0);
    Modify_SPI_Reg_bits(0x010D, 4 << 4 | 1, 0xF);

    //RBB
    SetDefaults(SECTION_RBB);
    Modify_SPI_Reg_bits(PD_LPFL_RBB, 1);
    Modify_SPI_Reg_bits(INPUT_CTL_PGA_RBB, 3);
    Modify_SPI_Reg_bits(G_PGA_RBB, 12);
    Modify_SPI_Reg_bits(RCC_CTL_PGA_RBB, 23);

    //TRF
    Modify_SPI_Reg_bits(EN_G_TRF, 0);

    //TBB
    SetDefaults(SECTION_TBB);
    Modify_SPI_Reg_bits(CG_IAMP_TBB, 1);
    Modify_SPI_Reg_bits(LOOPB_TBB, 3);

    //AFE
    //if(reg0020 & 0x3 == 2)
    {
        Modify_SPI_Reg_bits(PD_RX_AFE2, 0);
        Modify_SPI_Reg_bits(PD_TX_AFE2, 0);
    {
    const uint8_t isel_dac_afe = Get_SPI_Reg_bits(ISEL_DAC_AFE);
    SetDefaults(SECTION_AFE);
    Modify_SPI_Reg_bits(ISEL_DAC_AFE, isel_dac_afe);
    }

    //XBUF
    Modify_SPI_Reg_bits(PD_XBUF_RX, 0);
    Modify_SPI_Reg_bits(PD_XBUF_TX, 0);
    Modify_SPI_Reg_bits(EN_G_XBUF, 1);

    SetDefaults(SECTION_CGEN);
    Modify_SPI_Reg_bits(ICT_VCO_CGEN, ict_vco);

    //TxTSP
    SetDefaults(SECTION_TxTSP);
    SetDefaults(SECTION_TxNCO);
    Modify_SPI_Reg_bits(TSGMODE_TXTSP, 1);
    Modify_SPI_Reg_bits(INSEL_TXTSP, 1);

    //RXTSP
    SetDefaults(SECTION_RxTSP);
    SetDefaults(SECTION_RxNCO);
    Modify_SPI_Reg_bits(AGC_MODE_RXTSP, 1);
    Modify_SPI_Reg_bits(AGC_AVG_RXTSP, 1);
    Modify_SPI_Reg_bits(HBD_OVR_RXTSP, 4);
    Modify_SPI_Reg_bits(CMIX_SC_RXTSP, 1);

    EndBatch();
#endif // BATCH_TX_SETUP
    //BIAS
    /*{
    const uint8_t rp_calib_bias = Get_SPI_Reg_bits(RP_CALIB_BIAS);
    //SetDefaults(SECTION_BIAS);
    Modify_SPI_Reg_bits(RP_CALIB_BIAS, rp_calib_bias);
    }*/

    SetTxLPF_PDs(self, tx_lpf_IF);
    if(tx_lpf_IF <= TxLPF_RF_LimitLowMid/2)
    {
        const float freq = (16.0/20.0)*tx_lpf_IF/1e6;
        int16_t rcal_lpflad_tbb =
              pow(freq, 4)*1.29858903647958e-16
            + pow(freq, 3)*(-0.000110746929967704)
            + pow(freq, 2)*0.00277593485991029
            + freq*21.0384293169607
            + (-48.4092606238297);
        Modify_SPI_Reg_bits(self, 0x0109, 7, 0, clamp(rcal_lpflad_tbb, 0, 255));
    }
    else
    {
        const float freq = tx_lpf_IF/1e6;
        int16_t rcal_lpfh_tbb = pow(freq, 4)*1.10383261611112e-06
            + pow(freq, 3)*(-0.000210800032517545)
            + pow(freq,2)*0.0190494874803309
            + freq*1.43317445923528
            + (-47.6950779298333);
        Modify_SPI_Reg_bits(self, 0x0109, 15, 8, clamp(rcal_lpfh_tbb, 0, 255));
    }

    //CGEN
    status = ConfigCGEN_ForLPF_IF(self, tx_lpf_IF);
    if(status != MCU_NO_ERROR)
        return status;

    //SXR
    Modify_SPI_Reg_bits(self, 0x0020, 1, 0, 1);
    Modify_SPI_Reg_bits(self, 0x011C, 1, 1, 1);

    //SXT
    Modify_SPI_Reg_bits(self, 0x0020, 1, 0, 2);
    Modify_SPI_Reg_bits(self, 0x011C, 1, 1, 1);

    LMS7002M_spi_write(self, 0x0020, reg0020);

    //TXTSP
    LoadDC_REG_TX_IQ(self);
    SetNCOFrequency(self, LMS7002M_Tx, 1e6, 0);
    SetNCOFrequency(self, LMS7002M_Tx, tx_lpf_IF, 1);

    //RxTSP
    SetNCOFrequency(self, LMS7002M_Rx, 0.9e6, 0);
    SetNCOFrequency(self, LMS7002M_Rx, tx_lpf_IF-0.1e6, 1);

    EnableMIMOBuffersIfNecessary(self);
    EnableChannelPowerControls(self);
    return MCU_NO_ERROR;
}

static uint8_t SearchTxFilterCCAL_RCAL(LMS7002M_t *self, uint16_t addr, uint8_t msb,  uint8_t lsb)
{
    const ReachRSSIparams paramLPF_LAD_decrease = {0x010A, 12, 8, -1, 0, true};
    const ReachRSSIparams paramLPF_LAD_increase = {0x010A, 12, 8, 1, 31, false};
    bool targetLevelReached;
    uint8_t iterationsLeft = 6;
    do
    {
        uint16_t rssi_3dB_lad;
        uint8_t ccal_limit;
        int8_t rcal_step;
        bool rssiShouldBeLess;
        bool needToChangeCCAL;
        Modify_SPI_Reg_bits(self, 0x0240, 4, 1, 0);
        Modify_SPI_Reg_bits(self, 0x0440, 4, 1, 0);
        rssi_3dB_lad = GetRSSI(self) * 0.7071;
        Modify_SPI_Reg_bits(self, 0x0240, 4, 1, 1);
        Modify_SPI_Reg_bits(self, 0x0440, 4, 1, 1);

        if(GetRSSI(self) < rssi_3dB_lad)
        {
            ccal_limit = 0;
            rcal_step = 25;
            rssiShouldBeLess = true;
            needToChangeCCAL = false;
        }
        else
        {
            ccal_limit = 31;
            rcal_step = -10;
            rssiShouldBeLess = false;
            needToChangeCCAL = true;
        }

        if(rssiShouldBeLess)
            targetLevelReached = ChangeUntilReachRSSI(self, &paramLPF_LAD_decrease, rssi_3dB_lad);
        else
            targetLevelReached = ChangeUntilReachRSSI(self, &paramLPF_LAD_increase, rssi_3dB_lad);
        if( !targetLevelReached && Get_SPI_Reg_bits(self, 0x010A, 12, 8) == ccal_limit)
        {
            uint8_t R = Get_SPI_Reg_bits(self, addr, msb,  lsb);
            if(R == 0 || R == 255)
                return MCU_NO_ERROR; // reached filter bandwidth limit
            Modify_SPI_Reg_bits(self, addr, msb, lsb, clamp(R+rcal_step, 0, 255));
            Modify_SPI_Reg_bits(self, 0x010A, 12, 8, 16);
        }
        else if(needToChangeCCAL)
        {
            uint8_t ccal_lpflad_tbb = Get_SPI_Reg_bits(self, 0x010A, 12, 8);
            ccal_lpflad_tbb = clamp(++ccal_lpflad_tbb, 0, 31);
            Modify_SPI_Reg_bits(self, 0x010A, 12, 8, ccal_lpflad_tbb);
        }
        --iterationsLeft;
    } while(!targetLevelReached && iterationsLeft);
    return targetLevelReached ? MCU_NO_ERROR : MCU_RCAL_LPF_LIMIT_REACHED;
}

uint8_t TuneTxFilter(LMS7002M_t *self,const float tx_lpf_freq_RF)
{
    float tx_lpf_IF;
    uint8_t status;

    if(tx_lpf_freq_RF < TxLPF_RF_LimitLow || tx_lpf_freq_RF > TxLPF_RF_LimitHigh)
        return MCU_TX_LPF_OUT_OF_RANGE;
    //calculate intermediate frequency
    tx_lpf_IF = tx_lpf_freq_RF/2;
    if(tx_lpf_freq_RF > TxLPF_RF_LimitLowMid && tx_lpf_freq_RF < TxLPF_RF_LimitMidHigh)
        tx_lpf_IF = TxLPF_RF_LimitMidHigh/2;
//    SaveChipState(0);
    status = TuneTxFilterSetup(self, tx_lpf_IF);
    if(status != MCU_NO_ERROR)
        goto TxFilterSearchEndStage;
    UpdateRSSIDelay(self);

    Modify_SPI_Reg_bits(self, 0x0440, 4, 1, 0);
    Modify_SPI_Reg_bits(self, 0x0240, 4, 1, 0);
    {
        const ReachRSSIparams paramTX = {0x0108, 15, 10, 1, 43, true};
        ChangeUntilReachRSSI(self, &paramTX, 0x2700);
    }

    //LPFL
    if(tx_lpf_IF <= TxLPF_RF_LimitLowMid/2)
        status = SearchTxFilterCCAL_RCAL(self, 0x0109, 7, 0);
    else // LPFH
    {
        Modify_SPI_Reg_bits(self, 0x011A, 6, 0, 2);
        status = SearchTxFilterCCAL_RCAL(self, 0x0109, 15, 8);
    }
    {
        uint16_t ccal_lpflad_tbb = Get_SPI_Reg_bits(self, 0x010A, 12, 8);
        uint16_t rcal_lpfh_lpflad_tbb = LMS7002M_spi_read(self, 0x0109);
    TxFilterSearchEndStage:
//        SaveChipState(1);
        if(status != MCU_NO_ERROR)
            return status;
        Modify_SPI_Reg_bits(self, 0x010A, 12, 8, ccal_lpflad_tbb);
        LMS7002M_spi_write(self, 0x0106, 0x318C);
        LMS7002M_spi_write(self, 0x0107, 0x318C);
        LMS7002M_spi_write(self, 0x0109, rcal_lpfh_lpflad_tbb);
        SetTxLPF_PDs(self, tx_lpf_IF);
    }

    return MCU_NO_ERROR;
}

//uint8_t TuneVCO(LMS7002M_t *self, bool SX) // 0-cgen, 1-SXR, 2-SXT
//{
//    typedef struct
//    {
//        uint8_t high;
//        uint8_t low;
//        uint8_t hasLock;
//    } CSWInteval;
//
//    uint16_t addrCSW_VCO;
//    uint8_t msb;
//    uint8_t lsb;
//
//    CSWInteval cswSearch[2];
//    cswSearch[0].high = 0; //search interval lowest value
//    cswSearch[0].low = 127;
//    cswSearch[1].high = 128;
//    cswSearch[1].low = 255;
//    cswSearch[1].hasLock = cswSearch[0].hasLock = false;
//
//    if(SX)
//    {
//        addrCSW_VCO = 0x0121;
//        msb = 10;
//        lsb = 3; //CSW msb lsb
//        //assuming the active channel is already correct
//        Modify_SPI_Reg_bits(self, 0x011C, 2, 1, 0); //activate VCO and comparator
//    }
//    else //CGEN
//    {
//        addrCSW_VCO = 0x008B;
//        msb = 8;
//        lsb = 1; //CSW msb lsb
//        Modify_SPI_Reg_bits(self, 0x0086, 2, 1, 0); //activate VCO and comparator
//    }
//
//    //check if lock is within VCO range
//    Modify_SPI_Reg_bits(self, addrCSW_VCO, msb, lsb, 0);
//    if(ReadCMP(self, SX) == 3) //VCO too high
//        return MCU_ERROR;
//    Modify_SPI_Reg_bits(self, addrCSW_VCO, msb, lsb, 255);
//    if(ReadCMP(self, SX) == 0) //VCO too low
//        return MCU_ERROR;
//
//    //search intervals [0-127][128-255]
//    {
//        uint8_t t;
//        for(t=0; t<2; ++t)
//        {
//            uint8_t mask;
//            for(mask = (1 << 6); mask; mask >>= 1)
//            {
//                uint8_t cmpValue;
//                cswSearch[t].high |= mask; // CSW_VCO<i>=1
//                Modify_SPI_Reg_bits(self, addrCSW_VCO, msb, lsb, cswSearch[t].high);
//                cmpValue = ReadCMP(self, SX);
//                if(cmpValue == 0x03) // reduce CSW
//                    cswSearch[t].high ^= mask; // CSW_VCO<i>=0
//                else if(cmpValue == 0x02 && cswSearch[t].high <= cswSearch[t].low)
//                {
//                    cswSearch[t].hasLock = true;
//                    cswSearch[t].low = cswSearch[t].high;
//                }
//            }
//            for(; cswSearch[t].low; --cswSearch[t].low)
//            {
//                Modify_SPI_Reg_bits(self, addrCSW_VCO, msb, lsb, cswSearch[t].low);
//                if(ReadCMP(self, SX) != 0x2)
//                {
//                    if(cswSearch[t].low < cswSearch[t].high)
//                        ++cswSearch[t].low;
//                    break;
//                }
//            }
//        }
//    }
//    {
//        uint8_t cswValue;
//        //compare which interval is wider
//        {
//            if(cswSearch[1].hasLock && (cswSearch[1].high-cswSearch[1].low >= cswSearch[0].high-cswSearch[0].low))
//                cswValue = cswSearch[1].low +((cswSearch[1].high-cswSearch[1].low) >> 1);
//            else
//                cswValue = cswSearch[0].low +((cswSearch[0].high-cswSearch[0].low) >> 1);
//        }
//
//        Modify_SPI_Reg_bits(self, addrCSW_VCO, msb, lsb, cswValue);
//        if(ReadCMP(self, SX) != 0x2) //just in case high-low==1, if low fails, check if high locks
//            Modify_SPI_Reg_bits(self, addrCSW_VCO, msb, lsb, ++cswValue);
//    }
//    if(ReadCMP(self, SX) == 0x2)
//        return MCU_NO_ERROR;
//    return MCU_ERROR;
//}

uint8_t SetFrequencyCGEN(LMS7002M_t *self, float freq)
{
    float dFvco;
    float intpart;
    //VCO frequency selection according to F_CLKH
    {
        uint8_t iHdiv_high = (2.94e9/2 / freq)-1;
        uint8_t iHdiv_low = (1.93e9/2 / freq);
        uint8_t iHdiv = (iHdiv_low + iHdiv_high)/2;
        dFvco = 2 * (iHdiv+1) * freq;
        Modify_SPI_Reg_bits(self, 0x0089, 10, 3, iHdiv);
    }
    //Integer division
    intpart = dFvco/RefClk;
    Modify_SPI_Reg_bits(self, 0x0088, 13, 4, intpart - 1); //INT_SDM_CGEN
    //Fractional division
    {
        const float dFrac = intpart - (uint32_t)(dFvco/RefClk);
        const uint32_t gFRAC = (uint32_t)(dFrac * 1048576);
        Modify_SPI_Reg_bits(self, 0x0087, 15, 0, gFRAC&0xFFFF); //INT_SDM_CGEN[15:0]
        Modify_SPI_Reg_bits(self, 0x0088, 3, 0, gFRAC>>16); //INT_SDM_CGEN[19:16]
    }

#if VERBOSE
    //printf("CGEN: Freq=%g MHz, VCO=%g GHz, INT=%i, FRAC=%i, DIV_OUTCH_CGEN=%i\n", freq/1e6, dFvco/1e9, gINT, gFRAC, iHdiv);
#endif // NDEBUG
    if(TuneVCO(self, VCO_CGEN) != 0)
        return MCU_CGEN_TUNE_FAILED;
    return 0;
}

//uint8_t SetFrequencySX(LMS7002M_t *self, const bool tx, const float freq_Hz)
//{
//    int16_t tuneScore[3] = {255, 255, 255}; // best is closest to 0
//    const uint16_t macBck = LMS7002M_spi_read(self, 0x0020);
//    bool canDeliverFrequency = false;
//
//    Modify_SPI_Reg_bits(self, 0x0020, 1, 0, tx?2:1);
//    //find required VCO frequency
//    {
//        float VCOfreq;
//        float temp;
//        {
//            uint8_t div_loch;
//            for (div_loch = 7; div_loch; --div_loch)
//            {
//                VCOfreq = pow2(div_loch) * freq_Hz;
//                if ((VCOfreq >= 3800e6) && (VCOfreq <= 7714e6))
//                    break;
//            }
//            Modify_SPI_Reg_bits(self, 0x011F, 8, 6, div_loch-1);
//        }
//        {
//            uint32_t fractionalPart;
//            const uint8_t enDiv2 = (VCOfreq > 5500e6) ? 1 : 0;
//            Modify_SPI_Reg_bits(self, 0x011C, 10, 10, enDiv2); //EN_DIV2_DIVPROG
//            temp = VCOfreq / (RefClk * (1 + enDiv2));
//            fractionalPart = (uint32_t)((temp - (uint32_t)(temp)) * 1048576);
//
//            Modify_SPI_Reg_bits(self, 0x011E, 13, 4, (uint16_t)(temp - 4)); //INT_SDM
//            Modify_SPI_Reg_bits(self, 0x011E, 3, 0, (fractionalPart >> 16)); //FRAC_SDM[19:16]
//            LMS7002M_spi_write(self, 0x011D, fractionalPart & 0xFFFF); //FRAC_SDM[15:0]
//        }
//    }
//    while(1)
//    {
//        uint8_t sel_vco;
//        uint8_t bestvco = 0;
//        for (sel_vco = 0; sel_vco < 3; ++sel_vco)
//        {
//            Modify_SPI_Reg_bits(self, 0x0121, 2, 1, sel_vco);
//            if( TuneVCO(self, 1) == MCU_NO_ERROR)
//            {
//                tuneScore[sel_vco] = Get_SPI_Reg_bits(self, 0x0121, 10, 3)-128;
//                canDeliverFrequency = true;
//            }
//            if(abs(tuneScore[sel_vco]) < abs(tuneScore[bestvco]))
//                bestvco = sel_vco;
//        }
//        if(canDeliverFrequency)
//        {
//            Modify_SPI_Reg_bits(self, 0x0121, 2, 1, bestvco);
//            Modify_SPI_Reg_bits(self, 0x0121, 10, 3, tuneScore[bestvco] + 128);
//            break;
//        }
//        {
//            uint16_t bias = Get_SPI_Reg_bits(self, 0x0120, 7, 0);
//            if(bias == 255)
//                break;
//            Modify_SPI_Reg_bits(self, 0x0120, 7, 0, bias + 32 > 255 ? 255 : bias + 32);
//        }
//    }
//    LMS7002M_spi_write(self, 0x0020, macBck);
//    if (canDeliverFrequency == false)
//        return tx ? MCU_SXT_TUNE_FAILED : MCU_SXR_TUNE_FAILED;
//    return MCU_NO_ERROR;
//}
///APPROXIMATE conversion
float ChipRSSI_2_dBFS(uint32_t rssi)
{
    uint32_t maxRSSI = 0x15FF4;
    if(rssi == 0)
        rssi = 1;
    return 20*log10((float)(rssi)/maxRSSI);
}
int16_t ReadAnalogDC(LMS7002M_t *self, const uint16_t addr)
{
    const uint16_t mask = addr < 0x05C7 ? 0x03FF : 0x003F;
    uint16_t value;
    int16_t result;
    LMS7002M_spi_write(self, addr, 0);
    LMS7002M_spi_write(self, addr, 0x4000);
    value = LMS7002M_spi_read(self, addr);
    LMS7002M_spi_write(self, addr, value & ~0xC000);
    result = (value & mask);
    if(value & (mask+1))
        result *= -1;
    return result;
}

static void WriteAnalogDC(LMS7002M_t *self, const uint16_t addr, int16_t value)
{
    const uint16_t mask = addr < 0x05C7 ? 0x03FF : 0x003F;
    int16_t regValue = 0;
    if(value < 0)
    {
        regValue |= (mask+1);
        regValue |= (abs(value+mask) & mask);
    }
    else
        regValue |= (abs(value+mask+1) & mask);
    LMS7002M_spi_write(self, addr, regValue);
    LMS7002M_spi_write(self, addr, regValue | 0x8000);
}

void AdjustAutoDC(LMS7002M_t *self, const uint16_t address, bool tx)
{
    uint8_t i;
    uint16_t rssi;
    uint16_t minRSSI;
    int16_t minValue;
    int16_t initVal;
    int8_t valChange;
    uint16_t range = tx ? 1023 : 63;

    minValue = initVal = ReadAnalogDC(self, address);
    minRSSI = rssi = GetRSSI(self);
    WriteAnalogDC(self, address, clamp(initVal+1, -range, range));
    valChange = GetRSSI(self) < rssi ? 1 : -1;

    for(i = 8; i; --i)
    {
        initVal = clamp(initVal+valChange, -range, range);
        WriteAnalogDC(self, address, initVal);
        rssi = GetRSSI(self);
        if(rssi < minRSSI)
        {
            minRSSI = rssi;
            minValue = initVal;
        }
    }
    WriteAnalogDC(self, address, minValue);
}
void CalibrateRxDCAuto(LMS7002M_t *self)
{
    uint16_t dcRegAddr = 0x5C7;
    const uint8_t ch = Get_SPI_Reg_bits(self, 0x0020, 1, 0);
    Modify_SPI_Reg_bits(self, 0x0100, 0, 0, 0);
    Modify_SPI_Reg_bits(self, 0x040C, 2, 2, 1);

    //auto calibration
    Modify_SPI_Reg_bits(self, 0x05C0, 15, 15, 1);
    if(ch == 1)
    {
        Modify_SPI_Reg_bits(self, 0x05C0, 6, 6, 0);
        Modify_SPI_Reg_bits(self, 0x05C0, 2, 2, 0);
        LMS7002M_spi_write(self, 0x05C2, 0xFF30);
    }
    else
    {
        Modify_SPI_Reg_bits(self, 0x05C0, 7, 7, 0);
        Modify_SPI_Reg_bits(self, 0x05C0, 3, 3, 0);
        LMS7002M_spi_write(self, 0x05C2, 0xFFC0);
        dcRegAddr += 2;
    }

    {
        while(LMS7002M_spi_read(self, 0x05C1) & 0xF000);
    }

        int16_t dci = ReadAnalogDC(self, dcRegAddr);
        int16_t dcq = ReadAnalogDC(self, dcRegAddr+1);
        uint32_t rssi = GetRSSI(self);
        printf("Rx DC auto   I: %3i, Q: %3i, %3.1f dBFS\n", dci, dcq, ChipRSSI_2_dBFS(rssi));


        //manual adjustments
        Modify_SPI_Reg_bits(self, 0x0401, 10, 0, 0);
        AdjustAutoDC(self, dcRegAddr, false);
        Modify_SPI_Reg_bits(self, 0x0401, 10, 0, 2047);
        AdjustAutoDC(self, dcRegAddr+1, false);


        dci = ReadAnalogDC(self, dcRegAddr);
        dcq = ReadAnalogDC(self, dcRegAddr+1);
        rssi = GetRSSI(self);
        printf("Rx DC manual I: %3i, Q: %3i, %3.1f dBFS\n", dci, dcq, ChipRSSI_2_dBFS(rssi));


//    Modify_SPI_Reg_bits(self, 0x040C, 2, 2, 0); // DC_BYP 0

        printf("RxTSP DC corrector enabled %3.1f dBFS\n", ChipRSSI_2_dBFS(GetRSSI(self)));

//    Modify_SPI_Reg_bits(self, 0x0100, 0, 0, 1);

}
