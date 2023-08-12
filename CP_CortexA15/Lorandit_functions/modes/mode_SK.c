#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ti/ndk/inc/netmain.h>
#include "Lorandit_functions/Structs.h"
#include "Lorandit_functions/Cod_com.h"
#include "Lorandit_functions/modes/modes.h"

void init_mode_SK()
{
    uint32_t k = 0;

    memcpy(&acoustic_control.Central_Freq_band, &pBuf[k], sizeof(acoustic_control.Central_Freq_band));
    k += sizeof(acoustic_control.Central_Freq_band);

    memcpy(&acoustic_control.modulation_format, &pBuf[k], sizeof(acoustic_control.modulation_format));
    k += sizeof(acoustic_control.modulation_format);

    memcpy(&acoustic_control.bandpass, &pBuf[k], sizeof(acoustic_control.bandpass));
    k += sizeof(acoustic_control.bandpass);

    // Проверка на перестройу антенны //


    // настройка приёмника на ПРСУ - Л //



}


void start_mode_SK()
{

    ;;
}



