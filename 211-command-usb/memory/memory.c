#include "memory.h"
#include "command.h"
#include <stdlib.h>
#include "device.h"

extern char __flash_binary_start;
extern char __flash_binary_end;
extern char __boot2_start__;
extern char __boot2_end__;
extern char __etext;
extern char __data_start__;
extern char __data_end__;
extern char __bss_start__;
extern char __bss_end__;
extern char __HeapLimit;
extern char __StackBottom;
extern char __StackTop;


static void row(const char *name, uintptr_t start, uintptr_t end)
{
    printf("%-10s 0x%08x 0x%08x %8u\n",
           name, (unsigned)start, (unsigned)end, (unsigned)(end - start));
}


static void row_for_commands_with_value(const char *name, uint16_t* start)
{
    printf("%-10s 0x%08x 0x%08x\n", 
        name, (unsigned)start, (unsigned)*start);
}

static void row_for_commands_without_value(const char *name, uint16_t* start)
{
    printf("%-10s 0x%08x\n", 
        name, (unsigned)start);
}

uint32_t data_variable = 100;
uint32_t bss_variable;

void fw_info(void)
{
    
    int main(void);
    uint16_t *main_code = (uint16_t *)((uintptr_t)main & ~1u); 
    uint16_t *fw_info_code = (uint16_t *)((uintptr_t)fw_info & ~1u); 
    data_variable = data_variable + 1;
    bss_variable = bss_variable + 1;
    
    uint32_t stack_variable = 1946;
    uint32_t *heap_variable = malloc(sizeof(uint32_t));
    
    if (heap_variable != NULL)
    {
        *heap_variable = 1951;
    }
    
    
    printf("%-10s %-10s %-10s\n", "object", "adress", "value");
    printf("%-10s 0x%08x 0x%08x\n", "main", (unsigned)(uint16_t *)((uintptr_t)main), (unsigned)*main_code);
    printf("%-10s 0x%08x 0x%08x\n", "fw_info", (unsigned)(uint16_t *)((uintptr_t)fw_info), *fw_info_code);
    printf("%-10s 0x%08x\n", "commands", commands);
    for (uint i = 0; i < command_count; i++)
    {
        printf("-  %-10s 0x%08x\n",commands[i].name, commands[i].handler);
    }
    
    printf("%-10s 0x%08x %s\n", "DEVICE_PROJECT", device_project, device_project);
    printf("%-10s 0x%08x %s\n", "DEVICE_BOARD", device_board, device_board);
    printf("%-10s 0x%08x %u\n", "data_variable", &data_variable, data_variable);
    printf("%-10s 0x%08x %u\n", "bss_variable", &bss_variable, bss_variable);
    printf("%-10s 0x%08x %u\n", "stack_variable", &stack_variable, stack_variable);
    printf("%-10s 0x%08x %u\n", "heap_variable", heap_variable, *heap_variable);
    free(heap_variable);
}

int main(void);

void mem_info(void)
{
    // шапка таблицы: область, начало, конец, размер
    printf("%-10s %-10s %-10s %-8s\n", "area", "start", "end", "size");

    // flash — XIP_BASE и PICO_FLASH_SIZE_BYTES
    row("flash", XIP_BASE, (XIP_BASE + PICO_FLASH_SIZE_BYTES));

    // sram — базовый адрес из SDK, размер из datasheet
    row("sram", SRAM_BASE, SRAM_END);
    // rom — базовый адрес из SDK, размер из datasheet
    const uint32_t ROM_SIZE = 16384;
    row("rom", ROM_BASE, ROM_BASE + ROM_SIZE);

    // image — от __flash_binary_start до __flash_binary_end
    row("image", (uintptr_t)&__flash_binary_start, (uintptr_t)&__flash_binary_end);
    // free  — от __flash_binary_end до конца флеш-памяти
    row("free", (uintptr_t)&__flash_binary_end, ((XIP_BASE + PICO_FLASH_SIZE_BYTES)));
    // boot2 — от __boot2_start__ до __boot2_end__
    row("boot2", (uintptr_t)&__boot2_start__, (uintptr_t)&__boot2_end__);
    // text  — от __boot2_end__ до __etext: код и константы
    row("text", (uintptr_t)&__boot2_end__, (uintptr_t)&__etext);

    // data flash — хранение .data, от __etext, длиной с .data
    row("data flash", (uintptr_t)&__etext, (uintptr_t)(&__etext + (&__data_end__ - &__data_start__)));
    // data ram   — работа .data, от __data_start__ до __data_end__
    row("data ram", (uintptr_t)&__data_start__, (uintptr_t)&__data_end__);
    // bss        — от __bss_start__ до __bss_end__
    row("bss", (uintptr_t)&__bss_start__, (uintptr_t)&__bss_end__);
    // heap       — от __bss_end__ до __HeapLimit
    row("heap", (uintptr_t)&__bss_end__, (uintptr_t)&__HeapLimit);
    // stack      — от __StackBottom до __StackTop
    row("stack", (uintptr_t)&__StackBottom, (uintptr_t)&__StackTop);
    const uint32_t boot2_size = (unsigned)(&__boot2_end__ - &__boot2_start__);
    const uint32_t text_size = (unsigned)(&__etext - &__boot2_end__);
    const uint32_t data_size = (unsigned)(&__data_end__ - &__data_start__);
    const uint32_t flash_image_size = boot2_size + text_size + data_size;
    const uint32_t bss_size = (unsigned)(&__bss_end__ - &__bss_start__);
    const uint32_t ram_busy = bss_size + data_size;
    const uint32_t stack_size = (unsigned)(&__StackTop - &__StackBottom);
    const uint32_t heap_free_size = (unsigned)(&__HeapLimit - &__bss_end__);
    printf("\ntotal\n");
    printf("    flash image   %8u = boot2 %8u + text %8u + data %8u\n", flash_image_size, boot2_size, text_size, data_size);
    printf("    flash free  %8u of %8u\n", PICO_FLASH_SIZE_BYTES - flash_image_size, PICO_FLASH_SIZE_BYTES);
    printf("    ram used  %8u = data %8u + bss %8u \n", ram_busy, data_size, bss_size);
    printf("    ram free  %8u for heap and %8u for stack\n", heap_free_size, stack_size);
    
    // итог: образ во флеш и из чего он сложился
    // итог: свободно во флеш-памяти из всего её объёма
    // итог: занято в ОЗУ — .data и .bss
    // итог: свободно в ОЗУ — под кучу и под стек
}