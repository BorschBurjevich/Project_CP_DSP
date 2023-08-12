/*****************************************************************************
 * ALG_LORANDIT_D_UKV_5
 *****************************************************************************/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <ctype.h>
#include "Header_Func_USW_5.h"
#include "Header.h"
#include "variables_USW.h"


void Main_Detection_Grouping_Direction_Finding_USW_5(void)
{ 
// ��������� ���������� � �������
	uint32_t i,count_signal,bin_in_signal,start_group_bin,stop_group_bin,count_adjacent_bin,max_adjacent_bin;

	uint8_t flag_group_open;
	uint8_t flag_group_close;


		
	float P;
	float numerator_freq;
	float denominator_freq;	

	count_bin = word13;
	
	//������������� ���������� � ��������
	count_signal          = 0;
	bin_in_signal         = 0;	
	flag_group_open       = 0;
	flag_group_close      = 0;
	numerator_freq        = 0.0;
	denominator_freq      = 0.0;

	////////////////////////////////
	for(i = 0; i < count_bin; i++)
	{
		//������ �������� ���������� �������������� (����������, ������� ��� �������) ������������� �������
		stat_detection = Correlation_Detector_5(i);
		
		if(stat_detection >= bound_detection_USW)
		{
			
			
			//����������� �������� "�����������, ������������� � ������������"
			if(flag_group_open == 1)
			{
				//�������� 2-� ������� ���������� ����� �� �������� ������� (���������) ��������
				if((i-stop_group_bin) <= bound_max_distance_USW)
				{
					//������ �������� ���������� ������������� 2-� ������������ ��������
					Correlation_Grouping_5(&ReW_Accumulation[0],&ImW_Accumulation[0],&ReW_Bin[0],&ImW_Bin[0]);
					
					//�������� 2-� ������� ���������� ����� �� �������������� � ������ ������� �� �������� ��������� �������������
					if(stat_grouping >= bound_grouping_USW )
					{
						//���������� ��� ������������ ������� ���������� ������� � ����� ������������� ������� (�������)
                		P                 = ReW_Bin[0]+ReW_Bin[7]+ReW_Bin[13]+ReW_Bin[18]+ReW_Bin[22];
                		numerator_freq   += (float)i*P;
                		denominator_freq += P;
                		
                		//���������� ���-�� ����� � ������������ ������� (�������)
                		bin_in_signal     = bin_in_signal+1;
                		
                		//���������� ���-�� ������� ����� � ����� ������������ ������� (�������) � ����� ���������
                		if((i-stop_group_bin) == 1)
                		{
                			count_adjacent_bin = count_adjacent_bin+1;
                			
                			if(count_adjacent_bin > max_adjacent_bin)
                			{
                    			max_adjacent_bin = count_adjacent_bin;
                    		}
                			
                		}
                		else
                		{
                			count_adjacent_bin = 1;
                		}
                		
                		//���������� �������� � ������ ������ ��������� ����������� ������� �������� ������� ������������� �������
                		ReW_Accumulation[0]  += ReW_Bin[0];
						ReW_Accumulation[1]  += ReW_Bin[1];
						ReW_Accumulation[2]  += ReW_Bin[2];
						ReW_Accumulation[3]  += ReW_Bin[3];
						ReW_Accumulation[4]  += ReW_Bin[4];
						ReW_Accumulation[5]  += ReW_Bin[7];
						ReW_Accumulation[6]  += ReW_Bin[8];
						ReW_Accumulation[7]  += ReW_Bin[9];
						ReW_Accumulation[8]  += ReW_Bin[10];
						ReW_Accumulation[9]  += ReW_Bin[13];
						ReW_Accumulation[10] += ReW_Bin[14];
						ReW_Accumulation[11] += ReW_Bin[15];
						ReW_Accumulation[12] += ReW_Bin[18];
						ReW_Accumulation[13] += ReW_Bin[19];
						ReW_Accumulation[14] += ReW_Bin[22];


						ImW_Accumulation[0]  += ImW_Bin[0];
						ImW_Accumulation[1]  += ImW_Bin[1];
						ImW_Accumulation[2]  += ImW_Bin[2];
						ImW_Accumulation[3]  += ImW_Bin[3];
						ImW_Accumulation[4]  += ImW_Bin[4];
						ImW_Accumulation[5]  += ImW_Bin[7];
						ImW_Accumulation[6]  += ImW_Bin[8];
						ImW_Accumulation[7]  += ImW_Bin[9];
						ImW_Accumulation[8]  += ImW_Bin[10];
						ImW_Accumulation[9]  += ImW_Bin[13];
						ImW_Accumulation[10] += ImW_Bin[14];
						ImW_Accumulation[11] += ImW_Bin[15];
						ImW_Accumulation[12] += ImW_Bin[18];
						ImW_Accumulation[13] += ImW_Bin[19];
						ImW_Accumulation[14] += ImW_Bin[22];


                		//����������� ������� �������� c��������������� ����
                		stop_group_bin = i;
					}
					else
					{
						flag_group_close = 1;
					}
					
				}
				else
				{
					flag_group_close = 1;
				}
				
				
				//�������� � ��������� �������� ������
				if(flag_group_close == 1)
				{
					if(bin_in_signal >= bound_min_bin_signal_USW && max_adjacent_bin >= bound_min_adjacent_bin_USW)
					{
						// ��������� ��������� ���������������� ������� (����������� �������)
						central_Freq_Signal = (POO_CurrentCentralFreq - Half_Band_POO)+ bin_resolution_POO*numerator_freq/denominator_freq;
                    	begin_Freq_Signal   = (POO_CurrentCentralFreq - Half_Band_POO)+ bin_resolution_POO*start_group_bin;
                    	end_Freq_Signal     = (POO_CurrentCentralFreq - Half_Band_POO)+ bin_resolution_POO*stop_group_bin;
                    	
                    	// ����� ������� ������� ����� "������������"
                    	Direction_Finding_USW_5();
                    	
                    	//������ ����������� �� ������� (����������� �������) � �������� ������
                    	Inform.signal[count_signal].azimuth  = azimuth_Mtheta;// ������, ����.
                        Inform.signal[count_signal].elevation = 0.0;// ���� �����, ����.
                        Inform.signal[count_signal].central_Freq = central_Freq_Signal;// ����������� �������, ���
                        Inform.signal[count_signal].width  = end_Freq_Signal-begin_Freq_Signal; // ������, ���
                        Inform.signal[count_signal].left_Freq  = begin_Freq_Signal;// ��������� �������, ���
                        Inform.signal[count_signal].right_Freq  = end_Freq_Signal;// �������� �������, ���
                        Inform.signal[count_signal].energy_db  = Amp_IRI_db;// ������� �������, ��
                        Inform.signal[count_signal].coeff_reliability  = coeff_reliability;// ����������� �������������
                        Inform.signal[count_signal].max_value_Mtheta  = max_value_Mtheta;// ������������ �������� ��������� ��������������
                        Inform.signal[count_signal].reserved = 0.0;


                    	//������� ��������������� �������� (���������� ��������)
                    	count_signal                = count_signal+1;
					}
					
					//��������� ����� �������� ������ ��� ������ ����� �������� "�����������, ������������� � ������������"
                    flag_group_open   = 0;
                    
                   	//��������� ����� �������� ������ ��� ������ ����� �������� "�����������, ������������� � ������������"
                    flag_group_close = 0;
				}
				
			}

			
			
			
			//������ ����� �������� "�����������, ������������� � ������������"
        	if (flag_group_open == 0)
        	{
        		//������������� �������� �������� � ������ ������ ��������� ����������� ������� �������� ������� ������ ������������� �������
        		

        		ReW_Accumulation[0]  = ReW_Bin[0];
        		ReW_Accumulation[1]  = ReW_Bin[1];
        		ReW_Accumulation[2]  = ReW_Bin[2];
        		ReW_Accumulation[3]  = ReW_Bin[3];
        		ReW_Accumulation[4]  = ReW_Bin[4];
        		ReW_Accumulation[5]  = ReW_Bin[7];
        		ReW_Accumulation[6]  = ReW_Bin[8];
        		ReW_Accumulation[7]  = ReW_Bin[9];
        		ReW_Accumulation[8]  = ReW_Bin[10];
        		ReW_Accumulation[9]  = ReW_Bin[13];
        		ReW_Accumulation[10] = ReW_Bin[14];
        		ReW_Accumulation[11] = ReW_Bin[15];
        		ReW_Accumulation[12] = ReW_Bin[18];
        		ReW_Accumulation[13] = ReW_Bin[19];
        		ReW_Accumulation[14] = ReW_Bin[22];
        		
        		
        		ImW_Accumulation[0]  = ImW_Bin[0];
        		ImW_Accumulation[1]  = ImW_Bin[1];
        		ImW_Accumulation[2]  = ImW_Bin[2];
        		ImW_Accumulation[3]  = ImW_Bin[3];
        		ImW_Accumulation[4]  = ImW_Bin[4];
        		ImW_Accumulation[5]  = ImW_Bin[7];
        		ImW_Accumulation[6]  = ImW_Bin[8];
        		ImW_Accumulation[7]  = ImW_Bin[9];
        		ImW_Accumulation[8]  = ImW_Bin[10];
        		ImW_Accumulation[9]  = ImW_Bin[13];
        		ImW_Accumulation[10] = ImW_Bin[14];
        		ImW_Accumulation[11] = ImW_Bin[15];
        		ImW_Accumulation[12] = ImW_Bin[18];
        		ImW_Accumulation[13] = ImW_Bin[19];
        		ImW_Accumulation[14] = ImW_Bin[22];
        		

        		//������������� ���������� ��� ������������ ���������� � ������� ���������� ������� � ����� ������ ������������� ������� (�������)
            	P                  = ReW_Bin[0]+ReW_Bin[7]+ReW_Bin[13]+ReW_Bin[18]+ReW_Bin[22];
            	numerator_freq     = (float)i*P;
            	denominator_freq   = P;
            	
            	//������������� �������� ���-�� ����� � ����� ������������ ������� (�������)
            	bin_in_signal      = 1;
            	
            	//������������� �������� ���-�� ������� ����� � ����� ������������ ������� (�������) � �������� ������������� ���-�� ������� ���
            	count_adjacent_bin = 1;
            	max_adjacent_bin   = 1;
            	
            	//������������� ���������� � ��������� ����� ������ ������������� ������� (�������)
            	start_group_bin    = i;
            	stop_group_bin     = i;
            	
            	//������������� ����� ������������� ��� ����������� ����� �������� "�����������, ������������� � ������������"
            	flag_group_open     = 1;

        	}	
		}
		else
		{
			if(stat_detection < bound_noise_USW)
			{
				
			}	
		}
	} 	




//��������� ������, ����� ��������� ��� (������ ����: (count_bin-1)) ���������� � ��������� (������������) � ����������� ������� 
    if(flag_group_open == 1 && bin_in_signal >= bound_min_bin_signal_USW && max_adjacent_bin >= bound_min_adjacent_bin_USW)
    {
        // ��������� ��������� ���������������� ������� (����������� �������)
        central_Freq_Signal = (POO_CurrentCentralFreq - Half_Band_POO)+ bin_resolution_POO*numerator_freq/denominator_freq;
        begin_Freq_Signal   = (POO_CurrentCentralFreq - Half_Band_POO)+ bin_resolution_POO*start_group_bin;
        end_Freq_Signal     = (POO_CurrentCentralFreq - Half_Band_POO)+ bin_resolution_POO*stop_group_bin;

        // ����� ������� ������� ����� "������������"
        Direction_Finding_USW_5();

        Inform.signal[count_signal].azimuth  = azimuth_Mtheta;// ������, ����.
        Inform.signal[count_signal].elevation = 0.0;// ���� �����, ����.
        Inform.signal[count_signal].central_Freq = central_Freq_Signal;// ����������� �������, ���
        Inform.signal[count_signal].width  = end_Freq_Signal-begin_Freq_Signal; // ������, ���
        Inform.signal[count_signal].left_Freq  = begin_Freq_Signal;// ��������� �������, ���
        Inform.signal[count_signal].right_Freq  = end_Freq_Signal;// �������� �������, ���
        Inform.signal[count_signal].energy_db  = Amp_IRI_db;// ������� �������, ��
        Inform.signal[count_signal].coeff_reliability  = coeff_reliability;// ����������� �������������
        Inform.signal[count_signal].max_value_Mtheta  = max_value_Mtheta;// ������������ �������� ��������� ��������������
        Inform.signal[count_signal].reserved = 0.0;


        //������� ��������������� �������� (���������� ��������)
        count_signal                = count_signal+1;

    }

    
    //������ ���-�� ������������ � ��������������� �������� (���������� ��������) � ������ �������������� ������ (���)
    Inform.num_signals = count_signal;
    word08 = (uint32_t)&Inform;

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////\



