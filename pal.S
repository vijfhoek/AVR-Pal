#include <avr/io.h>
#define _SFR_ASM_COMPAT 1

.global scanline
.lcomm scanline, 2

.global syncsleft
.lcomm syncsleft, 1

.global field
.lcomm field, 1

.global int_func
.lcomm int_func, 2

.global IntInitS
IntInitS:
	; Set int_func to LongSyncPulseS
	ldi r31, hi8(LongSyncPulseS)
	ldi r30, lo8(LongSyncPulseS)
	sts int_func, r30
	sts int_func+1, r31

	; Set syncs left to 5
	ldi r30, 5
	sts syncsleft, r30

	; Clear field
	clr r30
	sts field, r30

	; Clear scanline
	sts scanline, r30
	sts scanline+1, r30

	ret

.global TIMER1_COMPA_vect
TIMER1_COMPA_vect:
	; Put SREG in r1 (__zero_register__), making sure to clr it afterwards!
	in r1, _SFR_IO_ADDR(SREG)				; 1	

	; Save r29 through r31
	push r31						; 3
	push r30						; 5

	; Call the correct interrupt
	ldi r30, lo8(int_func)					; 6
	ldi r31, hi8(int_func)					; 7
	icall							; 10

	; Put r29 through r31 back
	pop r30							; 13
	pop r31							; 15

	; Put SREG back
	out _SFR_IO_ADDR(SREG), r1				; 16
	clr r1							; 17

	; Return
	reti							; 21

.global LongSyncPulseS
LongSyncPulseS:
	; Disable sync pin
	cbi _SFR_IO_ADDR(PORTB), 2				; 2

	; Set the CTC value to 750 cycles
	ldi r31, hi8(750)					; 3
	ldi r30, lo8(750)					; 4
	sts OCR1AH, r31						; 6
	sts OCR1AL, r30						; 8

	; Set the next handler to LongSyncDelay
	ldi r31, hi8(LongSyncDelayS)				; 9
	ldi r30, lo8(LongSyncDelayS)				; 10
	sts int_func, r30					; 12
	sts int_func+1, r31					; 14

	ret							; 18

.global LongSyncDelayS
LongSyncDelayS:
	; Store r29
	push r29

	; Enable sync pin
	sbi _SFR_IO_ADDR(PORTB), 2				; 2

	; Set the CTC value to 50 cycle
	ldi r31, hi8(50)					; 3
	ldi r30, lo8(50)					; 4
	sts OCR1AH, r31						; 6
	sts OCR1AL, r30						; 8
	
	; Load syncsleft into r29
	ldi r31, hi8(syncsleft)					; 9
	ldi r30, lo8(syncsleft)					; 10
	ld r29, Z						; 12

	; Decrease it
	dec r29							; 13

	; If it's 0, skip ahead to LSD1
	brne _LSD1						; 14	15

	; Store syncsleft back
	st Z, r29						; 16

	; Set the interrupt handler
	ldi r31, hi8(LongSyncPulseS)				; 17
	ldi r30, lo8(LongSyncPulseS)				; 18
	sts int_func, r30					; 20
	sts int_func+1, r31					; 22

	pop r29

	ret							; 26

_LSD1:
	ldi r29, 5						;	16

	; Substract syncsleft from field
	ldi r31, hi8(field)					;	17
	ldi r30, hi8(field)					;	18
	ld r30, Z						;	20
	sub r29, r30						;	21

	; Store r29 again
	ldi r31, hi8(syncsleft)					;	22
	ldi r30, lo8(syncsleft)					;	23
	st Z, r29						;	25

	; Set ShortSyncPostPulse as handler
	ldi r31, hi8(ShortSyncPostPulseS)			;	26
	ldi r30, lo8(ShortSyncPostPulseS)			;	27
	sts int_func, r30					;	29
	sts int_func+1, r31					;	31

	pop r29

	ret							;	35

.global ShortSyncPostPulseS
ShortSyncPostPulseS:
	; Disable sync pin
	cbi _SFR_IO_ADDR(PORTB), 2						; 2

	; Set the CTC value to 50 cycles
	ldi r31, hi8(50)						; 3
	ldi r30, lo8(50)						; 4
	sts OCR1AH, r31						; 6
	sts OCR1AL, r30						; 8

	; Set the next handler to ShortSyncPostDelay
	ldi r31, hi8(ShortSyncPostDelayS)			; 9
	ldi r30, lo8(ShortSyncPostDelayS)			; 10
	sts int_func, r30					; 12
	sts int_func+1, r31					; 14

	ret							; 18

.global ShortSyncPostDelayS
ShortSyncPostDelayS:
	; Enable sync pin
	sbi _SFR_IO_ADDR(PORTB), 2				; 2

	; Save r29
	push r29

	; Set the CTC value to 750 cycles
	ldi r31, hi8(750)					; 3
	ldi r30, lo8(750)					; 4
	sts OCR1AH, r31						; 6
	sts OCR1AL, r30						; 8

	; Load syncsleft into r29
	ldi r31, hi8(syncsleft)					; 9
	ldi r30, lo8(syncsleft)					; 10
	ld r29, Z						; 12

	; Decrease it
	dec r29							; 13

	; Store syncsleft back
	st Z, r29						; 15

	pop r29

	; If it's 0, skip ahead to SSPD1
	brne _SSPD1						; 16	17

	; Set the interrupt handler
	ldi r31, hi8(ShortSyncPostPulseS)			; 17
	ldi r30, lo8(ShortSyncPostPulseS)			; 18
	sts int_func, r31					; 20
	sts int_func+1, r30					; 22

	ret							; 26

_SSPD1:
	; Set the next handler to HSyncPulse
	ldi r31, hi8(HSyncPulseS)				;	18
	ldi r30, lo8(HSyncPulseS)				;	19
	sts int_func, r31					;	20
	sts int_func+1, r30					;	22

	ret							;	26

.global HSyncPulseS
HSyncPulseS:
	; Disable sync- and vid pin
	cbi _SFR_IO_ADDR(PORTB), 3						; 2
	cbi _SFR_IO_ADDR(PORTB), 2						; 4

	; Set the CTC value to 100 cycles
	ldi r31, hi8(100)					; 5
	ldi r30, lo8(100)					; 6
	sts OCR1AH, r31						; 8
	sts OCR1AL, r30						; 10
	
	; Set the handler to BackPorch
	ldi r31, hi8(BackPorchS)					; 11
	ldi r30, lo8(BackPorchS)					; 12
	sts int_func, r31					; 14
	sts int_func+1, r30					; 16

	ret							; 20

.global BackPorchS
BackPorchS:
	; Enable sync pin
	sbi _SFR_IO_ADDR(PORTB), 2						; 2

	; Set the CTC value to 200 cycles
	ldi r31, hi8(200)					; 3
	ldi r30, lo8(200)					; 4
	sts OCR1AH, r31						; 6
	sts OCR1AL, r30						; 8

	; Set the handler to ImageData
	ldi r31, hi8(ImageDataS)					; 9
	ldi r30, lo8(ImageDataS)					; 10
	sts int_func, r31					; 12
	sts int_func+1, r30					; 14

	ret							; 18

.global ImageDataS
ImageDataS:

	ret

