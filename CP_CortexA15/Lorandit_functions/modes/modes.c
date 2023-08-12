#include "Lorandit_functions/modes/modes.h"
#include "AD9208_spi/AD9208.h"
#include "AD9208_spi/ad9208_reg.h"
#include "Lorandit_functions/modes/mode_Calibration/Calibration.h"
#include "Lorandit_functions/reciever/reciever.h"
#include "Lorandit_functions/modes/modes.h"
#include "Lorandit_functions/Structs.h"


//тела функций
uint32_t get_board_temp(I2C_Handle h)
{
    short fpga_temp, proc_temp;
    uint32_t t;
    proc_temp = sensor_drv(h,  0x49);
    fpga_temp = sensor_drv(h,  0x48);

    t = ((uint32_t)fpga_temp) | ((uint32_t)(proc_temp << 16));

    return (t);
}

void boot_file()
{
    uint8_t num_file;
    num_file = file_boot_loader[0];
    memcpy(&size, &file_boot_loader[1], 4);//размер файла в байтах
    length = (uint32_t)(size/511);//число блоков по 511 для корректной записи

    switch(num_file)
    {
        case APP_CP:
        {
            write_app_to_emmc();
            break;
        }

        case MLO_CP:
        {
            write_mlo_to_emmc();
            break;
        }

    }

}

void version()
{
    uint8_t buf[12];
    uint32_t tmp = 8;

    memcpy(&buf[0], &tmp, 4);
    tmp = (uint32_t)(version_);
    memcpy(&buf[4], &tmp, 4);

    tmp = 0x00130215;
    memcpy(&buf[8], &tmp, 4);

    send_eth(arm, &txEthBuf[0], 12);
}

void tresholds_func()
{
    uint32_t k = 0;
    uint32_t i, list_num;

    // list_num - номер порога из списка

    memcpy(&num_tresholds, &pBuf[k], sizeof(num_tresholds));
    k += sizeof(num_tresholds);

    for (i = 0; i < num_tresholds; i++)
    {
        memcpy(&list_num, &pBuf[k], sizeof(list_num));
        k += sizeof(list_num);

        switch (list_num)
        {
            case 0:
            {
                memcpy(&tresholds_item.auto_tresholds_uhf, &pBuf[k], sizeof(tresholds_item.auto_tresholds_uhf));
                k += sizeof(tresholds_item.auto_tresholds_uhf);
                break;
            }

            case 1:
            {
                memcpy(&tresholds_item.energy_hf, &pBuf[k], sizeof(tresholds_item.energy_hf));
                k += sizeof(tresholds_item.energy_hf);
                break;
            }

            case 2:
            {
                memcpy(&tresholds_item.energy_uhf, &pBuf[k], sizeof(tresholds_item.energy_uhf));
                k += sizeof(tresholds_item.energy_uhf);
                break;
            }

            case 3:
            {
                memcpy(&tresholds_item.resolution_hf, &pBuf[k], sizeof(tresholds_item.resolution_hf));
                k += sizeof(tresholds_item.resolution_hf);
                break;
            }

            case 4:
            {
                memcpy(&tresholds_item.resolution_uhf, &pBuf[k], sizeof(tresholds_item.resolution_uhf));
                k += sizeof(tresholds_item.resolution_uhf);
                if (tresholds_item.resolution_uhf == 0)
                {
                   useful_struct.fft_points = 256;      // fd = 2.568 МГц
                   useful_struct.bins_tocut = 28;       // (fd - 2MГц) / 10.031/2
                   useful_struct.bin_width  = 10.031;
                   useful_struct.start_val  = 28;
                   useful_struct.stop_val   = 228;
                }

                else if (tresholds_item.resolution_uhf == 1) // для отладки
                {
                   useful_struct.fft_points = 512;
                   useful_struct.bins_tocut = 57;
                   useful_struct.bin_width  = 5.015;
                   useful_struct.start_val  = 57;
                   useful_struct.stop_val   = 455;
                }

                else if (tresholds_item.resolution_uhf == 2)
                {
                    useful_struct.fft_points = 1024;
                    useful_struct.bins_tocut = 113;
                    useful_struct.bin_width  = 2.507;
                    useful_struct.start_val  = 113;
                    useful_struct.stop_val   = 911;
                }

                break;
            }

            case 5:
            {
                memcpy(&tresholds_item.duration_time_hf, &pBuf[k], sizeof(tresholds_item.duration_time_hf));
                k += sizeof(tresholds_item.duration_time_hf);
                break;
            }

            case 6:
            {
                memcpy(&tresholds_item.duration_time_uhf, &pBuf[k], sizeof(tresholds_item.duration_time_uhf));
                k += sizeof(tresholds_item.duration_time_uhf);
                break;
            }

            case 7:
            {
                memcpy(&tresholds_item.intens_fhss_hf, &pBuf[k], sizeof(tresholds_item.intens_fhss_hf));
                k += sizeof(tresholds_item.intens_fhss_hf);
                break;
            }

            case 8:
            {
                memcpy(&tresholds_item.intens_fhss_uhf, &pBuf[k], sizeof(tresholds_item.intens_fhss_uhf));
                k += sizeof(tresholds_item.intens_fhss_uhf);
                break;
            }

            case 9:
            {
                memcpy(&tresholds_item.duration_time_back_hf, &pBuf[k], sizeof(tresholds_item.duration_time_back_hf));
                k += sizeof(tresholds_item.duration_time_back_hf);
                break;
            }

            case 10:
            {
                memcpy(&tresholds_item.duration_time_back_uhf, &pBuf[k], sizeof(tresholds_item.duration_time_back_uhf));
                k += sizeof(tresholds_item.duration_time_back_uhf);
                break;
            }

            case 11:
            {
                memcpy(&tresholds_item.phase_trasholds_uhf1, &pBuf[k], sizeof(tresholds_item.phase_trasholds_uhf1));
                k += sizeof(tresholds_item.phase_trasholds_uhf1);
                break;
            }

            case 12:
            {
                memcpy(&tresholds_item.phase_trasholds_uhf2, &pBuf[k], sizeof(tresholds_item.phase_trasholds_uhf2));
                k += sizeof(tresholds_item.phase_trasholds_uhf2);
                break;
            }

            case 13:
            {
                memcpy(&tresholds_item.phase_trasholds_hf, &pBuf[k], sizeof(tresholds_item.phase_trasholds_hf));
                k += sizeof(tresholds_item.phase_trasholds_hf);
                break;
            }

            case 14:
            {
                memcpy(&tresholds_item.correct_antenna_hf, &pBuf[k], sizeof(tresholds_item.correct_antenna_hf));
                k += sizeof(tresholds_item.correct_antenna_hf);
                break;
            }

            case 15:
            {
                memcpy(&tresholds_item.correct_antenna_uhf1, &pBuf[k], sizeof(tresholds_item.correct_antenna_uhf1));
                k += sizeof(tresholds_item.correct_antenna_uhf1);
                break;
            }

            case 16:
            {
                memcpy(&tresholds_item.correct_antenna_uhf2, &pBuf[k], sizeof(tresholds_item.correct_antenna_uhf2));
                k += sizeof(tresholds_item.correct_antenna_uhf2);
                break;
            }

            case 17:
            {
                memcpy(&tresholds_item.auto_att, &pBuf[k], sizeof(tresholds_item.auto_att));
                k += sizeof(tresholds_item.auto_att);
                break;
            }

            case 18:
            {
                memcpy(&tresholds_item.att_hf, &pBuf[k], sizeof(tresholds_item.att_hf));
                k += sizeof(tresholds_item.att_hf);
                break;
            }

            case 19:
            {
                memcpy(&tresholds_item.att_uhf, &pBuf[k], sizeof(tresholds_item.att_uhf));
                k += sizeof(tresholds_item.att_uhf);
                break;
            }

            case 20:
            {
                memcpy(&tresholds_item.num_states, &pBuf[k], sizeof(tresholds_item.num_states));
                k += sizeof(tresholds_item.num_states);
                break;
            }

            case 21:
            {
                memcpy(&tresholds_item.synch_block, &pBuf[k], sizeof(tresholds_item.synch_block));
                k += sizeof(tresholds_item.synch_block);
                break;
            }

            case 22:
            {
                memcpy(&tresholds_item.bearing_with_spectrum, &pBuf[k], sizeof(tresholds_item.bearing_with_spectrum));
                k += sizeof(tresholds_item.bearing_with_spectrum);
                break;
            }

            case 23:
            {
                memcpy(&tresholds_item.spectrum_step, &pBuf[k], sizeof(tresholds_item.spectrum_step));
                k += sizeof(tresholds_item.spectrum_step);
                break;
            }

            case 24:
            {
                memcpy(&tresholds_item.num_poo, &pBuf[k], sizeof(tresholds_item.num_poo));
                k += sizeof(tresholds_item.num_poo);
                break;
            }

        }
    }

}



void init_tech_analisys()
{
    uint32_t k = 0;

    memcpy(&tech_analisys.ch_num, &pBuf[k], sizeof(tech_analisys.ch_num));
    k += sizeof(tech_analisys.ch_num);

    memcpy(&tech_analisys.cenrtal_freq, &pBuf[k], sizeof(tech_analisys.cenrtal_freq));
    k += sizeof(tech_analisys.cenrtal_freq);

    memcpy(&tech_analisys.bw, &pBuf[k], sizeof(tech_analisys.bw));
    k += sizeof(tech_analisys.bw);

    memcpy(&tech_analisys.amp, &pBuf[k], sizeof(tech_analisys.amp));
    k += sizeof(tech_analisys.amp);

    memcpy(&tech_analisys.DMR_TETRA, &pBuf[k], sizeof(tech_analisys.DMR_TETRA));
    k += sizeof(tech_analisys.DMR_TETRA);


}


