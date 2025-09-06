#include "oled_module.h"
#include "chprintf.h"

//static char buff[OLED_BUFF_SIZE];

/* Configurazione I2C */
static const I2CConfig i2ccfg = {
  STM32_TIMINGR_PRESC(8U)  |
  STM32_TIMINGR_SCLDEL(3U) | STM32_TIMINGR_SDADEL(3U) |
  STM32_TIMINGR_SCLH(3U)   | STM32_TIMINGR_SCLL(9U),
  0,
  0
};

/* Configurazione SSD1306 */
static const SSD1306Config ssd1306cfg = {
  &I2CD1,
  &i2ccfg,
  SSD1306_SAD_0X78,
};

static SSD1306Driver SSD1306D1;

void oled_init(void) {
    /* Configurazione pin I2C PB8 e PB9 */
    palSetLineMode(PAL_LINE(GPIOB, 8U), PAL_MODE_ALTERNATE(4) |
                   PAL_STM32_OTYPE_OPENDRAIN | PAL_STM32_OSPEED_HIGHEST |
                   PAL_STM32_PUPDR_PULLUP);
    palSetLineMode(PAL_LINE(GPIOB, 9U), PAL_MODE_ALTERNATE(4) |
                   PAL_STM32_OTYPE_OPENDRAIN | PAL_STM32_OSPEED_HIGHEST |
                   PAL_STM32_PUPDR_PULLUP);
}

void oled_print_string(const char *str, uint8_t col, uint8_t row) {
    ssd1306ObjectInit(&SSD1306D1);
    ssd1306Start(&SSD1306D1, &ssd1306cfg);
    ssd1306FillScreen(&SSD1306D1, 0x00);

    static char buff1[OLED_BUFF_SIZE];
    ssd1306GotoXy(&SSD1306D1, col, row);
    chsnprintf(buff1, OLED_BUFF_SIZE, "%s", str);
    ssd1306Puts(&SSD1306D1, buff1, &ssd1306_font_11x18, SSD1306_COLOR_WHITE);
    ssd1306UpdateScreen(&SSD1306D1);
}

void oled_print_double_string(const char *str1, const char *str2, uint8_t col1, uint8_t row1) {
    ssd1306ObjectInit(&SSD1306D1);

    // Avvia il driver
    ssd1306Start(&SSD1306D1, &ssd1306cfg);

    // Pulisce lo schermo
    ssd1306FillScreen(&SSD1306D1, 0x00);

    static char buff3[OLED_BUFF_SIZE];
    static char buff2[OLED_BUFF_SIZE];

    // Stampa la prima stringa alla riga 1
    ssd1306GotoXy(&SSD1306D1, col1, row1);
    chsnprintf(buff3, OLED_BUFF_SIZE, "%s", str1);
    ssd1306Puts(&SSD1306D1, buff3, &ssd1306_font_11x18, SSD1306_COLOR_WHITE);

    // Calcola la seconda riga (in base all'altezza del font 18 px)
    uint8_t row2 = row1 + 20;

    // Stampa la seconda stringa alla riga 2
    ssd1306GotoXy(&SSD1306D1, col1, row2);
    chsnprintf(buff2, OLED_BUFF_SIZE, "%s", str2);
    ssd1306Puts(&SSD1306D1, buff2, &ssd1306_font_11x18, SSD1306_COLOR_WHITE);

    // Aggiorna lo schermo
    ssd1306UpdateScreen(&SSD1306D1);
}

