#include "serial_module.h"
#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include <stdarg.h>

static BaseSequentialStream *chp;

// Configurazione custom della seriale
static const SerialConfig serial_cfg = {
    115200,    // baudrate
    0,
    USART_CR2_STOP1_BITS,
    0
};

void serial_init(void) {
    // Configurazione pin UART2 (PA2 = TX, PA3 = RX)
    palSetPadMode(GPIOA, 2, PAL_MODE_ALTERNATE(7));
    palSetPadMode(GPIOA, 3, PAL_MODE_ALTERNATE(7));

    // Avvio UART2 con configurazione a 115200 baud
    sdStart(&SD2, &serial_cfg);

    // Stream di output
    chp = (BaseSequentialStream *)&SD2;
    serial_printf("[MAIN] Test seriale OK @115200\r\n");
}

void serial_printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    chvprintf(chp, fmt, args);
    va_end(args);
}
