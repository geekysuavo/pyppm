
/* usb.c: PPM firmware univeral serial bus source code.
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

/* include the usb and clocking headers. */
#include "clk.h"
#include "usb.h"

/* include the usb defines header. */
#include "usb-defines.h"

/* * * * USB ENDPOINT CONFIGURATION TABLE * * * */

/* define the usb endpoint configuration table. */
static const uint8_t usb_ep_configuration_table[] PROGMEM = {
  0, /* Endpoint 1: not used for memory allocation reasons. */

  /* Endpoint 2: CDC ACM. interrupt IN endpoint, one bank. */
  1, USB_EP_TYPE_INTERRUPT, USB_EP_DIR_IN,
     USB_EP_SIZE_CDC_ACM,   USB_EP_BANKS_CDC_ACM,

  /* Endpoint 3: CDC RX. bulk OUT endpoint, two banks. */
  1, USB_EP_TYPE_BULK,      USB_EP_DIR_OUT,
     USB_EP_SIZE_CDC_RX,    USB_EP_BANKS_CDC_RX,

  /* Endpoint 4: CDC TX. bulk IN endpoint, two banks. */
  1, USB_EP_TYPE_BULK,      USB_EP_DIR_IN,
     USB_EP_SIZE_CDC_TX,    USB_EP_BANKS_CDC_TX
};

/* * * * USB DESCRIPTOR BYTE ARRAYS * * * */

/* define the usb device descriptor.
 * USB 2.0 spec, sec 9.6.1, pages 261-263, table 9-8.
 */
#define USB_DESC_SIZE_DEVICE 18
static const uint8_t PROGMEM usb_desc_device[USB_DESC_SIZE_DEVICE] = {
  USB_DESC_SIZE_DEVICE,         /* bLength            [18 bytes] */
  1,                            /* bDescriptorType    [DEVICE] */
  0x00, 0x02,                   /* bcdUSB             [0x0200 (USB 2.0)] */
  0x02,                         /* bDeviceClass       [CDC] */
  0x00,                         /* bDeviceSubClass    [Unused by CDC] */
  0x00,                         /* bDeviceProtocol    [Unused by CDC] */
  USB_EP_SIZE_CONTROL,          /* bMaxPacketSize0    [16 bytes] */
  LSB (USB_VID), MSB (USB_VID), /* idVendor           [Atmel] */
  LSB (USB_PID), MSB (USB_PID), /* idProduct          [AT90USBKEY] */
  0x00, 0x01,                   /* bcdDevice          [0x0100 (1.0)] */
  1,                            /* iManufacturer      [1] */
  2,                            /* iProduct           [2] */
  3,                            /* iSerialNumber      [3] */
  1                             /* bNumConfigurations [1] */
};

/* define the usb configuration descriptor.
 * USB 2.0 spec and CDC 1.1 spec. see below.
 */
#define USB_DESC_SIZE_CONFIG 67
static const uint8_t PROGMEM usb_desc_config[USB_DESC_SIZE_CONFIG] = {
  /* Configuration: USB 2.0 spec, sec 9.6.3, pages 264-266, table 9-10. */
  9,                          /* bLength             [9 bytes] */
  2,                          /* bDescriptorType     [CONFIGURATION] */
  LSB (USB_DESC_SIZE_CONFIG), /* wTotalLength        [67 bytes] */
  MSB (USB_DESC_SIZE_CONFIG),
  2,                          /* bNumInterfaces      [2] */
  1,                          /* bConfigurationValue [1] */
  0,                          /* iConfiguration      [None] */
  0xc0,                       /* bmAttributes        [Default] */
  250,                        /* bMaxPower           [500 mA] */

  /* Interface 0: USB 2.0 spec, sec 9.6.3, pages 267-269, table 9-12. */
  9,    /* bLength            [9 bytes] */
  4,    /* bDescriptorType    [INTERFACE] */
  0,    /* bInterfaceNumber   [0 (First)] */
  0,    /* bAlternateSetting  [0] */
  1,    /* bNumEndpoints      [1] */
  0x02, /* bInterfaceClass    [Communication Interface Class (CDC)] */
  0x02, /* bInterfaceSubClass [Abstract Control Model (ACM)] */
  0x01, /* bInterfaceProtocol [V.25ter (AT common commands)] */
  0,    /* iInterface         [None] */

  /* Header: CDC 1.1 spec, sec 5.2.3.1, page 34, table 26. */
  5,          /* bFunctionLength    [5 bytes] */
  0x24,       /* bDescriptorType    [CS_INTERFACE] */
  0x00,       /* bDescriptorSubtype [Header Function] */
  0x10, 0x01, /* bcdCDC             [0x0110 (CDC 1.1)] */

  /* Call Management: CDC 1.1 spec, sec 5.2.3.2, pages 34-35, table 27. */
  5,    /* bFunctionLength    [5 bytes] */
  0x24, /* bDescriptorType    [CS_INTERFACE] */
  0x01, /* bDescriptorSubtype [Call Management Function] */
  0x01, /* bmCapabilities     [CDC device handles call management] */
  1,    /* bDataInterface     [1] */

  /* ACM: CDC 1.1 spec, sec 5.2.3.3, pages 35-36, table 28. */
  4,    /* bFunctionLength    [4 bytes] */
  0x24, /* bDescriptorType    [CS_INTERFACE] */
  0x02, /* bDescriptorSubtype [Abstract Control Management Function] */
  0x06, /* bmCapabilities
         *  [Supported requests]
         *   Send break,
         *   Set line coding,
         *   Get line coding,
         *   Set control line state
         */

  /* Union: CDC 1.1 spec, sec 5.2.3.8, pages 40-41, table 33. */
  5,    /* bFunctionLength    [5 bytes] */
  0x24, /* bDescriptorType    [CS_INTERFACE] */
  0x06, /* bDescriptorSubtype [Union Function] */
  0,    /* bMasterInterface   [Interface 0 (ACM)] */
  1,    /* bSlaveInterface0   [Interface 1 (Data)] */

  /* Endpoint: USB 2.0 spec, sec 9.6.6, pages 269-271, table 9-13. */
  7,                           /* bLength          [7 bytes] */
  5,                           /* bDescriptorType  [ENDPOINT] */
  USB_EP_NUM_CDC_ACM | 0x80,   /* bEndpointAddress [Endpoint 2, IN] */
  0x03,                        /* bmAttributes     [Interrupt] */
  USB_EP_SIZE_CDC_ACM, 0,      /* wMaxPacketSize   [16 bytes] */
  64,                          /* bInterval        [?] */

  /* Interface: USB 2.0 spec, sec 9.6.5, pages 267-269, table 9-12. */
  9,    /* bLength            [9 bytes] */
  4,    /* bDescriptorType    [INTERFACE] */
  1,    /* bInterfaceNumber   [1 (Second)] */
  0,    /* bAlternateSetting  [0] */
  2,    /* bNumEndpoints      [2] */
  0x0a, /* bInterfaceClass    [CDC Data] */
  0x00, /* bInterfaceSubClass [Unused] */
  0x00, /* bInterfaceProtocol [Unused] */
  0,    /* iInterface         [None] */

  /* Endpoint: USB 2.0 spec, sec 9.6.6, pages 269-271, table 9-13. */
  7,                     /* bLength          [7 bytes] */
  5,                     /* bDescriptorType  [ENDPOINT] */
  USB_EP_NUM_CDC_RX,     /* bEndpointAddress [Endpoint 3, OUT] */
  0x02,                  /* bmAttributes     [Bulk] */
  USB_EP_SIZE_CDC_RX, 0, /* wMaxPacketSize   [32 bytes] */
  0,                     /* bInterval        [Endpoint never NAKs] */

  /* Endpoint: USB 2.0 spec, sec 9.6.6, pages 269-271, table 9-13. */
  7,                        /* bLength           [7 bytes] */
  5,                        /* bDescriptorType   [ENDPOINT] */
  USB_EP_NUM_CDC_TX | 0x80, /* bmEndpointAddress [Endpoint 4, IN] */
  0x02,                     /* bmAttributes      [Bulk] */
  USB_EP_SIZE_CDC_TX, 0,    /* wMaxPacketSize    [32 bytes] */
  0                         /* bInterval         [Endpoint never NAKs] */
};

/* define the usb descriptor string structure.
 * USB 2.0 spec, sec 9.6.7, pages 273-274, tables 9-15 and 9-16.
 */
struct usb_descriptor_string_t {
  uint8_t bLength;         /* bLength */
  uint8_t bDescriptorType; /* bDescriptorType [STRING] */
  int16_t wString[];       /* wLANGID[0] or bString */
};

/* declare the language descriptor string. */
static const struct usb_descriptor_string_t PROGMEM usb_str_lang = {
  4, 3, { 0x0409 }
};

/* declare the manufacturer descriptor string. */
static const struct usb_descriptor_string_t PROGMEM usb_str_manf = {
  sizeof (USB_STR_MANUFACT), 3, USB_STR_MANUFACT
};

/* declare the product descriptor string. */
static const struct usb_descriptor_string_t PROGMEM usb_str_prod = {
  sizeof (USB_STR_PRODUCT), 3, USB_STR_PRODUCT
};

/* declare the serial number descriptor string. */
static const struct usb_descriptor_string_t PROGMEM usb_str_sern = {
  sizeof (USB_STR_SERIAL), 3, USB_STR_SERIAL
};

/* declare the list of descriptors that may be accessed during device
 * enumeration and configuration by the host.
 */
static const struct usb_descriptor_list_t {
  uint16_t wValue;
  uint16_t wIndex;
  const uint8_t *addr;
  uint8_t length;
} PROGMEM usb_descriptor_list[] = {
  { 0x0100, 0x0000, usb_desc_device, sizeof (usb_desc_device) },
  { 0x0200, 0x0000, usb_desc_config, sizeof (usb_desc_config) },
  { 0x0300, 0x0000, (const uint8_t *) &usb_str_lang, 4 },
  { 0x0301, 0x0409, (const uint8_t *) &usb_str_manf,
    sizeof (USB_STR_MANUFACT) },
  { 0x0302, 0x0409, (const uint8_t *) &usb_str_prod,
    sizeof (USB_STR_PRODUCT) },
  { 0x0303, 0x0409, (const uint8_t *) &usb_str_sern,
    sizeof (USB_STR_SERIAL) }
};

/* define a counter for the number of descriptor list elements. */
#define USB_DESCRIPTOR_LIST_COUNT \
 (sizeof (usb_descriptor_list) / sizeof (struct usb_descriptor_list_t))

/* * * * USB SERIAL STATE AND CONFIGURATION VARIABLES * * * */

/* define the storage location of the current configuration state. */
static volatile uint8_t usb_config_state = 0;

/* declare variables to store the time remaining before the cdc device
 * transmits any partially full packet, or send a zero length packet.
 */
static volatile uint8_t usb_cdc_transmit_flush_timer = 0;
static uint8_t usb_cdc_transmit_previous_timeout = 0;

/* declare a variable that holds the serial port settings of the cdc.
 * CDC 1.1 spec, sec 6.2.13, page 58, table 50.
 */
static uint8_t usb_cdc_line_coding[7] = {
  0x00, 0xe1, 0x00, 0x00, /* dwDTERate   [kb/s] */
  0x00,                   /* bCharFormat [1 stop bit] */
  0x00,                   /* bParityType [None] */
  0x08                    /* bDataBits   [8] */
};

/* declare a variable that holds the serial port control signal states. */
static uint8_t usb_cdc_line_rtsdtr = 0;

/* * * * PUBLIC USB API FUNCTIONS * * * */

/* usb_init: initializes the usb controller and begins enumeration. */
void usb_init (void) {
  /* initialize the configuration state. */
  usb_config_state = 0;

  /* enable the usb controller in a frozen state. */
  usb_clk_freeze ();
  usb_enable ();

  /* initialize the usb controller clock pll. */
  pll_init ();

  /* unfreeze the usb controller clock. */
  usb_clk_unfreeze ();

  /* attach the device to the bus. */
  usb_attach ();

  /* disable and clear all device interrupts. */
  usb_int_disable_all ();
  usb_int_clear_all ();

  /* enable the end-of-reset and start-of-frame device interrupts. */
  usb_int_enable (EORSTE);
  usb_int_enable (SOFE);

  /* enable interrupts. */
  sei ();
}

/* usb_configured: returns the current configuration state of the usb device,
 * which is zero for non-configured and one for configured.
 */
uint8_t usb_configured (void) {
  /* return the current configuration state. */
  return usb_config_state;
}

/* usb_cdc_getchar: read the next buffered character, or -1 for no data. */
int16_t usb_cdc_getchar (void) {
  /* declare required variables. */
  uint8_t ch;

  /* freeze interrupts. */
  sreg_freeze ();

  /* check if the device is configured. */
  if (!usb_config_state) {
    /* not configured. return an error. */
    sreg_unfreeze ();
    return -1;
  }

  /* select the rx endpoint. */
  usb_ep_select (USB_EP_NUM_CDC_RX);

/* gross. */
retry:
  /* read the state of the endpoint interrupts. */
  ch = UEINTX;

  /* see if data is in the buffer. */
  if (!(ch & (1 << RWAL))) {
    /* no data exists in the buffer. */
    if (ch & (1 << RXOUTI)) {
      /* set a new interrupt state. */
      UEINTX = 0x6b;

      /* try again. */
      goto retry;
    }

    /* unfreeze interrupts and return an error. */
    sreg_unfreeze ();
    return -1;
  }

  /* take one byte out of the buffer. */
  ch = usb_ep_read_8 ();
  
  // if buffer completely used, release it
  if (!(UEINTX & (1 << RWAL))) {
    /* set a new interrupt state. */
    UEINTX = 0x6b;
  }

  /* unfreeze interrupts and return the data. */
  sreg_unfreeze ();
  return ch;
}

/* usb_cdc_avail: returns the number of available bytes to receive. */
uint8_t usb_cdc_avail (void) {
  /* declare required variables. */
  uint8_t i, n = 0;

  /* freeze interrupts. */
  sreg_freeze ();

  /* make sure the device is configured. */
  if (usb_config_state) {
    /* select the rx endpoint. */
    usb_ep_select (USB_EP_NUM_CDC_RX);

    /* read the number of bytes in the buffer. */
    n = usb_ep_bytes ();

    /* did we read none? */
    if (!n) {
      /* read the endpoint interrupt state. */
      i = UEINTX;

      /* see if interrupts need changing. */
      if (i & (1 << RXOUTI) && !(i & (1 << RWAL))) {
        /* set a new interrupt state. */
        UEINTX = 0x6b;
      }
    }
  }

  /* unfreeze interrupts and return the byte count. */
  sreg_unfreeze ();
  return n;
}

/* usb_cdc_flush_input: discard any buffered input data. */
void usb_cdc_flush_input (void) {
  /* make sure the device is configured. */
  if (usb_config_state) {
    /* freeze interrupts. */
    sreg_freeze ();

    /* select the rx endpoint. */
    usb_ep_select (USB_EP_NUM_CDC_RX);

    /* loop while bytes are available. */
    while (usb_ep_int_is_flagged (RWAL))
      UEINTX = 0x6b;

    /* unfreeze interrupts. */
    sreg_unfreeze ();
  }
}

/* usb_cdc_putchar: writes a character. returns 0 on success, -1 otherwise. */
int8_t usb_cdc_putchar (uint8_t ch) {
  /* declare required variables. */
  uint8_t timeout;

  /* return an error if the device is unconfigured. */
  if (!usb_config_state)
    return -1;

  /* freeze interrupts. */
  sreg_freeze ();

  /* select the tx endpoint. */
  usb_ep_select (USB_EP_NUM_CDC_TX);

  /* if a previous invocation timed out, don't wait again. */
  if (usb_cdc_transmit_previous_timeout) {
    /* see if data is ready. */
    if (!usb_ep_int_is_flagged (RWAL)) {
      /* return an error. */
      sreg_unfreeze ();
      return -1;
    }

    /* zero the timeout flag. */
    usb_cdc_transmit_previous_timeout = 0;
  }

  /* wait for the FIFO to become ready to accept data. */
  timeout = UDFNUML + USB_CDC_TRANSMIT_TIMEOUT;
  while (1) {
    /* stop waiting if the device is ready to transmit. */
    if (usb_ep_int_is_flagged (RWAL))
      break;

    /* unfreeze interrupts. */
    sreg_unfreeze ();

    /* see if the loop has gone past the timeout. */
    if (UDFNUML == timeout) {
      /* yes. return an error. */
      usb_cdc_transmit_previous_timeout = 1;
      return -1;
    }

    /* see if the host has been disconnected. */
    if (!usb_config_state)
      return -1;

    /* prepare to check for readiness again. */
    sreg_refreeze ();
    usb_ep_select (USB_EP_NUM_CDC_TX);
  }

  /* write the byte to the FIFO. */
  usb_ep_write_8 (ch);

  /* if this byte completed a packet, send it now. */
  if (!usb_ep_int_is_flagged (RWAL))
    UEINTX = 0x3a;

  /* reset the transmit timeout value. */
  usb_cdc_transmit_flush_timer = USB_CDC_TRANSMIT_FLUSH_TIMEOUT;

  /* unfreeze interrupts and return success. */
  sreg_unfreeze ();
  return 0;
}

/* usb_cdc_putchar_immediate: transmit a character without waiting if the
 * output FIFO is full. success returns 0, and buffer full or error is -1.
 */
int8_t usb_cdc_putchar_immediate (uint8_t ch) {
  /* return an error if the device is unconfigured. */
  if (!usb_config_state)
    return -1;

  /* freeze interrupts. */
  sreg_freeze ();

  /* select the tx endpoint. */
  usb_ep_select (USB_EP_NUM_CDC_TX);

  /* see if the buffer is full. */
  if (!usb_ep_int_is_flagged (RWAL)) {
    /* unfreeze interrupts and return an error. */
    sreg_unfreeze ();
    return -1;
  }

  /* write the byte into the FIFO. */
  usb_ep_write_8 (ch);

  /* if this byte completed a packet, send it now. */
  if (!usb_ep_int_is_flagged (RWAL))
    UEINTX = 0x3a;

  /* reset the transmit timeout value. */
  usb_cdc_transmit_flush_timer = USB_CDC_TRANSMIT_FLUSH_TIMEOUT;

  /* unfreeze interrupts and return success. */
  sreg_unfreeze ();
  return 0;
}

/* usb_cdc_write: transmits a multi-byte buffer, up to 32 bytes. */
int8_t usb_cdc_write (const uint8_t *buf, uint16_t size) {
  /* declare required variables. */
  uint8_t timeout, write_size;

  /* return an error if the device is unconfigured. */
  if (!usb_config_state)
    return -1;

  /* freeze interrupts. */
  sreg_freeze ();

  /* select the tx endpoint. */
  usb_ep_select (USB_EP_NUM_CDC_TX);

  /* if a previous invocation timed out, don't wait again. */
  if (usb_cdc_transmit_previous_timeout) {
    /* see if data is ready. */
    if (!usb_ep_int_is_flagged (RWAL)) {
      /* return an error. */
      sreg_unfreeze ();
      return -1;
    }

    /* zero the timeout flag. */
    usb_cdc_transmit_previous_timeout = 0;
  }

  /* each loop iteration transmits a packet. */
  while (size) {
    /* wait for the FIFO to become ready to accept data. */
    timeout = UDFNUML + USB_CDC_TRANSMIT_TIMEOUT;
    while (1) {
      /* stop waiting if the device is ready to transmit. */
      if (usb_ep_int_is_flagged (RWAL))
        break;

      /* unfreeze interrupts. */
      sreg_unfreeze ();

      /* see if the loop has gone past the timeout. */
      if (UDFNUML == timeout) {
        /* yes. return an error. */
        usb_cdc_transmit_previous_timeout = 1;
        return -1;
      }

      /* see if the host has been disconnected. */
      if (!usb_config_state)
        return -1;

      /* prepare to check for readiness again. */
      sreg_refreeze ();
      usb_ep_select (USB_EP_NUM_CDC_TX);
    }

    /* find the number of bytes that'll fit into the next packet. */
    write_size = USB_EP_SIZE_CDC_TX - usb_ep_bytes ();

    /* bound the write size to the size of the buffer. */
    if (write_size > size)
      write_size = size;

    /* subtract the bytes to be written from the bytes remaining. */
    size -= write_size;

    /* write the packet. */
    switch (write_size) {
      case 32: usb_ep_write_8 (*buf++);
      case 31: usb_ep_write_8 (*buf++);
      case 30: usb_ep_write_8 (*buf++);
      case 29: usb_ep_write_8 (*buf++);
      case 28: usb_ep_write_8 (*buf++);
      case 27: usb_ep_write_8 (*buf++);
      case 26: usb_ep_write_8 (*buf++);
      case 25: usb_ep_write_8 (*buf++);
      case 24: usb_ep_write_8 (*buf++);
      case 23: usb_ep_write_8 (*buf++);
      case 22: usb_ep_write_8 (*buf++);
      case 21: usb_ep_write_8 (*buf++);
      case 20: usb_ep_write_8 (*buf++);
      case 19: usb_ep_write_8 (*buf++);
      case 18: usb_ep_write_8 (*buf++);
      case 17: usb_ep_write_8 (*buf++);
      case 16: usb_ep_write_8 (*buf++);
      case 15: usb_ep_write_8 (*buf++);
      case 14: usb_ep_write_8 (*buf++);
      case 13: usb_ep_write_8 (*buf++);
      case 12: usb_ep_write_8 (*buf++);
      case 11: usb_ep_write_8 (*buf++);
      case 10: usb_ep_write_8 (*buf++);
      case  9: usb_ep_write_8 (*buf++);
      case  8: usb_ep_write_8 (*buf++);
      case  7: usb_ep_write_8 (*buf++);
      case  6: usb_ep_write_8 (*buf++);
      case  5: usb_ep_write_8 (*buf++);
      case  4: usb_ep_write_8 (*buf++);
      case  3: usb_ep_write_8 (*buf++);
      case  2: usb_ep_write_8 (*buf++);
      default:
      case  1: usb_ep_write_8 (*buf++);
      case  0: break;
    }

    /* if this byte completed a packet, send it now. */
    if (!usb_ep_int_is_flagged (RWAL))
      UEINTX = 0x3a;

    /* reset the transmit timeout value. */
    usb_cdc_transmit_flush_timer = USB_CDC_TRANSMIT_FLUSH_TIMEOUT;

    /* unfreeze interrupts and return success. */
    sreg_unfreeze ();
  }

  /* return success. */
  return 0;
}

/* usb_cdc_flush_output: immediately transmit any buffered output data.
 * actually, not immediate, but the FIFO is released for when the host
 * requests data.
 */
void usb_cdc_flush_output (void) {
  /* freeze interrupts. */
  sreg_freeze ();

  /* see if the flush timeout is nonzero. */
  if (usb_cdc_transmit_flush_timer) {
    /* nonzero. select the tx endpoint. */
    usb_ep_select (USB_EP_NUM_CDC_TX);

    /* set a new interrupt state. */
    UEINTX = 0x3a;

    /* reset the flush timer. */
    usb_cdc_transmit_flush_timer = 0;
  }

  /* unfreeze interrupts. */
  sreg_unfreeze ();
}

/* usb_cdc_control_line_dtr: returns whether DTR is on or off. */
uint8_t usb_cdc_control_line_dtr (void) {
  /* return the status of the DTR bit in the control line byte. */
  return (usb_cdc_line_rtsdtr & USB_CDC_CONTROL_MASK_DTR);
}

/* usb_cdc_control_line_rts: returns whether RTS is on or off. */
uint8_t usb_cdc_control_line_rts (void) {
  /* return the status of the RTS bit in the control line byte. */
  return (usb_cdc_line_rtsdtr & USB_CDC_CONTROL_MASK_RTS);
}

/* * * * USB PRIVATE FUNCTIONS * * * */

/* ISR (USB_GEN_vect): usb device interrupt service routine.
 */
ISR (USB_GEN_vect) {
  /* declare a variable to store timeout calculations. */
  uint8_t t;
  
  /* handler for: end-of-reset interrupt. */
  if (usb_int_is_flagged (EORSTI)) {
    /* clear the interrupt flag. */
    usb_int_clear (EORSTI);

    /* select and enable the control endpoint. */
    usb_ep_select (USB_EP_NUM_CONTROL);
    usb_ep_enable ();

    /* configure the control endpoint. */
    usb_ep_conf (USB_EP_TYPE_CONTROL, USB_EP_DIR_BOTH,
                 USB_EP_SIZE_CONTROL, USB_EP_BANKS_CONTROL);

    /* enable the setup-received endpoint interrupt. */
    usb_ep_int_disable_all ();
    usb_ep_int_enable (RXSTPE);

    /* reset the usb configuration state. */
    usb_config_state = 0;

    /* reset the cdc line state. */
    usb_cdc_line_rtsdtr = 0;
  }

  /* handler for: start-of-frame interrupt. */
  if (usb_int_is_flagged (SOFI)) {
    /* clear the interrupt flag. */
    usb_int_clear (SOFI);

    /* ensure that the usb device is configured. */
    if (usb_config_state) {
      /* get the value of the flush timer. */
      t = usb_cdc_transmit_flush_timer;

      /* is there time remaining on the flush timer? */
      if (t) {
        /* decrement the flush timer. */
        usb_cdc_transmit_flush_timer = --t;

        /* have we reached a timeout condition? */
        if (!t) {
          /* select the tx endpoint. */
          usb_ep_select (USB_EP_NUM_CDC_TX);

          /* set the interrupt state. */
          UEINTX = 0x3a;
        }
      }
    }
  }
}

/* ISR (USB_COM_vect): usb endpoint interrupt service routine.
 */
ISR (USB_COM_vect) {
  /* declare variables for endpoint configuration. */
  uint8_t ep_enab, ep_type, ep_dir, ep_size, ep_banks;

  /* declare a variable for storing the endpoint interrupt state. */
  uint8_t ep_ints;

  /* declare variables for handling usb requests. */
  uint8_t bmRequestType;
  uint8_t bRequest;
  uint16_t wValue;
  uint16_t wIndex;
  uint16_t wLength;

  /* declare program-space memory address variables for the descriptor
   * list, configuration struct, and descriptor address, respectively.
   */
  const uint8_t *lst, *cfg, *desc_addr;

  /* declare a value and index variable set for loading descriptors. */
  uint16_t desc_val;
  uint8_t desc_len;

  /* declare miscellaneous variables for doing the stupid, simple things. */
  uint8_t i, n, len;
  uint8_t *p;

  /* select the control endpoint. */
  usb_ep_select (USB_EP_NUM_CONTROL);

  /* read the endpoint interrupt state. */
  ep_ints = UEINTX;

  /* handler for: SETUP interrupts. */
  if (ep_ints & (1 << RXSTPI)) {
    /* read in the request type bitmap and request byte. */
    bmRequestType = usb_ep_read_8 ();
    bRequest = usb_ep_read_8 ();

    /* read in the value, index, and length words. */
    wValue = usb_ep_read_16le ();
    wIndex = usb_ep_read_16le ();
    wLength = usb_ep_read_16le ();

    /* clear the appropriate interrupt flags. */
    UEINTX = ~((1 << RXSTPI) | (1 << RXOUTI) | (1 << TXINI));

    /* handler for: GetDescriptor request. */
    if (bRequest == USB_REQUEST_GET_DESCRIPTOR) {
      /* locate the descriptor list pointer. */
      lst = (const uint8_t *) usb_descriptor_list;

      /* loop, not really indefinitely. */
      for (i = 0;; i++) {
        /* see if we've exceeded the number of descriptors. */
        if (i >= USB_DESCRIPTOR_LIST_COUNT) {
          /* stall and exit the interrupt service routine. */
          usb_ep_stall ();
          return;
        }

        /* read the wValue field of the current descriptor. */
        desc_val = pgm_read_word (lst);

        /* see if the wValue field matches the request. */
        if (desc_val != wValue) {
          /* no match. move to the next descriptor. */
          lst += sizeof (struct usb_descriptor_list_t);
          continue;
        }

        /* read in the wIndex field of the current descriptor. */
        lst += 2;
        desc_val = pgm_read_word (lst);

        /* see if the wIndex field matches the request. */
        if (desc_val != wIndex) {
          /* no match. move to the next descriptor. */
          lst += sizeof (struct usb_descriptor_list_t) - 2;
          continue;
        }

        /* read in the address of the current descriptor. */
        lst += 2;
        desc_addr = (const uint8_t *) pgm_read_word (lst);

        /* read in the length of the current descriptor. */
        lst += 2;
        desc_len = pgm_read_byte (lst);

        /* the values in desc_addr and desc_len are now valid. */
        break;
      }

      /* bound the transfer length by the requested size. */
      len = (wLength < 256 ? wLength : 255);

      /* bould the transfer length also by the descriptor size. */
      if (len > desc_len)
        len = desc_len;

      do {
        /* wait for the host to become ready. */
        do { i = UEINTX; } while (!(i & ((1 << TXINI) | (1 << RXOUTI))));

        /* abort if the wrong interrupt flagged. */
        if (i & (1 << RXOUTI)) return;

        /* set the number of bytes to transfer. */
        n = (len < USB_EP_SIZE_CONTROL ? len : USB_EP_SIZE_CONTROL);

        /* send the packet bytes. */
        for (i = n; i; i--)
          usb_ep_write_8 (pgm_read_byte (desc_addr++));

        /* re-tally the transfer byte counts. */
        len -= n;
        usb_ack_tx ();
      } while (len || n == USB_EP_SIZE_CONTROL);

      /* exit the interrupt service routine. */
      return;
    }

    /* handler for: SetAddress request. */
    if (bRequest == USB_REQUEST_SET_ADDRESS) {
      /* acknowledge the interrupt, set the address and enable its use. */
      usb_ack_tx ();
      usb_wait_tx ();
      UDADDR = wValue | (1 << ADDEN);

      /* exit the interrupt service routine. */
      return;
    }

    /* handler for: SetConfiguration request. */
    if (bRequest == USB_REQUEST_SET_CONFIG && bmRequestType == 0x00) {
      /* store the new configuration state. */
      usb_config_state = wValue;

      /* reset the configuration of the cdc. */
      usb_cdc_line_rtsdtr = 0;
      usb_cdc_transmit_flush_timer = 0;

      /* acknowledge the interrupt. */
      usb_ack_tx ();

      /* locate the endpoint configuration table in program memory. */
      cfg = usb_ep_configuration_table;

      /* loop through the implemented endpoints. */
      for (i = 1; i < USB_EP_COUNT; i++) {
        /* select the current endpoint. */
        usb_ep_select (i);

        /* get the enabled state of the endpoint. */
        ep_enab = pgm_read_byte (cfg++);
        UECONX = ep_enab;

        /* see if the endpoint is enabled. */
        if (ep_enab) {
          /* get the endpoint configuration values. */
          ep_type = pgm_read_byte (cfg++);
          ep_dir = pgm_read_byte (cfg++);
          ep_size = pgm_read_byte (cfg++);
          ep_banks = pgm_read_byte (cfg++);

          /* configure the endpoint. */
          usb_ep_conf (ep_type, ep_dir, ep_size, ep_banks);
        }
      }

      /* reset the endpoint and exit the interrupt service routine. */
      usb_ep_reset ();
      return;
    }

    /* handler for: GetConfiguration request. */
    if (bRequest == USB_REQUEST_GET_CONFIG && bmRequestType == 0x80) {
      /* wait for the endpoint to become ready. */
      usb_wait_tx ();

      /* write the configuration state. */
      usb_ep_write_8 (usb_config_state);

      /* transmit the packet and exit the interrupt service routine. */
      usb_ack_tx ();
      return;
    }

    /* handler for: GetLineCoding request (CDC). */
    if (bRequest == CDC_REQUEST_GET_LINE_CODING && bmRequestType == 0xa1) {
      /* wait for the endpoint to become ready. */
      usb_wait_tx ();

      /* locate the line coding array in memory. */
      p = usb_cdc_line_coding;

      /* transmit the bytes of the line coding array. */
      for (i = 0; i < 7; i++)
        usb_ep_write_8 (*p++);

      /* transmit the packet and exit the interrupt service routine. */
      usb_ack_tx ();
      return;
    }

    /* handler for: SetLineCoding request (CDC). */
    if (bRequest == CDC_REQUEST_SET_LINE_CODING && bmRequestType == 0x21) {
      /* wait for the endpoint to become ready. */
      usb_wait_rx ();

      /* locate the line coding array in memory. */
      p = usb_cdc_line_coding;

      /* read in the bytes of the line coding array. */
      for (i = 0; i < 7; i++)
        *p++ = usb_ep_read_8 ();

      /* acknowledge all required interrupts. */
      usb_ack_rx ();
      usb_ack_tx ();

      /* exit the interrupt service routine. */
      return;
    }

    /* handler for: SetControlLineState request (CDC). */
    if (bRequest == CDC_REQUEST_SET_CONTROL_LINE_STATE &&
        bmRequestType == 0x21) {
      /* read the new control line state. */
      usb_cdc_line_rtsdtr = wValue;

      /* acknowledge all required interrupts. */
      usb_wait_tx ();
      usb_ack_tx ();

      /* exit the interrupt service routine. */
      return;
    }

    /* handler for: GetStatus request. */
    if (bRequest == USB_REQUEST_GET_STATUS) {
      /* wait for the endpoint to become ready. */
      usb_wait_tx ();

      /* write the status bytes out. */
      usb_ep_write_16le (0x0000);

      /* transmit and exit the interrupt service routine. */
      usb_ack_tx ();
      return;
    }
  }


  /* stall. */
  usb_ep_stall ();
}
