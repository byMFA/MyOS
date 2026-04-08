#include "process.h"
#include "vga.h"

// =====================================================
// ROUND ROBIN SCHEDULER
// =====================================================
// Temel fikir: Her sürece eşit zaman dilimi (time slice) ver.
// Süre dolunca bir sonraki sürece geç. Döngüsel kuyruk.

static const int MAX_PROCESSES  = 16;
static const int DEFAULT_SLICE  = 5;   // Her süreç kaç tick çalışır

// Tüm PCB'ler statik dizide tutulur (dinamik bellek yok)
static PCB process_table[MAX_PROCESSES];
static int process_count  = 0;
static int current_index  = -1; // Şu an çalışan sürecin indeksi
static int next_pid       = 1;

// ---- Yardımcı: string kopyala (stdlib yok) ----
static void str_copy(char* dst, const char* src, int max) {
    int i = 0;
    while (src[i] && i < max - 1) { dst[i] = src[i]; i++; }
    dst[i] = '\0';
}

// ---- Yeni süreç oluştur ----
// Gerçek OS'ta burada sayfa tablosu kurulur, yığın ayrılır.
// Biz simülasyonda sadece PCB'yi dolduruyoruz.
int process_create(const char* name, int priority) {
    if (process_count >= MAX_PROCESSES) return -1;

    PCB& p = process_table[process_count];
    p.pid        = next_pid++;
    p.priority   = priority;
    p.state      = ProcessState::READY;
    p.time_slice = DEFAULT_SLICE;
    p.total_ticks = 0;
    p.stack_base = 0; // Simülasyonda gerçek adres yok
    p.stack_size = 4096;
    p.next       = nullptr;
    str_copy(p.name, name, 32);

    // CPU context sıfırla
    p.context = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    process_count++;
    return p.pid;
}

// ---- Round Robin: Bir sonraki süreci seç ----
// Döngüsel olarak READY olan bir sonraki süreci döndürür.
// Tüm süreçler FINISHED ise -1 döner.
int scheduler_next() {
    if (process_count == 0) return -1;

    // Mevcut süreci READY'e al (bitmemişse)
    if (current_index >= 0) {
        PCB& cur = process_table[current_index];
        if (cur.state == ProcessState::RUNNING)
            cur.state = ProcessState::READY;
    }

    // Sıradaki READY süreci bul (döngüsel)
    int start = (current_index + 1) % process_count;
    for (int i = 0; i < process_count; i++) {
        int idx = (start + i) % process_count;
        if (process_table[idx].state == ProcessState::READY) {
            current_index = idx;
            process_table[idx].state      = ProcessState::RUNNING;
            process_table[idx].time_slice = DEFAULT_SLICE;
            return process_table[idx].pid;
        }
    }
    return -1; // Çalışacak süreç yok
}

// ---- Simülasyon: Bir "tick" ilerlet ----
// Gerçek OS'ta bu timer interrupt ile tetiklenir.
// Simülasyonda manuel çağırıyoruz.
void scheduler_tick() {
    if (current_index < 0) return;
    PCB& cur = process_table[current_index];
    if (cur.state != ProcessState::RUNNING) return;

    cur.total_ticks++;
    cur.time_slice--;

    if (cur.time_slice <= 0) {
        // Zaman doldu → bir sonraki sürece geç
        scheduler_next();
    }
}

// ---- Süreci bitir ----
void process_finish(int pid) {
    for (int i = 0; i < process_count; i++) {
        if (process_table[i].pid == pid) {
            process_table[i].state = ProcessState::FINISHED;
            return;
        }
    }
}

// ---- Durum tablosunu ekrana yazdır ----
void scheduler_print_status() {
    VGA::set_color(VGA::YELLOW, VGA::BLACK);
    VGA::print("\n=== PROCESS TABLE ===\n");
    VGA::set_color(VGA::WHITE, VGA::BLACK);
    VGA::print("PID  NAME             PRIORITY  STATE       TICKS\n");
    VGA::print("---  ---------------  --------  ----------  -----\n");

    const char* state_names[] = {"READY   ", "RUNNING ", "BLOCKED ", "FINISHED"};

    for (int i = 0; i < process_count; i++) {
        PCB& p = process_table[i];

        // Çalışan süreci yeşil göster
        if (p.state == ProcessState::RUNNING)
            VGA::set_color(VGA::LIGHT_GREEN, VGA::BLACK);
        else if (p.state == ProcessState::FINISHED)
            VGA::set_color(VGA::DARK_GREY, VGA::BLACK);
        else
            VGA::set_color(VGA::WHITE, VGA::BLACK);

        VGA::print_int(p.pid);
        VGA::print("    ");
        VGA::print(p.name);
        VGA::print("  ");
        VGA::print_int(p.priority);
        VGA::print("         ");
        VGA::print(state_names[(int)p.state]);
        VGA::print("  ");
        VGA::print_int(p.total_ticks);
        VGA::print("\n");
    }
    VGA::set_color(VGA::WHITE, VGA::BLACK);
}
