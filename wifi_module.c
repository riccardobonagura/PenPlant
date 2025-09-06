#include "wifi_module.h"
#include "chprintf.h"
#include <string.h>

static SerialConfig wifi_cfg = {
    115200,       // Baudrate (puoi modificarlo in base al tuo modulo)
    0,
    USART_CR2_STOP1_BITS,
    0
};

#define WIFI_TX_PORT   GPIOC
#define WIFI_TX_PIN    4
#define WIFI_RX_PORT   GPIOC
#define WIFI_RX_PIN    5

// Driver UART da usare
#define WIFI_SD        SD1

// Inizializzazione
void wifi_init(void) {
    palSetPadMode(WIFI_TX_PORT, WIFI_TX_PIN, PAL_MODE_ALTERNATE(7)); // TX
    palSetPadMode(WIFI_RX_PORT, WIFI_RX_PIN, PAL_MODE_ALTERNATE(7)); // RX
    sdStart(&WIFI_SD, &wifi_cfg);
}

// Scrittura
size_t wifi_write(const char *data) {
    return sdWrite(&WIFI_SD, (const uint8_t *)data, strlen(data));
}

// Lettura di una riga
size_t wifi_readLine(char *buf, size_t maxlen, systime_t timeout) {
    size_t idx = 0;
    uint8_t c;

    while (idx < maxlen - 1) {
        if (sdReadTimeout(&WIFI_SD, &c, 1, timeout) == 0) {
            // timeout → nessun dato ricevuto
            if (idx == 0) return 0;
            break; // se avevo già ricevuto qualcosa, considero riga parziale
        }

        if (c == '\n' || c == '\r') {
            break; // fine comando
        }

        buf[idx++] = (char)c;
    }

    buf[idx] = '\0'; // termina stringa
    return idx;
}

size_t wifi_read(uint8_t *buf, size_t len, systime_t timeout) {
    return sdReadTimeout(&WIFI_SD, buf, len, timeout);
}
