#undef UNICODE
#undef _UNICODE

#include "includes.h"
#include "defines.h"

uint32_t bolts_gained = 0;
DWORD rpcs3_pid = 0;

int find_pids(const char* target_name) {
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "%sCreateToolhelp32Snapshot failed (err %lu)%s\n", RED, GetLastError(), DEF);
        return 1;
    }

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(pe);

    if (!Process32First(snap, &pe)) {
        fprintf(stderr, "%sProcess32First failed (err %lu)%s\n", RED, GetLastError(), DEF);
        CloseHandle(snap);
        return 1;
    }
    int found = 0;
    do {
        if (_stricmp(pe.szExeFile, target_name) == 0) {
            printf("%sFound: %s PID: %lu%s\n", GREEN, pe.szExeFile, (unsigned long)pe.th32ProcessID, DEF);
            rpcs3_pid = pe.th32ProcessID;
            found++;
            break;
        }
    } while (Process32Next(snap, &pe));
    CloseHandle(snap);
    return found > 0;
}

int main(void) {
    printf("Ratchad, made by Caleb Blackler\n\n"); // TODO: add a cool intro sequence instead of this basic shit

    if (!find_pids(TARGET)) {
        fprintf(stderr, "%sCould not find rpcs3.exe. Make sure it's running. (err %lu)%s\n", RED, GetLastError(), DEF);
        system("PAUSE"); // without the console will close instantly and end-user won't know what error was
        return 1;
    }

    printf("%sAttempting to attach Ratchad to PID %lu...%s\n", YELLOW, (unsigned long)rpcs3_pid, DEF);

    HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION, FALSE, rpcs3_pid);
    if (!hProcess) {
        fprintf(stderr, "%sRatchad failed to attach! (err %lu)%s\n", RED, GetLastError(), DEF);
        return 1;
    }

    printf("%sRatchad successfully attached PID %lu!%s\n\n%sLive bolt data updating at 60fps. Press Ctrl+C to exit\n",
        GREEN, (unsigned long)rpcs3_pid, DEF, CLR_CONS);

    while (1) {
        uint32_t raw_bolts = 0, raw_bomb = 0;
        SIZE_T bytesRead = 0;

        // read bolts
        if (!ReadProcessMemory(hProcess, (LPCVOID)BOLTS_ADDR, &raw_bolts, sizeof(raw_bolts), &bytesRead) || bytesRead != sizeof(raw_bolts)) {
            fprintf(stderr, "\n%sFailed reading bolts (err %lu)%s\n", RED, GetLastError(), DEF);
            Sleep(200);
            continue;
        }

        // read bomb glove ammo
        if (!ReadProcessMemory(hProcess, (LPCVOID)BOMB_GLOVE_ADDR, &raw_bomb, sizeof(raw_bomb), &bytesRead) || bytesRead != sizeof(raw_bomb)) {
            fprintf(stderr, "\n%sFailed reading bomb glove ammo (err %lu)%s\n", RED, GetLastError(), DEF);
            Sleep(200);
            continue;
        }

        uint32_t bolts_le = ntohl(raw_bolts); // convert raw bolt value from big-endian to little-endian
        uint32_t ammo = raw_bomb;

        // convert ammo to bolts
        if (ammo > 0) {
            uint32_t add = ammo * 5;
            bolts_le += add;

            // convert bolts back to big-endian and write to memory
            uint32_t new_be_bolts = htonl(bolts_le);
            SIZE_T bytesWritten = 0;
            WriteProcessMemory(hProcess, (LPVOID)BOLTS_ADDR, &new_be_bolts, sizeof(new_be_bolts), &bytesWritten);

            // set bomb ammo to zero
            uint32_t zero = 0;
            WriteProcessMemory(hProcess, (LPVOID)BOMB_GLOVE_ADDR, &zero, sizeof(zero), &bytesWritten);

            bolts_gained = bolts_gained += ammo * 5; // TODO: Add .txt file that tracks converted ammo over multiple Ratchad restarts
            ammo = 0;
        }

        // print live data every frame and move cursor back to start to avoid console spam
        printf("Raw Bolts: %s%08X%s | Actual: %s%u%s | Bolts Gained from Ammo: %s%u%s\r",
            CYAN, raw_bolts, DEF,
            GREEN, bolts_le, DEF,
            YELLOW, bolts_gained, DEF);

        fflush(stdout);
        Sleep(17); // update at ~60 fps (1000ms / 60 = 16.77ms)
    }
    CloseHandle(hProcess);
    return 0;
}
