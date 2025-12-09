#include "meas_proc.h" // Includiamo l'header per avere le definizioni delle struct (PlantProfile)

// ============================================================================
// Database Statico dei Profili Pianta
// Questo array contiene i 100 profili di piante predefiniti.
// Essendo 'const', il compilatore lo allocherà in memoria di sola lettura (Flash).
// ============================================================================
const PlantProfile plant_db[MAX_PLANT_PROFILES] = {
    {  1, "Monstera deliciosa",                22,  3150, 55, 60 },
    {  2, "Epipremnum aureum (Pothos)",        23, 22500, 60, 50 },
    {  3, "Sansevieria (Snake Plant)",         26,   600, 40, 20 },
    {  4, "Aloe vera",                         25, 30000, 40, 20 },
    {  5, "Spathiphyllum (Peace Lily)",        24,  8000, 70, 70 },
    {  6, "Chlorophytum comosum",              21,  6000, 45, 60 },
    {  7, "Zamioculcas zamiifolia",            22,   800, 50, 20 },
    {  8, "Ficus lyrata",                      23,  8000, 50, 50 },
    {  9, "Pilea peperomioides",               20,  5000, 50, 50 },
    { 10, "Dracaena marginata",                24,  6000, 50, 30 },
    { 11, "Ficus benjamina",                   22,  8000, 50, 60 },
    { 12, "Rosmarinus officinalis",            20, 50000, 40, 20 },
    { 13, "Ocimum basilicum",                  25, 50000, 50, 60 },
    { 14, "Mentha spp.",                       20, 30000, 60, 80 },
    { 15, "Lavandula spp.",                    20, 50000, 40, 20 },
    { 16, "Thymus vulgaris",                   20, 50000, 40, 20 },
    { 17, "Salvia officinalis",                20, 50000, 40, 30 },
    { 18, "Origanum vulgare",                  20, 50000, 40, 30 },
    { 19, "Tradescantia",                      22,  6000, 50, 60 },
    { 20, "Hedera helix",                      18,  5000, 50, 70 },
    { 21, "Geranium / Pelargonium",            20, 30000, 40, 40 },
    { 22, "Cyclamen",                          16,  5000, 50, 70 },
    { 23, "Sempervivum",                       18, 60000, 30, 10 },
    { 24, "Soleirolia (Baby Tears)",           20,  3000, 70, 80 },
    { 25, "Kentia Palm",                       22,  6000, 50, 60 },
    { 26, "Areca Palm",                        22,  8000, 60, 70 },
    { 27, "Dracaena fragrans",                 23,  6000, 50, 60 },
    { 28, "Aglaonema",                         22,  4000, 60, 60 },
    { 29, "Calathea / Goeppertia",             23,  5000, 70, 70 },
    { 30, "Cactus (generico)",                 25, 60000, 30, 15 },
    { 31, "Tillandsia (Air Plant)",            22,  8000, 70,  1 },
    { 32, "Crassula ovata",                    22, 25000, 40, 30 },
    { 33, "Beaucarnea recurvata",              23, 20000, 40, 20 },
    { 34, "Pothos variegati",                  23,  8000, 60, 50 },
    { 35, "Peace Lily (cultivar)",             24,  8000, 70, 70 },
    { 36, "Dracaena (cultivar)",               23,  6000, 50, 50 },
    { 37, "Anthurium andraeanum",              23,  8000, 70, 60 },
    { 38, "Begonia (rex, wax, ecc.)",          21,  6000, 60, 60 },
    { 39, "Stromanthe triostar",               23,  5000, 70, 70 },
    { 40, "Maranta leuconeura",                23,  5000, 70, 70 },
    { 41, "Oxalis triangularis",               20,  8000, 50, 50 },
    { 42, "Calathea orbifolia",                23,  5000, 70, 70 },
    { 43, "Ficus elastica",                    23,  8000, 50, 60 },
    { 44, "Zamioculcas zamiifolia (cultivar)", 22,   800, 50, 30 },
    { 45, "Pothos (cultivar)",                 23,  8000, 60, 50 },
    { 46, "Strelitzia reginae",                24, 20000, 50, 60 },
    { 47, "Strelitzia nicolai",                24, 20000, 50, 60 },
    { 48, "Anthurium scherzerianum",           23,  8000, 70, 60 },
    { 49, "Alocasia/Colocasia",                25, 10000, 70, 70 },
    { 50, "Nephrolepis exaltata",              21,  5000, 70, 70 },
    { 51, "Boston Fern",                       21,  5000, 70, 70 },
    { 52, "Lady Palm",                         22,  6000, 50, 60 },
    { 53, "African Blue Basil",                25, 50000, 50, 60 },
    { 54, "Sweet William",                     18, 20000, 50, 60 },
    { 55, "Ivy (coltivata in vaso)",           18,  5000, 50, 70 },
    { 56, "Tradescantia zebrina",              22,  6000, 50, 60 },
    { 57, "Tradescantia pallida",              22,  8000, 50, 60 },
    { 58, "Asparagus densiflorus",             21,  6000, 50, 60 },
    { 59, "Geranio aromatico",                 20, 30000, 40, 40 },
    { 60, "Pilea (varianti)",                  20,  5000, 50, 50 },
    { 61, "Philodendron",                      23,  8000, 60, 60 },
    { 62, "Schefflera arboricola",             23,  8000, 50, 60 },
    { 63, "Clivia miniata",                    18,  6000, 50, 60 },
    { 64, "Paphiopedilum",                     22,  6000, 60, 60 },
    { 65, "Phalaenopsis",                      23,  6000, 60, 70 },
    { 66, "Orchidee varie",                    23,  8000, 60, 70 },
    { 67, "Bromeliads",                        23,  8000, 70, 60 },
    { 68, "Fittonia",                          22,  5000, 70, 70 },
    { 69, "Anthurium andraeanum",              23,  8000, 70, 60 },
    { 70, "Hoya carnosa",                      22,  6000, 60, 50 },
    { 71, "Kalanchoe blossfeldiana",           22, 20000, 40, 40 },
    { 72, "Christmas Cactus",                  20,  6000, 60, 70 },
    { 73, "Succulente varie",                  24, 40000, 30, 20 },
    { 74, "Agave americana",                   24, 50000, 30, 20 },
    { 75, "Aloe (altre specie)",               25, 30000, 40, 20 },
    { 76, "Citrus in vaso",                    22, 40000, 50, 60 },
    { 77, "Laurus nobilis",                    20, 30000, 50, 50 },
    { 78, "Zingiber officinale",               25, 20000, 70, 70 },
    { 79, "Cymbopogon citratus",               25, 30000, 60, 70 },
    { 80, "Stevia rebaudiana",                 24, 25000, 60, 60 },
    { 81, "Saintpaulia",                       22,  5000, 60, 70 },
    { 82, "Begonia rex",                       21,  6000, 60, 60 },
    { 83, "Coleus",                            22,  8000, 60, 60 },
    { 84, "Impatiens",                         21,  8000, 60, 70 },
    { 85, "Tradescantia nanouk",               22,  6000, 50, 60 },
    { 86, "Peperomia obtusifolia",             22,  5000, 60, 60 },
    { 87, "Peperomia caperata",                22,  5000, 60, 60 },
    { 88, "Guzmania",                          23,  6000, 70, 60 },
    { 89, "Tillandsia varie",                  22,  8000, 70,  1 },
    { 90, "Neoregelia",                        23,  6000, 70, 60 },
    { 91, "Felci varie",                       21,  5000, 70, 70 },
    { 92, "Adiantum",                          21,  5000, 70, 70 },
    { 93, "Sansevieria 'Laurentii'",           25,   600, 40, 20 },
    { 94, "Ficus elastica 'Tineke'",           23,  8000, 50, 60 },
    { 95, "Hoya carnosa variegata",            22,  6000, 60, 50 },
    { 96, "Peperomia obtusifolia (var.)",      22,  5000, 60, 60 },
    { 97, "Peperomia caperata (var.)",         22,  5000, 60, 60 },
    { 98, "Dracaena sanderiana",               23,  6000, 60, 70 },
    { 99, "Bonsai comuni",                     20, 10000, 50, 50 },
    {100, "Cyclamen (cultivar)",               16,  5000, 50, 70 }
};

// ============================================================================
// Istanze di piante demo.
// // Essendo 'const', anche questo andrà in memoria di sola lettura (Flash).
// ============================================================================

const MiaPianta demo_plant_list[DEMO_PLANTS_COUNT] __attribute__((used)) = {
    { 4, 1, "Aloe di Rocco"},
    {99, 2, "Bonsai di Giovanni"},
    {22, 3, "Ciclamino di Domenico"}
};


// ============================================================================
// Storico Misurazioni di Esempio per la Demo (in Flash)
// Scenario: 5 misurazioni per l'Aloe di Rocco (idPianta: 1).
// Le prime 3 mostrano cure non ottimali (poca luce, poca acqua).
// Le ultime 2 mostrano un netto miglioramento.
// ============================================================================

const Misurazione demo_measurement_list[DEMO_MEAS_COUNT] __attribute__((used)) = {
    // --- Scenario 1: Aloe di Rocco (5 misurazioni) ---
    {.magic=MEASUREMENT_MAGIC, .idPianta=1, .idTipo=4, .temperatura_c=23, .luce_l=10000, .umi_out=45, .umi_in=5, .v_tot=70, .v_temp=25, .v_luce=10, .v_umi_out=25, .v_umi_in=10, .idx_temp_str=0, .idx_luce_str=8, .idx_umi_out_str=10, .idx_umi_in_str=18 },
    {.magic=MEASUREMENT_MAGIC, .idPianta=1, .idTipo=4, .temperatura_c=24, .luce_l=12000, .umi_out=42, .umi_in=8, .v_tot=70, .v_temp=25, .v_luce=10, .v_umi_out=25, .v_umi_in=10, .idx_temp_str=0, .idx_luce_str=8, .idx_umi_out_str=10, .idx_umi_in_str=18 },
    {.magic=MEASUREMENT_MAGIC, .idPianta=1, .idTipo=4, .temperatura_c=25, .luce_l=16000, .umi_out=40, .umi_in=10, .v_tot=80, .v_temp=25, .v_luce=20, .v_umi_out=25, .v_umi_in=10, .idx_temp_str=0, .idx_luce_str=6, .idx_umi_out_str=10, .idx_umi_in_str=18 },
    {.magic=MEASUREMENT_MAGIC, .idPianta=1, .idTipo=4, .temperatura_c=26, .luce_l=28000, .umi_out=38, .umi_in=18, .v_tot=100, .v_temp=25, .v_luce=25, .v_umi_out=25, .v_umi_in=25, .idx_temp_str=0, .idx_luce_str=5, .idx_umi_out_str=10, .idx_umi_in_str=15 },
    {.magic=MEASUREMENT_MAGIC, .idPianta=1, .idTipo=4, .temperatura_c=25, .luce_l=30000, .umi_out=40, .umi_in=20, .v_tot=100, .v_temp=25, .v_luce=25, .v_umi_out=25, .v_umi_in=25, .idx_temp_str=0, .idx_luce_str=5, .idx_umi_out_str=10, .idx_umi_in_str=15 },

    // --- Scenario 2: Bonsai di Giovanni (4 misurazioni) ---
    {.magic=MEASUREMENT_MAGIC, .idPianta=2, .idTipo=99, .temperatura_c=21, .luce_l=9800, .umi_out=52, .umi_in=50, .v_tot=100, .v_temp=25, .v_luce=25, .v_umi_out=25, .v_umi_in=25, .idx_temp_str=0, .idx_luce_str=5, .idx_umi_out_str=10, .idx_umi_in_str=15 },
    {.magic=MEASUREMENT_MAGIC, .idPianta=2, .idTipo=99, .temperatura_c=20, .luce_l=10000, .umi_out=50, .umi_in=50, .v_tot=100, .v_temp=25, .v_luce=25, .v_umi_out=25, .v_umi_in=25, .idx_temp_str=0, .idx_luce_str=5, .idx_umi_out_str=10, .idx_umi_in_str=15 },
    {.magic=MEASUREMENT_MAGIC, .idPianta=2, .idTipo=99, .temperatura_c=20, .luce_l=10500, .umi_out=42, .umi_in=48, .v_tot=95, .v_temp=25, .v_luce=25, .v_umi_out=20, .v_umi_in=25, .idx_temp_str=0, .idx_luce_str=5, .idx_umi_out_str=11, .idx_umi_in_str=15 },
    {.magic=MEASUREMENT_MAGIC, .idPianta=2, .idTipo=99, .temperatura_c=20, .luce_l=10000, .umi_out=50, .umi_in=50, .v_tot=100, .v_temp=25, .v_luce=25, .v_umi_out=25, .v_umi_in=25, .idx_temp_str=0, .idx_luce_str=5, .idx_umi_out_str=10, .idx_umi_in_str=15 },

    // --- Scenario 3: Ciclamino di Domenico (6 misurazioni) ---
    {.magic=MEASUREMENT_MAGIC, .idPianta=3, .idTipo=22, .temperatura_c=17, .luce_l=5200, .umi_out=55, .umi_in=70, .v_tot=100, .v_temp=25, .v_luce=25, .v_umi_out=25, .v_umi_in=25, .idx_temp_str=0, .idx_luce_str=5, .idx_umi_out_str=10, .idx_umi_in_str=15 },
    {.magic=MEASUREMENT_MAGIC, .idPianta=3, .idTipo=22, .temperatura_c=16, .luce_l=5000, .umi_out=50, .umi_in=72, .v_tot=100, .v_temp=25, .v_luce=25, .v_umi_out=25, .v_umi_in=25, .idx_temp_str=0, .idx_luce_str=5, .idx_umi_out_str=10, .idx_umi_in_str=15 },
    {.magic=MEASUREMENT_MAGIC, .idPianta=3, .idTipo=22, .temperatura_c=28, .luce_l=6000, .umi_out=30, .umi_in=40, .v_tot=50, .v_temp=10, .v_luce=25, .v_umi_out=10, .v_umi_in=5, .idx_temp_str=4, .idx_luce_str=5, .idx_umi_out_str=13, .idx_umi_in_str=18 },
    {.magic=MEASUREMENT_MAGIC, .idPianta=3, .idTipo=22, .temperatura_c=32, .luce_l=7000, .umi_out=25, .umi_in=25, .v_tot=35, .v_temp=10, .v_luce=20, .v_umi_out=10, .v_umi_in=-5, .idx_temp_str=4, .idx_luce_str=7, .idx_umi_out_str=13, .idx_umi_in_str=18 },
    {.magic=MEASUREMENT_MAGIC, .idPianta=3, .idTipo=22, .temperatura_c=35, .luce_l=8000, .umi_out=20, .umi_in=10, .v_tot=5, .v_temp=0, .v_luce=10, .v_umi_out=-5, .v_umi_in=0, .idx_temp_str=4, .idx_luce_str=8, .idx_umi_out_str=13, .idx_umi_in_str=18 },
    {.magic=MEASUREMENT_MAGIC, .idPianta=3, .idTipo=22, .temperatura_c=36, .luce_l=8500, .umi_out=18, .umi_in=5, .v_tot=0, .v_temp=0, .v_luce=0, .v_umi_out=0, .v_umi_in=0, .idx_temp_str=4, .idx_luce_str=8, .idx_umi_out_str=13, .idx_umi_in_str=18 }
};



// ============================================================================
// Stringhe Statiche di Valutazione
// Questo array contiene le 20 stringhe di valutazione usate per i consigli.
// Essendo 'const', anche questo andrà in memoria di sola lettura (Flash).
// ============================================================================
const char* score_strings[20] = {
    // Temperatura (Indici 0-4)
    "Temperatura ottimale",
    "Temperatura da aumentare",
    "Temperatura da ridurre",
    "Temperatura critica: aumentare subito!",
    "Temperatura critica: diminuire subito!",

    // Luce (Indici 5-9)
    "Luce ottimale",
    "Luce da aumentare",
    "Luce da ridurre",
    "Luce critica: aumentare subito!",
    "Luce critica: diminuire subito!",

    // Umidità esterna (aria) (Indici 10-14)
    "Umidita' esterna ottimale",
    "Umidita' esterna da aumentare",
    "Umidita' esterna da ridurre",
    "Umidita' esterna critica: aumentare subito!",
    "Umidita' esterna critica: diminuire subito!",

    // Umidità interna (suolo) (Indici 15-19)
    "Umidita' interna ottimale",
    "Umidita' interna da aumentare",
    "Umidita' interna da ridurre",
    "Umidita' interna critica: aumentare subito!",
    "Umidita' interna critica: diminuire subito!"
};
