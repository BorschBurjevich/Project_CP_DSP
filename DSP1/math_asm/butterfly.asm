	; void butterfly(float *sp_in,float * sp_out, half_N);
	; ���������� ������� ��� �������
	; float *sp_in �������  ������
	; float *sp_out ��������  ������
	; half_N - �������� ����� ��������� ��������

.sect
	.global butterfly
butterfly:

		MPY		.M1		A6,0x4,A0   ; ����� �� 0 �� ������ � ������
||		MV		.L2		A6, B1   	 ; ������� �������� N
		NOP     2

		ADD		.L1		B4,A0,A1	; ������ ������������ �������� ��������� �������
		ADD		.L1		A4,A0,A2	; ������ ������������ �������� �������� �������
||		LDW     .D1		*A4++,A3 	; ������� sp_in RE ��������
    	LDW     .D1   	*A4++,A5 	; ������� sp_in IM ��������
||		SUB		.L2		B1,2,B1		; ��������� ������� �������� �� 2

    	LDW     .D1   	*A2++,A7	; ������� sp_in RE �������� �  ������
    	LDW     .D1   	*A2++,A8	; ������� sp_in IM ��������	� ������
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

||		SUB		.S2		B1, 2, B1	;��������� ������� �������� �� 2
||	[B1]B		.S1		LOOP		;���� �������� �������� �������� �� ����� 0
		NOP		5

		B		.S2		B3
		NOP		5
