#undef UNICODE
#undef _UNICODE

#pragma comment(lib, "ws2_32.lib")

#include <winsock2.h>
#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#define BOLTS_ADDR      ((uintptr_t)0x300969CA0ULL)
#define BOMB_GLOVE_ADDR ((uintptr_t)0x30096C0D7ULL)

struct {
    const char* cyan;
    const char* green;
    const char* red;
    const char* yellow;
    const char* def;
    const char* clear;
} ansi = {
    .cyan   = "\033[0;36m",
    .green  = "\033[0;32m",
    .red    = "\033[0;31m",
    .yellow = "\033[0;33m",
    .def    = "\033[0m",
    .clear  = "\e[1;1H\e[2J"
};

uint32_t bolts_gained = 0;

DWORD rpcs3_pid = 0;
const char* target = "rpcs3.exe";

int find_pids(const char* target_name) {
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "%sCreateToolhelp32Snapshot failed (err %lu)%s\n", ansi.red, GetLastError(), ansi.def);
        return 0;
    }

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(pe);

    if (!Process32First(snap, &pe)) {
        fprintf(stderr, "%sProcess32First failed (err %lu)%s\n", ansi.red, GetLastError(), ansi.def);
        CloseHandle(snap);
        return 0;
    }

    int found = 0;
    do {
        if (_stricmp(pe.szExeFile, target_name) == 0) {
            printf("%sFound: %s PID: %lu%s\n", ansi.green, pe.szExeFile, (unsigned long)pe.th32ProcessID, ansi.def);
            rpcs3_pid = pe.th32ProcessID;
            found++;
            break;
        }
    } while (Process32Next(snap, &pe));

    CloseHandle(snap);

    return found > 0;
}

int main(void) {
    printf("Ratchad, made by Caleb Blackler\n\n");

    if (!find_pids(target)) {
        fprintf(stderr, "%sCould not find rpcs3.exe. Make sure it's running. (err %lu)%s\n", ansi.red, GetLastError(), ansi.def);
        system("PAUSE");
        return 1;
    }

    printf("%sAttempting to attach Ratchad to PID %lu...%s\n", ansi.yellow, (unsigned long)rpcs3_pid, ansi.def);

    HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION, FALSE, rpcs3_pid);
    if (!hProcess) {
        fprintf(stderr, "%sRatchad failed to attach! (err %lu)%s\n", ansi.red, GetLastError(), ansi.def);
        return 1;
    }

    printf("%sRatchad successfully attached PID %lu!%s\n\n", ansi.green, (unsigned long)rpcs3_pid, ansi.def);
    printf(ansi.clear);
    printf("Live bolt data (200ms). Press Ctrl+C to exit.\n");

    while (1) {
        uint32_t raw_bolts = 0, raw_bomb = 0;
        SIZE_T bytesRead = 0;

        // read bolts
        if (!ReadProcessMemory(hProcess, (LPCVOID)BOLTS_ADDR, &raw_bolts, sizeof(raw_bolts), &bytesRead) || bytesRead != sizeof(raw_bolts)) {
            fprintf(stderr, "\n%sFailed reading bolts (err %lu)%s\n", ansi.red, GetLastError(), ansi.def);
            Sleep(200);
            continue;
        }

        // read bomb glove ammo
        if (!ReadProcessMemory(hProcess, (LPCVOID)BOMB_GLOVE_ADDR, &raw_bomb, sizeof(raw_bomb), &bytesRead) || bytesRead != sizeof(raw_bomb)) {
            fprintf(stderr, "\n%sFailed reading bomb glove ammo (err %lu)%s\n", ansi.red, GetLastError(), ansi.def);
            Sleep(200);
            continue;
        }

        uint32_t bolts_le = ntohl(raw_bolts); // convert bolts to little-endiaan
        uint32_t ammo = raw_bomb;

        // convert ammo to bolts
        if (ammo > 0) {
            uint32_t add = ammo * 5;
            bolts_le += add;

            // write updated bolts
            uint32_t new_be_bolts = htonl(bolts_le);
            SIZE_T bytesWritten = 0;
            WriteProcessMemory(hProcess, (LPVOID)BOLTS_ADDR, &new_be_bolts, sizeof(new_be_bolts), &bytesWritten);

            // set bomb ammo to zero
            uint32_t zero = 0;
            WriteProcessMemory(hProcess, (LPVOID)BOMB_GLOVE_ADDR, &zero, sizeof(zero), &bytesWritten);

            bolts_gained = bolts_gained += ammo * 5;
            ammo = 0;
        }

        printf("Raw Bolts: %s%08X%s | Actual: %s%u%s | Bolts Gained from Ammo: %s%u%s\r",
            ansi.cyan, raw_bolts, ansi.def,
            ansi.green, bolts_le, ansi.def,
            ansi.yellow, bolts_gained, ansi.def);

        fflush(stdout);
        Sleep(200);
    }
    CloseHandle(hProcess);
    system("PAUSE");
    return 0;
}
