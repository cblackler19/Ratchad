#undef UNICODE
#undef _UNICODE

#include "../headers/includes.h"
#include "../headers/defines.h"

uint32_t bolts_gained;
uint32_t pid;

void timeout(int seconds) {
    for (int i = seconds; i > 0; i--) {
        printf("Exiting in %u seconds...\r", i);
        fflush(stdout);
        Sleep(1000);
    }
}

int init_ratchad(void) {
    printf("Ratchad-v%s\nMade by Caleb Blackler\n\nSpecial thanks to the Ratchet & Clank community for their work finding memory addresses for this game!\n\n", VERSION);

    if (tos() == 1) {
        printf("You can't use this program without accepting the terms.\n");
        timeout(5);
        return 1;
    }

    pid = find_pids(TARGET);

    if (pid == -1) {
        timeout(5);
        return 1;
    }
    
    return 0;
}

int main(void) {
    if (init_ratchad() == 1) {
        return 1;
    }

    HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (!hProcess) {
        fprintf(stderr, "%sRatchad failed to attach! (err %lu)%s\n", RED, GetLastError(), DEF);
        return 1;
    }

    printf("%s%sLive bolt data updating at 60fps. Press Ctrl+C to exit%s\n", CLR_CONS, GREEN, DEF);

    while (1) {
        uint32_t bolts_le = ntohl(read_u32(hProcess, BOLTS_ADDR)); // read bolt value in little-endian byte order
        uint32_t bg_ammo  = read_u32(hProcess, BOMB_GLOVE_ADDR);

        if (bg_ammo > 0) {
            uint32_t add = bg_ammo * 5;
            bolts_le += add;
            uint32_t bolts_be = htonl(bolts_le); // convert bolts back to big-endian
            write_u32(hProcess, BOLTS_ADDR, bolts_be); // write new bolt value to memory
            write_u32(hProcess, BOMB_GLOVE_ADDR, 0); // zero out bomb glove ammo
            bolts_gained += bg_ammo * 5; // TODO: Add .txt file that tracks converted ammo over multiple Ratchad restarts
        }

        printf("Bolts: %s%u%s | Bolts Gained from Ammo: %s%u%s\r",
            GREEN, bolts_le, DEF,
            YELLOW, bolts_gained, DEF);

        fflush(stdout);
        Sleep(17); // update at ~60 fps (1000ms / 60 = 16.77ms)
    }
    CloseHandle(hProcess);
    return 0;
}
