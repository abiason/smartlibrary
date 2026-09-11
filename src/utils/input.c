#include "input.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <io.h>
#define smartlibrary_isatty _isatty
#define smartlibrary_fileno _fileno
#else
#include <unistd.h>
#define smartlibrary_isatty isatty
#define smartlibrary_fileno fileno
#endif

int input_is_interactive(void) {
    return smartlibrary_isatty(smartlibrary_fileno(stdin)) && smartlibrary_isatty(smartlibrary_fileno(stdout));
}

void input_clear_screen(void) {
    if (!input_is_interactive()) {
        return;
    }

#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void input_wait_enter(void) {
    int ch;

    if (!input_is_interactive()) {
        return;
    }

    printf("\nPressione Enter para continuar...");
    fflush(stdout);
    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
}

int input_read_line(const char *prompt, char *buffer, size_t size) {
    size_t length;

    if (buffer == NULL || size == 0) {
        return 0;
    }

    if (prompt != NULL) {
        printf("%s", prompt);
    }

    if (fgets(buffer, (int)size, stdin) == NULL) {
        buffer[0] = '\0';
        return 0;
    }

    length = strlen(buffer);
    if (length > 0 && buffer[length - 1] == '\n') {
        buffer[length - 1] = '\0';
    } else {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {
        }
    }

    return 1;
}

int input_read_int(const char *prompt, int *value) {
    char buffer[64];
    char *endptr = NULL;
    long parsed;

    if (value == NULL || !input_read_line(prompt, buffer, sizeof(buffer))) {
        return 0;
    }

    parsed = strtol(buffer, &endptr, 10);
    if (endptr == buffer || *endptr != '\0') {
        return 0;
    }

    *value = (int)parsed;
    return 1;
}
