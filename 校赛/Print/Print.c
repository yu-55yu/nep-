#include "Print.h"

int i = 0;
int rx_count = 0;
size_t strlen(const char *str);

int fputc(int ch, FILE *f) {
  while (DL_UART_Main_isBusy(UART_LCD_INST))
    ;
  DL_UART_Main_transmitData(UART_LCD_INST, ch);
  return ch;
}

int fputs(const char *_ptr, register FILE *_fp) {
  unsigned int len;
  len = strlen(_ptr);
  for (unsigned int i = 0; i < len; i++) {
    while (DL_UART_Main_isBusy(UART_LCD_INST))
      ;
    DL_UART_Main_transmitData(UART_LCD_INST, (uint8_t)_ptr[i]);
  }
  return len;
}


int puts(const char *_ptr) {
  int count = fputs(_ptr, stdout);
  count += fputs("\n", stdout);
  return count;
}




