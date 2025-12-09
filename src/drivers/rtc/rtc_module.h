#ifndef RTC_MODULE_H
#define RTC_MODULE_H

#include "ch.h"
#include "hal.h"
#include <stdint.h>
#include <stddef.h>

/* Dimensioni buffer per le stringhe */
#define RTC_DATE_BUFF_SIZE 11  // "DD/MM/YYYY\0"
#define RTC_TIME_BUFF_SIZE 9   // "HH:MM:SS\0"

/* Funzioni del modulo RTC */
void rtc_module_init(void);
void rtc_set_from_string(const char *dateStr, const char *timeStr);
void rtc_get_time(char *dateBuff, size_t dateBuffSize, char *timeBuff, size_t timeBuffSize);

#endif /* RTC_MODULE_H */
