
; tcnt.S: PPM firmware timer/counter peripherals source code.
; Copyright (C) 2014  Bradley Worley  <geekysuavo@gmail.com>
;
; This program is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program.  If not, see <http://www.gnu.org/licenses/>.

; include the avr headers.
#include <avr/io.h>

; include the PPM GPIO definitions.
#include "gpio-defines.h"

; all that follows is program code.
.section .text

; declare C functions that must be externally visible to the assembly code.
.extern usb_cdc_putchar

; install memory addresses for interrupt service routines.
.org 0x001e
  rjmp TIMER1_COMPA_vect
.org 0x0026
  rjmp TIMER0_COMPA_vect
.org 0x002a
  rjmp TIMER0_OVF_vect

; tcnt_dds_lut: sine wave lookup table for direct digital synthesis
; the bytes are stored as 8-bit two's complement signed values.
.org 0x0074
.global tcnt_dds_lut
tcnt_dds_lut:
  .byte 0x00, 0x03, 0x06, 0x09, 0x0c, 0x10, 0x13, 0x16
  .byte 0x19, 0x1c, 0x1f, 0x22, 0x25, 0x28, 0x2b, 0x2e
  .byte 0x31, 0x33, 0x36, 0x39, 0x3c, 0x3f, 0x41, 0x44
  .byte 0x47, 0x49, 0x4c, 0x4e, 0x51, 0x53, 0x55, 0x58
  .byte 0x5a, 0x5c, 0x5e, 0x60, 0x62, 0x64, 0x66, 0x68
  .byte 0x6a, 0x6b, 0x6d, 0x6f, 0x70, 0x71, 0x73, 0x74
  .byte 0x75, 0x76, 0x78, 0x79, 0x7a, 0x7a, 0x7b, 0x7c
  .byte 0x7d, 0x7d, 0x7e, 0x7e, 0x7e, 0x7f, 0x7f, 0x7f
  .byte 0x7f, 0x7f, 0x7f, 0x7f, 0x7e, 0x7e, 0x7e, 0x7d
  .byte 0x7d, 0x7c, 0x7b, 0x7a, 0x7a, 0x79, 0x78, 0x76
  .byte 0x75, 0x74, 0x73, 0x71, 0x70, 0x6f, 0x6d, 0x6b
  .byte 0x6a, 0x68, 0x66, 0x64, 0x62, 0x60, 0x5e, 0x5c
  .byte 0x5a, 0x58, 0x55, 0x53, 0x51, 0x4e, 0x4c, 0x49
  .byte 0x47, 0x44, 0x41, 0x3f, 0x3c, 0x39, 0x36, 0x33
  .byte 0x31, 0x2e, 0x2b, 0x28, 0x25, 0x22, 0x1f, 0x1c
  .byte 0x19, 0x16, 0x13, 0x10, 0x0c, 0x09, 0x06, 0x03
  .byte 0x00, 0xfd, 0xfa, 0xf7, 0xf4, 0xf0, 0xed, 0xea
  .byte 0xe7, 0xe4, 0xe1, 0xde, 0xdb, 0xd8, 0xd5, 0xd2
  .byte 0xcf, 0xcd, 0xca, 0xc7, 0xc4, 0xc1, 0xbf, 0xbc
  .byte 0xb9, 0xb7, 0xb4, 0xb2, 0xaf, 0xad, 0xab, 0xa8
  .byte 0xa6, 0xa4, 0xa2, 0xa0, 0x9e, 0x9c, 0x9a, 0x98
  .byte 0x96, 0x95, 0x93, 0x91, 0x90, 0x8f, 0x8d, 0x8c
  .byte 0x8b, 0x8a, 0x88, 0x87, 0x86, 0x86, 0x85, 0x84
  .byte 0x83, 0x83, 0x82, 0x82, 0x82, 0x81, 0x81, 0x81
  .byte 0x81, 0x81, 0x81, 0x81, 0x82, 0x82, 0x82, 0x83
  .byte 0x83, 0x84, 0x85, 0x86, 0x86, 0x87, 0x88, 0x8a
  .byte 0x8b, 0x8c, 0x8d, 0x8f, 0x90, 0x91, 0x93, 0x95
  .byte 0x96, 0x98, 0x9a, 0x9c, 0x9e, 0xa0, 0xa2, 0xa4
  .byte 0xa6, 0xa8, 0xab, 0xad, 0xaf, 0xb2, 0xb4, 0xb7
  .byte 0xb9, 0xbc, 0xbf, 0xc1, 0xc4, 0xc7, 0xca, 0xcd
  .byte 0xcf, 0xd2, 0xd5, 0xd8, 0xdb, 0xde, 0xe1, 0xe4
  .byte 0xe7, 0xea, 0xed, 0xf0, 0xf4, 0xf7, 0xfa, 0xfd

; ============================================================================

; TIMER1_COMPA_vect: interrupt service routine: timer/counter1 compare match A.
; the spi code looks ugly, but it's 20x clean 50% duty cycle clocks at 2 MHz.
; (see tcnt1_run_acquire for register assignments)
.global TIMER1_COMPA_vect
TIMER1_COMPA_vect:
  ; store the status register state.
  in r2, _SFR_IO_ADDR(SREG)

  ; initialize registers to hold the conversion result.
  ldi r28, 0x00
  ldi r29, 0x00

  ; select the spi adc.
  cbi _SFR_IO_ADDR(GPIO_ADC_PORT), GPIO_ADC_PIN_CS

  ; clock #1.
  cbi _SFR_IO_ADDR(GPIO_ADC_PORT), GPIO_ADC_PIN_SCK
  nop
  nop
  nop
  sbi _SFR_IO_ADDR(GPIO_ADC_PORT), GPIO_ADC_PIN_SCK
  nop
  nop
  nop

  ; clock #2.
  cbi _SFR_IO_ADDR(GPIO_ADC_PORT), GPIO_ADC_PIN_SCK
  nop
  nop
  nop
  sbi _SFR_IO_ADDR(GPIO_ADC_PORT), GPIO_ADC_PIN_SCK
  nop
  nop
  nop

  ; clock #3.
  cbi _SFR_IO_ADDR(GPIO_ADC_PORT), GPIO_ADC_PIN_SCK
  nop
  nop
  nop
  sbi _SFR_IO_ADDR(GPIO_ADC_PORT), GPIO_ADC_PIN_SCK
  nop
  nop
  nop

  ; clock #4 (read DB15).
  cbi _SFR_IO_ADDR(GPIO_ADC_PORT), GPIO_ADC_PIN_SCK
  nop
  nop
  nop
  sbi _SFR_IO_ADDR(GPIO_ADC_PORT), GPIO_ADC_PIN_SCK
  in r26, _SFR_IO_ADDR(GPIO_ADC_PIN)
  andi r26, (1 << GPIO_ADC_PIN_MISO)
  swap r26

  ; clock #5 (DB15 -> r29[7], read DB14).
  cbi _SFR_IO_ADDR(GPIO_ADC_PORT), GPIO_ADC_PIN_SCK
  lsr r26
  or r29, r26
  lsl r29
  sbi _SFR_IO_ADDR(GPIO_ADC_PORT), GPIO_ADC_PIN_SCK
  in r26, _SFR_IO_ADDR(GPIO_ADC_PIN)
  andi r26, (1 << GPIO_ADC_PIN_MISO)
  swap r26

  ; clock #6 (DB14 -> r29[6], read DB13).
  cbi _SFR_IO_ADDR(GPIO_ADC_PORT), GPIO_ADC_PIN_SCK
  lsr r26
  or r29, r26
  lsl r29
  sbi _SFR_IO_ADDR(GPIO_ADC_PORT), GPIO_ADC_PIN_SCK
  in r26, _SFR_IO_ADDR(GPIO_ADC_PIN)
  andi r26, (1 << GPIO_ADC_PIN_MISO)
  swap r26

  ; clock #7 (DB13 -> r29[5], read DB12).
  cbi _SFR_IO_ADDR(GPIO_ADC_PORT), GPIO_ADC_PIN_SCK
  lsr r26
  or r29, r26
  lsl r29
  sbi _SFR_IO_ADDR(GPIO_ADC_PORT), GPIO_ADC_PIN_SCK
  in r26, _SFR_IO_ADDR(GPIO_ADC_PIN)
  andi r26, (1 << GPIO_ADC_PIN_MISO)
  swap r26

  ; clock #8 (DB12 -> r29[4], read DB11).
  cbi _SFR_IO_ADDR(GPIO_ADC_PORT), GPIO_ADC_PIN_SCK
  lsr r26
  or r29, r26
  lsl r29
  sbi _SFR_IO_ADDR(GPIO_ADC_PORT), GPIO_ADC_PIN_SCK
  in r26, _SFR_IO_ADDR(GPIO_ADC_PIN)
  andi r26, (1 << GPIO_ADC_PIN_MISO)
  swap r26

  ; clock #9 (DB11 -> r29[3], read DB10).
  cbi _SFR_IO_ADDR(GPIO_ADC_PORT), GPIO_ADC_PIN_SCK
  lsr r26
  or r29, r26
  lsl r29
  sbi _SFR_IO_ADDR(GPIO_ADC_PORT), GPIO_ADC_PIN_SCK
  in r26, _SFR_IO_ADDR(GPIO_ADC_PIN)
  andi r26, (1 << GPIO_ADC_PIN_MISO)
  swap r26

  ; clock #10 (DB10 -> r29[2], read DB9).
  cbi _SFR_IO_ADDR(GPIO_ADC_PORT), GPIO_ADC_PIN_SCK
  lsr r26
  or r29, r26
  lsl r29
  sbi _SFR_IO_ADDR(GPIO_ADC_PORT), GPIO_ADC_PIN_SCK
  in r26, _SFR_IO_ADDR(GPIO_ADC_PIN)
  andi r26, (1 << GPIO_ADC_PIN_MISO)
  swap r26

  ; clock #11 (DB9 -> r29[1], read DB8).
  cbi _SFR_IO_ADDR(GPIO_ADC_PORT), GPIO_ADC_PIN_SCK
  lsr r26
  or r29, r26
  lsl r29
  sbi _SFR_IO_ADDR(GPIO_ADC_PORT), GPIO_ADC_PIN_SCK
  in r26, _SFR_IO_ADDR(GPIO_ADC_PIN)
  andi r26, (1 << GPIO_ADC_PIN_MISO)
  swap r26

  ; clock #12 (DB8 -> r29[0], read DB7).
  cbi _SFR_IO_ADDR(GPIO_ADC_PORT), GPIO_ADC_PIN_SCK
  lsr r26
  or r29, r26
  nop
  sbi _SFR_IO_ADDR(GPIO_ADC_PORT), GPIO_ADC_PIN_SCK
  in r26, _SFR_IO_ADDR(GPIO_ADC_PIN)
  andi r26, (1 << GPIO_ADC_PIN_MISO)
  swap r26

  ; clock #13 (DB7 -> r28[7], read DB6).
  cbi _SFR_IO_ADDR(GPIO_ADC_PORT), GPIO_ADC_PIN_SCK
  lsr r26
  or r28, r26
  lsl r28
  sbi _SFR_IO_ADDR(GPIO_ADC_PORT), GPIO_ADC_PIN_SCK
  in r26, _SFR_IO_ADDR(GPIO_ADC_PIN)
  andi r26, (1 << GPIO_ADC_PIN_MISO)
  swap r26

  ; clock #14 (DB6 -> r28[6], read DB5).
  cbi _SFR_IO_ADDR(GPIO_ADC_PORT), GPIO_ADC_PIN_SCK
  lsr r26
  or r28, r26
  lsl r28
  sbi _SFR_IO_ADDR(GPIO_ADC_PORT), GPIO_ADC_PIN_SCK
  in r26, _SFR_IO_ADDR(GPIO_ADC_PIN)
  andi r26, (1 << GPIO_ADC_PIN_MISO)
  swap r26

  ; clock #15 (DB5 -> r28[5], read DB4).
  cbi _SFR_IO_ADDR(GPIO_ADC_PORT), GPIO_ADC_PIN_SCK
  lsr r26
  or r28, r26
  lsl r28
  sbi _SFR_IO_ADDR(GPIO_ADC_PORT), GPIO_ADC_PIN_SCK
  in r26, _SFR_IO_ADDR(GPIO_ADC_PIN)
  andi r26, (1 << GPIO_ADC_PIN_MISO)
  swap r26

  ; clock #16 (DB4 -> r28[4], read DB3).
  cbi _SFR_IO_ADDR(GPIO_ADC_PORT), GPIO_ADC_PIN_SCK
  lsr r26
  or r28, r26
  lsl r28
  sbi _SFR_IO_ADDR(GPIO_ADC_PORT), GPIO_ADC_PIN_SCK
  in r26, _SFR_IO_ADDR(GPIO_ADC_PIN)
  andi r26, (1 << GPIO_ADC_PIN_MISO)
  swap r26

  ; clock #17 (DB3 -> r28[3], read DB2).
  cbi _SFR_IO_ADDR(GPIO_ADC_PORT), GPIO_ADC_PIN_SCK
  lsr r26
  or r28, r26
  lsl r28
  sbi _SFR_IO_ADDR(GPIO_ADC_PORT), GPIO_ADC_PIN_SCK
  in r26, _SFR_IO_ADDR(GPIO_ADC_PIN)
  andi r26, (1 << GPIO_ADC_PIN_MISO)
  swap r26

  ; clock #18 (DB2 -> r28[2], read DB1).
  cbi _SFR_IO_ADDR(GPIO_ADC_PORT), GPIO_ADC_PIN_SCK
  lsr r26
  or r28, r26
  lsl r28
  sbi _SFR_IO_ADDR(GPIO_ADC_PORT), GPIO_ADC_PIN_SCK
  in r26, _SFR_IO_ADDR(GPIO_ADC_PIN)
  andi r26, (1 << GPIO_ADC_PIN_MISO)
  swap r26

  ; clock #19 (DB1 -> r28[1], read DB0).
  cbi _SFR_IO_ADDR(GPIO_ADC_PORT), GPIO_ADC_PIN_SCK
  lsr r26
  or r28, r26
  lsl r28
  sbi _SFR_IO_ADDR(GPIO_ADC_PORT), GPIO_ADC_PIN_SCK
  in r26, _SFR_IO_ADDR(GPIO_ADC_PIN)
  andi r26, (1 << GPIO_ADC_PIN_MISO)
  swap r26

  ; clock #20 (DB0 -> r28[0]).
  cbi _SFR_IO_ADDR(GPIO_ADC_PORT), GPIO_ADC_PIN_SCK
  lsr r26
  or r28, r26
  nop
  sbi _SFR_IO_ADDR(GPIO_ADC_PORT), GPIO_ADC_PIN_SCK
  nop
  nop
  nop

  ; deselect the spi adc.
  sbi _SFR_IO_ADDR(GPIO_ADC_PORT), GPIO_ADC_PIN_CS

  ; write the conversion word to the usb host.
  push r22
  push r23
  push r24
  push r25
  mov r24, r29
  call usb_cdc_putchar
  mov r24, r28
  call usb_cdc_putchar
  pop r25
  pop r24
  pop r23
  pop r22

  ; increment the (32-bit) overflow counter.
  ldi r26, 0x01
  add r13, r26
  adc r14, r1
  adc r15, r1
  adc r16, r1

  ; check if the overflow counter has reached its target.
  cp r13, r22
  cpc r14, r23
  cpc r15, r24
  cpc r16, r25
  brne cmp1_end

  ; disable the timer/counter1 compare match interrupt.
  ldi r30, TIMSK1
  ldi r31, 0x00
  ld r26, Z
  andi r26, ~(1 << OCIE1A)
  st Z, r26

  ; raise the completion flag.
  ldi r26, 0x01
  mov r3, r26

  ; restore the status register state and return.
cmp1_end:
  out _SFR_IO_ADDR(SREG), r2
  reti

; ============================================================================

; TIMER0_COMPA_vect: interrupt service routine: timer/counter0 compare match A.
; this subroutine should require 144 cycles @ 16.0 MHz => 9.0 us.
; (see tcnt0_run_pulse for register assignments)
.global TIMER0_COMPA_vect
TIMER0_COMPA_vect:
  ; store the status register state. (+1 => 1)
  in r2, _SFR_IO_ADDR(SREG)

  ; increment the phase accumulator. (+4 => 5)
  add r27, r18
  adc r28, r19
  adc r29, r20
  adc r30, r21

  ; load a byte from the sine wave lookup table. (+3 => 8)
  lpm r0, Z

  ; multiply the lookup table byte by the sine wave gain. (+5 => 13)
  ; the result is stored as two's complement in r1:r0.
  mov r26, r16
  mov r16, r0
  mulsu r16, r17
  mov r16, r26

  ; select the spi dac. (+1 => 14)
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_CS

  ; set the spi dac mosi line low. (+1 => 15)
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_MOSI

  ; clock the spi bus eight times. (+16 => 31)
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK ; 1 (R/W)
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK ; 2 (Zero)
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK ; 3 (REG2)
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK ; 4 (REG1)
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK ; 5 (REG0)
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK ; 6 (A2)
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK ; 7 (A1)
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK ; 8 (A0)

  ; clock out bit 15. (+6 => 37)
  sbrs r1, 7
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_MOSI
  sbrc r1, 7
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_MOSI
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK

  ; clock out bit 14. (+6 => 43)
  sbrs r1, 6
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_MOSI
  sbrc r1, 6
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_MOSI
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK

  ; clock out bit 13. (+6 => 49)
  sbrs r1, 5
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_MOSI
  sbrc r1, 5
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_MOSI
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK

  ; clock out bit 12. (+6 => 55)
  sbrs r1, 4
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_MOSI
  sbrc r1, 4
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_MOSI
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK

  ; clock out bit 11. (+6 => 61)
  sbrs r1, 3
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_MOSI
  sbrc r1, 3
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_MOSI
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK

  ; clock out bit 10. (+6 => 67)
  sbrs r1, 2
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_MOSI
  sbrc r1, 2
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_MOSI
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK

  ; clock out bit 9. (+6 => 73)
  sbrs r1, 1
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_MOSI
  sbrc r1, 1
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_MOSI
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK

  ; clock out bit 8. (+6 => 79)
  sbrs r1, 0
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_MOSI
  sbrc r1, 0
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_MOSI
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK

  ; clock out bit 7. (+6 => 85)
  sbrs r0, 7
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_MOSI
  sbrc r0, 7
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_MOSI
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK

  ; clock out bit 6. (+6 => 91)
  sbrs r0, 6
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_MOSI
  sbrc r0, 6
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_MOSI
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK

  ; clock out bit 5. (+6 => 97)
  sbrs r0, 5
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_MOSI
  sbrc r0, 5
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_MOSI
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK

  ; clock out bit 4. (+6 => 103)
  sbrs r0, 4
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_MOSI
  sbrc r0, 4
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_MOSI
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK

  ; clock out bit 3. (+6 => 109)
  sbrs r0, 3
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_MOSI
  sbrc r0, 3
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_MOSI
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK

  ; clock out bit 2. (+6 => 115)
  sbrs r0, 2
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_MOSI
  sbrc r0, 2
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_MOSI
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK

  ; clock out bit 1. (+6 => 121)
  sbrs r0, 1
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_MOSI
  sbrc r0, 1
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_MOSI
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK

  ; clock out bit 0. (+6 => 127)
  sbrs r0, 0
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_MOSI
  sbrc r0, 0
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_MOSI
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK

  ; deselect the spi dac. (+1 => 128)
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_CS

  ; reset r1 to zero.
  eor r1, r1

  ; increment the (32-bit) overflow counter. (+5 => 133)
  ldi r26, 0x01
  add r13, r26
  adc r14, r1
  adc r15, r1
  adc r16, r1

  ; check if the overflow counter has reached its target. (+6 => 139)
  cp r13, r18
  cpc r14, r19
  cpc r15, r20
  cpc r16, r21
  brne cmp0_end

  ; disable the timer/counter0 compare match interrupt.
  ldi r30, TIMSK0
  ldi r31, 0x00
  ld r26, Z
  andi r26, ~(1 << OCIE0A)
  st Z, r26

  ; raise the completion flag.
  ldi r26, 0x01
  mov r3, r26

  ; restore the status register state and return. (+5 => 144)
cmp0_end:
  out _SFR_IO_ADDR(SREG), r2
  reti

; ============================================================================

; TIMER0_OVF_vect: interrupt service routine: timer/counter0 overflow.
; (see tcnt0_run_interp for register assignments)
.global TIMER0_OVF_vect
TIMER0_OVF_vect:
  ; store the status register state.
  in r2, _SFR_IO_ADDR(SREG)

  ; check if the ADD bit (r19[0]) is set.
  sbrc r19, 0
  rjmp ovf0_add

  ; check if the SUB bit (r19[1]) is set.
  sbrc r19, 1
  rjmp ovf0_sub

  ; we skipped both relative jumps. no change will be made to the dac value.
  rjmp ovf0_write

ovf0_add:
  ; add the accumulation value into the dac output registers.
  add r22, r20
  adc r23, r21
  rjmp ovf0_write

ovf0_sub:
  ; subtract the accumulation value from the dac output registers.
  sub r22, r20
  sbc r23, r21
  rjmp ovf0_write

  ; select the spi dac.
ovf0_write:
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_CS

  ; set the spi dac mosi line low.
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_MOSI

  ; clock the spi bus five times.
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK
  nop
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK ; 1 (R/W)
  nop
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK
  nop
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK ; 2 (Zero)
  nop
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK
  nop
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK ; 3 (REG2)
  nop
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK
  nop
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK ; 4 (REG1)
  nop
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK
  nop
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK ; 5 (REG0)
  nop

  ; clock the A2 DAC selection bit.
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK
  nop
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK ; 6 (A2)
  nop

  ; clock the A1 DAC selection bit.
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_MOSI
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK ; 7 (A1)
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_MOSI

  ; clock the A0 DAC selection bit.
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK
  nop
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK ; 8 (A0)
  nop

  ; clock out bits 15 through 8 (r1).
  ldi r26, 0x80
ovf0_hi_setup:
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_MOSI
  mov r27, r23
  and r27, r26
  brbs 1, ovf0_hi_clock
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_MOSI
ovf0_hi_clock:
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK
  lsr r26
  cpi r26, 0x00
  brne ovf0_hi_setup

  ; clock out bits 7 through 0 (r0).
  ldi r26, 0x80
ovf0_lo_setup:
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_MOSI
  mov r27, r22
  and r27, r26
  brbs 1, ovf0_lo_clock
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_MOSI
ovf0_lo_clock:
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK
  cbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_SCK
  lsr r26
  cpi r26, 0x00
  brne ovf0_lo_setup

  ; deselect the spi dac.
  sbi _SFR_IO_ADDR(GPIO_DAC_PORT), GPIO_DAC_PIN_CS

  ; increment the (16-bit) overflow counter.
  ldi r26, 0x01
  add r17, r26
  adc r18, r1

  ; check if the overflow counter has reached its target.
  cp r17, r24
  cpc r18, r25
  brne ovf0_end

  ; disable the timer/counter0 overflow interrupt.
  ldi r30, TIMSK0
  ldi r31, 0x00
  ld r26, Z
  andi r26, ~(1 << TOIE0)
  st Z, r26

  ; raise the completion flag.
  ldi r26, 0x01
  mov r3, r26

  ; restore the status register state and return.
ovf0_end:
  out _SFR_IO_ADDR(SREG), r2
  reti

; ============================================================================

; tcnt_init: initialize timer/counter peripherals.
.global tcnt_init
tcnt_init:
  ; configure the timer0 prescaler mux for 4.0 us resolution.
  in r20, _SFR_IO_ADDR(TCCR0B)
  ori r20, (1 << CS01)
  ori r20, (1 << CS00)
  out _SFR_IO_ADDR(TCCR0B), r20

  ; configure the timer1 prescaler mux for 62.5 ns resolution.
  ldi r30, TCCR1B
  ldi r31, 0x00
  ld r20, Z
  ori r20, (1 << CS00)
  st Z, r20

  ; configure timer1 to clear on compare match A.
  ldi r30, TCCR1B
  ldi r31, 0x00
  ld r20, Z
  ori r20, (1 << WGM12)
  st Z, r20

  ; return.
  ret

; ============================================================================

; tcnt0_run_interp: execute a timed interpolation between two dac values.
; kicks off TIMER0_OVF_vect
; --
; r2      => status register.
; r3      => completion flag.
; --
; r26     => misc.
; r25:r24 => number of overflows to perform.
; r23:r22 => starting dac output value.
; r21:r20 => accumulation value.
; r19     => accumulation direction (1 is up, 2 is down, 0 is none).
; r18:r17 => overflow counter.
.global tcnt0_run_interp
tcnt0_run_interp:
  ; push all used registers to the stack for protection.
  push r0
  push r2
  push r3
  push r4
  push r5
  push r6
  push r7
  push r8
  push r9
  push r10
  push r11
  push r12
  push r13
  push r14
  push r15
  push r16
  push r28
  push r29


  ; initialize the completion flag.
  ldi r26, 0x00
  mov r3, r26

  ; initialize the overflow counter.
  eor r18, r18
  eor r17, r17

  ; configure timer/counter0 for overflows every 1.024 ms
  in r26, _SFR_IO_ADDR(TCCR0A)
  andi r26, ~(1 << WGM01)
  out _SFR_IO_ADDR(TCCR0A), r26

  ; enable the timer/counter0 overflow interrupt.
  ldi r30, TIMSK0
  ldi r31, 0x00
  ld r26, Z
  ori r26, (1 << TOIE0)
  st Z, r26

  ; loop until the completion flag is raised.
  cp r3, 0x00
  breq .-4

  ; return the final value of the dac output registers.
  mov r25, r23
  mov r24, r22

  ; pop protected registers from the stack.
  pop r29
  pop r28
  pop r16
  pop r15
  pop r14
  pop r13
  pop r12
  pop r11
  pop r10
  pop r9
  pop r8
  pop r7
  pop r6
  pop r5
  pop r4
  pop r3
  pop r2
  pop r0

  ; return.
  ret

; tcnt0_run_pulse: execute a timed direct digital synthesis pulse.
; kicks off TIMER0_COMPA_vect
; --
; r2      => status register.
; r3      => completion flag.
; r12:r11 => sine wave output.
; r16:r13 => overflow counter.
; --
; r31     => lut address high byte.
; r30:r27 => phase accumulator.
; r26     => misc.
; r25:r22 => number of overflows to perform.
; r21:r18 => frequency tuning word.
; r17     => sine wave amplitude multiplier.
.global tcnt0_run_pulse
tcnt0_run_pulse:
  ; push all used registers to the stack for protection.
  push r0
  push r2
  push r3
  push r4
  push r5
  push r6
  push r7
  push r8
  push r9
  push r10
  push r11
  push r12
  push r13
  push r14
  push r15
  push r16
  push r28
  push r29


  ; initialize the completion flag.
  ldi r26, 0x00
  mov r3, r26

  ; initialize the overflow counter.
  eor r13, r13
  eor r14, r14
  eor r15, r15
  eor r16, r16

  ; initialize the low bytes of the phase accumulator.
  eor r27, r27
  eor r28, r28
  eor r29, r29

  ; configure timer/counter0 to overflow on compare match A.
  in r26, _SFR_IO_ADDR(TCCR0A)
  ori r26, (1 << WGM01)
  out _SFR_IO_ADDR(TCCR0A), r26

  ; configure the number of ticks before compare match overflow.
  out _SFR_IO_ADDR(OCR0A), 0x03

  ; zero the timer counter.
  out _SFR_IO_ADDR(TCNT0), 0x00

  ; enable the timer/counter0 compare match A interrupt.
  ldi r30, TIMSK0
  ldi r31, 0x00
  ld r26, Z
  ori r26, (1 << OCIE0A)
  st Z, r26

  ; initialize the high bytes of the phase accumulator.
  ldi r30, lo8(tcnt_dds_lut)
  ldi r31, hi8(tcnt_dds_lut)

  ; loop until the completion flag is raised.
  cp r3, 0x00
  breq .-4

  ; pop protected registers from the stack.
  pop r29
  pop r28
  pop r16
  pop r15
  pop r14
  pop r13
  pop r12
  pop r11
  pop r10
  pop r9
  pop r8
  pop r7
  pop r6
  pop r5
  pop r4
  pop r3
  pop r2
  pop r0

  ; return.
  ret

; tcnt1_run_acquire: execute a timed data acquisition.
; kicks off TIMER1_COMPA_vect
; --
; r2      => status register.
; r3      => completion flag.
; r16:r13 => overflow counter.
; r29:r28 => analog-to-digital conversion.
; --
; r27:r26 => misc.
; r25:r22 => number of overflows to perform.
; r21:r20 => timer/counter1 overflow size (used only during init).
.global tcnt1_run_acquire
tcnt1_run_acquire:
  ; push all used registers to the stack for protection.
  push r0
  push r2
  push r3
  push r4
  push r5
  push r6
  push r7
  push r8
  push r9
  push r10
  push r11
  push r12
  push r13
  push r14
  push r15
  push r16
  push r28
  push r29


  ; initialize the completion flag.
  ldi r26, 0x00
  mov r3, r26

  ; initialize the overflow counter.
  eor r13, r13
  eor r14, r14
  eor r15, r15
  eor r16, r16

  ; configure the number of ticks before compare match overflow.
  ldi r30, OCR1AH
  ldi r31, 0x00
  st Z, r21
  ldi r30, OCR1AL
  ldi r31, 0x00
  st Z, r20

  ; zero the timer counter.
  ldi r30, TCNT1H
  ldi r31, 0x00
  st Z, r1
  ldi r30, TCNT1L
  ldi r31, 0x00
  st Z, r1

  ; enable the timer/counter1 compare match A interrupt.
  ldi r30, TIMSK1
  ldi r31, 0x00
  ld r26, Z
  ori r26, (1 << OCIE1A)
  st Z, r26

  ; loop until the completion flag is raised.
  cp r3, 0x00
  breq .-4

  ; pop protected registers from the stack.
  pop r29
  pop r28
  pop r16
  pop r15
  pop r14
  pop r13
  pop r12
  pop r11
  pop r10
  pop r9
  pop r8
  pop r7
  pop r6
  pop r5
  pop r4
  pop r3
  pop r2
  pop r0

  ; return.
  ret

