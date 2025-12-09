#ifndef FSM_H
#define FSM_H

typedef enum {
    STATE_INIT = 0,        // inizializzazione periferiche
    STATE_WAIT_RTC,        // attesa ricezione data e ora
    STATE_UPDATE,          // fase 2: update con istanze e tipi pianta
    STATE_HOME,            // fase 3: home page caricata
    STATE_CONFIRM,
    STATE_MEASURE,         // esegui misurazione
    STATE_HISTORY,         // visualizza storico
    STATE_ERROR            // errore
} app_state_t;

void fsm_init(void);
void fsm_update(void);

#endif
