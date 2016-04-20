debug:
	avr-g++ -g -Wall -Os -mcall-prologues -fno-exceptions -mmcu=atmega328p main.c
	rm a.out

upload:
	avr-g++ -Os -mcall-prologues -fno-exceptions -mmcu=atmega328p -o main.obj main.c
	avr-objcopy -R .eeprom -O ihex main.obj main.hex
	avrdude -c stk500v1 -P /dev/ttyACM0 -b 19200 -p m328p  -U flash:w:main.hex
	rm main.obj
	rm main.hex
