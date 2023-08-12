#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

#include <USB_N/LMS7002M/LMS7002M_logger.h>
#include <USB_N/LMS7002M/LMS7002M.h>
#include "delay/delay.h"

typedef struct
{
    uint16_t address;
    uint8_t msb;
    uint8_t lsb;
} LMS7Parameter;

typedef struct
{
    LMS7Parameter param;
    int16_t result;
    int16_t minValue;
    int16_t maxValue;
} BinSearchParam;

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

extern void Modify_SPI_Reg_bits (LMS7002M_t *self, unsigned short SPI_reg_addr, unsigned char MSB_bit, unsigned char LSB_bit, unsigned short new_bits_data);
extern unsigned short Get_SPI_Reg_bits (LMS7002M_t *self, unsigned short SPI_reg_addr, unsigned char MSB_bit, unsigned char LSB_bit);
extern void SetNCOFrequency(LMS7002M_t *self, const bool tx, const float freq, uint8_t index);
extern void UpdateRSSIDelay(LMS7002M_t *self);
extern void CalibrateRxDCAuto(LMS7002M_t *self);
extern uint32_t GetRSSI(LMS7002M_t *self);
extern float ChipRSSI_2_dBFS(uint32_t rssi);
extern void LoadDC_REG_TX_IQ(LMS7002M_t *self);
extern void EnableMIMOBuffersIfNecessary(LMS7002M_t *self);
extern void EnableChannelPowerControls(LMS7002M_t *self);
extern uint8_t SetFrequencySX(LMS7002M_t *self, const bool tx, const float freq_Hz);
extern void WriteMaskedRegs(LMS7002M_t *self, const RegisterBatch * regs);
extern uint8_t SetFrequencyCGEN(LMS7002M_t *self, float freq);
extern float GetFrequencySX(LMS7002M_t *self, bool tx);
extern float GetFrequencyCGEN(LMS7002M_t *self);
extern void SetDefaultsSX(LMS7002M_t *self);
extern uint8_t GetValueOf_c_ctl_pga_rbb(uint8_t g_pga_rbb);
extern int16_t ReadAnalogDC(LMS7002M_t *self, const uint16_t addr);
extern uint16_t pow2(const uint8_t power);
extern uint8_t TuneVCO(LMS7002M_t *self, bool SX);

static uint16_t stateAddresses[] = {0x0081, 0x010F, 0x0126, 0x040A, 0x040C};
//double space to store both channels
static uint16_t stateStorage[2*sizeof(stateAddresses)/sizeof(uint16_t)];
double bandwidthRF = 1.8e6;
#define calibrationSXOffset_Hz 1e6
#define offsetNCO 0.1e6
#define calibUserBwDivider 5
bool hasStopped = false;
bool stopProcedure = false;

#define MSB_LSB(m, l) (m << 4 | l)
#define TABLE_ENTRY(gain_setting, gainLNA, gainPGA) (gainPGA << 4 | gainLNA)
#define GET_PGA_GAIN(value) ((value >> 4) & 0x1F)
#define GET_LNA_GAIN(value) ((value) & 0xF)

static const uint16_t AGC_gain_table[] = {
TABLE_ENTRY(-12,1,0),
TABLE_ENTRY(-11,1,1),
TABLE_ENTRY(-10,1,2),
TABLE_ENTRY(-9,2,0),
TABLE_ENTRY(-8,2,1),
TABLE_ENTRY(-7,2,2),
TABLE_ENTRY(-6,2,3),
TABLE_ENTRY(-5,3,1),
TABLE_ENTRY(-4,3,2),
TABLE_ENTRY(-3,3,3),
TABLE_ENTRY(-2,3,4),
TABLE_ENTRY(-1,4,2),
TABLE_ENTRY(0,4,3),
TABLE_ENTRY(1,4,4),
TABLE_ENTRY(2,4,5),
TABLE_ENTRY(3,5,3),
TABLE_ENTRY(4,5,4),
TABLE_ENTRY(5,5,5),
TABLE_ENTRY(6,5,6),
TABLE_ENTRY(7,6,4),
TABLE_ENTRY(8,6,5),
TABLE_ENTRY(9,6,6),
TABLE_ENTRY(10,6,7),
TABLE_ENTRY(11,7,5),
TABLE_ENTRY(12,7,6),
TABLE_ENTRY(13,7,7),
TABLE_ENTRY(14,7,8),
TABLE_ENTRY(15,8,6),
TABLE_ENTRY(16,8,7),
TABLE_ENTRY(17,8,8),
TABLE_ENTRY(18,8,9),
TABLE_ENTRY(19,9,7),
TABLE_ENTRY(20,9,8),
TABLE_ENTRY(21,9,9),
TABLE_ENTRY(22,9,10),
TABLE_ENTRY(23,10,10),
TABLE_ENTRY(24,10,11),
TABLE_ENTRY(25,10,12),
TABLE_ENTRY(26,10,13),
TABLE_ENTRY(27,11,13),
TABLE_ENTRY(28,11,14),
TABLE_ENTRY(29,11,15),
TABLE_ENTRY(30,11,16),
TABLE_ENTRY(31,11,17),
TABLE_ENTRY(32,11,18),
TABLE_ENTRY(33,11,19),
TABLE_ENTRY(34,11,20),
TABLE_ENTRY(35,11,21),
TABLE_ENTRY(36,11,22),
TABLE_ENTRY(37,11,23),
TABLE_ENTRY(38,11,24),
TABLE_ENTRY(39,11,25),
TABLE_ENTRY(40,11,26),
TABLE_ENTRY(41,11,27),
TABLE_ENTRY(42,11,28),
TABLE_ENTRY(43,11,29),
TABLE_ENTRY(44,11,30),
TABLE_ENTRY(45,11,31),
TABLE_ENTRY(46,12,31),
TABLE_ENTRY(47,13,31),
TABLE_ENTRY(48,14,31),
TABLE_ENTRY(49,15,31)
};


static int16_t clamp(int16_t value, int16_t minBound, int16_t maxBound)
{
    if(value < minBound)
        return minBound;
    if(value > maxBound)
        return maxBound;
    return value;
}

int16_t toSigned(int16_t val, uint8_t msblsb)
{
    val <<= 15-((msblsb >> 4) & 0xF);
    val >>= 15-((msblsb >> 4) & 0xF);
    return val;
}

uint8_t SetupCGEN(LMS7002M_t *self)
{
    uint8_t cgenMultiplier;
    uint8_t gfir3n;
    cgenMultiplier = clamp((GetFrequencyCGEN(self) / 20.0e6) + 0.5, 2, 13);
    gfir3n = 4 * cgenMultiplier;
    if(Get_SPI_Reg_bits(self, 0x0086, 11, 11) == 1)
    gfir3n /= pow2(Get_SPI_Reg_bits(self, 0x0089, 12, 11));

    { //gfir3n = pow2((uint8_t)log2(gfir3n))-1
        uint8_t power;
        for(power = 0x3F; power; power >>= 1)
        {
            if(gfir3n >= power)
                break;
        }
        Modify_SPI_Reg_bits(self, 0x0407, 7, 0, power);
    }
    //CGEN VCO is powered up in SetFrequencyCGEN/Tune
    return SetFrequencyCGEN(self, 20.0e6 * cgenMultiplier);
}

static void SetRxGFIR3Coefficients(LMS7002M_t *self)
{
    //FIR coefficients symmetrical, storing only one half
    const int16_t firCoefs[] =
    {
        8,4,0,-6,-11,-16,-20,-22,-22,-20,-14,-5,6,20,34,46,
        56,61,58,48,29,3,-29,-63,-96,-123,-140,-142,-128,-94,-44,20,
        93,167,232,280,302,291,244,159,41,-102,-258,-409,-539,-628,-658,-614,
        -486,-269,34,413,852,1328,1814,2280,2697,3038,3277,3401,
    };
    uint8_t index = 0;
    for (; index < sizeof(firCoefs)/sizeof(int16_t); ++index)
        LMS7002M_spi_write(self, 0x0500 + index + 24 * (index / 40), firCoefs[index]);
    for (; index < sizeof(firCoefs)/sizeof(int16_t)*2; ++index)
        LMS7002M_spi_write(self, 0x0500 + index + 24 * (index / 40), firCoefs[119-index]);
}

static void StoreState(LMS7002M_t *self, bool write)
{
    uint16_t x0020 = LMS7002M_spi_read(self, 0x0020);
    uint8_t storageIndex = 0;
    uint8_t ch;
    for(ch = 2; ch>0; --ch)
    {
        uint8_t i = 0;
        LMS7002M_spi_write(self, 0x0020, (x0020 & 0xFFFC) | i);
        for(; i < sizeof(stateAddresses)/sizeof(uint16_t); ++i)
        {
            if(write)
                LMS7002M_spi_write(self, stateAddresses[i], stateStorage[storageIndex]);
            else
                stateStorage[storageIndex] = LMS7002M_spi_read(self, stateAddresses[i]);
            ++storageIndex;
        }
    }
    LMS7002M_spi_write(self, 0x0020, x0020);
}

static bool IsPLLTuned(LMS7002M_t *self)
{
    if( Get_SPI_Reg_bits(self, 0x0123, 13, 12) == 2 )
        return true;
    return TuneVCO(self, 1) == 0;
}

void ClockLogicResets(LMS7002M_t *self)
{
    //MCLK2 toggle
    uint16_t reg = LMS7002M_spi_read(self, 0x002B);
    LMS7002M_spi_write(self, 0x002B, reg ^ (1<<9));
    LMS7002M_spi_write(self, 0x002B, reg);

    //TSP logic reset
    reg = LMS7002M_spi_read(self, 0x0020);
    LMS7002M_spi_write(self, 0x0020, reg & ~0xAA00);
    LMS7002M_spi_write(self, 0x0020, reg);
}
void BinarySearch(LMS7002M_t *self, BinSearchParam * args)
{
    uint16_t rssiLeft = ~0;
    uint16_t rssiRight;
    int16_t left = args->minValue;
    int16_t right = args->maxValue;
    int16_t step;
    const uint16_t addr = args->param.address;
    const uint8_t msb = args->param.msb;
    const uint8_t lsb = args->param.lsb;

    Modify_SPI_Reg_bits(self, addr, msb, lsb, right);
    rssiRight = GetRSSI(self);
    while(right-left >= 1)
    {
        step = (right-left)/2;
        if(rssiLeft < rssiRight)
        {
            Modify_SPI_Reg_bits(self, addr, msb, lsb, right);
            rssiRight = GetRSSI(self);
        }
        else
        {
            Modify_SPI_Reg_bits(self, addr, msb, lsb, left);
            rssiLeft = GetRSSI(self);
        }
        if(step <= 0)
            break;
        if(rssiLeft < rssiRight)
            right -= step;
        else
            left += step;
    }
    args->result = rssiLeft < rssiRight ? left : right;
    Modify_SPI_Reg_bits(self, addr, msb, lsb, args->result);
}

void CalibrateIQImbalance(LMS7002M_t *self, bool tx)
{
    uint16_t gcorriAddress;
    uint16_t gcorrqAddress;
    BinSearchParam argsPhase;
    BinSearchParam argsGain;
    argsGain.param.msb = 10;
    argsGain.param.lsb = 0;
    argsPhase.param.msb = 11;
    argsPhase.param.lsb = 0;
    if(tx)
    {
        gcorrqAddress = 0x0201;
        gcorriAddress = 0x0202;
        argsPhase.param.address = 0x0203;
    }
    else
    {
        gcorrqAddress = 0x0401;
        gcorriAddress = 0x0402;
        argsPhase.param.address = 0x0403;
    }

    argsPhase.maxValue = 128;
    argsPhase.minValue = -128;
    BinarySearch(self, &argsPhase);

    printf("#0 %s IQCORR: %i, %3.1f dBFS\n", "Rx", argsPhase.result, ChipRSSI_2_dBFS(GetRSSI(self)));

    //coarse gain
    {
        uint16_t rssiIgain;
        uint16_t rssiQgain;
        LMS7002M_spi_write(self, gcorriAddress, 2047 - 64);
        LMS7002M_spi_write(self, gcorrqAddress, 2047);
        rssiIgain = GetRSSI(self);
        LMS7002M_spi_write(self, gcorriAddress, 2047);
        LMS7002M_spi_write(self, gcorrqAddress, 2047 - 64);
        rssiQgain = GetRSSI(self);

        if(rssiIgain < rssiQgain)
            argsGain.param.address = gcorriAddress;
        else
            argsGain.param.address = gcorrqAddress;
        LMS7002M_spi_write(self, gcorrqAddress, 2047);
    }
    argsGain.maxValue = 2047;
    argsGain.minValue = 2047-512;
    BinarySearch(self, &argsGain);

    const char* chName = (argsGain.param.address == gcorriAddress ? "I" : "Q");
    printf("#1 %s GAIN_%s: %i, %3.1f dBFS\n", "Rx", chName, argsGain.result, ChipRSSI_2_dBFS(GetRSSI(self)));

    argsPhase.maxValue = argsPhase.result+16;
    argsPhase.minValue = argsPhase.result-16;
    BinarySearch(self, &argsPhase);

    printf("#2 %s IQCORR: %i, %3.1f dBFS\n", "Rx", argsPhase.result, ChipRSSI_2_dBFS(GetRSSI(self)));


    LMS7002M_spi_write(self, argsGain.param.address, argsGain.result);
    Modify_SPI_Reg_bits(self, argsPhase.param.address, argsPhase.param.msb, argsPhase.param.lsb, argsPhase.result);
}

uint8_t CalibrateRxSetup(LMS7002M_t *self, bool extLoopback)
{
    uint8_t status;
    const uint16_t x0020val = LMS7002M_spi_read(self, 0x0020);
    //rfe
    {
        const uint16_t RxSetupAddr[] = {0x0084, 0x0085,0x00AE,0x010C,0x010D,0x0113,0x0115,0x0119};
        const uint16_t RxSetupData[] = {0x0400, 0x0001,0xF000,0x0000,0x0046,0x000C,0x0000,0x0000};
        const uint16_t RxSetupMask[] = {0xF8FF, 0x0007,0xF000,0x001A,0x0046,0x003C,0xC000,0x8000};
        const uint16_t RxSetupWrOnlyAddr[] = {0x0100,0x0101,0x0102,0x0103,0x0104,0x0105,0x0106,0x0107,0x0108,0x0109,0x010A,0x0200,0x0201,0x0202,0x0208,0x0240,0x0400,0x0401,0x0402,0x0403,0x0407,0x040A,0x040C,0x0440,0x05C0,0x05CB,0x0203,0x0204,0x0205,0x0206,0x0207,0x0241,0x0404,0x0405,0x0406,0x0408,0x0409,0x0441,0x05C1,0x05C2,0x05C3,0x05C4,0x05C5,0x05C6,0x05C7,0x05C8,0x05C9,0x05CA,0x05CC, 0x0081};
        const uint16_t RxSetupWrOnlyData[] = {0x3408,0x6001,0x3180,0x0A12,0x0088,0x0007,0x318C,0x318C,0x0426,0x61C1,0x104C,0x008D,0x07FF,0x07FF,0x2070,0x0020,0x0081,0x07FF,0x07FF,0x4000,0x0700,0x1000,0x2098,0x0020,0x00FF,0x2020};
        const RegisterBatch batch = {
            RxSetupAddr, RxSetupData, RxSetupMask, sizeof(RxSetupAddr)/sizeof(uint16_t),
            RxSetupWrOnlyAddr, RxSetupWrOnlyData, sizeof(RxSetupWrOnlyAddr)/sizeof(uint16_t), sizeof(RxSetupWrOnlyData)/sizeof(uint16_t)};
        WriteMaskedRegs(self, &batch);
    }

    //AFE
    if((x0020val & 0x3) == 1)
        Modify_SPI_Reg_bits(self, 0x0082, 2, 2, 0);
    else
        Modify_SPI_Reg_bits(self, 0x0082, 1, 1, 0);

    {
        switch(Get_SPI_Reg_bits(self, 0x010D, 8, 7))
        {
        case 2: //LNA_L
            //Modify_SPI_Reg_bits(SEL_BAND2_TRF, 1);
            //Modify_SPI_Reg_bits(SEL_BAND1_TRF, 0);
            Modify_SPI_Reg_bits(self, 0x0103, 11, 10, 1);
            break;
        case 3: //LNA_W
        case 1: //LNA_H
            //Modify_SPI_Reg_bits(SEL_BAND2_TRF, 0);
            //Modify_SPI_Reg_bits(SEL_BAND1_TRF, 1);
            Modify_SPI_Reg_bits(self, 0x0103, 11, 10, 2);
            break;
        default:
            return 0;
        }
    }

    Modify_SPI_Reg_bits(self, 0x0020, 1, 0, 2); //Get freq already changes/restores ch

    if(Get_SPI_Reg_bits(self, 0x011C, 6, 6) == 0) //isTDD
    {
        //in TDD do nothing
        Modify_SPI_Reg_bits(self, 0x0020, 1, 0, 1);
        SetDefaultsSX(self);
        Modify_SPI_Reg_bits(self, 0x0120, 7, 0, 255);
        status = SetFrequencySX(self, 0, GetFrequencySX(self, 1) - bandwidthRF/ calibUserBwDivider - 1e6);
    }
    else
    {
        //SXR
        float SXRfreqHz;
        Modify_SPI_Reg_bits(self, 0x0020, 1, 0, 1);
        //check if Rx is tuned
        if(!IsPLLTuned(self))
          return -1;
        SXRfreqHz = GetFrequencySX(self, 0);

        //SXT
        Modify_SPI_Reg_bits(self, 0x0020, 1, 0, 2);
       // SetDefaultsSX(self);
        Modify_SPI_Reg_bits(self, 0x0120, 7, 0, 255);
        status = SetFrequencySX(self, 1, SXRfreqHz/* + bandwidthRF/ calibUserBwDivider + 1e6*/);
    }
    LMS7002M_spi_write(self, 0x0020, x0020val);
    if(status != 0)
        return status;

    LoadDC_REG_TX_IQ(self);

    //CGEN
    status = SetupCGEN(self);
    if(status != 0)
        return status;
//    SetRxGFIR3Coefficients(self);
    SetNCOFrequency(self, 1, 1e6, 0);
    SetNCOFrequency(self, 0, 1e6/*bandwidthRF/calibUserBwDivider - offsetNCO*/, 0);
    //modifications when calibrating channel B
    EnableMIMOBuffersIfNecessary(self);
    EnableChannelPowerControls(self);
    return 0;
}
uint8_t CheckSaturationRx(LMS7002M_t *self, const float bandwidth_Hz, bool extLoopback)
{
    const uint16_t target_rssi = 0x07000; //0x0B000 = -3 dBFS
    uint16_t rssi;
    uint8_t cg_iamp = (uint8_t)Get_SPI_Reg_bits(self, 0x0108, 15, 10);
    Modify_SPI_Reg_bits(self, 0x040C, 13, 13, 1);
    Modify_SPI_Reg_bits(self, 0x040C, 7, 7, 0);
    SetNCOFrequency(self, 0, 1e6/*bandwidth_Hz / calibUserBwDivider - offsetNCO*/, 0);

    {
        uint8_t g_rxloopb_rfe = 2;
        Modify_SPI_Reg_bits(self, 0x0113, 5, 2, g_rxloopb_rfe);
        rssi = GetRSSI(self);

        printf("Initial gains:\tG_RXLOOPB: %2i, CG_IAMP: %2i | %2.3f dbFS\n", g_rxloopb_rfe, cg_iamp, ChipRSSI_2_dBFS(rssi));

        while (rssi < target_rssi)
        {
            g_rxloopb_rfe += 2;
            if(g_rxloopb_rfe > 15)
                break;
            Modify_SPI_Reg_bits(self, 0x0113, 5, 2, g_rxloopb_rfe);
            rssi = GetRSSI(self);
        }
    }

    while(rssi < 0x01000)
    {
        cg_iamp += 2;
        if(cg_iamp > 63-6)
            break;
        Modify_SPI_Reg_bits(self, 0x0108, 15, 10, cg_iamp);
        rssi = GetRSSI(self);
    }

    while(rssi < target_rssi)
    {
        cg_iamp += 1;
        if(cg_iamp > 62)
            break;
        Modify_SPI_Reg_bits(self, 0x0108, 15, 10, cg_iamp);
        rssi = GetRSSI(self);
    }

    if( rssi < 0xB21 ) // ~(-30 dbFS)
    {
        printf("Signal strength (%3.1f dBFS) very low, loopback not working?\n", ChipRSSI_2_dBFS(rssi));
        return 0;
    }
    return 0;
}
uint8_t CalibrateRx(LMS7002M_t *self, bool extLoopback, bool dcOnly)
{
    uint8_t status;
    const uint16_t x0020val = LMS7002M_spi_read(self, 0x0020); //remember used channel


    double rxFreq = GetFrequencySX(self, 0);
    const char* lnaName;
    switch(Get_SPI_Reg_bits(self, 0x010D, 8, 7))
    {
    case 0:
        lnaName = "none";
        break;
    case 1:
        lnaName = "LNAH";
        break;
    case 2:
        lnaName = "LNAW";
        break;
    case 3:
        lnaName = "LNAL";
        break;
    default:
        lnaName = "none";
        break;
    }
    printf("Rx ch.%s @ %4g MHz, BW: %g MHz, RF input: %s, PGA: %i, LNA: %i, TIA: %i\n",
           (x0020val & 0x3) == 1 ? "A" : "B", rxFreq/1e6,
           bandwidthRF/1e6, lnaName,
           Get_SPI_Reg_bits(self, 0x0119, 4, 0),
           Get_SPI_Reg_bits(self, 0x0113, 9, 6),
           Get_SPI_Reg_bits(self, 0x0113, 1, 0));
    printf("Rx calibration started\n");


    status = CalibrateRxSetup(self, extLoopback);
    if(status != 0)
        goto RxCalibrationEndStage;
    UpdateRSSIDelay(self);
//    CalibrateRxDCAuto(self);
    if(dcOnly)
        goto RxCalibrationEndStage;
    if(!extLoopback)
    {
        if ((uint8_t)Get_SPI_Reg_bits(self, 0x010D, 8, 7) == 2)
        {
            Modify_SPI_Reg_bits(self, 0x010C, 5, 5, 0);
            Modify_SPI_Reg_bits(self, 0x010D, 3, 3, 0);
        }
        else
        {
            Modify_SPI_Reg_bits(self, 0x010C, 6, 6, 0);
            Modify_SPI_Reg_bits(self, 0x010D, 4, 4, 0);
        }
    }

    Modify_SPI_Reg_bits(self, 0x0020, 1, 0, 2);
    if (Get_SPI_Reg_bits(self, 0x011C, 6, 6) == false)
    {
        Modify_SPI_Reg_bits(self, 0x011C, 6, 6, 1);
        //TDD MODE
        Modify_SPI_Reg_bits(self, 0x0020, 1, 0, 1);
        Modify_SPI_Reg_bits(self, 0x011C, 1, 1, 0);
    }
    LMS7002M_spi_write(self, 0x0020, x0020val);
    status = CheckSaturationRx(self, bandwidthRF, extLoopback);
    if(status != 0)
        goto RxCalibrationEndStage;
    Modify_SPI_Reg_bits(self, 0x040C, 13, 13, 0);
    Modify_SPI_Reg_bits(self, 0x040C, 7, 7, 0);
    SetNCOFrequency(self, 0, 1e6/*bandwidthRF/calibUserBwDivider + offsetNCO*/, 0);
//    CalibrateIQImbalance(self, 0);
RxCalibrationEndStage:
    {
        uint16_t gcorri = Get_SPI_Reg_bits(self, 0x0402, 10, 0);
        uint16_t gcorrq = Get_SPI_Reg_bits(self, 0x0401, 10, 0);
        uint16_t phaseOffset = Get_SPI_Reg_bits(self, 0x0403, 11, 0);

        LMS7002M_spi_write(self, 0x0020, x0020val);
        if (status != 0)
        {

            printf("Rx calibration failed\n");

            return status;
        }
        // dc corrector values not overwritten by chip state restore
        if(!dcOnly)
        {
            Modify_SPI_Reg_bits(self, 0x0402, 10, 0, gcorri);
            Modify_SPI_Reg_bits(self, 0x0401, 10, 0, gcorrq);
            Modify_SPI_Reg_bits(self, 0x0403, 11, 0, phaseOffset);
        }

        int16_t dcI = ReadAnalogDC(self, (x0020val & 1) ? 0x5C7 : 0x5C8);
        int16_t dcQ = ReadAnalogDC(self, (x0020val & 1) ? 0x5C9 : 0x5CA);
        int16_t phaseSigned = toSigned(phaseOffset, MSB_LSB(11, 0));
        printf("Tx | DC   | GAIN | PHASE\n");
        printf("---+------+------+------\n");
        printf("I: | %4i | %4i | %i\n", dcI, gcorri, phaseSigned);
        printf("Q: | %4i | %4i |\n", dcQ, gcorrq);

    }
    Modify_SPI_Reg_bits(self, 0x05C0, 15, 15, 1);
    if(x0020val & 0x1)
        Modify_SPI_Reg_bits(self, 0x05C0, 6, 6, 0);
    else
        Modify_SPI_Reg_bits(self, 0x05C0, 7, 7, 0);
    Modify_SPI_Reg_bits(self, 0x040C, 2, 0, 0); //DC_BYP 0, GC_BYP 0, PH_BYP 0
    Modify_SPI_Reg_bits(self, 0x040C, 8, 8, 0); //DCLOOP_STOP
    //Log("Rx calibration finished", LOG_INFO);
    return 0;
}
uint8_t RunAGC(LMS7002M_t *self, uint32_t wantedRSSI)
{
    uint32_t lastRSSI[2];
    uint16_t x0400[2];
    uint8_t status;
    uint8_t gainLNA[2];
    uint8_t gainPGA[2];
    uint8_t ch;
    uint16_t x0020 = LMS7002M_spi_read(self, 0x0020);
    gainLNA[1] = gainLNA[0] = 11;
    gainPGA[1] = gainPGA[0] = 31;
    hasStopped = false;
    StoreState(self, false);
    //Setup
    if(Get_SPI_Reg_bits(self, 0x0081, 15, 15) == false)
    {
        for(ch=0; ch<1; ++ch)
        {
            LMS7002M_spi_write(self, 0x0020, (x0020 & 0xFFFC) | (ch+1));
            Modify_SPI_Reg_bits(self, 0x0125, 15, 10, Get_SPI_Reg_bits(self, 0x0108, 15, 10));
            Modify_SPI_Reg_bits(self, 0x0125, 9, 5, Get_SPI_Reg_bits(self, 0x0101, 10, 6));
            Modify_SPI_Reg_bits(self, 0x0125, 4, 0, Get_SPI_Reg_bits(self, 0x0101, 5, 1));
        }
    }
    Modify_SPI_Reg_bits(self, 0x0081, 15, 15, 1);
    for(ch=0; ch<1; ++ch)
    {
        LMS7002M_spi_write(self, 0x0020, (x0020 & 0xFFFC) | (ch+1));
        Modify_SPI_Reg_bits(self, 0x010F, 9, 5, 31);
        Modify_SPI_Reg_bits(self, 0x010F, 4, 0, 4);
        //C_CTL_PGA_RBB 0, TIA 2
        LMS7002M_spi_write(self, 0x0126, (gainPGA[ch] << 6) | (gainLNA[ch] << 2) | 2);

        status = CalibrateRx(self, false, false);
        //if(status != MCU_NO_ERROR)
            //goto AGC_END;
        //Modify_SPI_Reg_bits(AGC_MODE_RXTSP, 1);
        //Modify_SPI_Reg_bits(AGC_AVG_RXTSP, 0);
        LMS7002M_spi_write(self, 0x040A, 0x1000);
        Modify_SPI_Reg_bits(self, 0x040C, 6, 6, 0);

        if(Get_SPI_Reg_bits(self, 0x040C, 7, 7) == 0)
        {
            Modify_SPI_Reg_bits(self, 0x040C, 15, 14, 1);
            Modify_SPI_Reg_bits(self,0x040C, 12, 12, 0);
        }
        x0400[ch] = LMS7002M_spi_read(self, 0x0400) & ~0xF000; //CAPTURE 0, CAPSEL 0
    }
    UpdateRSSIDelay(self);

//    while(!stopProcedure)
//    {
        //Need to wait only
        for(ch=0; ch<1; ++ch)
        {
            float dBdiff;
            uint8_t LNA_gain_available;
            uint32_t rssi;
            bool needUpdate = false;
            LMS7002M_spi_write(self, 0x0020, (x0020 & 0xFFFC) | (ch+1));
            //CAPTURE RSSI
            LMS7002M_spi_write(self, 0x0400, x0400[ch]);
            LMS7002M_spi_write(self, 0x0400, x0400[ch] | 0x8000);
            rssi = LMS7002M_spi_read(self, 0x040F);
            rssi = (rssi << 2 | (LMS7002M_spi_read(self, 0x040E) & 0x3));

            if(rssi == lastRSSI[ch] || rssi == 0)
                continue;
            lastRSSI[ch] = rssi;

            if(gainLNA[ch] <= 9)
                LNA_gain_available = 3*(11-gainLNA[ch]); //G_LNA=0 not allowed
            else
                LNA_gain_available = 15-gainLNA[ch];

            dBdiff = 20*log10((float)wantedRSSI/rssi);
            if (dBdiff < 0 && rssi > 0x14000)
            {
                gainPGA[ch] = clamp(gainPGA[ch] - 12, 0, 31);
                gainLNA[ch] = clamp(gainLNA[ch] -  3, 0, 15);
                needUpdate = true;
            }
            else if(dBdiff < 0 || (dBdiff > 0 && LNA_gain_available+31-gainPGA[ch] > 0))
            {
                int8_t total_gain_current = 30-LNA_gain_available + gainPGA[ch];
                const uint16_t gains = AGC_gain_table[clamp(total_gain_current+dBdiff, 0, 61)];
                uint8_t newLNA = GET_LNA_GAIN(gains);
                uint8_t newPGA = GET_PGA_GAIN(gains);
                if(newPGA != gainPGA[ch] || newLNA != gainLNA[ch])
                {
                    gainLNA[ch] = GET_LNA_GAIN(gains);
                    gainPGA[ch] = GET_PGA_GAIN(gains);
                    needUpdate = true;
                }
            }
            if(needUpdate)
            {
                uint16_t reg126;
                //set C_CTL_PGA_RBB
                reg126 = GetValueOf_c_ctl_pga_rbb(gainPGA[ch]) << 11;
                LMS7002M_spi_write(self, 0x0126, reg126 | (gainPGA[ch] << 6) | (gainLNA[ch] << 2) | 2);
            }
        }
//    }
    StoreState(self, true);
    LMS7002M_spi_write(self, 0x0020, x0020);
    //ClockLogicResets(self);
    hasStopped = true;
    return status;
}
