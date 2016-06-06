;INIT OUTPUT
    LDI		R31,	$FF		
	OUT		DDRB,	R31	
	;INIT INPUT PULLUP
	LDI		R31,	0
	OUT		DDRD,	R31
	LDI		R31,	$FF
	OUT		PORTD,	R31
	
MAIN:
	LDI		R18,	1		;INIT
	LDI		R31,	$FF
	IN		R29,	PIND
	EOR		R29,	R31
	JMP		LOOP

LOOP:
	CPI		R18,	8
	BREQ	MAIN
	;GET INPUT 
	LDI		R28,	1		 
	AND		R28,	R29
	BRNE	WHERE
CONLOOP:
	
	;CALL	DELAY
	INC		R18
	LSR		R29
	JMP		LOOP

WHERE:
	MOV		R30,	R18
	CALL	GETSEG
	JMP		CONLOOP

GETSEG: 					;INIT 7-SEGMENT
	CPI		R30,	0		;compare
	BREQ	SEG0			;branch if eq
	CPI		R30,	1
	BREQ	SEG1
	CPI		R30,	2
	BREQ	SEG2
	CPI		R30,	3
	BREQ	SEG3
	CPI		R30,	4
	BREQ	SEG4
	CPI		R30,	5
	BREQ	SEG5
	CPI		R30,	6
	BREQ	SEG6
	CPI		R30,	7
	BREQ	SEG7
	RET
	

SEG0:	; C0 F9 A4 0B 99 92 82 F8
	LDI		R17,	$C0
	OUT		PORTB,	R17
	RET
SEG1:
	LDI		R17,	$F9
	OUT		PORTB,	R17
	RET
SEG2:
	LDI		R17,	$A4
	OUT		PORTB,	R17
	RET
SEG3:
	LDI		R17,	$30
	OUT		PORTB,	R17
	RET
SEG4:
	LDI		R17,	$99
	OUT		PORTB,	R17
	RET
SEG5:
	LDI		R17,	$92
	OUT		PORTB,	R17
	RET
SEG6:
	LDI		R17,	$82
	OUT		PORTB,	R17
	RET
SEG7:
	LDI		R17,	$F8
	OUT		PORTB,	R17
	RET

DELAY:
	LDI		R20,	1
L2: LDI		R21,	0xFF
L3: LDI		R22,	0xFF
L4: DEC		R22
	BRNE	L4
	DEC		R21
	BRNE	L3
	DEC		R20
	BRNE	L2
	RET
