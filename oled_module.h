#ifndef OLED_MODULE_H
#define OLED_MODULE_H

#include "ch.h"
#include "hal.h"
#include "ssd1306.h"
#include <stdint.h>

/* Dimensione buffer per le stringhe */
#define OLED_BUFF_SIZE 20

/* Funzioni pubbliche */
void oled_init(void);
void oled_print_string(const char *str, uint8_t col, uint8_t row);
void oled_print_double_string(const char *str1, const char *str2, uint8_t col1, uint8_t row1);

#endif /* OLED_MODULE_H */
