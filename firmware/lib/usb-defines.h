
/* usb-defines.h: PPM firmware univeral serial bus macro function header.
 * Copyright (C) 2014  Bradley Worley  <geekysuavo@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* * * * AVR USB CHIP-SPECIFIC DEFINITIONS * * * */

/* redefine the EPNUM[2:0] bits in UENUM to match the m32u4 datasheet. */
#ifdef _AVR_IOM32U4_H_
#define EPNUM0 UENUM_0
#define EPNUM1 UENUM_1
#define EPNUM2 UENUM_2
#endif

/* * * * USB CDC IMPLEMENTATION-SPECIFIC DEFINITIONS * * * */

/* define the manufacturer, product and serial number strings. */
#define USB_STR_MANUFACT L"Bradley Worley"
#define USB_STR_PRODUCT  L"PPM-USB"
#define USB_STR_SERIAL   L"PPMv1r1"

/* define the usb vendor and product id words. */
#define USB_VID 0x03eb /* Atmel corporation */
#define USB_PID 0x2018 /* AT90USBKEY CDC-ACM */

/* usb-serial implementation endpoint count. */
#define USB_EP_COUNT 5

/* usb-serial implementation endpoint numbers. */
#define USB_EP_NUM_CONTROL 0
#define USB_EP_NUM_CDC_ACM 2
#define USB_EP_NUM_CDC_RX  3
#define USB_EP_NUM_CDC_TX  4

/* usb-serial implementation endpoint sizes. */
#define USB_EP_SIZE_CONTROL 16
#define USB_EP_SIZE_CDC_ACM 16
#define USB_EP_SIZE_CDC_RX  32
#define USB_EP_SIZE_CDC_TX  32

/* usb-serial implementation endpoint bank counts. */
#define USB_EP_BANKS_CONTROL 1
#define USB_EP_BANKS_CDC_ACM 1
#define USB_EP_BANKS_CDC_RX  2
#define USB_EP_BANKS_CDC_TX  2

/* * * * USB DEFINITIONS * * * */

/* usb address bitmask. used to set only the UADD bits in UDADDR.
 * m32u2 datasheet, sec 21.18.4, page 212.
 */
#define USB_SET_ADDRESS_MASK \
  ((1 << UADD6) | (1 << UADD5) | (1 << UADD4) | (1 << UADD3) | \
   (1 << UADD2) | (1 << UADD1) | (1 << UADD0))

/* usb endpoint type definitions. these are OR'ed onto the UECFG0X register
 * (alongside the EPDIR bit) during endpoint configuration.
 * m32u2 datasheet, sec 21.18.11, page 215, table 21-2.
 */
#define USB_EP_TYPE_CONTROL     (0x00)
#define USB_EP_TYPE_ISOCHRONOUS (1 << EPTYPE0)
#define USB_EP_TYPE_BULK        (1 << EPTYPE1)
#define USB_EP_TYPE_INTERRUPT   ((1 << EPTYPE1) | (1 << EPTYPE0))

/* usb endpoint direction definitions. these are OR'ed onto the UECFG0X
 * register (alongside the EPTYPE bits) during endpoint configuration.
 * m32u2 datasheet, sec 21.18.11, page 215.
 */
#define USB_EP_DIR_IN   (1 << EPDIR)
#define USB_EP_DIR_OUT  (0x00)
#define USB_EP_DIR_BOTH (0x00)

/* usb standard device request codes.
 * USB 2.0 spec, sec 9.4, page 251, table 9-4.
 */
#define USB_REQUEST_GET_STATUS      0
#define USB_REQUEST_CLEAR_FEATURE   1
#define USB_REQUEST_SET_FEATURE     3
#define USB_REQUEST_SET_ADDRESS     5
#define USB_REQUEST_GET_DESCRIPTOR  6
#define USB_REQUEST_SET_DESCRIPTOR  7
#define USB_REQUEST_GET_CONFIG      8
#define USB_REQUEST_SET_CONFIG      9
#define USB_REQUEST_GET_INTERFACE   10
#define USB_REQUEST_SET_INTERFACE   11

/* * * * CDC DEFINITIONS * * * */

/* usb communications device class request codes.
 * CDC 1.1 spec, sec 6.2, page 53, table 46.
 */
#define CDC_REQUEST_SET_LINE_CODING         0x20
#define CDC_REQUEST_GET_LINE_CODING         0x21
#define CDC_REQUEST_SET_CONTROL_LINE_STATE  0x22

/* mask bits for checking the status of the usb-serial control line.
 * CDC 1.1 spec, sec 6.2, pages 58-59, table 51.
 */
#define USB_CDC_CONTROL_MASK_DTR (1 << 0)
#define USB_CDC_CONTROL_MASK_RTS (1 << 1)

/* mask bits for checking the line coding state of the usb-serial port.
 * CDC 1.1 spec, sec 6.2, page 58, table 50.
 */
#define USB_CDC_CODING_MASK_1_STOP        0
#define USB_CDC_CODING_MASK_1_5_STOP      1
#define USB_CDC_CODING_MASK_2_STOP        2
#define USB_CDC_CODING_MASK_PARITY_NONE   0
#define USB_CDC_CODING_MASK_PARITY_ODD    1
#define USB_CDC_CODING_MASK_PARITY_EVEN   2
#define USB_CDC_CODING_MASK_PARITY_MARK   3
#define USB_CDC_CODING_MASK_PARITY_SPACE  4

/* timeout value, in milliseconds, used to send a zero-length packet
 * to inform the host that any further writes are not currently
 * buffered.
 */
#define USB_CDC_TRANSMIT_FLUSH_TIMEOUT  5

/* timeout value, in milliseconds, to wait before returning an error because
 * the connected host is not listening for incoming data packets.
 */
#define USB_CDC_TRANSMIT_TIMEOUT  25

/* * * * AVR USB DEVICE FUNCTIONS * * * */

/* usb_enable: enables the usb controller and the usb data buffers (D+, D-).
 * m32u2 datasheet, sec 20.10.1, page 195.
 */
#ifdef _AVR_IOM32U4_H_
#define usb_enable() \
  UHWCON |= (1 << UVREGE); \
  USBCON |= (1 << USBE);
#else
#define usb_enable() \
  USBCON |= (1 << USBE);
#endif

/* usb_disable: disables the usb controller and the usb data buffers (D+, D-).
 * m32u2 datasheet, sec 20.10.1, page 195.
 */
#define usb_disable() \
  USBCON &= ~(1 << USBE);

/* usb_reset: resets the usb controller. this function ensures that the usb
 * controller and data buffers have been in a reset state beforehand.
 */
#define usb_reset() \
  usb_disable (); \
  usb_enable ();

/* usb_attach: attaches the usb controller to the bus by enabling the internal
 * 1.5k pull-up resistor on the D+ usb data line.
 * m32u2 datasheet, sec 21.18.1, page 210.
 */
#define usb_attach() \
  UDCON &= ~(1 << DETACH);

/* usb_detach: detaches the usb controller from the bus by disabling the
 * internal 1.5k pull-up resistor on the D+ usb data line.
 * m32u2 datasheet, sec 21.18.1, page 210.
 */
#define usb_detach() \
  UDCON |= (1 << DETACH);

/* usb_clk_freeze: disables the usb controller clock.
 * m32u2 datasheet, sec 20.10.1, page 195.
 */
#define usb_clk_freeze() \
  USBCON |= (1 << FRZCLK);

/* usb_clk_unfreeze: enables the usb controller clock.
 * m32u2 datasheet, sec 20.10.1, page 195.
 */
#define usb_clk_unfreeze() \
  USBCON &= ~(1 << FRZCLK);

/* * * * AVR USB DEVICE INTERRUPT FUNCTIONS * * * */

/* usb_int_enable: enables the specified usb device interrupt.
 * m32u2 datasheet, sec 21.18.3, pages 211-212.
 */
#define usb_int_enable(bit) \
  UDIEN |= (1 << bit);

/* usb_int_disable: disables the specified usb device interrupt.
 * m32u2 datasheet, sec 21.18.3, pages 211-212.
 */
#define usb_int_disable(bit) \
  UDIEN &= ~(1 << bit);

/* usb_int_disable_all: disables all usb device interrupts.
 * m32u2 datasheet, sec 21.18.3, pages 211-212.
 */
#define usb_int_disable_all() \
  UDIEN = 0;

/* usb_int_is_enabled: checks if the specified usb device interrupt is enabled.
 * m32u2 datasheet, sec 21.18.3, pages 211-212.
 */
#define usb_int_is_enabled(bit) \
  (UDIEN & (1 << bit))

/* usb_int_is_flagged: checks if the specified usb device interrupt is flagged.
 * m32u2 datasheet, sec 21.18.2, pages 210-211.
 */
#define usb_int_is_flagged(bit) \
  (UDINT & (1 << bit))

/* usb_int_clear: clears the specified usb device interrupt flag.
 * m32u2 datasheet, sec 21.18.2, pages 210-211.
 */
#define usb_int_clear(bit) \
  UDINT &= ~(1 << bit);

/* usb_int_clear_all: clears all usb device interrupt flags.
 * m32u2 datasheet, sec 21.18.2, pages 210-211.
 */
#define usb_int_clear_all() \
  UDINT = 0;

/* * * * AVR USB ENDPOINT FUNCTIONS * * * */

/* usb_ep_select: selects the specified usb endpoint.
 * m32u2 datasheet, sec 21.18.8, page 214.
 */
#define usb_ep_select(epnum) \
  UENUM = (epnum & ((1 << EPNUM2) | (1 << EPNUM1) | (1 << EPNUM0)));

/* usb_ep_enable: enables the currently selected endpoint.
 * m32u2 datasheet, sec 21.18.10, page 215.
 */
#define usb_ep_enable() \
  UECONX = (1 << EPEN);

/* usb_ep_disable: disables the currently selected endpoint.
 * m32u2 datasheet, sec 21.18.10, page 215.
 */
#define usb_ep_disable() \
  UECONX &= ~(1 << EPEN);

/* usb_ep_stall: enables returning a STALL handshake request.
 * m32u2 datasheet, sec 21.18.10, page 214.
 */
#define usb_ep_stall() \
  UECONX = ((1 << STALLRQ) | (1 << EPEN));

/* usb_ep_end_stall: clears the STALLRQ bit in the UECONX register.
 * m32u2 datasheet, sec 21.18.10, page 214.
 */
#define usb_ep_end_stall() \
  UECONX |= (1 << STALLRQC);

/* usb_ep_reset: assert and de-assert the currently select endpoint reset.
 * m32u2 datasheet, sec 21.18.9, page 214.
 */
#define usb_ep_reset() \
  UERST = ((1 << EPRST4) | (1 << EPRST3) | (1 << EPRST2) | (1 << EPRST1)); \
  UERST = 0x00;

/* usb_ep_size: usb endpoint size bitfield calculation macro. used to set the
 * EPSIZE[2:0] bits in the UECFG1X register during endpoint configuration.
 * m32u2 datasheet, sec 21.18.12, page 216, table 21-3.
 */
#define usb_ep_size(sz) \
  ((sz) == 64 ? ((1 << EPSIZE1) | (1 << EPSIZE0)) : \
   ((sz) == 32 ? (1 << EPSIZE1) : \
    ((sz) == 16 ? (1 << EPSIZE0) : 0x00)))

/* usb_ep_banks: usb endpoint banks bitfield calculation macro. used to set
 * the EPBK[1:0] bits in the UECFG1X register during endpoint configuration.
 * m32u2 datasheet, sec 21.18.12, page 216, table 21-4.
 */
#define usb_ep_banks(banks) \
  (banks > 1 ? (1 << EPBK0) : 0x00)

/* usb_ep_conf: configures the currently selected endpoint.
 * m32u2 datasheet, secs 21.18.11-12, pages 215-216.
 */
#define usb_ep_conf(typ,dir,sz,banks) \
  UECFG0X = (typ | dir); \
  UECFG1X = (usb_ep_size (sz) | usb_ep_banks (banks) | (1 << ALLOC));

/* usb_ep_bytes: returns the number of bytes in the current endpoint bank.
 * m32u2 datasheet, sec 21.18.18, page 221.
 */
#define usb_ep_bytes() \
  UEBCLX

/* * * * AVR USB ENDPOINT INTERRUPT FUNCTIONS * * * */

/* usb_ep_int_enable: enables the specified usb endpoint interrupt.
 * m32u2 datasheet, sec 21.18.16, pages 220-221.
 */
#define usb_ep_int_enable(bit) \
  UEIENX |= (1 << bit);

/* usb_ep_int_disable: disables the specified usb endpoint interrupt.
 * m32u2 datasheet, sec 21.18.16, pages 220-221.
 */
#define usb_ep_int_disable(bit) \
  UEIENX &= ~(1 << bit);

/* usb_ep_int_disable_all: disables all usb endpoint interrupts.
 * m32u2 datasheet, sec 21.18.16, pages 220-221.
 */
#define usb_ep_int_disable_all() \
  UEIENX = 0;

/* usb_ep_int_is_enabled: checks if a usb endpoint interrupt is enabled.
 * m32u2 datasheet, sec 21.18.16, pages 220-221.
 */
#define usb_ep_int_is_enabled(bit) \
  (UEIENX & (1 << bit))

/* usb_ep_int_is_flagged: checks if a usb endpoint interrupt is flagged.
 * m32u2 datasheet, sec 21.18.15, pages 219-220.
 */
#define usb_ep_int_is_flagged(bit) \
  (UEINTX & (1 << bit))

/* usb_ep_int_clear: clears the specified usb endpoint interrupt flag.
 * m32u2 datasheet, sec 21.18.15, pages 219-220.
 */
#define usb_ep_int_clear(bit) \
  UEINTX &= ~(1 << bit);

/* usb_ep_int_clear_all: clears all usb endpoint interrupt flags.
 * m32u2 datasheet, sec 21.18.15, pages 219-220.
 */
#define usb_ep_int_clear_all() \
  UEINTX = 0;

/* * * * AVR USB IN/OUT ENDPOINT FUNCTIONS * * * */

/* usb_wait_tx: wait for the current IN endpoint bank to become ready.
 * m32u2 datasheet, sec 21.18.15, page 220.
 */
#define usb_wait_tx() \
  while (!usb_ep_int_is_flagged (TXINI));

/* usb_ack_tx: acknowledge that the current IN endpoint bank can be filled.
 * m32u2 datasheet, sec 21.18.15, page 220.
 */
#define usb_ack_tx() \
  usb_ep_int_clear (TXINI);

/* usb_wait_rx: wait for new data on the current OUT endpoint bank.
 * m32u2 datasheet, sec 21.18.15, pages 219-220.
 */
#define usb_wait_rx() \
  while (!usb_ep_int_is_flagged (RXOUTI));

/* usb_ack_rx: acknowledge that the current OUT endpoint bank has new data.
 * m32u2 datasheet, sec 21.18.15, pages 219-220.
 */
#define usb_ack_rx() \
  usb_ep_int_clear (RXOUTI);

/* * * * USB ENDPOINT INLINE TRANSFER FUNCTIONS * * * */

/* usb_ep_read_8: reads a byte from the selected endpoint FIFO.
 * m32u2 datasheet, sec 21.18.17, page 221.
 */
static inline uint8_t usb_ep_read_8 (void) {
  /* return the value of the data register. */
  return UEDATX;
}

/* usb_ep_read_16le: reads a little-endian 16-bit word from the selected
 * endpoint FIFO.
 * m32u2 datasheet, sec 21.18.17, page 221.
 */
static inline uint16_t usb_ep_read_16le (void) {
  /* declare the output value. */
  uint16_t data = 0;

  /* read the value into the variable. */
  data = UEDATX;
  data |= (UEDATX << 8);

  /* return the value. */
  return data;
}

/* usb_ep_write_8: writes a byte to the selected endpoint FIFO.
 * m32u2 datasheet, sec 21.18.17, page 221.
 */
static inline void usb_ep_write_8 (uint8_t data) {
  /* write the value to the data register. */
  UEDATX = data;
}

/* usb_ep_write_16le: writes a little-endian 16-bit word to the selected
 * endpoint FIFO.
 * m32u2 datasheet, sec 21.18.17, page 221.
 */
static inline void usb_ep_write_16le (uint16_t data) {
  /* write the LSB first. */
  UEDATX = LSB (data);
  UEDATX = MSB (data);
}

/* usb_ep_control_write_P: performs a control write from program memory.
 * m32u2 datasheet, sec 21.12.2, page 203.
 */
static inline void usb_ep_control_write_P (const uint8_t *data_addr,
                                           uint8_t data_len,
                                           uint16_t wLength) {
  /* declare required variables. */
  uint8_t last_packet_full = 0;
  uint8_t len = data_len;
  uint8_t bytes = 0;

  /* see if the data size exceeds the size of the transfer. */
  if (len > wLength) {
    /* resize the transfer to send the maximum size. */
    len = wLength;
  }
  else if (!len) {
    /* clear the endpoint bank. */
    usb_ack_tx ();
  }

  /* loop while memory remains to be sent. */
  while (len || last_packet_full) {
    /* wait for the transmitter. */
    usb_wait_tx ();

    /* get the number of bytes in the endpoint bank. */
    bytes = usb_ep_bytes ();

    /* loop while data needs to be sent. */
    while (len && (bytes < USB_EP_SIZE_CONTROL)) {
      /* transfer a byte of the data. */
      usb_ep_write_8 (pgm_read_byte (data_addr));
      data_addr++;

      /* change the byte counters. */
      len--;
      bytes++;
    }

    /* transmit. */
    last_packet_full = (bytes == USB_EP_SIZE_CONTROL);
    usb_ack_tx ();
  }
}
