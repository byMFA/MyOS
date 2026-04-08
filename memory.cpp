#include "memory.h"
#include "vga.h"

// Global bellek haritası tanımı
MemoryMap g_mem_map;

// ---- Bitmap yardımcıları ----

// Belirli bloğu dolu işaretle
static void bitmap_set(int index) {
    g_mem_map.bitmap[index / 8] |= (1 << (index % 8));
}

// Belirli bloğu boş işaretle
static void bitmap_clear(int index) {
    g_mem_map.bitmap[index / 8] &= ~(1 << (index % 8));
}

// Belirli bloğun durumunu oku (1: dolu, 0: boş)
static int bitmap_test(int index) {
    return (g_mem_map.bitmap[index / 8] >> (index % 8)) & 1;
}

// ---- Belleği başlat ----
// İlk 16 blok (64 KB) çekirdek için ayrılmış kabul edilir.
void memory_init() {
    // Tüm bitmap'i sıfırla (hepsi boş)
    for (int i = 0; i < (int)BITMAP_SIZE; i++)
        g_mem_map.bitmap[i] = 0;

    g_mem_map.total_blocks = TOTAL_BLOCKS;
    g_mem_map.used_blocks  = 0;

    // Çekirdek alanını rezerve et (ilk 16 blok = 64 KB)
    for (int i = 0; i < 16; i++) {
        bitmap_set(i);
        g_mem_map.used_blocks++;
    }
}

// ---- 1 blok ayır ----
// İlk boş bloğu bulur, işaretler ve indeksi döner.
// Başarısız olursa -1 döner.
int memory_alloc_block() {
    for (int i = 0; i < (int)TOTAL_BLOCKS; i++) {
        if (!bitmap_test(i)) {
            bitmap_set(i);
            g_mem_map.used_blocks++;
            return i;
        }
    }
    return -1; // Bellek dolu
}

// ---- Bloğu serbest bırak ----
void memory_free_block(int block_index) {
    if (block_index < 0 || block_index >= (int)TOTAL_BLOCKS) return;
    if (!bitmap_test(block_index)) return; // Zaten boş

    bitmap_clear(block_index);
    g_mem_map.used_blocks--;
}

// ---- Boş blok sayısı ----
uint32_t memory_free_count() {
    return g_mem_map.total_blocks - g_mem_map.used_blocks;
}

// ---- Durumu ekrana yaz ----
void memory_print_status() {
    VGA::set_color(VGA::YELLOW, VGA::BLACK);
    VGA::print("\n=== MEMORY MAP ===\n");
    VGA::set_color(VGA::WHITE, VGA::BLACK);

    VGA::print("Toplam bellek : ");
    VGA::print_int(MEM_SIZE / 1024);
    VGA::print(" KB  (");
    VGA::print_int(TOTAL_BLOCKS);
    VGA::print(" blok x 4KB)\n");

    VGA::print("Kullanilan    : ");
    VGA::print_int(g_mem_map.used_blocks);
    VGA::print(" blok (");
    VGA::print_int(g_mem_map.used_blocks * BLOCK_SIZE / 1024);
    VGA::print(" KB)\n");

    VGA::print("Bos           : ");
    VGA::print_int(memory_free_count());
    VGA::print(" blok (");
    VGA::print_int(memory_free_count() * BLOCK_SIZE / 1024);
    VGA::print(" KB)\n");

    // Görsel bitmap (ilk 64 blok, her '#' dolu, '.' boş)
    VGA::print("\nBitmap (ilk 64 blok): [");
    for (int i = 0; i < 64; i++) {
        if (bitmap_test(i)) {
            VGA::set_color(VGA::LIGHT_RED, VGA::BLACK);
            VGA::putchar('#');
        } else {
            VGA::set_color(VGA::LIGHT_GREEN, VGA::BLACK);
            VGA::putchar('.');
        }
    }
    VGA::set_color(VGA::WHITE, VGA::BLACK);
    VGA::print("]\n");
    VGA::print("  # = dolu (used)   . = bos (free)\n");
}
