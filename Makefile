all: compile flash

compile:
	avr-gcc pal.c pal.S -o out.elf -g -mmcu=atmega328p -O2 -Wl,--undefined=_mmcu,--section-start=.mmcu=0x1000 
	avr-objcopy -I ihex out.elf out.hex

listing:
	avr-gcc -c -g -Wa,-a,-ad -mmcu=atmega328p -O2 pal.c > out.lst

flash:
	sudo avrdude -b 57600 -p m328p -c dapa -U flash:w:out.hex

sim: compile
	simavr out.elf

dbg: compile
	screen -dmS simavr simavr -g -f 25000000 -m atmega328p
	avr-gdb -x gdb-cfg
	killall simavr
