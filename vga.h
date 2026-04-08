#pragma once
#include <stdint.h>

// VGA text modunda ekrana yazmak için basit sürücü
// Ekran: 80 sütun x 25 satır

namespace VGA {

    // Renkler
    enum Color : uint8_t {
        BLACK = 0, BLUE, GREEN, CYAN, RED,
        MAGENTA, BROWN, LIGHT_GREY, DARK_GREY,
        LIGHT_BLUE, LIGHT_GREEN, LIGHT_CYAN,
        LIGHT_RED, LIGHT_MAGENTA, YELLOW, WHITE
    };

    static const int WIDTH  = 80;
    static const int HEIGHT = 25;

    // VGA bellek adresi (donanım sabiti)
    static volatile uint16_t* const BUFFER = (uint16_t*)0xB8000;

    static int cursor_x = 0;
    static int cursor_y = 0;
    static uint8_t current_color = (BLACK << 4) | WHITE; // beyaz yazı, siyah arka plan

    // Renk baytı oluştur
    inline uint8_t make_color(Color fg, Color bg) {
        return (uint8_t)bg << 4 | (uint8_t)fg;
    }

    // Tek karakter yaz (belirli konuma)
    inline void put_char_at(char c, int x, int y, uint8_t color) {
        BUFFER[y * WIDTH + x] = (uint16_t)c | ((uint16_t)color << 8);
    }

    // Ekranı temizle
    inline void clear() {
        for (int y = 0; y < HEIGHT; y++)
            for (int x = 0; x < WIDTH; x++)
                put_char_at(' ', x, y, current_color);
        cursor_x = 0;
        cursor_y = 0;
    }

    // Bir satır kaydır (scroll)
    inline void scroll() {
        for (int y = 0; y < HEIGHT - 1; y++)
            for (int x = 0; x < WIDTH; x++)
                BUFFER[y * WIDTH + x] = BUFFER[(y+1) * WIDTH + x];
        for (int x = 0; x < WIDTH; x++)
            put_char_at(' ', x, HEIGHT - 1, current_color);
        cursor_y = HEIGHT - 1;
    }

    // Tek karakter yaz (imleci ilerlet)
    inline void putchar(char c) {
        if (c == '\n') {
            cursor_x = 0;
            cursor_y++;
        } else if (c == '\r') {
            cursor_x = 0;
        } else {
            put_char_at(c, cursor_x, cursor_y, current_color);
            cursor_x++;
            if (cursor_x >= WIDTH) {
                cursor_x = 0;
                cursor_y++;
            }
        }
        if (cursor_y >= HEIGHT) scroll();
    }

    // String yaz
    inline void print(const char* str) {
        for (int i = 0; str[i] != '\0'; i++)
            putchar(str[i]);
    }

    // Sayı yaz (decimal)
    inline void print_int(int n) {
        if (n < 0) { putchar('-'); n = -n; }
        if (n == 0) { putchar('0'); return; }
        char buf[12];
        int i = 0;
        while (n > 0) { buf[i++] = '0' + (n % 10); n /= 10; }
        for (int j = i - 1; j >= 0; j--) putchar(buf[j]);
    }

    // Renk değiştir
    inline void set_color(Color fg, Color bg) {
        current_color = make_color(fg, bg);
    }

} // namespace VGA
