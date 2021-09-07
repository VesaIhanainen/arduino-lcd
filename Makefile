CC=avr-gcc
OBJCOPY=avr-objcopy

CFLAGS=-Os -DF_CPU=16000000UL -mmcu=atmega328p

main.hex: main.elf
	${OBJCOPY} -O ihex -R .eeprom main.hex main.elf
main.elf: lcd.o main.o 
	${CC} $(CFLAGS) -o main.elf lcd.o main.o
lcd.o: lcd.c
	${CC} $(CFLAGS) -c -o lcd.o lcd.c
main.o: main.c
	${CC} $(CFLAGS) -c -o main.o main.c
