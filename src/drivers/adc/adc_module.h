#ifndef ADC_MODULE_H
#define ADC_MODULE_H

#include "ch.h"
#include "hal.h"

// Inizializza l'ADC (da chiamare nel main all'avvio)
void adc_init(void);

// Legge il valore percentuale (0-100%) dal canale configurato
int readADCPercent(void);

#endif
