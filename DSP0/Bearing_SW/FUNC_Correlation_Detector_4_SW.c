#include <Bearing_SW/DEFINES_SW.h>
#include <Bearing_SW/FUNC_SW.h>
#include "variables_SW.h"
#include "string.h"
void FUNC_Correlation_Detector_4_SW(int Nr,int Nb,float *Re0,float *Re1,float *Re2,float *Re3,float *Im0,float *Im1,float *Im2,float *Im3,int number_bin)  // ( на вход поступает текущий спектральный отчёт / бин )

{
	
	int   i;
   	int   j;

    float   rv0 = 0, iv0 = 0,
            rv1 = 0, iv1 = 0,
            rv2 = 0, iv2 = 0,
            rv3 = 0, iv3 = 0,
            stat_detection_SW = 0;

    float   *ptr0 = 0,
            *ptr1 = 0,
            *ptr2 = 0,
            *ptr3 = 0;

   	float mean_Amp;       // среднее значение амплитуды
   	int   flag_0,flag_3,flag_5, flag_all;

   
   	//Инициализациа элементов матрицы взаимных энергий
   	memset(&ReW_Bin_SW[0],0,sizeof(ReW_Bin_SW)*10);
   	memset(&ReW_Bin_SW[0],0,sizeof(ReW_Bin_SW)*10);

    *ptr0 = word11;
    *ptr1 = word12;
    *ptr2 = word13;
    *ptr3 = word14;

/*    for(i = 0; i < 10; i++)
    {
        ReW_Bin_SW[i] = 0.0;
        ImW_Bin_SW[i] = 0.0;
    }*/


   	//Накопление элементов матрицы взаимных энергий
    for (i = 0; i < count_read_SW; i++ )
    {	
         //j   = number_bin+i*count_bin_SW;
        j   = 2*number_bin+i*count_bin_SW;

         /*rv0 = Re0[j];   iv0 = Im0[j];
         rv1 = Re1[j];   iv1 = Im1[j];
         rv2 = Re2[j];   iv2 = Im2[j];
         rv3 = Re3[j];   iv3 = Im3[j];*/

        rv0 = ptr0[j];   iv0 = ptr0[j+1];
        rv1 = ptr1[j];   iv1 = ptr1[j+1];
        rv2 = ptr2[j];   iv2 = ptr2[j+1];
        rv3 = ptr3[j];   iv3 = ptr3[j+1];


        ReW_Bin_SW[0]  += rv0*rv0 + iv0*iv0;  // элементы главной диагонали
        ReW_Bin_SW[3]  += rv1*rv1 + iv1*iv1;
        ReW_Bin_SW[5]  += rv2*rv2 + iv2*iv2;
        ReW_Bin_SW[9]  += rv3*rv3 + iv3*iv3;


        ReW_Bin_SW[1]  += rv0*rv1 + iv0*iv1; 	ImW_Bin_SW[1]  += iv0*rv1 - rv0*iv1;
        ReW_Bin_SW[2]  += rv0*rv2 + iv0*iv2; 	ImW_Bin_SW[2]  += iv0*rv2 - rv0*iv2;
        ReW_Bin_SW[4]  += rv1*rv2 + iv1*iv2;	ImW_Bin_SW[4]  += iv1*rv2 - rv1*iv2;


        ReW_Bin_SW[6]  += rv0*rv3 + iv0*iv3; 	ImW_Bin_SW[6]  += iv0*rv3 - rv0*iv3;
        ReW_Bin_SW[7]  += rv1*rv3 + iv1*iv3; 	ImW_Bin_SW[7]  += iv1*rv3 - rv1*iv3;
        ReW_Bin_SW[8]  += rv2*rv3 + iv2*iv3; 	ImW_Bin_SW[8]  += iv2*rv3 - rv2*iv3;

    }
    
    mean_Amp        = (ReW_Bin_SW[0] + ReW_Bin_SW[3] + ReW_Bin_SW[5]) * 0.3333;
    flag_0			= 1;
    flag_3			= 1;
    flag_5			= 1;
    
    if (mean_Amp-ReW_Bin_SW[0] > 3.0)
    {
    	flag_0  = 0;	               // если флаг 0 , одна из рамок содержит шум
    
    }
    if (mean_Amp-ReW_Bin_SW[3] > 3.0)
    {
    	flag_3  = 0;	
    
    }
    if (mean_Amp-ReW_Bin_SW[5] > 3.0)
    {
    	flag_5  = 0;	
    
    }
    flag_all = flag_0 * flag_3 * flag_5;
    
    if (flag_all == 1)
    {
    	
        // Формирование решающей статистики
        stat_detection_SW  = (ReW_Bin_SW[1] *ReW_Bin_SW[1]  + ImW_Bin_SW[1] *ImW_Bin_SW[1])  / (ReW_Bin_SW[0] *ReW_Bin_SW[3]);
        stat_detection_SW += (ReW_Bin_SW[2] *ReW_Bin_SW[2]  + ImW_Bin_SW[2] *ImW_Bin_SW[2])  / (ReW_Bin_SW[0] *ReW_Bin_SW[5]);
        stat_detection_SW += (ReW_Bin_SW[4] *ReW_Bin_SW[4]  + ImW_Bin_SW[4] *ImW_Bin_SW[4])  / (ReW_Bin_SW[3] *ReW_Bin_SW[5]);

        stat_detection_SW += (ReW_Bin_SW[6] *ReW_Bin_SW[6]  + ImW_Bin_SW[6] *ImW_Bin_SW[6])  / (ReW_Bin_SW[0] *ReW_Bin_SW[9]);
        stat_detection_SW += (ReW_Bin_SW[7] *ReW_Bin_SW[7]  + ImW_Bin_SW[7] *ImW_Bin_SW[7])  / (ReW_Bin_SW[3] *ReW_Bin_SW[9]);

        stat_detection_SW += (ReW_Bin_SW[8] *ReW_Bin_SW[8]  + ImW_Bin_SW[8] *ImW_Bin_SW[8])  / (ReW_Bin_SW[5] *ReW_Bin_SW[9]);

        //Нормировка решающей статистики
        stat_detection_SW = stat_detection_SW * 0.166666;
    
    }
    else
    {
    	if (flag_0 == 0)
    	{
    	
    		stat_detection_SW  = (ReW_Bin_SW[4] *ReW_Bin_SW[4]  + ImW_Bin_SW[4] *ImW_Bin_SW[4])  / (ReW_Bin_SW[3] *ReW_Bin_SW[5]);
    		stat_detection_SW += (ReW_Bin_SW[7] *ReW_Bin_SW[7]  + ImW_Bin_SW[7] *ImW_Bin_SW[7])  / (ReW_Bin_SW[3] *ReW_Bin_SW[9]);
    		stat_detection_SW += (ReW_Bin_SW[8] *ReW_Bin_SW[8]  + ImW_Bin_SW[8] *ImW_Bin_SW[8])  / (ReW_Bin_SW[5] *ReW_Bin_SW[9]);
    		stat_detection_SW  = stat_detection_SW * 0.33333;
    		
    	}
    	
    	if (flag_3 == 0)
    	{
    	
    		stat_detection_SW  = (ReW_Bin_SW[2] *ReW_Bin_SW[2]  + ImW_Bin_SW[2] *ImW_Bin_SW[2])  / (ReW_Bin_SW[0] *ReW_Bin_SW[5]);
    		stat_detection_SW += (ReW_Bin_SW[6] *ReW_Bin_SW[6]  + ImW_Bin_SW[6] *ImW_Bin_SW[6])  / (ReW_Bin_SW[0] *ReW_Bin_SW[9]);
    		stat_detection_SW += (ReW_Bin_SW[8] *ReW_Bin_SW[8]  + ImW_Bin_SW[8] *ImW_Bin_SW[8])  / (ReW_Bin_SW[5] *ReW_Bin_SW[9]);
    		stat_detection_SW  = stat_detection_SW * 0.33333;
    	}
    	
    	if (flag_5 == 0)
    	{
    		stat_detection_SW  = (ReW_Bin_SW[1] *ReW_Bin_SW[1]  + ImW_Bin_SW[1] *ImW_Bin_SW[1])  / (ReW_Bin_SW[0] *ReW_Bin_SW[3]);
    		stat_detection_SW += (ReW_Bin_SW[6] *ReW_Bin_SW[6]  + ImW_Bin_SW[6] *ImW_Bin_SW[6])  / (ReW_Bin_SW[0] *ReW_Bin_SW[9]);
    		stat_detection_SW += (ReW_Bin_SW[7] *ReW_Bin_SW[7]  + ImW_Bin_SW[7] *ImW_Bin_SW[7])  / (ReW_Bin_SW[3] *ReW_Bin_SW[9]);
    		stat_detection_SW  = stat_detection_SW * 0.33333;
    	}	
    		
    }	
    
    			      
}	
