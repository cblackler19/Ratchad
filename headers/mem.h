#ifndef MEM_H
#define MEM_H

#include <Windows.h>
#include <stdint.h>

uint32_t read_u32(HANDLE hProcess, uintptr_t addr);
BOOL write_u32(HANDLE hProcess, uintptr_t addr, uint32_t value);

#endif