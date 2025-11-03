// memory read and write functions to keep main.c tidy

#include "../headers/includes.h"
#include "../headers/mem.h"

uint32_t read_u32(HANDLE hProcess, uintptr_t addr) {
    uint32_t value = 0;
    SIZE_T bytes_read = 0;
    ReadProcessMemory(hProcess, (LPCVOID)addr, &value, sizeof(value), &bytes_read);
    return value;
}

BOOL write_u32(HANDLE hProcess, uintptr_t addr, uint32_t value) {
    SIZE_T bytes_written = 0;
    return WriteProcessMemory(hProcess, (LPVOID)addr, &value, sizeof(value), &bytes_written);
}