#include "../headers/tos.h"
#include "../headers/includes.h"

int tos(void) {
    printf("By using Ratchad you agree to the following terms:\n\n"
        "-> You are using a legally acquired copy of Ratchet & Clank,\n"
        "-> You understand that this program will modify your save data,\n"
        "-> You understand that this program is in early-alpha and bugs may be present that could corrupt your save-file\n\n"
        "Do you agree to these terms? (yes/no): "
    );

    char answer[2] = "";
    scanf_s("%1s", answer, 2);
    printf("\n");
    answer[0] = tolower(answer[0]);

    if (answer[0] == 'y') {
        return 0;
    } else {
        return 1;
    }
}