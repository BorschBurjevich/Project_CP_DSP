/*������� ���������� ���������� ���� ����� (��������, �������� � �����������) ����� �������� ��������� ���������� ��������*/

#include <Bearing_SW/DEFINES_SW.h>
#include <Bearing_SW/FUNC_SW.h>
#include "variables_SW.h"

void FUNC_Calculating_Derivate_Value_Argue_Points_SW (int ind_max, float *mas, int count_point)
{
	
	float inv_point;   // ��� ����� (2Pi / c_azimuth) 

	//	������������� ����������
			
	inv_point = (float) 6.283185307179586 / count_point;
	xL		  = (float) ind_max * inv_point;   //  � ��������
	yL        = mas[ind_max];
	
	// ������ ������� ���������� � ����� x �������� �����������  2Pi -������������� ��������������� �������,
	//�������� �������� ���������� �������� � ������� ����������������� ���� ������������
	
	FUNC_Modif_Kotel_Derivative_Point_SW(xL,&mas[0],count_point);
	dyL       = dy;
	
	if (dyL >= 0.0)
	{
		
		if (ind_max == count_point)
		{
		    
			xR  = 0.0;
			yR  = mas[0];
			FUNC_Modif_Kotel_Derivative_Point_SW(xR,&mas[0],count_point);
			dyR = dy;
			
			if (dyR > 0.0)
			{
					
				xL  = xR;
				yL  = yR;
				dyL = dyR;
				xR  = inv_point;
				yR  = mas[1];      
				FUNC_Modif_Kotel_Derivative_Point_SW(xR,&mas[0],count_point);
				dyR = dy; 
					
			}
			
		}
			
		else
		{
			xR  = (float) ind_max * inv_point;
			yR  = mas[ind_max];
			FUNC_Modif_Kotel_Derivative_Point_SW(xR,&mas[0],count_point);
			dyR = dy;	
			if (dyR > 0.0)
			{
				xL  = xR;
				yL  = yR;
				dyL = dyR;
					
				if (ind_max + 1 == count_point)
				{
					xR  = 0;
					yR  = mas[0];
								
				}
				else
				{
					xR  = (float)(ind_max + 1) * inv_point;
					yR  = mas[ind_max + 1];
							
				}
					
				FUNC_Modif_Kotel_Derivative_Point_SW(xR,&mas[0],count_point);
				dyR = dy;
					
								
			}
			
				
					
		}
								
								
	}
	else
	{
		if (ind_max == 1)
		{
			xR  = xL;
			yR  = yL;
			dyR = dyL;
			xL  = (count_point - 1) * inv_point;
			yL  = mas[count_point - 1];
			FUNC_Modif_Kotel_Derivative_Point_SW(xL,&mas[0],count_point);
			dyL = dy;
			
				
		}
		else
		{
			xR  = xL;
			yR  = yL;
			dyR = dyL;
			xL  = (float)(ind_max - 2.0) * inv_point;
			yL  = mas[ind_max - 2];
			FUNC_Modif_Kotel_Derivative_Point_SW(xL,&mas[0],count_point);
			dyL = dy;
			
				
		}					
			
	}
			
}
