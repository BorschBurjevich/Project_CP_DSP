//////////////// Alg�6 ������������ ����� ��������� �������� ������� ///////////////////////

#include <Bearing_USW/Header_Func_USW_5.h>
#include <math.h>
#include "Header.h"
#include "variables_USW.h"

void Iteration_Search_Global_Max_USW_5  (float *rW_signal, float *iW_signal)
{
    
	int 	 na;
	int 	 nc;
	int 	 i;
	int 	 max_index;
	float    B1;
	float    max_x_Vertex; 		  //�������� ��� ������������� �������� ������� �������� 
	float    max_y_Vertex; 		  //�������� �������� ������������� �������� ������� ��������
	int      index_max_Vertex;     //������ ���� ������� ���������� ������������ ��������
	float 	 xL;
	float 	 yL;           		  //���������� ������� �������� 
	float 	 yR;
	float 	 xR;
	float 	 dyL;          		  //�������� ������ �����������
	float 	 dyR;
	float 	 max_elem;     		  // �������� ������������� �������� �� ������� M_teta 
	float 	 elem_ReU;
	float 	 elem_ImU;
	float    x_1,  x_2, y_1,  y_2,  dy_1;
	float    Multi;      
      
 ////////////////////  ����� ���� ���� ������� M_teta //////////////
 
    Multi    			= 2 * M_PI * Znam;
    max_elem 			= Mteta[0];
    max_index           = 0;

    
    for ( na = 1; na < c_azimuth; na++ )
    {
   	    
   	      if (Mteta[na] > max_elem)
   	      {
   	  	      max_elem  = Mteta[na];
   	  	      max_index = na;
   	      }
   	 
   	    	    	 
    }
        	
    max_value_Mtheta   = max_elem;  
    B1 			       = 0.0;
    azimuth_Mtheta     = Multi * max_index;
   
   
   ////////////////////// ������ ���������  ������������� �������     ////////////////////////////////
   
    for ( nc = 0; nc < c_channel; nc++ )
    {
          elem_ReU = ReU[ max_index + nc * c_azimuth ];
          elem_ImU = ImU[ max_index + nc * c_azimuth ];  	
          B1 	   = B1 + (elem_ReU * elem_ReU) + (elem_ImU * elem_ImU) ;
       
            	
    }
                
    Amp_Azimuth_Mtheta = max_value_Mtheta / B1;
    Amp_IRI_db          = 10 * log10(Amp_Azimuth_Mtheta);
//////////////////// ����������� 2-� ������� ���������� �������� ����� ���. ����� ����. ����. �������� �����. ///////////////

   	max_x_Vertex     = 0.0;
   	max_y_Vertex     = 0.0;
   	index_max_Vertex = 0;
/////////////////////////////////////////////////////////////////////////////
    for ( na = 0; na < c_azimuth ; na++ )
    {
        if ( na == c_azimuth - 1 )
        {
            if ((D_Mteta[na] >= 0.0)&&(D_Mteta[0] <= 0.0))
            {
                xL  = Multi * na;
                yL  = Mteta[na];
                xR  = 0.0;
                yR  = Mteta [0];
                dyL = D_Mteta[na];
                dyR = D_Mteta[0];

                if ( yL >= yR )
                {
                    x_1 = xL;
                    x_2 = xR;
                    y_1 = yL;
                    y_2 = yR;
                    dy_1= dyL;
                    Finding_Coord_Vertex_USW_5(x_1, x_2, y_1, y_2, dy_1);

                    if ( Mas_Vertex[1] > max_y_Vertex  )
                    {
                         index_max_Vertex = na;
                         max_x_Vertex     = Mas_Vertex[0];
                         max_y_Vertex     = Mas_Vertex[1];
                    }
                }

                if ( yR >= yL )
                {
                    x_1  = xR;
                    x_2  = xL;
                    y_1  = yR;
                    y_2  = yL;
                    dy_1 = dyR;
                    Finding_Coord_Vertex_USW_5(x_1, x_2, y_1, y_2, dy_1);

                    if ( Mas_Vertex[1] > max_y_Vertex  )
                    {
                        index_max_Vertex = na;
                        max_x_Vertex     = Mas_Vertex[0];
                        max_y_Vertex       = Mas_Vertex[1];
                    }
                }
            }
        }

        if ( na < c_azimuth - 1 )
        {
            if ((D_Mteta[na] >= 0.0)&&(D_Mteta[na + 1] <= 0.0))
            {
                xL  = Multi * na;
                yL  = Mteta[na];
                xR  = Multi * ( na + 1 );
                yR  = Mteta[na + 1];
                dyL = D_Mteta[na];
                dyR = D_Mteta[na + 1];

                if ( yL >= yR )
                {
                    x_1 = xL;
                    x_2 = xR;
                    y_1 = yL;
                    y_2 = yR;
                    dy_1= dyL;
                    Finding_Coord_Vertex_USW_5(x_1, x_2, y_1, y_2, dy_1);

                    if ( Mas_Vertex[1] > max_y_Vertex  )
                    {
                         index_max_Vertex = na;
                         max_x_Vertex     = Mas_Vertex[0];
                         max_y_Vertex     = Mas_Vertex[1];
                    }
                }

                if ( yR >= yL )
                {
                    x_1  = xR;
                    x_2  = xL;
                    y_1  = yR;
                    y_2  = yL;
                    dy_1 = dyR;
                    Finding_Coord_Vertex_USW_5(x_1, x_2, y_1, y_2, dy_1);

                    if ( Mas_Vertex[1] > max_y_Vertex  )
                    {
                        index_max_Vertex  = na;
                        max_x_Vertex      = Mas_Vertex[0];
                        max_y_Vertex      = Mas_Vertex[1];
                    }
                }
            }
        }
    }

/////////////////////////////////////////////////////////////////////////////////
  
////////////// ��������� ����������� ��������� ��� ������� /////////////////

   x = max_x_Vertex;
  
   if ( index_max_Vertex < c_azimuth - 1 )
   {
  	    xL  = index_max_Vertex * Multi;
  	    xR  = xL + Multi;
  	    yL  = Mteta[index_max_Vertex];
  	    yR  = Mteta[index_max_Vertex + 1];
  	    dyL = D_Mteta[index_max_Vertex];
  	    dyR = D_Mteta[index_max_Vertex + 1];  	
   }
   else if ( index_max_Vertex == c_azimuth - 1)
   {
   	   
   	    xL  = ( c_azimuth - 1 ) * Multi;
        xR  = 2 * M_PI;
        yL  = Mteta[c_azimuth - 1];
        yR  = Mteta[0];
        dyL = D_Mteta[c_azimuth - 1];
        dyR = D_Mteta[0];	
   }	
   
  
//////////    ��������� ��������� � ������ ���������   //////////////////

   for ( i = 0; i < Bound_Coun_It; i++  ) 
   {
   	
///////////   ����� ������� ������� ��������� ���� � ������ ���� ��� �������//////// 
   	
  	  Rasch_Proizv_Znach_Vtochke(&ReW_Accumulation[0],&ImW_Accumulation[0]);
  	 
  	  if  ( Mteta_x >= max_value_Mtheta )
  	   {
  	        max_value_Mtheta   = Mteta_x;
  	        azimuth_Mtheta     = x;
  	        Amp_Azimuth_Mtheta = Amp_IRI_db;
  	   }
  	   
  	  if ( abs(D_Mteta_x) <= Bound_D_Global_Max )
  	   {
  	   	   break;
  	   }
  	   
  	  if ( D_Mteta_x >= 0.0 )
  	   {
  	       xL  = x;
  	       yL  = Mteta_x;
  	       dyL = D_Mteta_x; 	
  	   }
  	   
  	   else 
  	   {
  	       xR  = x;
  	       yR  = Mteta_x;
  	       dyR = D_Mteta_x; 	
  	   }
  	   
  	   x = (xL + xR) * 0.5;
  	   Rasch_Proizv_Znach_Vtochke(&ReW_Accumulation[0],&ImW_Accumulation[0]);
  	   
  	   if ( Mteta_x >= max_value_Mtheta )
  	   {
  	        max_value_Mtheta   = Mteta_x;
  	        azimuth_Mtheta     = x;
  	        Amp_Azimuth_Mtheta = Amp_IRI_db;
  	      //  Amp_Azimuth_Mtheta = 10.0 * log10f( Mteta_x * fletch);
  	   }
  	   
  	   if ( abs(D_Mteta_x) <= Bound_D_Global_Max )       // porog_D - ����� ����������� ������ �������� ���� ���������
  	   {
  	   	    break;
  	   }
  	   
  	   if ( D_Mteta_x >= 0.0 )
  	   {
  	        xL  = x;
  	        yL  = Mteta_x;
  	        dyL = D_Mteta_x; 	
  	   }
  	   
  	   else 
  	   {
  	        xR  = x;
  	        yR  = Mteta_x;
  	        dyR = D_Mteta_x; 	
  	   }
  	   
  	   if ( yL >= yR )
       {
       	    x_1  = xL;
       	    x_2  = xR;
       	    y_1  = yL;
       	    y_2  = yR;
       	    dy_1 = dyL;
       	    Finding_Coord_Vertex_USW_5(x_1, x_2, y_1, y_2, dy_1);
    	    x    = Mas_Vertex[0];
       }
         	
  	   if ( yR >= yL )
       {
       		
       	    x_1  = xR;
       	    x_2  = xL;
       	    y_1  = yR;
       	    y_2  = yL;
       	    dy_1 = dyR;     	
       	    Finding_Coord_Vertex_USW_5(x_1, x_2, y_1, y_2, dy_1);
    	    x    = Mas_Vertex[0]; 
       }
       	       	       	
   }
   
  ////////////// ������ ������������� ////////////////
  
    TrW   = 0.0;
    TrW = rW_signal[0] + rW_signal[5] + rW_signal[9] + rW_signal[12] + rW_signal[14];
    coeff_reliability   = max_value_Mtheta / TrW;


}
