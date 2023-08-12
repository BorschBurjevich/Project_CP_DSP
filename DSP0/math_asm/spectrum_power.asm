	;  void spectrum_power(float * cmplx_sp,float * spp, N);
	; ������� ���������� spp = re^2 + im^2
	; ������� �������� ������� 2*N
	; ������ ��������� ������� N
.text
	.global spectrum_power_asm
spectrum_power_asm:

		LDW     .D1		*A4++,A0 	; ������� RE ������
    	LDW     .D1   	*A4++,A1 	; ������� IM ������
		LDW		.D2   	*B4,A16	 	; ������� spp ������
||		MV		.L2		A6, B1   	; ������� �������� N
		NOP     4

LOOP:
		MPYSP	.M1     A0,A0,A16	; ������������ RE[0]^2
		MPYSP	.M1     A1,A1,A17	; ������������ IM[0]^2
		NOP     3

        FADDSP	.L1		A16,A17,A18 ; RE[0]^2 + IM[0]^2 = spp[0]
||		LDW     .D1		*A4++,A0 	; ������� RE ������
    	LDW     .D1   	*A4++,A1 	; ������� IM ������
    	NOP
		STW     .D2     A18,*B4++
||		SUB		.S2		B1, 1, B1		;��������� ������� �������� �� 1
||	[B1]B		.S1		LOOP		;���� �������� �������� �������� �� ����� 0

		NOP		5

		B		.S2		B3
		NOP		5

