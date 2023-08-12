#include <Bearing_USW/Header_Func_USW_5.h>
#include "Header.h"
#include "variables_USW.h"
#include "defines.h"


float Correlation_Detector_5(int number_bin)  // ( на вход поступает текущий спектральный отчёт / бин )

{
	
	int   i;
   	int   j;
    j = 0;

    float   rv0 = 0, iv0 = 0,
            rv1 = 0, iv1 = 0,
            rv2 = 0, iv2 = 0,
            rv3 = 0, iv3 = 0,
            rv4 = 0, iv4 = 0,
            stat_detection = 0;

    float   *ptr0 = 0,
            *ptr1 = 0,
            *ptr2 = 0,
            *ptr3 = 0,
            *ptr4 = 0;

    *ptr0 = word11;
    *ptr1 = word12;
    *ptr2 = word13;
    *ptr3 = word14;
    *ptr4 = word15;

    //Инициализациа элементов матрицы взаимных энергий
    memset(ReW_Bin, 0, sizeof(ReW_Bin)*28);
    memset(ImW_Bin, 0, sizeof(ImW_Bin)*28);


   	//Накопление элементов матрицы взаимных энергий
   for (i = 0; i < count_read; i++ )      
   {
      //  j   = number_bin+i*count_bin;
        j   = 2*number_bin+i*count_bin;

        rv0 = ptr0[j];   iv0 = ptr0[j+1];
        rv1 = ptr1[j];   iv1 = ptr1[j+1];
        rv2 = ptr2[j];   iv2 = ptr2[j+1];
        rv3 = ptr3[j];   iv3 = ptr3[j+1];
        rv4 = ptr4[j];   iv4 = ptr4[j+1];

       /* rv0 = ReS0[j];   iv0 = ImS0[j];
        rv1 = ReS1[j];   iv1 = ImS1[j];
        rv2 = ReS2[j];   iv2 = ImS2[j];
        rv3 = ReS3[j];   iv3 = ImS3[j];
        rv4 = ReS4[j];   iv4 = ImS4[j];*/

        ReW_Bin[0]  += rv0*rv0 + iv0*iv0;  // элементы главной диагонали
        ReW_Bin[7]  += rv1*rv1 + iv1*iv1;
        ReW_Bin[13] += rv2*rv2 + iv2*iv2;
        ReW_Bin[18] += rv3*rv3 + iv3*iv3;
        ReW_Bin[22] += rv4*rv4 + iv4*iv4;

        ReW_Bin[1]  += rv0*rv1 + iv0*iv1; 	ImW_Bin[1]  += iv0*rv1 - rv0*iv1;
        ReW_Bin[2]  += rv0*rv2 + iv0*iv2; 	ImW_Bin[2]  += iv0*rv2 - rv0*iv2;
        ReW_Bin[3]  += rv0*rv3 + iv0*iv3;	ImW_Bin[3]  += iv0*rv3 - rv0*iv3;
        ReW_Bin[4]  += rv0*rv4 + iv0*iv4; 	ImW_Bin[4]  += iv0*rv4 - rv0*iv4;

        ReW_Bin[8]  += rv1*rv2 + iv1*iv2; 	ImW_Bin[8]  += iv1*rv2 - rv1*iv2;
        ReW_Bin[9]  += rv1*rv3 + iv1*iv3; 	ImW_Bin[9]  += iv1*rv3 - rv1*iv3;
        ReW_Bin[10] += rv1*rv4 + iv1*iv4; 	ImW_Bin[10] += iv1*rv4 - rv1*iv4;

        ReW_Bin[14] += rv2*rv3 + iv2*iv3; 	ImW_Bin[14] += iv2*rv3 - rv2*iv3;
        ReW_Bin[15] += rv2*rv4 + iv2*iv4; 	ImW_Bin[15] += iv2*rv4 - rv2*iv4;

        ReW_Bin[19] += rv3*rv4 + iv3*iv4; 	ImW_Bin[19] += iv3*rv4 - rv3*iv4;
		 
		 
		 		
    }
	
    // Формирование решающей статистики
    stat_detection  = (ReW_Bin[1] *ReW_Bin[1]  + ImW_Bin[1] *ImW_Bin[1])  / (ReW_Bin[0] *ReW_Bin[7]);
    stat_detection += (ReW_Bin[2] *ReW_Bin[2]  + ImW_Bin[2] *ImW_Bin[2])  / (ReW_Bin[0] *ReW_Bin[13]);
    stat_detection += (ReW_Bin[3] *ReW_Bin[3]  + ImW_Bin[3] *ImW_Bin[3])  / (ReW_Bin[0] *ReW_Bin[18]);
    stat_detection += (ReW_Bin[4] *ReW_Bin[4]  + ImW_Bin[4] *ImW_Bin[4])  / (ReW_Bin[0] *ReW_Bin[22]);


    stat_detection += (ReW_Bin[8] *ReW_Bin[8]  + ImW_Bin[8] *ImW_Bin[8])  / (ReW_Bin[7] *ReW_Bin[13]);
    stat_detection += (ReW_Bin[9] *ReW_Bin[9]  + ImW_Bin[9] *ImW_Bin[9])  / (ReW_Bin[7] *ReW_Bin[18]);
    stat_detection += (ReW_Bin[10]*ReW_Bin[10] + ImW_Bin[10]*ImW_Bin[10]) / (ReW_Bin[7] *ReW_Bin[22]);
    

    stat_detection += (ReW_Bin[14]*ReW_Bin[14] + ImW_Bin[14]*ImW_Bin[14]) / (ReW_Bin[13]*ReW_Bin[18]);
    stat_detection += (ReW_Bin[15]*ReW_Bin[15] + ImW_Bin[15]*ImW_Bin[15]) / (ReW_Bin[13]*ReW_Bin[22]);
    
    
    stat_detection += (ReW_Bin[19]*ReW_Bin[19] + ImW_Bin[19]*ImW_Bin[19]) / (ReW_Bin[18]*ReW_Bin[22]);

	
   
	  
   
   //Нормировка решающей статистики  	
   stat_detection = stat_detection * detection_norm_coeff_5; 

   return stat_detection;
}	
	

