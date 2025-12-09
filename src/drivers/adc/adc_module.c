#include "adc_module.h"
#include "serial_module.h"


#define ADC_GRP_NUM_CHANNELS   1
#define ADC_GRP_BUF_DEPTH      16

static adcsample_t samples[ADC_GRP_NUM_CHANNELS * ADC_GRP_BUF_DEPTH];

static const ADCConversionGroup adcgrpcfg1 = {
    .circular     = false,
    .num_channels = ADC_GRP_NUM_CHANNELS,
    .end_cb       = NULL,
    .error_cb     = NULL,
    .cfgr         = ADC_CFGR_CONT,
    .cfgr2        = 0U,
    .tr1          = ADC_TR_DISABLED,
    .tr2          = ADC_TR_DISABLED,
    .tr3          = ADC_TR_DISABLED,
    .awd2cr       = 0U,
    .awd3cr       = 0U,
    .smpr         = {
        ADC_SMPR1_SMP_AN1(ADC_SMPR_SMP_247P5),
        0U
    },
    .sqr          = {
        ADC_SQR1_SQ1_N(ADC_CHANNEL_IN1),
        0U, 0U, 0U
    }
};

void adc_init(void) {
    /* Configuro pin analogico */
    palSetPadMode(GPIOA, 0U, PAL_MODE_INPUT_ANALOG);
    /* Avvio driver ADC */
    adcStart(&ADCD1, NULL);
    //serial_printf("[ADC] Test ADC OK\r\n");
}

int readADCPercent(void) {
    float avg = 0.0f;

    adcConvert(&ADCD1, &adcgrpcfg1, samples, ADC_GRP_BUF_DEPTH);

    for (int i = 0; i < ADC_GRP_BUF_DEPTH; i++) {
        avg += (float)samples[i];
    }
    avg /= ADC_GRP_BUF_DEPTH;

    int percent = (int)((avg / 4095.0f) * 100.0f);
    return percent;
}
