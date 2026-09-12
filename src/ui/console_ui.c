#include "ui/console_ui.h"

#include "utils/input.h"

#include <stdio.h>
#include <string.h>

static void print_padded_text(const char *text) {
    int length = text != NULL ? (int)strlen(text) : 0;
    int visible = length;

    if (visible > UI_WIDTH - 4) {
        visible = UI_WIDTH - 4;
    }

    printf("| %-*.*s |\n", UI_WIDTH - 4, visible, text != NULL ? text : "");
}

void ui_clear(void) {
    input_clear_screen();
}

void ui_line(void) {
    putchar('+');
    for (int i = 0; i < UI_WIDTH - 2; i++) {
        putchar('-');
    }
    puts("+");
}

void ui_header(const char *title, const char *context) {
    ui_line();
    print_padded_text(title);
    if (context != NULL && context[0] != '\0') {
        print_padded_text(context);
    }
    ui_line();
    putchar('\n');
}

void ui_context(const char *label, const char *value) {
    printf("  %-12s: %s\n", label != NULL ? label : "", value != NULL ? value : "");
}

void ui_menu_item(int option, const char *description) {
    printf("  [%d] %s\n", option, description != NULL ? description : "");
}

void ui_menu_back(const char *description) {
    printf("\n  [0] %s\n", description != NULL ? description : "Voltar");
}

void ui_prompt_label(const char *label) {
    printf("  %-14s: ", label != NULL ? label : "");
}

static void ui_message(const char *type, const char *message) {
    putchar('\n');
    ui_line();
    print_padded_text(type);
    ui_line();
    print_padded_text(message);
    ui_line();
}

void ui_success(const char *message) {
    char buffer[UI_WIDTH];

    snprintf(buffer, sizeof(buffer), "[OK] %s", message != NULL ? message : "");
    ui_message("SUCESSO", buffer);
}

void ui_error(const char *message) {
    char buffer[UI_WIDTH];

    snprintf(buffer, sizeof(buffer), "[ERRO] %s", message != NULL ? message : "");
    ui_message("ERRO", buffer);
}

void ui_info(const char *message) {
    char buffer[UI_WIDTH];

    snprintf(buffer, sizeof(buffer), "[INFO] %s", message != NULL ? message : "");
    ui_message("INFO", buffer);
}

void ui_warning(const char *message) {
    char buffer[UI_WIDTH];

    snprintf(buffer, sizeof(buffer), "[WARN] %s", message != NULL ? message : "");
    ui_message("AVISO", buffer);
}
