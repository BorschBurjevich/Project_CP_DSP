#include <Bearing_SW/DEFINES_SW.h>
#include <Bearing_SW/FUNC_SW.h>
#include "variables_SW.h"


void FUNC_Main_Detection_Grouping_Direction_Finding_SW()
{ 
// ��������� ���������� � �������
	int i,j;
	int count_signal;
	int bin_in_signal;
	int flag_group_open;
	int flag_group_close;	
	int start_group_bin;
	int stop_group_bin;	
	int count_adjacent_bin;
	int max_adjacent_bin;
		
	float P;
	float numerator_freq;
	float denominator_freq;	
	
	//������������� ���������� � ��������
	count_signal          = 0;
	bin_in_signal         = 0;	
	flag_group_open       = 0;
	flag_group_close      = 0;
	numerator_freq        = 0.0;
	denominator_freq      = 0.0;
	count_bin_SW          = word13;
	
	/////////////////////////////

		
	for(i = 0; i < count_bin_SW; i++)
	{
		//������ �������� ���������� �������������� (����������, ������� ��� �������) ������������� �������
		FUNC_Correlation_Detector_4_SW(count_read_SW,count_bin_SW,&ReS0_SW[0],&ReS1_SW[0],&ReS2_SW[0],&ReS3_SW[0],&ImS0_SW[0],&ImS1_SW[0],&ImS2_SW[0],&ImS3_SW[0],i);
		
		if(stat_detection_SW >= bound_detection_SW)
		{
			
			
			//����������� �������� "�����������, ������������� � ������������"
			if(flag_group_open == 1)
			{
				//�������� 2-� ������� ���������� ����� �� �������� ������� (���������) ��������
				if((i-stop_group_bin) <= bound_max_distance_SW)
				{
					//������ �������� ���������� ������������� 2-� ������������ ��������
					FUNC_Correlation_Grouping_4_SW(&ReW_Accumulation_SW[0],&ImW_Accumulation_SW[0],&ReW_Bin_SW[0],&ImW_Bin_SW[0]);
					
					//�������� 2-� ������� ���������� ����� �� �������������� � ������ ������� �� �������� ��������� �������������
					if(stat_grouping_SW >= bound_grouping_SW )
					{
						//���������� ��� ������������ ������� ���������� ������� � ����� ������������� ������� (�������)
                		P                 = ReW_Bin_SW[0]+ReW_Bin_SW[3]+ReW_Bin_SW[5]+ReW_Bin_SW[9];
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
                		for(j = 0; j < 10; j++)
        				{
        					ReW_Accumulation_SW[j] += ReW_Bin_SW[j];
        					ImW_Accumulation_SW[j] += ImW_Bin_SW[j];
        				}

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
					if(bin_in_signal >= bound_min_bin_signal_SW && max_adjacent_bin >= bound_min_adjacent_bin_SW)
					{
						// ��������� ��������� ���������������� ������� (����������� �������)
						central_Freq_Signal_SW = (POO_CurrentCentralFreq_SW - Half_Band_POO_SW)+ bin_resolution_POO_SW*numerator_freq/denominator_freq;
                    	begin_Freq_Signal_SW   = (POO_CurrentCentralFreq_SW - Half_Band_POO_SW)+ bin_resolution_POO_SW*start_group_bin;
                    	end_Freq_Signal_SW     = (POO_CurrentCentralFreq_SW - Half_Band_POO_SW)+ bin_resolution_POO_SW*stop_group_bin;
                    	 
                    	//���� ������������ �������� mas_ReW, mas_ImW, mas_ReQ, mas_ImQ
                    	mas_ReW[0] = ReW_Accumulation_SW[0]; mas_ImW[0] =  0.0;
                    	mas_ReW[1] = ReW_Accumulation_SW[1]; mas_ImW[1] =  ImW_Accumulation_SW[1];
                    	mas_ReW[2] = ReW_Accumulation_SW[2]; mas_ImW[2] =  ImW_Accumulation_SW[2];
                    	mas_ReW[3] = ReW_Accumulation_SW[1]; mas_ImW[3] = -ImW_Accumulation_SW[1];
                    	mas_ReW[4] = ReW_Accumulation_SW[3]; mas_ImW[4] =  0.0;
                    	mas_ReW[5] = ReW_Accumulation_SW[4]; mas_ImW[5] =  ImW_Accumulation_SW[4];
                    	mas_ReW[6] = ReW_Accumulation_SW[2]; mas_ImW[6] = -ImW_Accumulation_SW[2];
                    	mas_ReW[7] = ReW_Accumulation_SW[4]; mas_ImW[7] = -ImW_Accumulation_SW[4];
                    	mas_ReW[8] = ReW_Accumulation_SW[5]; mas_ImW[8] =  0.0;

                    	mas_ReQ[0] = ReW_Accumulation_SW[6]; mas_ImQ[0] = ImW_Accumulation_SW[6];
                    	mas_ReQ[1] = ReW_Accumulation_SW[7]; mas_ImQ[1] = ImW_Accumulation_SW[7];
                    	mas_ReQ[2] = ReW_Accumulation_SW[8]; mas_ImQ[2] = ImW_Accumulation_SW[8];


                    	
                    	// ����� ������� ������� ����� "������������"
                    	FUNC_Direction_F_SW();
                    	
                    	//������ ����������� �� ������� (����������� �������) � �������� ������


                    	Inform_SW.signal[count_signal].azimuth  = azimuth_Mtheta_SW;// ������, ����.
                    	Inform_SW.signal[count_signal].elevation = 0.0;// ���� �����, ����.
                        Inform_SW.signal[count_signal].central_Freq = central_Freq_Signal_SW;// ����������� �������, ���
                        Inform_SW.signal[count_signal].width  = end_Freq_Signal_SW-begin_Freq_Signal_SW;; // ������, ���
                        Inform_SW.signal[count_signal].left_Freq  = begin_Freq_Signal_SW;// ��������� �������, ���
                        Inform_SW.signal[count_signal].right_Freq  = end_Freq_Signal_SW;// �������� �������, ���
                        Inform_SW.signal[count_signal].energy_db  = AMP_IRI_dB_SW;// ������� �������, ��
                        Inform_SW.signal[count_signal].coeff_reliability  = coeff_reability_SW;// ����������� �������������
                        Inform_SW.signal[count_signal].max_value_Mtheta  = max_value_SDN;// ������������ �������� ��������� ��������������
                        Inform_SW.signal[count_signal].reserved = 0.0;

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
        	if(flag_group_open == 0)
        	{
        		//������������� �������� �������� � ������ ������ ��������� ����������� ������� �������� ������� ������ ������������� �������
        		for(j = 0; j < 10; j++)
        		{
        			ReW_Accumulation_SW[j] = ReW_Bin_SW[j];
        			ImW_Accumulation_SW[j] = ImW_Bin_SW[j];
        		}
        		
        		//������������� ���������� ��� ������������ ���������� � ������� ���������� ������� � ����� ������ ������������� ������� (�������)
            	P                  = ReW_Bin_SW[0]+ReW_Bin_SW[3]+ReW_Bin_SW[5]+ReW_Bin_SW[9];
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
            if(stat_detection_SW < bound_noise_SW)
            {

            }
        }
	  
	} 	

	//��������� ������, ����� ��������� ��� (������ ����: (count_bin-1)) ���������� � ��������� (������������) � ����������� ������� 
	if(flag_group_open == 1 && bin_in_signal >= bound_min_bin_signal_SW && max_adjacent_bin >= bound_min_adjacent_bin_SW)
	{
		// ��������� ��������� ���������������� ������� (����������� �������)
		central_Freq_Signal_SW = (POO_CurrentCentralFreq_SW - Half_Band_POO_SW)+ bin_resolution_POO_SW*numerator_freq/denominator_freq;
    	begin_Freq_Signal_SW   = (POO_CurrentCentralFreq_SW - Half_Band_POO_SW)+ bin_resolution_POO_SW*start_group_bin;
    	end_Freq_Signal_SW     = (POO_CurrentCentralFreq_SW - Half_Band_POO_SW)+ bin_resolution_POO_SW*stop_group_bin;
    
    	//���� ������������ �������� mas_ReW, mas_ImW, mas_ReQ, mas_ImQ
    	mas_ReW[0] = ReW_Accumulation_SW[0]; mas_ImW[0] =  0.0;
    	mas_ReW[1] = ReW_Accumulation_SW[1]; mas_ImW[1] =  ImW_Accumulation_SW[1];
    	mas_ReW[2] = ReW_Accumulation_SW[2]; mas_ImW[2] =  ImW_Accumulation_SW[2];
    	mas_ReW[3] = ReW_Accumulation_SW[1]; mas_ImW[3] = -ImW_Accumulation_SW[1];
    	mas_ReW[4] = ReW_Accumulation_SW[3]; mas_ImW[4] =  0.0;
    	mas_ReW[5] = ReW_Accumulation_SW[4]; mas_ImW[5] =  ImW_Accumulation_SW[4];
    	mas_ReW[6] = ReW_Accumulation_SW[2]; mas_ImW[6] = -ImW_Accumulation_SW[2];
    	mas_ReW[7] = ReW_Accumulation_SW[4]; mas_ImW[7] = -ImW_Accumulation_SW[4];
    	mas_ReW[8] = ReW_Accumulation_SW[5]; mas_ImW[8] =  0.0;

    	mas_ReQ[0] = ReW_Accumulation_SW[6]; mas_ImQ[0] = ImW_Accumulation_SW[6];
    	mas_ReQ[1] = ReW_Accumulation_SW[7]; mas_ImQ[1] = ImW_Accumulation_SW[7];
    	mas_ReQ[2] = ReW_Accumulation_SW[8]; mas_ImQ[2] = ImW_Accumulation_SW[8];
    	// ����� ������� ������� ����� "������������"
    	FUNC_Direction_F_SW();
                    	
    	//������ ����������� �� ������� (����������� �������) � �������� ������


    	Inform_SW.signal[count_signal].azimuth  = azimuth_Mtheta_SW;// ������, ����.
    	Inform_SW.signal[count_signal].elevation = 0.0;// ���� �����, ����.
        Inform_SW.signal[count_signal].central_Freq = central_Freq_Signal_SW;// ����������� �������, ���
        Inform_SW.signal[count_signal].width  = end_Freq_Signal_SW-begin_Freq_Signal_SW;; // ������, ���
        Inform_SW.signal[count_signal].left_Freq  = begin_Freq_Signal_SW;// ��������� �������, ���
        Inform_SW.signal[count_signal].right_Freq  = end_Freq_Signal_SW;// �������� �������, ���
        Inform_SW.signal[count_signal].energy_db  = AMP_IRI_dB_SW;// ������� �������, ��
        Inform_SW.signal[count_signal].coeff_reliability  = coeff_reability_SW;// ����������� �������������
        Inform_SW.signal[count_signal].max_value_Mtheta  = max_value_SDN;// ������������ �������� ��������� ��������������
        Inform_SW.signal[count_signal].reserved = 0.0;
                    
     	//������� ��������������� �������� (���������� ��������)
    	count_signal                = count_signal+1;
   
	}
    

	//������ ���-�� ������������ � ��������������� �������� (���������� ��������) � ������ �������������� ������ (���)
    Inform_SW.num_signals = count_signal;
    word08 = (uint32_t)&Inform_SW;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


}		
