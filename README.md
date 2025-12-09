# PEN PLANT

# PenPlant — Prototype Smart Plant Monitor

A prototype embedded system for rapid assessment of plant health using a compact multi‑sensor probe.  
PenPlant reads soil moisture/EC, ambient temperature & humidity, and light; computes per‑parameter scores against plant profiles; aggregates a global health score and provides immediate feedback (OLED / serial / Wi‑Fi). The codebase is an educational prototype with a clear modular design (drivers, measurement pipeline, FSM, JSON comms) suitable for demos, research prototypes or teaching.

This README describes what the prototype does, how it is organized, the main modules and interfaces, and how to operate and extend it. The project is not a finished product — many higher‑level features (persistent storage, mobile app integration, full calibration flows) are left for future work.

Table of contents
- Summary
- Quick demo & user story
- Main features 
- Architecture & module map
- Hardware mapping (pins observed in code)
- JSON / Wi‑Fi protocol (examples)
- Key APIs (meas_proc)
- Build & run (prototype guidance)
- Limitations & prototype notes

---

Summary
PenPlant is a compact, battery‑powered (or USB‑powered) device intended to help hobbyists or small growers monitor plant wellbeing with objective sensor readings. After a quick scan, the device reports:
- a global "health" score (0–100),
- per‑category indicators (hydration, nutrients/EC, microclimate, light),
- actionable suggestions when values are outside optimal ranges,
- an optional history of past measurements.

The system is designed to be simple to demo: push a button or request a measurement over Wi‑Fi, and get an immediate readout on the OLED and via serial/JSON to a connected ESP8266 or host.

Quick demo & user story
- Power the device and wait for the boot process (ChibiOS init).
- From a terminal (serial) or the Wi‑Fi link, send a JSON command to start a measurement.
- The device reads sensors, computes normalized values and scores, prints a short summary on the OLED and replies over serial/Wi‑Fi with a JSON measurement object.
- The user can later request history for a plant, add a plant, or set the RTC.

---

Main features (implemented in the prototype)
- Sensor drivers:
  - ADC sampling with averaging (adc_module) for soil‑related sensors (moisture / EC).
  - DHT11 driver (dht11_module) using ICU capture for temperature & humidity.
  - TCS3200 color/light sensor (tcs3200_module) using ICU period capture and a linearized conversion to lux.
  - OLED text display via SSD1306 (oled_module).
- Application framework:
  - FSM (fsm.c / fsm.h) managing lifecycle and user flows (init, wait RTC, update, home, measure, history).
  - Measurement processing pipeline (meas_proc.*) that maps raw sensor inputs to per‑sector scores and an aggregate health score, and stores measurements in RAM for the session.
- Communication:
  - Structured JSON protocol with a compact/robust parser (jsmn) and json_builder/ parser for commands and responses.
  - Wi‑Fi serial interface (wifi_module) targeted at an ESP8266 or similar (simple line‑based read/write helpers).
  - Serial console helpers for debugging (serial_module).
- Demo data:
  - Built‑in plant profile database in flash (database.c) with many common species and their ideal ranges for scoring.
- Utility/test helpers:
  - Simple test harness utilities using jsmn tokens (testutil.h/test.h) to validate parsing or build small unit tests on target.

---

Architecture & file/module map
- Top level runtime
  - main.c — Initialize HAL / ChibiOS, serial, FSM loop.
  - fsm.c / fsm.h — Finite State Machine driving app states and transitions.
- Sensor / peripheral drivers
  - adc_module.c / adc_module.h — ADC sampling, averaging, percentage conversion.
  - dht11_module.c / dht11_module.h — DHT11 reading via ICU timing, averaging.
  - tcs3200_module.c / TCS3200_module.h — Frequency capture & approximate lux conversion for the TCS3200 sensor.
  - oled_module.c / oled_module.h — SSD1306 initialization and print helpers (single or double string).
  - wifi_module.c / wifi_module.h — UART interface for Wi‑Fi module (SD1), line reads and writes.
  - serial_module.c / serial_module.h — Serial (SD2) init and printf wrapper.
- Application logic
  - meas_proc.c / meas_proc.h — Core measurement and scoring pipeline, in‑RAM storage of user plants and measurements, demo population.
  - database.c — Const flash plant profiles (PlantProfile array).
- Communication & JSON
  - jsmn.h — Small, non‑allocating JSON token parser.
  - json_parser.c — Parse incoming JSON to Cmd struct.
  - json_builder.c — Build outgoing JSON messages (init data, measurement, history).
  - cmd_defs.h — Command type definitions and Cmd struct layout.
- Tests & utilities
  - testutil.h, test.h — token/parse helpers and a tiny unit test runner for embedded environment.

---

Hardware mapping (pins & interfaces observed)
The code reveals the following pin mappings and interfaces. Use this as a starting point — confirm against your schematic.

- ADC (analog input)
  - Configured at GPIOA pin 0 (palSetPadMode(GPIOA, 0U, PAL_MODE_INPUT_ANALOG)) — used by adc_module.
- Button
  - BUTTON_PAD = GPIOA, BUTTON_PIN = 5 (PA5 input pull‑up).
- Serial (console)
  - UART2 used for console (PA2 = TX, PA3 = RX).
  - Console baud: 115200.
- Wi‑Fi module (ESP-like)
  - UART1 used for Wi‑Fi module: WIFI_TX = PC4, WIFI_RX = PC5 (sdStart SD1 at 115200).
- TCS3200 sensor pins (example mapping used in code)
  - S0: PB1, S1: PB2, S2: PB11, S3: PB12, OUT: PB6 (ICU TIM4 CH1).
- OLED (SSD1306)
  - I2C pins PB8 (SCL) and PB9 (SDA) configured for alternate function.
- RTC
  - HAL RTC driver (RTC object RTCD1) via rtc_module.

Note: pin names (PAx / PBx / PCx) and peripheral instances (SD1/SD2/ICUDx/ICUDy) are used as examples in code — adjust if your board variant differs.

---

JSON protocol (examples)
The prototype communicates using a compact JSON command structure. The recognized commands (CmdType in cmd_defs.h) include:
- SET_RTC — set date/time
- GET_INIT_DATA — request initialization/catalog data
- ADD_PLANT — add a new user plant
- START_MEAS — request a measurement for a plant
- GET_HISTORY — request measurement history for a plant

Example request to start a measurement:
{"cmd":"START_MEAS","idPianta":2}

Example request to add a plant:
{"cmd":"ADD_PLANT","idTipo":5,"nomePianta":"MyAloe"}

Example set RTC:
{"cmd":"SET_RTC","timestamp":1690000000}
or
{"cmd":"SET_RTC","date":"05/09/2025","time":"15:30:45"}

How parsing works:
- The device uses jsmn (non‑allocating token parser) to tokenize the JSON string, then json_parser walks tokens and fills a Cmd struct. The builder side builds JSON replies (measurement objects, init data, ack).

Measurement reply (example shape — fields depend on json_builder implementation):
{
  "type":"MEASUREMENT",
  "idPianta":2,
  "score_global":78,
  "hydration":72,
  "nutrients":55,
  "microclimate":89,
  "light":70,
  "timestamp":1690001234
}

(Exact field names and structure are defined in json_builder.c — check that file for precise payloads your firmware emits.)

---

Key application APIs (meas_proc)
Core public functions in meas_proc.h used by the FSM and JSON builder:
- void init_runtime_storage(void);
  - Initialize in‑RAM storage and populate demo plants/measurements.
- int registra_nuova_pianta(const char* nomePianta, int idTipo);
  - Register a new user plant in RAM. Returns new id or 0 on error.
- const MiaPianta* get_mia_pianta_by_id(uint32_t idPianta);
  - Retrieve plant instance by id.
- const MiaPianta* get_elenco_piante(uint32_t* count);
  - Return the array of user plants and fill count.
- Misurazione esegui_pipeline_misurazione(int temp, int luce, int umi_out, int umi_in, uint32_t idPianta);
  - Run the measurement pipeline given raw sensor values. Returns a full Misurazione struct with computed scores and fields prefilled. Also stores the measurement in RAM.
- const PlantProfile* get_plant_profile(int idTipo);
  - Fetch the profile for a plant type from the flash database.
- uint32_t get_measurements(Misurazione *buffer, uint32_t max_count);
  - Copy stored measurements into a caller buffer (for history export).

These functions are the main abstraction point between low‑level drivers and higher‑level UI / comms.

---

Build & run (guidance for a prototype)
This project targets an STM32-based Nucleo/board using ChibiOS. A generic outline to build & flash:

Prerequisites
- ChibiOS source tree matched to the project configuration.
- ARM cross toolchain (arm-none-eabi-gcc / binutils).
- OpenOCD or vendor programmer (ST‑Link / ST‑Link‑V2).
- Makefile or CMake build system for your board (if not present, you may need to create one).

Recommended steps
1. Get ChibiOS and set up the board target configuration matching the used timers/peripherals.
2. Place this repo under the ChibiOS workspace or adapt the build system to include the files.
3. Configure CPU target in Makefile (MCU, F_CPU, linker script).
4. Build: make all (or the equivalent CMake invocation).
5. Flash the binary via OpenOCD / ST‑Link:
   - openocd -f interface/stlink.cfg -f target/stm32f4x.cfg
   - then use `program` or the vendor tool to flash.
6. Connect a serial console to the configured UART (PA2/PA3 at 115200) to observe debug prints.
7. If using a Wi‑Fi module (ESP8266), connect its RX/TX to PC4/PC5 and interact via text commands.

Notes:
- This repo currently doesn't include a top‑level Makefile / board configuration in the pasted snippets. If you want, I can provide a sample Makefile or CMakeLists tailored to a specific Nucleo board once you confirm the MCU part number and ChibiOS version.
- The firmware was developed with ChibiOS HAL APIs (adcStart, icuStart, sdStart, etc). Ensure your ChibiOS config enables those drivers.

---

Limitations and prototype notes (important)
- Prototype status: several high‑level features are intentionally minimal or missing (persistent flash/FS storage across power cycles, full mobile app, calibration UI, in‑field calibration constants).
- The plant profiles are embedded as a const array in flash (database.c); editing requires firmware rebuild.
- JSON protocol and Wi‑Fi integration assume a simple line-based UART conversation with an ESP-like module; reliability and connection management are minimal in the prototype.
- Sensor calibration: EC/soil moisture/lux conversions use simple linear or lookup heuristics;
- RAM-only history: measurements are stored in RAM during a session (no guaranteed persistence after power loss) — extend with flash/EEPROM if persistent history is required.

