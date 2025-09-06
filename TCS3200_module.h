#ifndef TCS3200_MODULE_H
#define TCS3200_MODULE_H

#include "ch.h"
#include "hal.h"

// Inizializza il sensore TCS3200 e l’ICU
void tcs3200_init(void);

// Legge la luminosità in LUX
float tcs3200_read_lux(void);

#endif
