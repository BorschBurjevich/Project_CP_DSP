
 .text
 	   .global func_log


;; Reg A4 - adr K[0], reg B4 - adr Y[0], reg A6 - adr Y16[0]

r29    .set		45.1544993
r28    .set     0.0
r21    .set     0.000030517
r20    .set     1.0 						;; задается множитель перед log(x)
r200   .set     0.5
;; A8 - Mask_Pos, B8 - Mask_Len
func_log:
		LDW     .D2   *B14,A16 	        ; забрали N адресс (A16 = r22, input value)
		NOP     4
		MVKL    .S2    0xFFFFFFF1,B16		;запись младших разрядов
		MVKL    .S1    65535,A15            ;r20=65535 запись младших в A15
		MVKH    .S1    65535,A15		    ;запись старших
		MVKL    .S2    r29,B15              ;запись в B15 младших разрядов константы float
		MVKH    .S2    r29,B15				;запись в B15 старших разрядов константы float
		MVKL    .S1    r28,A14				;запись в A14 константы float
		NOP      2
		SUB     .L1    A15,A16,A15        	;A15 = A16 - A15 ;A15 = r20
	    CMPLT   .L1    A15,A14,A1			;A1 = (A15 < 0.0) ? , A1 = 1 - True, A1 = 0 - False

 [A1]   MV            B15,A14
 [A1]   MVKL    .S2    r21,B12            	;запись в B12 младших разрядов константы r21 float
 [A1]	MVKH    .S2    r21,B12
 [A1]   INTSP   .S1    A16,A16            	; приводим A16 к типу float
 		NOP      5


 [A1]	MPYSP   .M1    B12,A16,A12			;r23 = r22 *  r21 (A12 = r23)
||[A1]  SPINT   .S1    A16,A16				;приводим A16 к типу int
	    NOP		 5

 [A1]   SHRU    .S1    A16,15,A16			; A16 >> 15, A16 = 3;
        MVKL    .S1    r28,A15            	; A15 = 0.0  (r20=0.0)
 [A1]   INTSP   .S1    A16,A16		    	; приводим A16 к типу float                  (r24 = A16)
   		NOP	     5
 [A1]   SUBSP   .L1    A12,A16,A15      	; охраняем остаток ,A15 = A12 - A16,         (r23 - r24), A15 is r20
||      SPINT   .S1    A16,A16
		NOP      3
		MVK     .S1    0x4,A12		 		; r23 = 0x4 (A12 = r23)
;;;;;;;;;;;;;;;;;;
;; формула r23 = fext r22 by r23
;; r23 - это xr3
;; r22 = A16, r23 = A12

		MVKL    .S1    0xFF00,A8
		MVKH    .S1    0xFF00,A8
		MVK     .S2    0x7F,B8           	; Mask_Len
		AND     .S2    B8,A12,B8         	; запись значения Len
	    AND     .S1    A8,A12,A8
        SHR     .S1    A8,8,A8		    	; запись значения Pos
		SHR     .S1    A16,A8,A16			; сдвиг вправо исходгого регистра на Pos
		MVKL    .S1    0x00FF,A8
		MVKH    .S1    0x00FF,A8		    	; заводим маску для сохранения количества бит равного Len
		AND     .S1    A16,A8,A12        	; выходной регистр после выполнения функции fext
;;;;;;;;;;;;;;;;;;
;; формула r24 = fext r22 by r24
;; r24 = A10
;; r22 = A16
        MVK     .S1    0x40C,A10
		MVKL    .S1    0xFF00,A8
		MVKH    .S1    0xFF00,A8
		MVK     .S2    0x7F,B8           	; Mask_Len
		AND     .S2    B8,A10,B8         	; запись значения Len
	    AND     .S1    A8,A10,A8
        SHR     .S1    A8,8,A8		    	; запись значения Pos
		SHR     .S1    A16,A8,A16			; сдвиг вправо исходгого регистра на Pos
		MVKL    .S1    0x00FF,A8
		MVKH    .S1    0x00FF,A8		    ; заводим маску для сохранения количества бит равного Len
		AND     .S1    A16,A8,A10
;		MVK     .S1    0x02,A10
		ZERO    .S1    A14
		CMPEQ   .L1    A10,A14,A1			; A1 = (A10 = 0?)  , A1 = 1 - True, A1 = 0 - False
||[A1]  MV      .S1    A12,A14				; k24 = xr23, k24 is A14
  [A1]	LDW     .D1    *++A6[A14],A11       ; забрали адресс 3ьего элемента из массива Y16, записали значение в A11
  		NOP      4
 [!A1]	MV      .S1    A10,A14			    ; k24=xr24;
||[!A1]	MV      .S2    A10,B10              ; j24=xr24, B10 is j24
 [!A1]	LDW     .D2    *++B4[B10],B8        ; забрали адресс элемента из массива Y, записали значение в B8
||[!A1]	LDW     .D1    *++A4[A14],A11       ; забрали адресс элемента из массива A4, записали значение в A11
 [!A1]  INTSP   .S1    A12,A12            	; приводим A12 к типу float
 	    NOP      4
 [!A1]	ADDSP   .L1    A12,A15,A12          ; fr23=r23+r20
		NOP      3
 [!A1]	MPYSP   .M1    A12,A11,A12
		NOP      3
 [!A1]  ADDSP   .L1    A12,B8,A13           ; A13 is fr25
 		MVKL    .S2    r20,B11
 		MVKH    .S2    r20,B11
		NOP		 2
 [!A1]	ADDSP   .S1    A13,B15,A13
 [A1]	ADDSP   .S1    A11,B15,A13
		NOP		 3
		MPYSP   .M1    A13,B11,A13
||		MVKL    .S2    r200,B11
 		MVKH    .S2    r200,B11
 		ADDSP   .S1    B11,A13,A13         ;fr25=r25+r20 , r25 is A13, B11 is r20
		NOP      3
		SPTRUNC .L1    A13,A13			   ; fix fr25, convert floating-point to integer with truncation (преобразование с усечением)
		NOP      3
		NOP

		B		.S2		B3
		NOP		5
;;;;;;;;;;;;;;;;;;
;
;		MVKL    .S1   0xFF00,A8
;		MVKH    .S1   0xFF00,A8
;||		MVK     .S2   0x7F,B8           ; Mask_Len
;		MVKL    .S1   0x00001234,A11    ; исходные данные
;		MVKH    .S1   0x00001234,A11
;		MVKL    .S1   0x00000508,A10    ;регистр xr3; формула fext A12 by xr3
;		MVKH    .S1   0x00000508,A10
;||		AND     .S2   B8,A10,B8         ; запись значения Len
;	    AND     .S1   A8,A10,A8
;        SHR     .S1   A8,8,A8		    ; запись значения Pos
;		SHR     .S1   A11,A8,A11		; сдвиг вправо исходгого регистра на Pos
;		MVKL    .S1   0x00FF,A8
;		MVKH    .S1   0x00FF,A8		    ; заводим маску для сохранения количества бит равного Len
;		AND     .S1   A11,A8,A11        ; выходной регистр после выполнения функции fext
;;;;;;;;;;;;;;;;;;
