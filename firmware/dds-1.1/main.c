
/* include the main header. */
#include "main.h"

/* declare the assembly functions for the core dds algorithm. */
extern void dds_init (void);

/* main: firmware execution entry point. */
int main (void) {
  /* disable the watchdog timer. */
  MCUSR = 0x00;
  wdt_disable ();

  cli ();

  /* configure the clock prescaler for 16 mhz operation. */
  CLKPR = (1 << CLKPCE);
  CLKPR = 0;

  /* enable output on GPIO D0, D1 and D2. */
  DDRD |= ((1 << DDD0) | (1 << DDD1) | (1 << DDD2));

  /* enable output on GPIO C5. */
  DDRC |= (1 << DDC5);

  /* initialize the dds core functions and start the dds. */
  dds_init ();

  /* loop indefinitely. */
  while (1);

  /* nope. */
  return 0;
}

