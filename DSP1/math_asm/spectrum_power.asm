	;  void spectrum_power(float * cmplx_sp,float * spp, N);
	; ‘ункци€ вычислени€ spp = re^2 + im^2
	; –азмеры входного вектора 2*N
	; –азмер выходного вектора N
.text
	.global spectrum_power_asm
spectrum_power_asm:

		LDW     .D1		*A4++,A0 	; забрали RE адресс
    	LDW     .D1   	*A4++,A1 	; забрали IM адресс
		LDW		.D2   	*B4,A16	 	; забрали spp адресс
||		MV		.L2		A6, B1   	; забрали значение N
		NOP     4

LOOP:
		MPYSP	.M1     A0,A0,A16	; перемножение RE[0]^2
		MPYSP	.M1     A1,A1,A17	; перемножение IM[0]^2
		NOP     3

        FADDSP	.L1		A16,A17,A18 ; RE[0]^2 + IM[0]^2 = spp[0]
||		LDW     .D1		*A4++,A0 	; забрали RE адресс
    	LDW     .D1   	*A4++,A1 	; забрали IM адресс
    	NOP
		STW     .D2     A18,*B4++
||		SUB		.S2		B1, 1, B1		;уменьшаем счетчик итераций на 1
||	[B1]B		.S1		LOOP		;если значение счетчика итераций не равно 0

		NOP		5

		B		.S2		B3
		NOP		5

