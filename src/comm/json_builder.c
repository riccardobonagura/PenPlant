/* ---------------------------------------------------------------------------
 * json_builder.c
 * ---------------------------------------------------------------------------
 * In questo file risiedono le funzioni che "impacchettano" i dati in
 * messaggi JSON da inviare all'ESP8266.
 *
 * Le principali funzioni sono:
 *  - build_init_data   : costruisce il JSON con catalogo piante + istanze
 *  - build_ack         : risposta di conferma semplice
 *  - build_measurement : serializza una singola misurazione completa
 *  - build_history     : serializza lo storico di una pianta
 *
 * Ognuna ritorna il numero di caratteri scritti (o -1 in caso di buffer insufficiente).
 * -------------------------------------------------------------------------*/

#include "cmd_defs.h"   // Accesso a plant_db, get_elenco_piante, ...
#include <stdio.h>      // snprintf
#include <string.h>     // memset, ecc.

/* ---------------------------------------------------------------------------
 * build_init_data
 * ---------------------------------------------------------------------------
 * Costruisce un JSON di questo tipo:
 *
 * {
 *   "type": "INIT_DATA",
 *   "catalogoPiante": [
 *      { "idTipo":1, "nomeTipo":"Monstera deliciosa" }, ...
 *   ],
 *   "istanzePiante": [
 *      { "idTipo":4, "idPianta":1, "nomePianta":"Aloe di Rocco" }, ...
 *   ]
 * }
 *
 * - catalogoPiante: derivato dall'array `plant_db`
 * - istanzePiante : ritorno di `get_elenco_piante()`
 *
 * @param buf  : buffer destinazione (preallocato dal chiamante)
 * @param len  : dimensione del buffer
 * @return     : n° di caratteri scritti, oppure -1 se buf insufficiente.
 * -------------------------------------------------------------------------*/
int build_init_data(char* buf, size_t len) {
    size_t pos = 0;        // indice corrente di scrittura
    uint32_t nPlants = 0;  // verrà riempito da get_elenco_piante
    uint32_t nProfiles = 0;

    /* Apertura dell'oggetto principale */
    pos += snprintf(buf + pos, len - pos, "{\"type\":\"INIT_DATA\",\"catalogoPiante\":[");

    /* 1) Catalogo di tutti i tipi pianta predefiniti (array `plant_db`) */
    for (int i = 0; i < MAX_PLANT_PROFILES && plant_db[i].idTipo != 0; ++i) {
        if (i > 0) pos += snprintf(buf + pos, len - pos, ",");
        pos += snprintf(buf + pos, len - pos,
                        "{\"idTipo\":%d,\"nomeTipo\":\"%s\"}",
                        plant_db[i].idTipo, plant_db[i].nomeTipo);
        nProfiles++;  // contatore profili (non usato a fini logici, ma utile per debug)
    }

    /* 2) Elenco istanze piante attualmente in RAM */
    pos += snprintf(buf + pos, len - pos, "],\"istanzePiante\":[");
    const MiaPianta* plants = get_elenco_piante(&nPlants);
    for (uint32_t i = 0; i < nPlants; ++i) {
        if (i > 0) pos += snprintf(buf + pos, len - pos, ",");
        pos += snprintf(buf + pos, len - pos,
                        "{\"idTipo\":%d,\"idPianta\":%u,\"nomePianta\":\"%s\"}",
                        plants[i].idTipo, plants[i].idPianta, plants[i].nomePianta);
    }

    /* Chiusura oggetto principale */
    pos += snprintf(buf + pos, len - pos, "]}");

    /* Se pos supera len, significa che il buffer era insufficiente */
    return (pos < len) ? (int)pos : -1;
}

/* ---------------------------------------------------------------------------
 * build_ack
 * ---------------------------------------------------------------------------
 * Costruisce un semplice JSON di conferma:
 * { "type": "ACK" }
 * -------------------------------------------------------------------------*/
int build_ack(char* buf, size_t len) {
    return snprintf(buf, len, "{\"type\":\"ACK\"}");
}

/* ---------------------------------------------------------------------------
 * build_measurement
 * ---------------------------------------------------------------------------
 * Serializza una struct `Misurazione` in un JSON con tutti i campi richiesti:
 *
 * {
 *   "type": "MEAS_RESULTS",
 *   "temperatura_valore": 25,
 *   "luce_valore": 700,
 *   ...
 *   "umidita_interna_stringa": "Umidita' interna ottimale"
 * }
 *
 * I nomi delle stringhe di valutazione provengono da `score_strings`.
 *
 * @param m   : puntatore alla misurazione da impacchettare.
 * @param buf : buffer destinazione.
 * @param len : dimensione del buffer.
 * -------------------------------------------------------------------------*/
int build_measurement(const Misurazione* m, char* buf, size_t len) {
    return snprintf(buf, len,
        "{\"type\":\"MEAS_RESULTS\","
        "\"temperatura_valore\":%d,\"luce_valore\":%d,"
        "\"umidita_esterna_valore\":%d,\"umidita_interna_valore\":%d,"
        "\"temperatura_score\":%d,\"luce_score\":%d,\"umidita_esterna_score\":%d,"
        "\"umidita_interna_score\":%d,\"score_totale\":%d,"
        "\"temperatura_stringa\":\"%s\",\"luce_stringa\":\"%s\","
        "\"umidita_esterna_stringa\":\"%s\",\"umidita_interna_stringa\":\"%s\"}",
        m->temperatura_c, m->luce_l, m->umi_out, m->umi_in,
        m->v_temp, m->v_luce, m->v_umi_out, m->v_umi_in, m->v_tot,
        score_strings[m->idx_temp_str], score_strings[m->idx_luce_str],
        score_strings[m->idx_umi_out_str], score_strings[m->idx_umi_in_str]);
}

/* ---------------------------------------------------------------------------
 * build_history
 * ---------------------------------------------------------------------------
 * Costruisce uno storico delle misurazioni per una pianta specifica.
 * JSON di esempio:
 *
 * {
 *   "type":"HISTORY_RESULTS",
 *   "misurazioni":[
 *     {"timestamp":12345,"temperatura_score":25,"luce_score":20,"umidita_esterna_score":25,"umidita_interna_score":20,"score_totale":90},
 *     ...
 *   ],
 *   "score_avg": 85
 * }
 *
 * - Vengono prima raccolte tutte le misurazioni in RAM (get_measurements).
 * - Si filtrano quelle con `idPianta` richiesto.
 * - Si accumula la somma dei punteggi per calcolare la media.
 *
 * Nota: `timestamp.year` è usato come esempio; in un'applicazione reale
 *       andrebbe integrato un vero timestamp (es. epoch, ecc.).
 * -------------------------------------------------------------------------*/
int build_history(uint32_t idPianta, char* buf, size_t len) {
    Misurazione tmp[MAX_MEASUREMENTS];
    uint32_t count = get_measurements(tmp, MAX_MEASUREMENTS);

    int totalScore = 0;
    int entries = 0;
    size_t pos = snprintf(buf, len,
                          "{\"type\":\"HISTORY_RESULTS\",\"misurazioni\":[");

    /* Scorre tutte le misurazioni disponibili */
    for (uint32_t i = 0; i < count; ++i) {
        if (tmp[i].idPianta != idPianta) continue;  // Filtra per idPianta

        if (entries++ > 0) {
            pos += snprintf(buf + pos, len - pos, ",");
        }

        /* Nota: `timestamp.year` è usato qui come placeholder */
        pos += snprintf(buf + pos, len - pos,
                        "{\"timestamp\":%lu,"
                        "\"temperatura_score\":%d,\"luce_score\":%d,"
                        "\"umidita_esterna_score\":%d,\"umidita_interna_score\":%d,"
                        "\"score_totale\":%d}",
                        (unsigned long)tmp[i].timestamp.year,
                        tmp[i].v_temp, tmp[i].v_luce,
                        tmp[i].v_umi_out, tmp[i].v_umi_in, tmp[i].v_tot);

        totalScore += tmp[i].v_tot;
    }

    int avg = (entries > 0) ? totalScore / entries : 0;

    /* Chiude array e aggiunge score medio */
    pos += snprintf(buf + pos, len - pos, "],\"score_avg\":%d}", avg);

    return (pos < len) ? (int)pos : -1;
}
