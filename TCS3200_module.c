#include "tcs3200_module.h"

// Pin mapping TCS3200
#define TCS_S0     PAL_LINE(GPIOB, 1)
#define TCS_S1     PAL_LINE(GPIOB, 2)
#define TCS_S2     PAL_LINE(GPIOB, 11)
#define TCS_S3     PAL_LINE(GPIOB, 12)
#define TCS_OUT    PAL_LINE(GPIOB, 6)

// Frequenza timer ICU
#define ICU_TIM_FREQ  1000000

// Variabile globale per memorizzare ultimo periodo
static volatile icucnt_t last_period = 0;

// Callback ICU → salva il periodo
static void icuperiodcb(ICUDriver *icup) {
    last_period = icuGetPeriodX(icup);
}

// Callback dummy (non usata)
static void icuwidthcb(ICUDriver *icup) {
    (void)icup;
}

// Configurazione ICU
static ICUConfig icucfg = {
    ICU_INPUT_ACTIVE_HIGH,
    ICU_TIM_FREQ,
    icuwidthcb,
    icuperiodcb,
    NULL,
    ICU_CHANNEL_1,
    0
};

// Inizializzazione TCS3200
void tcs3200_init(void) {
    // Configuro e avvio ICU su TIM4 CH1 (esempio → PB6)
    icuStart(&ICUD4, &icucfg);
    icuStartCapture(&ICUD4);
    icuEnableNotifications(&ICUD4);

    // Pin di controllo del sensore
    palSetLineMode(TCS_S0, PAL_MODE_OUTPUT_PUSHPULL);
    palSetLineMode(TCS_S1, PAL_MODE_OUTPUT_PUSHPULL);
    palSetLineMode(TCS_S2, PAL_MODE_OUTPUT_PUSHPULL);
    palSetLineMode(TCS_S3, PAL_MODE_OUTPUT_PUSHPULL);

    // Pin uscita sensore (frequenza in uscita) → collegato a ICU
    palSetLineMode(TCS_OUT, PAL_MODE_ALTERNATE(2));

    // Configuro il TCS3200 (scaling 20%, selezione filtri)
    palSetLine(TCS_S0);
    palClearLine(TCS_S1);
    palSetLine(TCS_S2);
    palClearLine(TCS_S3);

    // Piccola attesa
    chThdSleepMilliseconds(100);
}

// Funzione di lettura in LUX
float tcs3200_read_lux(void) {
    icucnt_t p;

    // Aspetto un periodo valido
    do {
        chThdSleepMilliseconds(5);
        p = last_period;
    } while (p == 0);

    float freq = (float)ICU_TIM_FREQ / (float)p;
    // Conversione approssimata in Lux (coefficiente da calibrare)
    float lux = ((49.0f/3.0f) * freq - (2450.0f/3.0f));
    return lux;
}
