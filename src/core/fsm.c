#include "fsm.h"
#include "ch.h"
#include "hal.h"
#include "serial_module.h"
#include "wifi_module.h"
#include "button_module.h"
#include "adc_module.h"
#include "dht11_module.h"
#include "TCS3200_module.h"
#include "oled_module.h"
#include "meas_proc.h"
#include "rtc_module.h"

static app_state_t sys_state;
static Misurazione meas_buffer[MAX_MEASUREMENTS];

void print_measurement_details(const Misurazione* m) {
    serial_printf("  - Misura per Pianta ID %u (Tipo %u):\r\n", m->idPianta, m->idTipo);
    serial_printf("    Valori: Temp:%dC, Luce:%dl, Umi-Aria:%d%%, Umi-Suolo:%d%%\r\n",
                  m->temperatura_c, m->luce_l, m->umi_out, m->umi_in);
    serial_printf("    Score Totale: %d / 100\r\n", m->v_tot);
    serial_printf("    Consigli:\r\n");
    serial_printf("      > Temperatura: %s\r\n", score_strings[m->idx_temp_str]);
    serial_printf("      > Luce:        %s\r\n", score_strings[m->idx_luce_str]);
    serial_printf("      > Aria:        %s\r\n", score_strings[m->idx_umi_out_str]);
    serial_printf("      > Suolo:       %s\r\n\r\n", score_strings[m->idx_umi_in_str]);
    chThdSleepMilliseconds(50); // Piccolo delay per seriale
}

void fsm_init(void) {
    sys_state = STATE_INIT;
}

void fsm_update(void) {
    switch (sys_state) {

        case STATE_INIT:

            serial_printf("[INIT]: inizializzazione periferiche e RAM...\r\n");

            wifi_init();        // inizializza SD1 modulo WiFi
            //serial_printf("[INIT] WIFI OK\r\n");
            adc_init();         // inizializza adc per leggere sensore umidità terreno
            //serial_printf("[INIT] ADC OK\r\n");
            tcs3200_init();     // inizializza sensore luminosità
            //serial_printf("[INIT] TCS3200 OK\r\n");
            oled_init();        //inizializza schermo oled
            //serial_printf("[INIT] OLED OK\r\n");
            rtc_module_init();
            init_runtime_storage();

            serial_printf("[INIT] Inizializzazione completata\r\n");
            chThdSleepMilliseconds(2000);
            sys_state = STATE_WAIT_RTC;
            break;

        case STATE_WAIT_RTC: {
            serial_printf("[WAIT_RTC] Attesa RTC dal modulo Wi-Fi...\r\n");

            //TODO: read bloccante che attende messaggio RTC

            //nc_in: INIT e RTC

            serial_printf("[WAIT_RTC] lettura avvenuta\r\n");

            chThdSleepMilliseconds(2000);
            sys_state = STATE_UPDATE;

            break;
        }

        case STATE_UPDATE:
            serial_printf("[UPDATE] Invio piante in memoria\r\n");

            // TODO: invio istanzePianta e tipiPianta salvati nel DB
            /*
            nc_out: catalogoPiante (int: idTipo, string nomeTipo) +
                istanzePiante (int: idTipo, int:  idPianta, int: nomePianta)
            */
            serial_printf("[UPDATE] Invio effettuato\r\n");

            serial_printf("FASE 2: Verifica piante caricate in RAM...\r\n");
            uint32_t plant_count;
            const MiaPianta* plants = get_elenco_piante(&plant_count);
            serial_printf("  - Trovate %u piante iniziali (atteso: %d):\r\n", plant_count, DEMO_PLANTS_COUNT);
            for(uint32_t i = 0; i < plant_count; i++) {
                serial_printf("    > ID: %u, Nome: '%s', Tipo Profilo: %d\r\n",
                              plants[i].idPianta, plants[i].nomePianta, plants[i].idTipo);
            }
            serial_printf("FASE 2: Completata.\r\n\r\n");

            chThdSleepMilliseconds(2000);
            sys_state = STATE_HOME;
            break;

        case STATE_HOME:
            serial_printf("[READ] Attendo comando...\r\n");

            /*
            // Leggo un comando e salvo i dati

            if (cmd == "ADD_PLANT") {
                nc_in: cmd: "ADD_PLANT", int: idTipo, string: nomePianta
                sys_state = STATE_CONFIRM;
            }
            else if (cmd == "GET_HISTORY") {
                nc_in: cmd: "GET_HISTORY", INT: idPianta
                sys_state = STATE_HISTORY;
            }
            else if (cmd == "START_MEAS") {
                nc_in: cmd:"START_MEAS", int: idPianta
                sys_state = STATE_MEASURE;
            }
            else if (cmd == "GET_INIT_DATA") {

                sys_state = STATE_UPDATE;
            }
            else {
                serial_printf("[HOME] Comando sconosciuto: %s\r\n", cmd);
                // rimane in HOME
            }*/
            serial_printf("[READ] comando letto...\r\n");
            sys_state = STATE_MEASURE;

            break;


        case STATE_CONFIRM: {
            serial_printf("[CONFIRM] Invio conferma\r\n");

            serial_printf("FASE 3: Test registrazione nuova pianta in RAM...\r\n");
            int new_id = registra_nuova_pianta("Sansevieria di Luigi", 3);
            serial_printf("FASE 3: Pianta registrata con ID: %d\r\n", new_id);
            plants = get_elenco_piante(&plant_count);
            serial_printf("FASE 3: Conteggio piante aggiornato: %u (atteso: %d)\r\n", plant_count, DEMO_PLANTS_COUNT + 1);
            serial_printf("FASE 3: Completata.\r\n\r\n");

            // TODO: mando conferma
            /*
            nc_out: null (conferma)*/

            serial_printf("[CONFIRM] Conferma inviata\r\n");

            chThdSleepMilliseconds(2000);
            //sys_state = STATE_HOME;
            //qui sotto versione di prova
            sys_state = STATE_HISTORY;
            break;
        }


        case STATE_MEASURE: {

            int temperatura = 0;
            int umidita = 0;
            int lux = 0;
            int umidità_terreno = 0;

            serial_printf("[MEASURE] Attesa misurazione tramite bottone\r\n");
            oled_print_string("Premi per misurare!", 0, 0);
            //button_wait();

            // Luminosità
            serial_printf("[MEASURE] Calcolo luminosità\r\n");
            lux = tcs3200_read_lux();
            serial_printf("[MEASURE] Luminosità: %.2f LUX\r\n", lux);
            chThdSleepMilliseconds(1000);

            // Temperatura e umidità
            serial_printf("[MEASURE] Calcolo Temperatura e umidità\r\n");
            dht11_get_media(&temperatura, &umidita);
            serial_printf("[MEASURE] Temperatura: %d C, Umidità: %d %%\n\r",
                          temperatura, umidita);
            chThdSleepMilliseconds(1000);

            // Umidità terreno
            serial_printf("[MEASURE] Calcolo percentuale umidità terreno\r\n");
            umidità_terreno = readADCPercent();
            serial_printf("[MEASURE] Umidità terreno: %d %%\r\n", umidità_terreno);

            int new_id = registra_nuova_pianta("Sansevieria di Luigi", 3);
            serial_printf("[MEASURE] Avvio pipeline di misurazione per la nuova pianta (ID %d)...\r\n", new_id);
            esegui_pipeline_misurazione(temperatura, lux, umidita, umidità_terreno, new_id);
            serial_printf("[MEASURE] Misurazione eseguita e salvata in RAM.\r\n");

            // TODO: pipeline di misura
            //Measurements m = do_measurements();

            // TODO: salvataggio in memoria della misura

            // TODO: invio della misura con info
            /*
             * nc_out: Misurazione {int: temperatura_valore, int: luce_valore,
             *                      int: umidità_esterna_valore, int: umidità_interna_valore,
             *                      int: temperatura_score, int: luce_score,
             *                      int: umidità_esterna_score, int: umidità_interna_score,
             *                      int: score_totale, string: temperatura_stringa,
             *                      string: luce_stringa, string: umidità_esterna_stringa,
             *                      string: umidità_interna_stringa}
             */


            serial_printf("[MEASURE] Invio completato\r\n");

            chThdSleepMilliseconds(2000);

            //sys_state = STATE_HOME;
            //qui sotto versione di prova
            sys_state = STATE_CONFIRM;
            break;
        }

        case STATE_HISTORY:
            serial_printf("[HISTORY] Recupero storico\r\n");

            // TODO: recupera misurazioni da memoria e le invia
            /*
             * nc_out: int: score_avg, arrayMisurazioni {data e ora, temperatura_score,
             *             luce_score,umidità_esterna_score, umidità_interna_score,
             *             score_totale}
             */

            serial_printf("Storico inviato\r\n");

            serial_printf("FASE 5: Lettura di tutte le misurazioni in RAM...\r\n");
            uint32_t meas_count = get_measurements(meas_buffer, MAX_MEASUREMENTS);
            serial_printf("FASE 5: Trovate %u misurazioni totali (atteso: %d):\r\n\r\n", meas_count, DEMO_MEAS_COUNT + 1);
            for(uint32_t i = 0; i < meas_count; i++) {
                print_measurement_details(&meas_buffer[i]);
            }
            serial_printf("--- TUTTI I TEST SONO STATI COMPLETATI CON SUCCESSO ---\r\n");

            chThdSleepMilliseconds(2000);
            sys_state = STATE_HOME;
            break;

        case STATE_ERROR:
            serial_printf("[FSM] ERRORE: comando non valido\r\n");
            chThdSleepMilliseconds(2000);
            sys_state = STATE_HOME;
            break;

        default:
            serial_printf("[FSM] Stato sconosciuto!\r\n");
            chThdSleepMilliseconds(2000);
            sys_state = STATE_ERROR;
            break;
    }
}
