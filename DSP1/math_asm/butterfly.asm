	; void butterfly(float *sp_in,float * sp_out, half_N);
	; Вычисление бабочки для спектра
	; float *sp_in входной  спектр
	; float *sp_out выходной  спектр
	; half_N - половина числа элементов массивов

.sect
	.global butterfly
butterfly:

		MPY		.M1		A6,0x4,A0   ; сдвиг от 0 до центра в словах
||		MV		.L2		A6, B1   	 ; забрали значение N
		NOP     2

		ADD		.L1		B4,A0,A1	; адресс центрального элемента выходного массива
		ADD		.L1		A4,A0,A2	; адресс центрального элемента входного массива
||		LDW     .D1		*A4++,A3 	; забрали sp_in RE значение
    	LDW     .D1   	*A4++,A5 	; забрали sp_in IM значение
||		SUB		.L2		B1,2,B1		; уменьшаем счетчик итераций на 2

    	LDW     .D1   	*A2++,A7	; забрали sp_in RE значение с  центра
    	LDW     .D1   	*A2++,A8	; забрали sp_in IM значение	с центра
		NOP		2
LOOP:

		STW     .D1     A3,*A1++
		STW     .D1     A5,*A1++
		STW     .D2     A7,*B4++
		STW     .D2     A8,*B4++

		LDW     .D1		*A4++,A3
    	LDW     .D1   	*A4++,A5
    	LDW     .D1   	*A2++,A7
    	LDW     .D1   	*A2++,A8

||		SUB		.S2		B1, 2, B1	;уменьшаем счетчик итераций на 2
||	[B1]B		.S1		LOOP		;если значение счетчика итераций не равно 0
		NOP		5

		B		.S2		B3
		NOP		5
