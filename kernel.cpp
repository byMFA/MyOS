#include "vga.h"
#include "memory.h"
#include "process.h"

extern int  process_create(const char* name, int priority);
extern int  scheduler_next();
extern void scheduler_tick();
extern void process_finish(int pid);
extern void scheduler_print_status();
extern void memory_init();
extern int  memory_alloc_block();
extern void memory_free_block(int index);
extern void memory_print_status();

struct MultibootHeader {
    unsigned int magic;
    unsigned int flags;
    unsigned int checksum;
};

__attribute__((section(".multiboot")))
volatile MultibootHeader multiboot_header = {
    0x1BADB002,
    0x0,
    (unsigned int)(-(0x1BADB002 + 0x0))
};

static void print_banner() {
    VGA::set_color(VGA::LIGHT_CYAN, VGA::BLACK);
    VGA::print("  __  __       ___  ____  \n");
    VGA::print(" |  \\/  |_   _/ _ \\/ ___| \n");
    VGA::print(" | |\\/| | | | | | |\\___ \\ \n");
    VGA::print(" | |  | | |_| | |_| |___) |\n");
    VGA::print(" |_|  |_|\\__, |\\___/|____/ \n");
    VGA::print("          |___/             \n");
    VGA::set_color(VGA::YELLOW, VGA::BLACK);
    VGA::print("\n  MyOS v1.0 - C++ Mini Kernel\n");
    VGA::print("  OS Tasarimi Projesi 2025\n");
    VGA::set_color(VGA::WHITE, VGA::BLACK);
    VGA::print("  ==============================\n\n");
}

extern "C" void kernel_main() {
    VGA::clear();
    print_banner();

    VGA::set_color(VGA::LIGHT_GREEN, VGA::BLACK);
    VGA::print("[BOOT] Sistem baslatiliyor...\n\n");
    VGA::set_color(VGA::WHITE, VGA::BLACK);

    VGA::print("[1/3] Bellek yonetimi... ");
    memory_init();
    VGA::set_color(VGA::LIGHT_GREEN, VGA::BLACK);
    VGA::print("OK\n");
    VGA::set_color(VGA::WHITE, VGA::BLACK);

    int b1 = memory_alloc_block();
    int b2 = memory_alloc_block();
    int b3 = memory_alloc_block();
    memory_free_block(b2);
    (void)b1; (void)b3;
    memory_print_status();

    VGA::print("[2/3] Surecler olusturuluyor... ");
    process_create("init",       1);
    process_create("scheduler",  2);
    process_create("driver_mgr", 3);
    process_create("user_shell", 1);
    VGA::set_color(VGA::LIGHT_GREEN, VGA::BLACK);
    VGA::print("OK\n");
    VGA::set_color(VGA::WHITE, VGA::BLACK);

    VGA::print("[3/3] Round Robin scheduler... ");
    VGA::set_color(VGA::LIGHT_GREEN, VGA::BLACK);
    VGA::print("OK\n\n");
    VGA::set_color(VGA::WHITE, VGA::BLACK);

    scheduler_next();
    for (int tick = 0; tick < 20; tick++) {
        scheduler_tick();
        if (tick == 4) process_finish(2);
    }

    scheduler_print_status();

    VGA::set_color(VGA::LIGHT_CYAN, VGA::BLACK);
    VGA::print("\n[KERNEL] Tum sistemler aktif. MyOS hazir!\n");
    VGA::set_color(VGA::WHITE, VGA::BLACK);

    while (true) {}
}
