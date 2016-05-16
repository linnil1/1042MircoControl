# Lab10 homework

1. Use the Atmel Studio debugging functions to find the machine codes for the program in Fig. 9. Make a copy of the machine codes to your report.

see [lssfile](asm.lss)

2. Identify the opcodes for the instructions in the following table. Report them in binary forms. Leave the digit cross (X) if it is a part of an operand.

| Instruction | Opcode |
| ----------- | ------ |
| LDI | 1110 KKKK DDDD KKKK |
| ADD | 0000 11rd dddd rrrr |
| OUT | 1011 1AAr rrrr AAAA |
| INC | 1001 010d dddd 0011 |
| STS | 1001 001d dddd 0000 kkkk kkkk kkkk kkkk |
| IN  | 1011 0AAd dddd AAAA |

3. Observe the value changes in general purpose registers (GPR), status register (SREG), and program counter (PC) “after” the execution of every instruction. Report your observations in a table.


| PC | Instruction      | CYCLE | SREG  | R16  | R17  | R18  | R20  | R21  | 
| -- | -----------      | ----- | ----  | ---  | ---  | ---  | ---  | ---  |
| 0  | LDI R16, 0x25 	|0      |       | 0X25 | 0x00 | 0x00 | 0x00 | 0x00 |
| 1  | LDI R17, 0x34	|0		|  		| 0X25 | 0x00 | 0x00 | 0x00 | 0x00 |
| 2  | LDI R18, 0x31	|1		|  		| 0X25 | 0x34 | 0x00 | 0x00 | 0x00 |
| 3  | ADD R16, R17		|2		|  		| 0X25 | 0x34 | 0x31 | 0x00 | 0x00 |
| 4  | ADD R16, R18		|3		|  		| 0X59 | 0x34 | 0x31 | 0x00 | 0x00 |
| 5  | LDI R17, 0x74	|4		|  VN	| 0X8A | 0x34 | 0x31 | 0x00 | 0x00 |
| 6  | ADD R16, R17		|5		|  VN   | 0X8A | 0x74 | 0x31 | 0x00 | 0x00 |
| 7  | LDI R20, 0		|6		|  SN   | 0XFE | 0x74 | 0x31 | 0x00 | 0x00 |
| 8  | ADD R16, R20		|7		|  SN   | 0XFE | 0x74 | 0x31 | 0x00 | 0x00 |
| 9  | LDI R21, 0xFF	|8		|  SN   | 0XFE | 0x74 | 0x31 | 0x00 | 0x00 |
| A  | OUT DDRD, R21	|9		|  SN   | 0XFE | 0x74 | 0x31 | 0x00 | 0xFF |
| B  | L2: INC R20		|10		|  SN   | 0XFE | 0x74 | 0x31 | 0x00 | 0xFF |
| C  | OUT PORTD, R20	|11		|       | 0XFE | 0x74 | 0x31 | 0x01 | 0xFF |
| D  | STS SUM, R20		|12		|       | 0XFE | 0x74 | 0x31 | 0x01 | 0xFF |
| F  | RJMP L2			|13		|       | 0XFE | 0x74 | 0x31 | 0x01 | 0xFF |

4. Find out how the value of memory address at 0x300 varies? What do you do if you want to store the value of R20 to the addresses of 0x150 instead of 0x300?

| COUNTER   | 0X300 | R20   |
| --        | --    | --    |
| 12		| 0		| 0X01	|
| 18		| 1		| 0X02	|
| 24		| 2		| 0X03	|
| 30		| 3		| 0X04	|

......

If you want to store at 0x150
just modify your file 
` .EQU SUM = 0x300 `
to
` .EQU SUM = 0x150 `
















