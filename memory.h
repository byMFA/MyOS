#pragma once
#include <stdint.h>

// =====================================================
// BELLEK YÖNETİMİ - Basit Bitmap Allocator
// =====================================================
// Gerçek OS'ta paging + page table kurulur.
// Biz 1 MB'lık simüle edilmiş belleği 4KB'lık
// bloklara bölerek bitmap ile yönetiyoruz.
//
// Bitmap: Her bit bir bloğu temsil eder.
//   0 = boş (free)
//   1 = dolu (used)

static const uint32_t MEM_SIZE       = 1024 * 1024; // 1 MB simüle bellek
static const uint32_t BLOCK_SIZE     = 4096;         // 4 KB her blok (1 sayfa)
static const uint32_t TOTAL_BLOCKS   = MEM_SIZE / BLOCK_SIZE; // 256 blok
static const uint32_t BITMAP_SIZE    = TOTAL_BLOCKS / 8;      // 32 byte bitmap

// Bellek haritası
struct MemoryMap {
    uint8_t  bitmap[BITMAP_SIZE]; // Hangi bloklar dolu?
    uint32_t total_blocks;        // Toplam blok sayısı
    uint32_t used_blocks;         // Kullanılan blok sayısı
};

// Global bellek haritası
extern MemoryMap g_mem_map;

// ---- API ----
void     memory_init();                        // Belleği başlat
int      memory_alloc_block();                 // 1 blok ayır → blok indeksi döner (-1: başarısız)
void     memory_free_block(int block_index);   // Bloğu serbest bırak
void     memory_print_status();                // Durumu ekrana yaz
uint32_t memory_free_count();                  // Boş blok sayısı
