/* ---------------------------------------------------------------------------
 * json_parser.c
 * ---------------------------------------------------------------------------
 * File dedicato al "parsing" dei comandi JSON in arrivo dalla seriale.
 * Usa la libreria jsmn (https://github.com/zserge/jsmn).
 *
 * Il flusso principale:
 *  - `parse_command` riceve una stringa JSON.
 *  - Inizializza il parser JSMN.
 *  - Analizza i token generati da jsmn (sequenza key/value).
 *  - Riempie una struttura `Cmd` con i campi individuati.
 * -------------------------------------------------------------------------*/

#include "cmd_defs.h"
#include "jsmn.h"
#include <string.h>  // memcpy, strcmp, ecc.
#include <stdlib.h>  // strtoul, strtol

/* ---------------------------------------------------------------------------
 * Parser globale (statica) e array di token.
 * jsmn è “non allocante”, quindi occorre fornire un buffer di token
 * sufficientemente grande per la complessità dei messaggi previsti.
 * -------------------------------------------------------------------------*/
static jsmn_parser parser;
static jsmntok_t tokens[32];  // Capienza scelta in base ai nostri messaggi

/* ---------------------------------------------------------------------------
 * Utility interna per estrarre una stringa da un token JSMN.
 * - `json`: il testo originale
 * - `tok`: il token che indica inizio/fine substring
 * - `buf`: buffer destinazione dove copiare la substring
 * - `len`: dimensione del buffer
 * -------------------------------------------------------------------------*/
static const char* json_get_string(const char* json,
                                   jsmntok_t* tok,
                                   char* buf,
                                   size_t len) {
    int l = tok->end - tok->start;     // lunghezza effettiva nel JSON
    if (l >= (int)len) l = (int)len - 1;  // tronca se necessario
    memcpy(buf, json + tok->start, l);    // copia i caratteri
    buf[l] = '\0';                        // termina con '\0'
    return buf;
}

/* ---------------------------------------------------------------------------
 * Funzione pubblica per eseguire il parsing di un JSON in arrivo.
 * @param json  : stringa JSON in ingresso.
 * @param out   : struttura `Cmd` da riempire se il parsing ha successo.
 * @return true se il parsing è andato a buon fine; false altrimenti.
 *
 * Funzionamento:
 * 1. Inizializza il parser JSMN.
 * 2. Esegue jsmn_parse per ottenere il numero di token.
 * 3. Scorre i token alla ricerca delle chiavi importanti (cmd, idTipo, …).
 * 4. Assegna i valori corrispondenti nella struttura `Cmd`.
 * -------------------------------------------------------------------------*/
bool parse_command(const char* json, Cmd* out) {
    /* 1. Reset e parsing */
    jsmn_init(&parser);
    int ret = jsmn_parse(&parser, json, strlen(json),
                         tokens, sizeof(tokens)/sizeof(tokens[0]));
    if (ret < 0) return false;  // Errore di parsing

    /* Default: nessun comando riconosciuto */
    out->type = CMD_UNKNOWN;

    /* 2. Scansione dei token (saltando il token 0 che rappresenta l’oggetto root) */
    for (int i = 1; i < ret; ++i) {
        if (tokens[i].type == JSMN_STRING) {
            /* Copia la chiave nel buffer temporaneo `key` */
            char key[32];
            json_get_string(json, &tokens[i], key, sizeof(key));

            /* Confronta la chiave per capire che valore stiamo leggendo */
            if (!strcmp(key, "cmd")) {
                /* Lettura del tipo di comando */
                char value[32];
                json_get_string(json, &tokens[i + 1], value, sizeof(value));
                if      (!strcmp(value, "SET_RTC"))      out->type = CMD_SET_RTC;
                else if (!strcmp(value, "GET_INIT_DATA")) out->type = CMD_GET_INIT;
                else if (!strcmp(value, "ADD_PLANT"))    out->type = CMD_ADD_PLANT;
                else if (!strcmp(value, "START_MEAS"))   out->type = CMD_START_MEAS;
                else if (!strcmp(value, "GET_HISTORY"))  out->type = CMD_GET_HISTORY;
            }
            else if (!strcmp(key, "timestamp")) {
                /* Converte il valore numerico in intero 32 bit */
                out->timestamp = strtoul(json + tokens[i + 1].start, NULL, 10);
            }
            else if (!strcmp(key, "idPianta")) {
                out->idPianta = strtoul(json + tokens[i + 1].start, NULL, 10);
            }
            else if (!strcmp(key, "idTipo")) {
                out->idTipo = strtol(json + tokens[i + 1].start, NULL, 10);
            }
            else if (!strcmp(key, "nomePianta")) {
                /* Copia del nome nel buffer `out->nomePianta` */
                json_get_string(json, &tokens[i + 1],
                                out->nomePianta, sizeof(out->nomePianta));
            }

            i++;  // Salta il token successivo che rappresenta il “value” appena letto
        }
    }

    /* Ritorna true se il comando è stato riconosciuto */
    return (out->type != CMD_UNKNOWN);
}



