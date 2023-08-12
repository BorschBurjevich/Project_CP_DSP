// настрояка ячейки коммутатора //
#include"Lorandit_functions/modes/mode_Calibration/Calibration.h"
#include"Lorandit_functions/Cod_com.h"


void Set_Cell_Switchboard(uint8_t rgm_in, uint8_t value_att, uint8_t flag_over)
{
    if (flag_over == 0)         // нет переполнения
    {

       spi2_LVDS_write(MCSPI2_LVDS_CS0,rgm_in,1);

    }

    else                        // есть переполнение, требуется дополнительное ослабление
    {
       spi2_LVDS_write(MCSPI2_LVDS_CS0,value_att,1);

    }

}

//pr_work == 0 - пеленг, контроль,спектр;; pr_work == 1 - калибровка;; pr_work == 2 - тех_анализ;;
uint8_t selection_litera(uint32_t freq_in_mhz, uint8_t flag_now_aerial, uint8_t pr_work)
{
    if (pr_work == 0)
    {
        if (flag_now_aerial == 1)       // изначально была скоммутирована 1ая литера
        {
           if (freq_in_mhz >= 100 && freq_in_mhz < 800) // если частота соответвсует 2 ой литере
           {
               Set_Cell_Switchboard(RGM_IN_ANT2,0x0,0x0);
               flag_now_aerial  = 2;
           }

           else if (freq_mhz >= 800)           // если частота соответвсует 3  литере
           {
               Set_Cell_Switchboard(RGM_IN_ANT3,0x0,0x0);
               flag_now_aerial  = 3;
           }

        }

        else if (flag_now_aerial == 2)             // изначально была скоммутирована 2ая литера
        {
           if ((freq_mhz >= 25) && (freq_mhz < 100))   // если частота соответвсует 1 ой литере
           {
               Set_Cell_Switchboard(RGM_IN_ANT1,0x0,0x0);
               flag_now_aerial  = 1;
           }

           else if (freq_mhz >= 800) // если частота соответвсует 3  литере
           {
               Set_Cell_Switchboard(RGM_IN_ANT3,0x0,0x0);
               flag_now_aerial  = 3;
           }

        }

        else if (flag_now_aerial == 3)    // изначально была скоммутирована 3ья литера
        {

           if (freq_mhz >= 25 && freq_mhz < 100)   // если частота соответвсует 1 ой литере
           {
               Set_Cell_Switchboard(RGM_IN_ANT1,0x0,0x0);
               flag_now_aerial  = 1;
           }

           else if (freq_in_mhz >= 100 && freq_in_mhz < 800) // если частота соответвсует 2 ой  литере
           {
               Set_Cell_Switchboard(RGM_IN_ANT2,0x0,0x0);
               flag_now_aerial  = 2;
           }

        }
        else if (flag_now_aerial == 0) // инит коммутатора
        {
            if  (freq_mhz >= 25 && freq_mhz < 100)      // 1 литера
            {
                Set_Cell_Switchboard(RGM_IN_ANT1,0x0,0x0);  // настройка ячейки коммутатора
                flag_now_aerial         = 1;
            }

            else if (freq_mhz >=100 && freq_mhz < 800)  // 2 литера
            {
                Set_Cell_Switchboard(RGM_IN_ANT2,0x0,0x0);
                flag_now_aerial         = 2;
            }

            else if (freq_mhz >= 800)                   // 3 литера
            {
                Set_Cell_Switchboard(RGM_IN_ANT3,0x0,0x0);
                flag_now_aerial         = 3;
            }
        }
    }
    else if (pr_work == 1)
    {
        if (flag_now_aerial == 1)       // изначально была скоммутирована 1ая литера
        {
            if (freq_in_mhz >= 100 && freq_in_mhz < 800) // если частота соответвсует 2 ой литере
            {
               Set_Cell_Switchboard(RGM_IN_CAL_ANT2,0x0,0x0);
               flag_now_aerial  = 2;
            }

            else if (freq_mhz >= 800)           // если частота соответвсует 3  литере
            {
               Set_Cell_Switchboard(RGM_IN_CAL_ANT3,0x0,0x0);
               flag_now_aerial  = 3;
            }

        }

        else if (flag_now_aerial == 2)             // изначально была скоммутирована 2ая литера
        {
            if ((freq_mhz >= 25) && (freq_mhz < 100))   // если частота соответвсует 1 ой литере
            {
                Set_Cell_Switchboard(RGM_IN_CAL_ANT1,0x0,0x0);
                flag_now_aerial  = 1;
            }

            else if (freq_mhz >= 800) // если частота соответвсует 3  литере
            {
                Set_Cell_Switchboard(RGM_IN_CAL_ANT3,0x0,0x0);
                flag_now_aerial  = 3;
            }

        }

        else if (flag_now_aerial == 3)    // изначально была скоммутирована 3ья литера
        {

            if (freq_mhz >= 25 && freq_mhz < 100)   // если частота соответвсует 1 ой литере
            {
                Set_Cell_Switchboard(RGM_IN_CAL_ANT1,0x0,0x0);
                flag_now_aerial  = 1;
            }

            else if (freq_in_mhz >= 100 && freq_in_mhz < 800) // если частота соответвсует 2 ой  литере
            {
                Set_Cell_Switchboard(RGM_IN_CAL_ANT2,0x0,0x0);
                flag_now_aerial  = 2;
            }

        }
        else if (flag_now_aerial == 0) // инит коммутатора
        {
            if  (freq_mhz >= 25 && freq_mhz < 100)      // 1 литера
            {
                Set_Cell_Switchboard(RGM_IN_CAL_ANT1,0x0,0x0);  // настройка ячейки коммутатора
                flag_now_aerial         = 1;
            }

            else if (freq_mhz >=100 && freq_mhz < 800)  // 2 литера
            {
                Set_Cell_Switchboard(RGM_IN_CAL_ANT2,0x0,0x0);
                flag_now_aerial         = 2;
            }

            else if (freq_mhz >= 800)                   // 3 литера
            {
                Set_Cell_Switchboard(RGM_IN_CAL_ANT3,0x0,0x0);
                flag_now_aerial         = 3;
            }
        }
    }

    else if (pr_work == 2)
    {
        if (flag_now_aerial == 1)       // изначально была скоммутирована 1ая литера
        {
            if (freq_in_mhz >= 100 && freq_in_mhz < 800) // если частота соответвсует 2 ой литере
            {
                Set_Cell_Switchboard(RGM_IN_TEH_ANT2,0x0,0x0);
                flag_now_aerial  = 2;
            }

            else if (freq_mhz >= 800)           // если частота соответвсует 3  литере
            {
                Set_Cell_Switchboard(RGM_IN_TEH_ANT3,0x0,0x0);
                flag_now_aerial  = 3;
            }

        }

        else if (flag_now_aerial == 2)             // изначально была скоммутирована 2ая литера
        {
            if ((freq_mhz >= 25) && (freq_mhz < 100))   // если частота соответвсует 1 ой литере
            {
                Set_Cell_Switchboard(RGM_IN_TEH_ANT1,0x0,0x0);
                flag_now_aerial  = 1;
            }

            else if (freq_mhz >= 800) // если частота соответвсует 3  литере
            {
                Set_Cell_Switchboard(RGM_IN_TEH_ANT3,0x0,0x0);
                flag_now_aerial  = 3;
            }

        }

        else if (flag_now_aerial == 3)    // изначально была скоммутирована 3ья литера
        {

            if (freq_mhz >= 25 && freq_mhz < 100)   // если частота соответвсует 1 ой литере
            {
                Set_Cell_Switchboard(RGM_IN_TEH_ANT1,0x0,0x0);
                flag_now_aerial  = 1;
            }

            else if (freq_in_mhz >= 100 && freq_in_mhz < 800) // если частота соответвсует 2 ой  литере
            {
                Set_Cell_Switchboard(RGM_IN_TEH_ANT2,0x0,0x0);
                flag_now_aerial  = 2;
            }

        }
        else if (flag_now_aerial == 0) // инит коммутатора
        {
            if  (freq_mhz >= 25 && freq_mhz < 100)      // 1 литера
            {
                Set_Cell_Switchboard(RGM_IN_TEH_ANT1,0x0,0x0);  // настройка ячейки коммутатора
                flag_now_aerial         = 1;
            }

            else if (freq_mhz >=100 && freq_mhz < 800)  // 2 литера
            {
                Set_Cell_Switchboard(RGM_IN_TEH_ANT2,0x0,0x0);
                flag_now_aerial         = 2;
            }

            else if (freq_mhz >= 800)                   // 3 литера
            {
                Set_Cell_Switchboard(RGM_IN_TEH_ANT3,0x0,0x0);
                flag_now_aerial         = 3;
            }
        }
    }


    return flag_now_aerial;
}

