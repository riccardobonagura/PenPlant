#include "button_module.h"
#include "ch.h"
#include "hal.h"
#include "serial_module.h"

#define BUTTON_PAD GPIOA
#define BUTTON_PIN 5

void button_wait(void) {
    palSetPadMode(BUTTON_PAD, BUTTON_PIN, PAL_MODE_INPUT_PULLUP);
    serial_printf("wait for [Button]\r\n");
    // Aspetta che il pulsante venga premuto (livello basso)
    while (palReadPad(BUTTON_PAD, BUTTON_PIN) == PAL_HIGH)
        chThdSleepMilliseconds(20);

    // Debounce pressione
    chThdSleepMilliseconds(20);

    // Debounce: aspetta che venga rilasciato
    while (palReadPad(BUTTON_PAD, BUTTON_PIN) == PAL_LOW)
        chThdSleepMilliseconds(20);

    // Debounce pressione
    chThdSleepMilliseconds(20);

    serial_printf("[Button] Premuto\r\n");
}

