#ifndef DEFINES_SW_H
 
#define DEFINES_SW_H

////////////////////////////////////////////////
#define SIZE_S_SW                     3072                    // (count_bin*count_read) размерность массивов спектральных отсчётов
#define SIZE_NAC_SW                   10					  // Размерность ReW_Bin[],ImW_Bin[]
#define SIZE_FORM_SW			      100                     // Размерность выходного массива  Mas_Signals_Inform
////////////////////////////////////////////////


#define SIZE_INF   				   11        // размер массива Mas_Inform_AC_Loop

#define SIZE_MAS_AC_SW             336       // размер массива  юстировочных коэффициентов Mas_AC_Loop

#define SIZE_U_SW    			   168       // c_azimuth * c_channel для ReU, ImU

#define SIZE_SW     			   9         // mas_ReW, mas_ImW

#define SIZE_Q					   3 		 // mas_ReQ. mas_ImQ размерность

#define SIZE_ACCUM                 10

#define SIZE_THETA				   56		 // размер массива  Mtheta, znam_Mtheta 

#define	SIZE_MAS				   56        // размер массива Mas (массива суммы значений смежных пар эквидистантных точек пеленгационного рельефа)
		
#define PII                        3.1415926535

#define detection_norm_coeff_7     0.047619047619048 


//#define ddelta					   0.001745	

#endif  /* DEFINES_SW_H */
