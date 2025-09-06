#ifndef MEAS_PROC_H
#define MEAS_PROC_H

#include <stdint.h>

// ============================================================================
// Costanti di Configurazione
// ============================================================================

// --- Capacità Massima del Sistema ---
#define MAX_PLANT_PROFILES 100    // Tipi di pianta predefiniti nel database
#define MAX_USER_PLANTS    10     // Massimo numero di piante totali in RAM (demo + nuove)
#define MAX_MEASUREMENTS   100    // Massimo numero di misurazioni totali in RAM
#define MAX_PLANT_NAME_LEN 32     // Lunghezza massima nome pianta utente (con \0)

// --- Costanti per la Demo ---
#define DEMO_PLANTS_COUNT  3      // NUOVO: Numero di piante demo caricate all'avvio
#define DEMO_MEAS_COUNT    15     // NUOVO: Numero di misurazioni demo (5+4+6)

// --- Costanti Magiche per Validazione Dati ---
#define MEASUREMENT_MAGIC  0xBEEFCAFE // Un valore univoco per identificare una struct Misurazione valida

// ============================================================================
// Strutture Dati (Logica invariata, rimosse dipendenze da Flash)
// ============================================================================

// Struttura per il timestamp (mantenuta per usi futuri)
typedef struct {
    uint16_t year; uint8_t month; uint8_t day;
    uint8_t hour; uint8_t min; uint8_t sec;
} DateTime;

// Profilo generico di un tipo di pianta (letto da `const` in Flash)
typedef struct {
    int idTipo;
    const char *nomeTipo;
    int temp_ott; int luce_ott; int umi_out_ott; int umi_in_ott;
} PlantProfile;

// Istanza di una pianta specifica dell'utente (gestita in RAM)
typedef struct {
  uint32_t idPianta;
  int idTipo;
  char nomePianta[MAX_PLANT_NAME_LEN];
} MiaPianta;

// Struct di una singola misurazione (gestita in RAM)
typedef struct {
    uint32_t magic;
    uint32_t idPianta;
    int idTipo;
    DateTime timestamp; // Attualmente non utilizzata ma pronta per il futuro
    int temperatura_c; int luce_l; int umi_out; int umi_in;
    int v_tot; int v_temp; int v_luce; int v_umi_out; int v_umi_in;
    uint8_t idx_temp_str; uint8_t idx_luce_str; uint8_t idx_umi_out_str; uint8_t idx_umi_in_str;
} Misurazione;


// ============================================================================
// Dichiarazioni Esterne per Dati di Demo (in database.c)
// ============================================================================

// Dichiarazione per le stringhe di valutazione
extern const char* score_strings[20];
// NUOVO: Dichiarazioni per i dati `const` usati per inizializzare la RAM
extern const PlantProfile plant_db[MAX_PLANT_PROFILES];
extern const MiaPianta demo_plant_list[DEMO_PLANTS_COUNT];
extern const Misurazione demo_measurement_list[DEMO_MEAS_COUNT];


// ============================================================================
// API Pubbliche (Refactoring per architettura in RAM)
// ============================================================================

/**
 * @brief Inizializza lo storage in RAM.
 * @details Pulisce i buffer in RAM e li popola con i dati di demo
 *          (piante e misurazioni) letti dalla Flash.
 *          DA CHIAMARE UNA SOLA VOLTA ALL'AVVIO.
 */
void init_runtime_storage(void); // NUOVA FUNZIONE

/**
 * @brief Registra una nuova pianta nella sessione RAM corrente.
 * @param nomePianta Nome assegnato dall'utente alla pianta.
 * @param idTipo ID del profilo generico (da PlantProfile).
 * @return L'ID della nuova pianta registrata (>= 1), oppure 0 se non c'è più spazio.
 */
int registra_nuova_pianta(const char* nomePianta, int idTipo);

/**
 * @brief Recupera i dati di una pianta specifica tramite il suo ID.
 * @param idPianta L'ID della pianta da cercare.
 * @return Un puntatore `const` alla pianta se trovata, altrimenti `NULL`.
 */
const MiaPianta* get_mia_pianta_by_id(uint32_t idPianta);

/**
 * @brief Recupera l'elenco di tutte le piante attualmente in RAM.
 * @param[out] count Puntatore a una variabile che verrà riempita con il numero di piante trovate.
 * @return Un puntatore `const` al primo elemento dell'array di piante.
 */
const MiaPianta* get_elenco_piante(uint32_t* count);

/**
 * @brief Esegue la pipeline di analisi su un set di valori sensoriali.
 * @details Calcola punteggi, genera consigli e salva la misurazione risultante
 *          nello storage in RAM.
 * @param idPianta L'ID della pianta a cui si riferisce la misurazione.
 * @return Una struct Misurazione completa con i risultati dell'analisi.
 */
Misurazione esegui_pipeline_misurazione(int temp, int luce, int umi_out, int umi_in, uint32_t idPianta);

/**
 * @brief Recupera il profilo generico di una pianta dato il suo tipo.
 * @param idTipo L'ID del tipo di pianta.
 * @return Un puntatore `const` al profilo se trovato, altrimenti `NULL`.
 */
const PlantProfile* get_plant_profile(int idTipo);

/**
 * @brief Recupera tutte le misurazioni salvate nella sessione RAM.
 * @param[out] buffer Un buffer fornito dal chiamante dove copiare le misurazioni.
 * @param max_count La dimensione massima del buffer fornito.
 * @return Il numero di misurazioni effettivamente copiate nel buffer.
 */
uint32_t get_measurements(Misurazione *buffer, uint32_t max_count); // MODIFICATA (ex load_measurements)

#endif // MEAS_PROC_H
