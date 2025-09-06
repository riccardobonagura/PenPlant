#ifndef DHT11_MODULE_H
#define DHT11_MODULE_H

// Include base
#include "ch.h"
#include "hal.h"

// API pubblica del modulo, salva temperatura e umidità media nella variabili passate
void dht11_get_media(int *temp_media, int *hr_media);

#endif
