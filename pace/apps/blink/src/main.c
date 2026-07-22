#include <libmsp/mspbase.h>
#include <libmspio/uartio.h>
#include <libmspprintf/mspprintf.h>
#include <msp430.h>

int main() {
  watchdog_disable();
  gpio_init_all_ports();
  gpio_unlock();
  gpio_clear_interrupts();
  clock_init();
  uartio_open(0);

  gpio_dir_out(1, 0);
  gpio_dir_out(1, 1);
  gpio_clear(1, 0);
  gpio_set(1, 1);
  int count = 0;
  while (1) {
    gpio_toggle(1, 0);
    gpio_toggle(1, 1);
    msp_printf("Count %i\r\n", count);
    __delay_cycles(100000);
    count++;
  }
}