#include "rtc_module.h"
#include "chprintf.h" // necessario per chsnprintf
#include "hal_rtc.h"
#include "serial_module.h"


static RTCDriver *rtc = &RTCD1;

/* Inizializza la periferica RTC */
void rtc_module_init(void){
    rtcObjectInit(rtc);
    //rtcStart(rtc, NULL);
}

/* Converte 4 caratteri numerici in intero (es. "2025") */
static int str_to_int4(const char *s) {
    return (s[0]-'0')*1000 + (s[1]-'0')*100 + (s[2]-'0')*10 + (s[3]-'0');
}

/* Imposta RTC da stringhe "DD/MM/YYYY" e "HH:MM:SS" */
/* Imposta RTC da stringhe "DD/MM/YYYY" e "HH:MM:SS" */
void rtc_set_from_string(const char *dateStr, const char *timeStr){
    RTCDateTime dt;

    int day   = (dateStr[0]-'0')*10 + (dateStr[1]-'0');
    int month = (dateStr[3]-'0')*10 + (dateStr[4]-'0');
    int year  = str_to_int4(&dateStr[6]);

    int hour  = (timeStr[0]-'0')*10 + (timeStr[1]-'0');
    int min   = (timeStr[3]-'0')*10 + (timeStr[4]-'0');
    int sec   = (timeStr[6]-'0')*10 + (timeStr[7]-'0');

    dt.day = day;
    dt.month = month;
    dt.year = year - 1980;
    dt.millisecond = (hour*3600 + min*60 + sec) * 1000;

    rtcSetTime(rtc, &dt);
}



/* Legge l’ora e la data corrente in due buffer */
void rtc_get_time(char *dateBuff, size_t dateBuffSize, char *timeBuff, size_t timeBuffSize){
    RTCDateTime dt;
    rtcGetTime(rtc, &dt);

    // Data
    chsnprintf(dateBuff, dateBuffSize, "%02lu/%02lu/%04lu",
               dt.day, dt.month, dt.year + 1980);

    // Ora
    unsigned long millis = dt.millisecond;
    unsigned long hours = millis / 3600000;
    millis %= 3600000;
    unsigned long minutes = millis / 60000;
    millis %= 60000;
    unsigned long seconds = millis / 1000;

    chsnprintf(timeBuff, timeBuffSize, "%02lu:%02lu:%02lu",
               hours, minutes, seconds);
}
