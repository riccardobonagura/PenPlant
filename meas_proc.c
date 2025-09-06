#include "meas_proc.h"

#include <string.h> // Necessario per memset e memcpy

// ============================================================================
// NUOVO: Database Dinamico in RAM
// Questa singola struttura statica contiene l'intero stato della sessione:
// sia i dati di demo caricati all'avvio, sia i nuovi dati creati dall'utente.
// Essendo 'static', è privata e accessibile solo dalle funzioni in questo file.
// ============================================================================

static struct {
    // Array per contenere tutte le piante (demo + nuove)
    MiaPianta elenco_piante[MAX_USER_PLANTS];
    uint32_t numero_piante_attuali; // Contatore per le piante

    // Array per contenere tutte le misurazioni (demo + nuove)
    Misurazione elenco_misure[MAX_MEASUREMENTS];
    uint32_t numero_misure_attuali; // Contatore per le misurazioni
} ram_db;

// ============================================================================
// NUOVA FUNZIONE: Inizializzazione dello Storage in RAM
// ============================================================================
/**
 * @brief Inizializza lo storage in RAM per la sessione corrente.
 * @details Questa funzione deve essere chiamata una sola volta all'avvio.
 *          1. Azzera completamente il database in RAM per partire da uno stato pulito.
 *          2. Copia le piante di demo (definite in database.c) dalla Flash alla RAM.
 *          3. Copia le misurazioni di demo dalla Flash alla RAM.
 *          4. Imposta i contatori iniziali in base ai dati di demo caricati.
 */
void init_runtime_storage(void) {
    // 1. Pulisce l'intero database in RAM.
    memset(&ram_db, 0, sizeof(ram_db));

    // 2. Copia le piante di demo e imposta il contatore.
    memcpy(ram_db.elenco_piante, demo_plant_list, sizeof(demo_plant_list));
    ram_db.numero_piante_attuali = DEMO_PLANTS_COUNT;

    // 3. Copia le misurazioni di demo e imposta il contatore.
    memcpy(ram_db.elenco_misure, demo_measurement_list, sizeof(demo_measurement_list));
    ram_db.numero_misure_attuali = DEMO_MEAS_COUNT;
}

// ============================================================================
// MODIFICATO: Gestione Piante Utente (ora solo in RAM)
// ============================================================================
/**
 * @brief Registra una nuova pianta nella sessione RAM corrente.
 * @details Aggiunge una nuova pianta all'array in RAM se c'è spazio.
 *          Non esegue più operazioni di scrittura su Flash.
 * @return L'ID della nuova pianta (>=1) o 0 in caso di errore (spazio esaurito).
 */
int registra_nuova_pianta(const char* nomePianta, int idTipo) {
    if (ram_db.numero_piante_attuali >= MAX_USER_PLANTS) {
        return 0; // Errore: limite massimo di piante raggiunto in RAM
    }

    uint32_t idx = ram_db.numero_piante_attuali;
    uint32_t nuovoId = idx + 1; // L'ID è progressivo (1, 2, 3...)

    // Popola la prossima entry libera nell'array `elenco_piante` in RAM
    ram_db.elenco_piante[idx].idPianta = nuovoId;
    ram_db.elenco_piante[idx].idTipo = idTipo;
    strncpy(ram_db.elenco_piante[idx].nomePianta, nomePianta, MAX_PLANT_NAME_LEN - 1);
    ram_db.elenco_piante[idx].nomePianta[MAX_PLANT_NAME_LEN - 1] = '\0';

    ram_db.numero_piante_attuali++; // Incrementa il contatore delle piante

    return nuovoId;
}

/**
 * @brief Recupera una pianta dall'archivio in RAM tramite il suo ID.
 * @return Un puntatore const alla pianta se trovata, altrimenti NULL.
 */
const MiaPianta* get_mia_pianta_by_id(uint32_t idPianta) {
    if (idPianta == 0 || idPianta > ram_db.numero_piante_attuali) {
        return NULL;
    }
    // L'ID utente parte da 1, l'indice dell'array da 0
    return &ram_db.elenco_piante[idPianta - 1];
}

/**
 * @brief Recupera l'intero elenco delle piante presenti in RAM.
 * @param[out] count Puntatore che verrà riempito con il numero totale di piante.
 * @return Puntatore const al primo elemento dell'array delle piante.
 */
const MiaPianta* get_elenco_piante(uint32_t* count) {
    *count = ram_db.numero_piante_attuali;
    return ram_db.elenco_piante;
}

/**
 * @brief Recupera un profilo generico dal database `const` in Flash.
 * @details Questa funzione non è cambiata, perché i profili sono sempre `const`.
 * @return Puntatore const al profilo se trovato, altrimenti NULL.
 */
const PlantProfile* get_plant_profile(int idTipo) {
    // I profili sono in un array, ma potrebbero non essere ordinati per ID.
    // Una ricerca lineare garantisce di trovare il profilo corretto.
    for (size_t i = 0; i < MAX_PLANT_PROFILES; ++i) {
        if (plant_db[i].idTipo == idTipo) {
            return &plant_db[i];
        }
    }
    return NULL; // ID non trovato
}

// ============================================================================
// MODIFICATO: Pipeline e Salvataggio Misurazioni
// ============================================================================

/**
 * @brief Funzione helper statica per salvare una misurazione in RAM.
 * @details Controlla se c'è spazio nell'array delle misurazioni e, in caso
 *          affermativo, aggiunge la nuova misurazione e incrementa il contatore.
 */
static void save_measurement_in_ram(const Misurazione *m) {
    if (ram_db.numero_misure_attuali >= MAX_MEASUREMENTS) {
        // Spazio esaurito, per ora ignoriamo la nuova misurazione.
        // In futuro si potrebbe implementare un buffer circolare.
        return;
    }
    ram_db.elenco_misure[ram_db.numero_misure_attuali] = *m;
    ram_db.numero_misure_attuali++;
}

/**
 * @brief Logica di valutazione di un singolo parametro (INVARIATA).
 */
static void valuta_parametro(int valore, int riferimento, int *score, uint8_t *idx_str, int idx_base) {
    if (riferimento <= 0) { *score = 0; *idx_str = idx_base; return; }
    int diff = valore - riferimento;
    int perc = ((int64_t)diff * 100) / riferimento;
    if (perc >= -20 && perc <= 20) { *score = 25; *idx_str = idx_base; }
    else if (perc < -20 && perc >= -40) { *score = 20; *idx_str = idx_base + 1; }
    else if (perc > 20 && perc <= 40) { *score = 20; *idx_str = idx_base + 2; }
    else if (perc < -40) { *score = 10; *idx_str = idx_base + 3; }
    else { *score = 10; *idx_str = idx_base + 4; }
}

/**
 * @brief Esegue la pipeline di misurazione e salva il risultato in RAM.
 * @details La logica di calcolo è invariata. La modifica chiave è che alla fine
 *          chiama la funzione interna `save_measurement_in_ram`.
 * @return La struct Misurazione completa di calcoli.
 */
Misurazione esegui_pipeline_misurazione(int temp, int luce, int umi_out, int umi_in, uint32_t idPianta) {
    Misurazione m;
    memset(&m, 0, sizeof(Misurazione));

    const MiaPianta* p = get_mia_pianta_by_id(idPianta);
    if (!p) {
        m.magic = 0; // Invalida la misurazione se la pianta non esiste
        return m;
    }

    m.magic = MEASUREMENT_MAGIC;
    m.idPianta = idPianta;
    m.idTipo = p->idTipo;
    // TODO: Popolare m.timestamp se si aggiunge un RTC

    m.temperatura_c = temp;
    m.luce_l = luce;
    m.umi_out = umi_out;
    m.umi_in = umi_in;

    const PlantProfile *prof = get_plant_profile(p->idTipo);
    if (prof) {
        valuta_parametro(temp, prof->temp_ott, &m.v_temp, &m.idx_temp_str, 0);
        valuta_parametro(luce, prof->luce_ott, &m.v_luce, &m.idx_luce_str, 5);
        valuta_parametro(umi_out, prof->umi_out_ott, &m.v_umi_out, &m.idx_umi_out_str, 10);
        valuta_parametro(umi_in, prof->umi_in_ott, &m.v_umi_in, &m.idx_umi_in_str, 15);
        m.v_tot = m.v_temp + m.v_luce + m.v_umi_out + m.v_umi_in;
    }

    // MODIFICA CHIAVE: salva il risultato nella RAM invece che su Flash.
    save_measurement_in_ram(&m);
    return m;
}

// ============================================================================
// MODIFICATO: Lettura Misurazioni dalla RAM
// ============================================================================
/**
 * @brief Recupera le misurazioni salvate nella sessione RAM.
 * @details Copia le misurazioni dall'array `ram_db.elenco_misure` al buffer
 *          fornito dal chiamante.
 * @return Il numero di misurazioni effettivamente copiate.
 */

uint32_t get_measurements(Misurazione *buffer, uint32_t max_count) {
    //chprintf(chp, ">>> entra in get_measurements\r\n");

    if (buffer == NULL || max_count == 0) {
        //chprintf(chp, "buffer NULL o max_count=0, ritorno subito.\r\n");
        return 0;
    }

    uint32_t count_to_copy = ram_db.numero_misure_attuali;
    //chprintf(chp, "numero_misure_attuali = %u\r\n", ram_db.numero_misure_attuali);

    if (count_to_copy > max_count) {
        //chprintf(chp, "ATTENZIONE: troppi dati (%u > %u), tronco.\r\n", count_to_copy, max_count);
        count_to_copy = max_count;
    }
    //chprintf(chp, "sizeof(Misurazione)=%u, MAX_MEASUREMENTS=%u\r\n",
             //(unsigned)sizeof(Misurazione), MAX_MEASUREMENTS);

    if (count_to_copy > 0) {
        //chprintf(chp, "Copio %u misurazioni in buffer...\r\n", count_to_copy);
        memcpy(buffer, ram_db.elenco_misure, count_to_copy * sizeof(Misurazione));
        //chprintf(chp, "memcpy completata!\r\n");
    } else {
        //chprintf(chp, "Nessuna misurazione da copiare.\r\n");
    }
    //count_to_copy = 0;
    //chprintf(chp, "<<< esco da get_measurements con count=%u\r\n", count_to_copy);
    return count_to_copy;
}


