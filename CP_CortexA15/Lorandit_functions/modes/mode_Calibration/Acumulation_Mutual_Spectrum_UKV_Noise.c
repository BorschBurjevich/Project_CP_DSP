#include"Calibration.h"
#include "Lorandit_functions/Structs.h"
#include"Lorandit_functions/Cod_com.h"


 /*
 * bin_number ����� ������������ ����
 * flag �������� ������� ������
 */
void Acumulation_Mutual_Spectrum_UKV_Noise(uint32_t bin_number,uint8_t flag)
{
    //������� ������� ������ � ������� � ��� ����������� �������, ���� ������ �����, �� ����������
    if (flag == 0)
    {
        memset(rsw0, 0, sizeof(float)*84);
        memset(isw0, 0, sizeof(float)*84);
        memset(rsw1, 0, sizeof(float)*84);
        memset(isw1, 0, sizeof(float)*84);
        memset(rsw2, 0, sizeof(float)*84);
        memset(isw2, 0, sizeof(float)*84);
        memset(rsw3, 0, sizeof(float)*84);
        memset(isw3, 0, sizeof(float)*84);
        memset(rsw4, 0, sizeof(float)*84);
        memset(isw4, 0, sizeof(float)*84);

    }

    //������ �������� ��������
    rsw0[bin_number] += Re_Bin_Ch0[bin_number]*Re_Bin_Ch0[bin_number]+
                        Im_Bin_Ch0[bin_number]*Im_Bin_Ch0[bin_number];//�������
    isw0[bin_number] += Im_Bin_Ch0[bin_number]*Re_Bin_Ch0[bin_number]-
                        Re_Bin_Ch0[bin_number]*Im_Bin_Ch0[bin_number];

    rsw1[bin_number] += Re_Bin_Ch1[bin_number]*Re_Bin_Ch0[bin_number] +
                        Im_Bin_Ch1[bin_number]*Im_Bin_Ch0[bin_number];
    isw1[bin_number] += Im_Bin_Ch1[bin_number]*Re_Bin_Ch0[bin_number] -
                        Re_Bin_Ch1[bin_number]*Im_Bin_Ch0[bin_number];

    rsw2[bin_number] += Re_Bin_Ch2[bin_number]*Re_Bin_Ch0[bin_number]+
                        Im_Bin_Ch2[bin_number]*Im_Bin_Ch0[bin_number];
    isw2[bin_number] += Im_Bin_Ch2[bin_number]*Re_Bin_Ch0[bin_number]-
                        Re_Bin_Ch2[bin_number]*Im_Bin_Ch0[bin_number];

    rsw3[bin_number] += Re_Bin_Ch3[bin_number]*Re_Bin_Ch0[bin_number]+
                        Im_Bin_Ch3[bin_number]*Im_Bin_Ch0[bin_number];
    isw3[bin_number] += Im_Bin_Ch3[bin_number]*Re_Bin_Ch0[bin_number]-
                        Re_Bin_Ch3[bin_number]*Im_Bin_Ch0[bin_number];

    rsw4[bin_number] += Re_Bin_Ch4[bin_number]*Re_Bin_Ch0[bin_number]+
                        Im_Bin_Ch4[bin_number]*Im_Bin_Ch0[bin_number];
    isw4[bin_number] += Im_Bin_Ch4[bin_number]*Re_Bin_Ch0[bin_number]-
                        Re_Bin_Ch4[bin_number]*Im_Bin_Ch0[bin_number];
}
