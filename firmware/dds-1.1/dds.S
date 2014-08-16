
#include <avr/io.h>

; all that follows is program code.
.section .text

; ISR(TIMER0_COMPA_vect): place the timer0 compare match A interrupt in the
; interrupt vector table.
.org 0x0026
  jmp TIMER0_COMPA_vect

; dds_lut_msb: sine wave lookup table for direct digital synthesis.
.org 0x0074
.global dds_lut_msb
dds_lut_msb:
  .byte 0x80, 0x83, 0x86, 0x89, 0x8c, 0x8f, 0x92, 0x95
  .byte 0x98, 0x9c, 0x9f, 0xa2, 0xa5, 0xa8, 0xab, 0xae
  .byte 0xb0, 0xb3, 0xb6, 0xb9, 0xbc, 0xbf, 0xc1, 0xc4
  .byte 0xc7, 0xc9, 0xcc, 0xce, 0xd1, 0xd3, 0xd5, 0xd8
  .byte 0xda, 0xdc, 0xde, 0xe0, 0xe2, 0xe4, 0xe6, 0xe8
  .byte 0xea, 0xec, 0xed, 0xef, 0xf0, 0xf2, 0xf3, 0xf5
  .byte 0xf6, 0xf7, 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfc
  .byte 0xfd, 0xfe, 0xfe, 0xff, 0xff, 0xff, 0xff, 0xff
  .byte 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe
  .byte 0xfd, 0xfc, 0xfc, 0xfb, 0xfa, 0xf9, 0xf8, 0xf7
  .byte 0xf6, 0xf5, 0xf3, 0xf2, 0xf0, 0xef, 0xed, 0xec
  .byte 0xea, 0xe8, 0xe6, 0xe4, 0xe2, 0xe0, 0xde, 0xdc
  .byte 0xda, 0xd8, 0xd5, 0xd3, 0xd1, 0xce, 0xcc, 0xc9
  .byte 0xc7, 0xc4, 0xc1, 0xbf, 0xbc, 0xb9, 0xb6, 0xb3
  .byte 0xb0, 0xae, 0xab, 0xa8, 0xa5, 0xa2, 0x9f, 0x9c
  .byte 0x98, 0x95, 0x92, 0x8f, 0x8c, 0x89, 0x86, 0x83
  .byte 0x80, 0x7c, 0x79, 0x76, 0x73, 0x70, 0x6d, 0x6a
  .byte 0x67, 0x63, 0x60, 0x5d, 0x5a, 0x57, 0x54, 0x51
  .byte 0x4f, 0x4c, 0x49, 0x46, 0x43, 0x40, 0x3e, 0x3b
  .byte 0x38, 0x36, 0x33, 0x31, 0x2e, 0x2c, 0x2a, 0x27
  .byte 0x25, 0x23, 0x21, 0x1f, 0x1d, 0x1b, 0x19, 0x17
  .byte 0x15, 0x13, 0x12, 0x10, 0x0f, 0x0d, 0x0c, 0x0a
  .byte 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x03
  .byte 0x02, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00
  .byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01
  .byte 0x02, 0x03, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08
  .byte 0x09, 0x0a, 0x0c, 0x0d, 0x0f, 0x10, 0x12, 0x13
  .byte 0x15, 0x17, 0x19, 0x1b, 0x1d, 0x1f, 0x21, 0x23
  .byte 0x25, 0x27, 0x2a, 0x2c, 0x2e, 0x31, 0x33, 0x36
  .byte 0x38, 0x3b, 0x3e, 0x40, 0x43, 0x46, 0x49, 0x4c
  .byte 0x4f, 0x51, 0x54, 0x57, 0x5a, 0x5d, 0x60, 0x63
  .byte 0x67, 0x6a, 0x6d, 0x70, 0x73, 0x76, 0x79, 0x7c

; TIM0_COMPA_vect: core function that gets executes a single sample output.
.global TIMER0_COMPA_vect
TIMER0_COMPA_vect:
  ; store the status register state.
  in r2, _SFR_IO_ADDR(SREG)

  ; turn on the led.
  sbi _SFR_IO_ADDR(PORTC), PORTC5
  
  ; increment the phase accumulator.
  add r28, r24
  adc r29, r25
  adc r30, r26

  ; load the msb from the lookup table.
  ;ldi r31, hi8(dds_lut_msb)
  lpm r21, Z

  ; load the lsb from the lookup table.
  ;ldi r31, hi8(dds_lut_lsb)
  ;lpm r20, Z

  ; select the spi dac.
  cbi _SFR_IO_ADDR(PORTD), PORTD0

  ; set the spi mosi line low.
  cbi _SFR_IO_ADDR(PORTD), PORTD2

  ; clock the spi bus four times.
  sbi _SFR_IO_ADDR(PORTD), PORTD1
  cbi _SFR_IO_ADDR(PORTD), PORTD1 ; 1
  sbi _SFR_IO_ADDR(PORTD), PORTD1
  cbi _SFR_IO_ADDR(PORTD), PORTD1 ; 2
  sbi _SFR_IO_ADDR(PORTD), PORTD1
  cbi _SFR_IO_ADDR(PORTD), PORTD1 ; 3
  sbi _SFR_IO_ADDR(PORTD), PORTD1
  cbi _SFR_IO_ADDR(PORTD), PORTD1 ; 4

  ; clock out bit 7.
  cbi _SFR_IO_ADDR(PORTD), PORTD2
  sbrc r21, 7
  sbi _SFR_IO_ADDR(PORTD), PORTD2
  sbi _SFR_IO_ADDR(PORTD), PORTD1
  cbi _SFR_IO_ADDR(PORTD), PORTD1

  ; clock out bit 6.
  cbi _SFR_IO_ADDR(PORTD), PORTD2
  sbrc r21, 6
  sbi _SFR_IO_ADDR(PORTD), PORTD2
  sbi _SFR_IO_ADDR(PORTD), PORTD1
  cbi _SFR_IO_ADDR(PORTD), PORTD1

  ; clock out bit 5.
  cbi _SFR_IO_ADDR(PORTD), PORTD2
  sbrc r21, 5
  sbi _SFR_IO_ADDR(PORTD), PORTD2
  sbi _SFR_IO_ADDR(PORTD), PORTD1
  cbi _SFR_IO_ADDR(PORTD), PORTD1

  ; clock out bit 4.
  cbi _SFR_IO_ADDR(PORTD), PORTD2
  sbrc r21, 4
  sbi _SFR_IO_ADDR(PORTD), PORTD2
  sbi _SFR_IO_ADDR(PORTD), PORTD1
  cbi _SFR_IO_ADDR(PORTD), PORTD1

  ; clock out bit 3.
  cbi _SFR_IO_ADDR(PORTD), PORTD2
  sbrc r21, 3
  sbi _SFR_IO_ADDR(PORTD), PORTD2
  sbi _SFR_IO_ADDR(PORTD), PORTD1
  cbi _SFR_IO_ADDR(PORTD), PORTD1

  ; clock out bit 2.
  cbi _SFR_IO_ADDR(PORTD), PORTD2
  sbrc r21, 2
  sbi _SFR_IO_ADDR(PORTD), PORTD2
  sbi _SFR_IO_ADDR(PORTD), PORTD1
  cbi _SFR_IO_ADDR(PORTD), PORTD1

  ; clock out bit 1.
  cbi _SFR_IO_ADDR(PORTD), PORTD2
  sbrc r21, 1
  sbi _SFR_IO_ADDR(PORTD), PORTD2
  sbi _SFR_IO_ADDR(PORTD), PORTD1
  cbi _SFR_IO_ADDR(PORTD), PORTD1

  ; clock out bit 0.
  cbi _SFR_IO_ADDR(PORTD), PORTD2
  sbrc r21, 0
  sbi _SFR_IO_ADDR(PORTD), PORTD2
  sbi _SFR_IO_ADDR(PORTD), PORTD1
  cbi _SFR_IO_ADDR(PORTD), PORTD1

  ; set the spi mosi line low.
  cbi _SFR_IO_ADDR(PORTD), PORTD2

  ; clock the spi bus four more times.
  sbi _SFR_IO_ADDR(PORTD), PORTD1
  cbi _SFR_IO_ADDR(PORTD), PORTD1 ; 1
  sbi _SFR_IO_ADDR(PORTD), PORTD1
  cbi _SFR_IO_ADDR(PORTD), PORTD1 ; 2
  sbi _SFR_IO_ADDR(PORTD), PORTD1
  cbi _SFR_IO_ADDR(PORTD), PORTD1 ; 3
  sbi _SFR_IO_ADDR(PORTD), PORTD1
  cbi _SFR_IO_ADDR(PORTD), PORTD1 ; 4

  ; deselect the spi dac.
  sbi _SFR_IO_ADDR(PORTD), PORTD0

  ; turn off the led.
  cbi _SFR_IO_ADDR(PORTC), PORTC5
  
  ; return the status register state.
  out _SFR_IO_ADDR(SREG), r2
  reti

; dds_init: initializes the direct digital synthesis registers.
.global dds_init
dds_init:
  ; configure the timer0 prescaler mux for 4.0 us resolution.
  in r20, _SFR_IO_ADDR(TCCR0B)
  ori r20, (1 << CS01)
  ori r20, (1 << CS00)
  out _SFR_IO_ADDR(TCCR0B), r20

  ; configure the timer0 counter to clear on compare match.
  in r20, _SFR_IO_ADDR(TCCR0A)
  ori r20, (1 << WGM01)
  out _SFR_IO_ADDR(TCCR0A), r20

  ; configure the timer0 counter to clear every 16.0 us.
  ldi r20, 0x02
  out _SFR_IO_ADDR(OCR0A), r20

  ; zero the timer0 counter.
  out _SFR_IO_ADDR(TCNT0), r1

  ; set up the Z pointer.
  ldi r31, hi8(dds_lut_msb)
  ldi r30, lo8(dds_lut_msb)

  ; clear the phase accumulator.
  ldi r29, 0x00
  ldi r28, 0x00

  ; initialize the adder registers. the currently set value produces a
  ; 1.000 kHz tone when executed.
  ldi r24, 0x80
  ldi r25, 0x12
  ldi r26, 0x03

  ; enable interrupts.
  lds r20, TIMSK0
  ori r20, (1 << OCIE0A)
  sts TIMSK0, r20
  sei

  ; return.
  ret

