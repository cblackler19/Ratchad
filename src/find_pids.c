#include "../headers/find_pids.h"
#include "../headers/defines.h"
#include "../headers/includes.h"

DWORD rpcs3_pid = 0;

int find_pids(const char* target_name) {
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "%sCreateToolhelp32Snapshot failed (err %lu)%s\n", RED, GetLastError(), DEF);
        return -1;
    }

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(pe);

    if (!Process32First(snap, &pe)) {
        fprintf(stderr, "%sProcess32First failed (err %lu)%s\n", RED, GetLastError(), DEF);
        CloseHandle(snap);
        return -1;
    }

    do {
        if (_stricmp(pe.szExeFile, target_name) == 0) {
            printf("%sFound: %s PID: %lu%s\n", GREEN, pe.szExeFile, (unsigned long)pe.th32ProcessID, DEF);
            rpcs3_pid = pe.th32ProcessID;
            break;
        }
    } while(Process32Next(snap, &pe));
    CloseHandle(snap);
    return rpcs3_pid;
}