////////////////////////// Alg № 9 /////////////////////////////////

#include "variables_USW.h"


void Finding_Coord_Vertex_USW_5( float x_1, float  x_2, float y_1, float y_2,float dy_1)
{
   float R_0; // вспомогат коэф.
   float R_1;
   float R_2;
   float R_3;
   float R_4;
   float R_5;
   float B_0;
   float B_1;
   float a;    // коэф. уравнепниа  параболы. 
   float b;
   float c;
   
	
   R_0 = x_1 * x_1;
   R_1 = x_2 * x_2;
   R_2 = R_0 - R_1;
   R_3 = x_1 - x_2;
   R_4 = 1.0 / R_3;
   R_5 = y_1  - y_2;
   B_0 = x_1 * x_2 * R_4;
   B_1 = (-R_5) * ( B_0 + x_2 );
	
	///////////// Рассчёт коэф. уравнениа параболоы //////////
	
   a = ( R_5 - (R_3 * dy_1) ) * R_4;
   b = ( (R_2 * dy_1) - (2.0 * x_1 * R_5) ) * R_4;
   c = B_1 * R_4 + B_0 * dy_1 + y_2;
	
	///////////  Коорд. вершины параболы  //////////////////////
	
   Mas_Vertex[0] = ((-0.5) * b) / a;     // абсцисса
   Mas_Vertex[1] = c - (0.5 * b * R_4 * Mas_Vertex[0]); // ордината
  // Mas_Vertex[2] = F_shag;
}
