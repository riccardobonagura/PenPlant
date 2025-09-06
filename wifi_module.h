#ifndef WIFI_MODULE_H
#define WIFI_MODULE_H

#include "ch.h"
#include "hal.h"
#include <stddef.h>
#include <stdint.h>

// Inizializza la USART dedicata al modulo Wi-Fi
void wifi_init(void);

// Scrive dati sulla seriale Wi-Fi
size_t wifi_write(const char *data);

// Legge una riga (terminata da \n o \r\n)
// Restituisce il numero di byte letti (escluso terminatore, stringa terminata con \0)
// Restituisce 0 se non riceve nulla entro il timeout
size_t wifi_readLine(char *buf, size_t maxlen, systime_t timeout);

// Legge dati dal modulo Wi-Fi (con timeout)
size_t wifi_read(uint8_t *buf, size_t len, systime_t timeout);

#endif
