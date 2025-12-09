/* ---------------------------------------------------------------------------
 * cmd_defs.h
 * ---------------------------------------------------------------------------
 * Qui definiamo tutti i tipi e le enumerazioni necessari a rappresentare
 * i comandi scambiati tra la Nucleo (STM32) e la sonda ESP8266 via JSON.
 *
 * - `CmdType` elenca i possibili comandi riconosciuti dal sistema.
 * - `Cmd` è una struttura generica che contiene tutti i campi che possono
 *   comparire nei vari comandi in ingresso.
 * - Le costanti e i tipi usati (es. `MiaPianta`) sono definiti nel file
 *   `meas_proc.h`, in cui sono descritte le strutture dei dati applicativi.
 * -------------------------------------------------------------------------*/

#ifndef CMD_DEFS_H
#define CMD_DEFS_H

#include "meas_proc.h"   // Definisce Max PLANT name e le strutture per i dati
#include <stdint.h>      // Tipi interi standard (uint32_t, ecc.)

/* ---------------------------------------------------------------------------
 * Enum per identificare il tipo di comando ricevuto.
 * I valori sono autoincrementali (0, 1, 2, …).
 * -------------------------------------------------------------------------*/
typedef enum {
    CMD_SET_RTC,     // Comando per impostare data e ora della Nucleo
    CMD_GET_INIT,    // Richiesta di invio catalogo + istanze piante
    CMD_ADD_PLANT,   // Richiesta di aggiunta nuova pianta
    CMD_START_MEAS,  // Richiesta di avviare una nuova misurazione
    CMD_GET_HISTORY, // Richiesta storico misurazioni di una pianta
    CMD_UNKNOWN      // Comando non riconosciuto (default)
} CmdType;

/* ---------------------------------------------------------------------------
 * Struttura generica per un comando in arrivo via JSON.
 * In base al tipo (`CmdType`) saranno usati (o ignorati) i campi sottostanti.
 * -------------------------------------------------------------------------*/
typedef struct {
    CmdType type;                          // Tipo di comando riconosciuto

    uint32_t timestamp;                    // Usato per CMD_SET_RTC
    uint32_t idPianta;                     // Usato per START_MEAS e GET_HISTORY
    int      idTipo;                       // Usato per ADD_PLANT

    /* Nome della pianta (ADD_PLANT); dimensione massima definita in meas_proc.h */
    char     nomePianta[MAX_PLANT_NAME_LEN];
} Cmd;

#endif  /* CMD_DEFS_H */
