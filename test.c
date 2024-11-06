#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define I2C_PORT i2c0
#define OLED_ADDR 0x3C
#define OLED_WIDTH 128
#define OLED_HEIGHT 64

#define OLED_CMD 0x00
#define OLED_DATA 0x40

#define LOCHAR 65
#define HICHAR 90
#define FONT_BWIDTH 1
#define FONT_HEIGHT 8

// Definición de la fuente para caracteres A-Z
const char FONT[HICHAR - LOCHAR + 1][FONT_HEIGHT * FONT_BWIDTH] = {
    { 0xC0,0x30,0x2C,0x30,0xC0,0x00,0x00,0x00 }, // A
    { 0xFC,0x94,0x94,0x94,0x68,0x00,0x00,0x00 }, // B
    { 0x78,0x84,0x84,0x84,0x48,0x00,0x00,0x00 }, // C
    { 0xFC,0x84,0x84,0x48,0x30,0x00,0x00,0x00 }, // D
    { 0xFC,0x94,0x94,0x84,0x84,0x00,0x00,0x00 }, // E
    { 0xFC,0x14,0x14,0x04,0x04,0x00,0x00,0x00 }, // F
    { 0x78,0x84,0xA4,0xA4,0x60,0x00,0x00,0x00 }, // G
    { 0xFC,0x10,0x10,0x10,0xFC,0x00,0x00,0x00 }, // H
    { 0x00,0x84,0xFC,0x84,0x00,0x00,0x00,0x00 }, // I
    { 0x60,0x80,0x84,0x84,0x7C,0x00,0x00,0x00 }, // J
    { 0xFC,0x20,0x30,0x48,0x84,0x00,0x00,0x00 }, // K
    { 0xFC,0x80,0x80,0x80,0x80,0x00,0x00,0x00 }, // L
    { 0xFC,0x08,0x10,0x08,0xFC,0x00,0x00,0x00 }, // M
    { 0xFC,0x08,0x10,0x20,0xFC,0x00,0x00,0x00 }, // N
    { 0x78,0x84,0x84,0x84,0x78,0x00,0x00,0x00 }, // O
    { 0xFC,0x24,0x24,0x24,0x18,0x00,0x00,0x00 }, // P
    { 0x78,0x84,0x84,0x44,0xB8,0x00,0x00,0x00 }, // Q
    { 0xFC,0x14,0x14,0x14,0xE8,0x00,0x00,0x00 }, // R
    { 0x48,0x94,0x94,0x94,0x60,0x00,0x00,0x00 }, // S
    { 0x04,0x04,0xFC,0x04,0x04,0x00,0x00,0x00 }, // T
    { 0x7C,0x80,0x80,0x80,0x7C,0x00,0x00,0x00 }, // U
    { 0x1C,0x60,0x80,0x60,0x1C,0x00,0x00,0x00 }, // V
    { 0x3C,0xC0,0x70,0xC0,0x3C,0x00,0x00,0x00 }, // W
    { 0xC4,0x28,0x10,0x28,0xC4,0x00,0x00,0x00 }, // X
    { 0x0C,0x10,0xE0,0x10,0x0C,0x00,0x00,0x00 }, // Y
    { 0xC4,0xA4,0x94,0x8C,0x84,0x00,0x00,0x00 }, // Z
};

// Inicializar I2C para la pantalla OLED
void i2c_init_oled() {
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(4, GPIO_FUNC_I2C);
    gpio_set_function(5, GPIO_FUNC_I2C);
    gpio_pull_up(4);
    gpio_pull_up(5);
}

// Enviar comando al OLED
void oled_send_command(uint8_t cmd) {
    uint8_t data[2] = {OLED_CMD, cmd};
    i2c_write_blocking(I2C_PORT, OLED_ADDR, data, 2, false);
}

// Inicializar el OLED con la configuración básica
void oled_init() {
    uint8_t init_sequence[] = {0xAE, 0x20, 0x00, 0xB0, 0xC8, 0x00, 0x10, 0x40, 0x81, 0xFF, 
                               0xA1, 0xA6, 0xA8, 0x3F, 0xA4, 0xD3, 0x00, 0xD5, 0xF0, 0xD9, 
                               0x22, 0xDA, 0x12, 0xDB, 0x20, 0x8D, 0x14, 0xAF};
    for (int i = 0; i < sizeof(init_sequence); i++) {
        oled_send_command(init_sequence[i]);
    }
}

// Dibujar carácter en la posición dada
void oled_draw_char(uint8_t page, uint8_t col, char c) {
    if (c < LOCHAR || c > HICHAR) return; // Validar rango de caracteres
    oled_send_command(0xB0 + page);
    oled_send_command(0x00 + (col & 0x0F));
    oled_send_command(0x10 + (col >> 4));
    for (int i = 0; i < FONT_HEIGHT; i++) {
        uint8_t data[2] = {OLED_DATA, FONT[c - LOCHAR][i]};
        i2c_write_blocking(I2C_PORT, OLED_ADDR, data, 2, false);
    }
}

// Dibujar texto en una posición dada
void oled_draw_text(uint8_t page, uint8_t start_col, const char* text) {
    for (int i = 0; text[i] != '\0'; i++) {
        oled_draw_char(page, start_col + (i * FONT_BWIDTH * 8), text[i]);
    }
}

int main() {
    stdio_init_all();
    i2c_init_oled();
    oled_init();

    // Escribir el nombre en el OLED
    const char* line1 = "DIEGO";
    oled_draw_text(0, 0, line1);  // Primera línea

    const char* line2 = "HERNANDEZ";
    oled_draw_text(1, 0, line2);  // Segunda línea

    const char* line3 = "GOMEZ";
    oled_draw_text(2, 0, line3);  // Tercera línea

    return 0;
}
