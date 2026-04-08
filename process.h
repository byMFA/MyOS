#pragma once
#include <stdint.h>

// =====================================================
// PROCESS MANAGEMENT - PCB ve Round Robin Scheduler
// =====================================================

// Süreç durumları
enum class ProcessState {
    READY,    // Çalışmaya hazır
    RUNNING,  // Şu an çalışıyor
    BLOCKED,  // Bekliyor (I/O vs.)
    FINISHED  // Tamamlandı
};

// CPU register durumu (context switch için)
struct CPUContext {
    uint32_t eax, ebx, ecx, edx;
    uint32_t esp, ebp, esi, edi;
    uint32_t eip;    // Instruction pointer
    uint32_t eflags;
};

// Process Control Block (PCB)
// Her sürecin kimliği ve durumu bu yapıda tutulur
struct PCB {
    int          pid;           // Süreç ID
    int          priority;      // Öncelik (1-10, şu an gösterim için)
    ProcessState state;         // Sürecin durumu
    CPUContext   context;       // CPU kayıt durumu
    uint32_t     stack_base;    // Yığın başlangıcı
    uint32_t     stack_size;    // Yığın boyutu (byte)
    int          time_slice;    // Kalan zaman dilimi (tick)
    int          total_ticks;   // Toplam çalışma süresi
    char         name[32];      // Süreç adı

    PCB* next; // Bağlı liste için sonraki PCB
};
