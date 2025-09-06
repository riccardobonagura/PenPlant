#include "ch.h"
#include "hal.h"
#include "fsm.h"
#include "adc_module.h"
#include "dht11_module.h"
#include "button_module.h"
#include "serial_module.h"
#include "wifi_module.h"
#include "TCS3200_module.h"
#include "oled_module.h"
#include "meas_proc.h"
#include "rtc_module.h"
#include "chprintf.h"
//#include "jsmn.h"



int main(void) {
    halInit();
    chSysInit();

    serial_init();      // inizializza SD2 per leggere su terminale
    //serial_printf("terminale ok\r\n");
    fsm_init();     // stato iniziale FSM

    while(true) {

        serial_printf("[MAIN]: State Update\r\n");
        fsm_update();   // aggiorna FSM

        chThdSleepMilliseconds(2000);
    }
}


/*
 *
 *
 * ESEMPIO USO LIBRERIA RTC
 *
 *
 *
char dateBuff[RTC_DATE_BUFF_SIZE];
char timeBuff[RTC_TIME_BUFF_SIZE];

int main(void){
    halInit();
    chSysInit();

    rtc_module_init();
    serial_init();

    // Imposta RTC a data/ora personalizzata
    rtc_set_from_string("05/09/2025", "15:30:45");

    while(1){
        rtc_get_time(dateBuff, RTC_DATE_BUFF_SIZE, timeBuff, RTC_TIME_BUFF_SIZE);

        char serial_output[64];
        chsnprintf(serial_output, sizeof(serial_output), "%s - %s\r\n", dateBuff, timeBuff);
        serial_printf(serial_output);

        chThdSleepMilliseconds(1000);
    }
}*/

