#include "ch.h"
#include "hal.h"
#include "chprintf.h"
/*
 * Enable if your terminal supports ANSI ESCAPE CODE
 */
#define ANSI_ESCAPE_CODE_ALLOWED                  FALSE
static BaseSequentialStream * chp = (BaseSequentialStream*) &SD2;
/*===========================================================================*/
/* DHT11 related defines (NECESSARI PER LA COMUNICAZIONE CON DHT11           */
/*===========================================================================*/

#define    MCU_REQUEST_WIDTH                     18000
#define    DHT_ERROR_WIDTH                         200
#define    DHT_START_BIT_WIDTH                      80
#define    DHT_LOW_BIT_WIDTH                        28
#define    DHT_HIGH_BIT_WIDTH                       70
/*===========================================================================*/
/* CODICE RELATIVO ALL'INTERRUPT DELL'ICU                                    */
/*===========================================================================*/
#define    ICU_TIM_FREQ                        1000000
static uint8_t TEMP, HR, CHECK_SUM, tmp, bit_counter = 0;;
static icucnt_t widths [40];
static void icuwidthcb(ICUDriver *icup) {
  icucnt_t width = icuGetWidthX(icup);
  if(width >= DHT_START_BIT_WIDTH){
    /* starting bit resetting the bit counter */
    bit_counter = 0;
  }
  else{
    /* Recording current width. Just for fun  */
    widths[bit_counter] = width;
    if(width > DHT_LOW_BIT_WIDTH){
      tmp |= (1 << (7 - (bit_counter % 8)));
    }
    else{
      tmp &= ~(1 << (7 - (bit_counter % 8)));
    }
    /* When bit_counter is 7, tmp contains the bit from 0 to 7 corresponding to
       The Humidity Rate integer part (Decimal part is 0 on DHT 11) */
    if(bit_counter == 7)
      HR = tmp;
    /* When bit_counter is 23, tmp contains the bit from 16 to 23 corresponding to
       The Temperature integer part (Decimal part is 0 on DHT 11) */
    if(bit_counter == 23)
      TEMP = tmp;
    /* When bit_counter is 39, tmp contains the bit from 32 to 39 corresponding to
       The Check sum value */
    if(bit_counter == 39)
      CHECK_SUM = tmp;
    bit_counter++;
  }
}
/*===========================================================================*/
/* CONFIGURAZIONE DELL'ICU                                                   */
/*===========================================================================*/
static ICUConfig icucfg = {
  ICU_INPUT_ACTIVE_HIGH,
  ICU_TIM_FREQ,                                /* 1MHz ICU clock frequency.   */
  icuwidthcb,
  NULL,
  NULL,
  ICU_CHANNEL_2,
  0
};

/*===========================================================================*/
/* FUNZIONE PER PRENDERE LA MEDIA                                            */
/*===========================================================================*/
void dht11_get_media(int *temp_media, int *hr_media) {

  uint16_t tmp_temp = 0;
  uint16_t tmp_hr = 0;
  uint16_t cont = 0;

  float hr_media_f = 0;
  float temp_media_f = 0;

  while (true) {

    /* ---- codice originale invariato ---- */
    palSetPadMode(GPIOA, GPIOA_PIN4, PAL_MODE_OUTPUT_PUSHPULL);
    palWritePad(GPIOA, GPIOA_PIN4, PAL_LOW);
    chThdSleepMicroseconds(MCU_REQUEST_WIDTH);
    palWritePad(GPIOA, GPIOA_PIN4, PAL_HIGH);

    palSetPadMode(GPIOA, GPIOA_PIN4, PAL_MODE_ALTERNATE(2));
    icuStart(&ICUD3, &icucfg);
    icuStartCapture(&ICUD3);
    icuEnableNotifications(&ICUD3);
    chThdSleepMilliseconds(700);

    icuStopCapture(&ICUD3);
    icuStop(&ICUD3);

    if (cont <= 10) {
      cont++;
      tmp_temp = TEMP + tmp_temp;
      tmp_hr   = HR   + tmp_hr;
    } else {
      temp_media_f = tmp_temp / (cont);
      hr_media_f   = tmp_hr   / (cont);

      /* Restituisco i valori come interi */
      *temp_media = (int)temp_media_f;
      *hr_media   = (int)hr_media_f;

      /* reset per la prossima chiamata */
      tmp_temp = 0;
      tmp_hr = 0;
      cont = 0;

      break; // esco dal while
    }

    if (CHECK_SUM == (TEMP + HR)) {
      // opzionale: debug
    } else {
      // opzionale: debug
    }
  }
}

