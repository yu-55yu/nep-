#ifndef __PRINT_H
#define __PRINT_H

#include "ti_msp_dl_config.h"
#include <stdio.h>

int fputc(int ch, FILE *f);
int fputs(const char *_ptr, register FILE *_fp);
int puts(const char *_ptr);

#endif