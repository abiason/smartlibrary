#include "ui/console_ui.h"

#include "utils/input.h"

#include <stdio.h>
#include <string.h>

#define TABLE_MIN_COL_WIDTH 8
#define TABLE_MAX_COL_WIDTH 28

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

int ui_read_line(const char *label, char *buffer, size_t size) {
    ui_prompt_label(label);
    return input_read_line("", buffer, size);
}

int ui_read_int(const char *label, int *value) {
    ui_prompt_label(label);
    return input_read_int("", value);
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

static int text_width(const char *text) {
    int length = text != NULL ? (int)strlen(text) : 0;

    if (length < TABLE_MIN_COL_WIDTH) {
        return TABLE_MIN_COL_WIDTH;
    }
    if (length > TABLE_MAX_COL_WIDTH) {
        return TABLE_MAX_COL_WIDTH;
    }
    return length;
}

static void print_table_separator(const int *widths, int cols) {
    putchar('+');
    for (int col = 0; col < cols; col++) {
        for (int i = 0; i < widths[col] + 2; i++) {
            putchar('-');
        }
        putchar('+');
    }
    putchar('\n');
}

static void print_table_cell(const char *text, int width) {
    int length = text != NULL ? (int)strlen(text) : 0;
    int visible = length;

    if (visible > width) {
        visible = width;
    }

    printf(" %-*.*s ", width, visible, text != NULL ? text : "");
}

static void print_table_row(PGresult *result, int row, const int *widths, int cols) {
    putchar('|');
    for (int col = 0; col < cols; col++) {
        const char *value = row < 0 ? PQfname(result, col) : PQgetvalue(result, row, col);
        print_table_cell(value, widths[col]);
        putchar('|');
    }
    putchar('\n');
}

void ui_print_pgresult_table(PGresult *result, const char *empty_message) {
    int rows;
    int cols;
    int widths[32];

    if (result == NULL) {
        return;
    }

    rows = PQntuples(result);
    cols = PQnfields(result);

    if (rows == 0) {
        ui_info(empty_message != NULL ? empty_message : "Nenhum registro encontrado.");
        PQclear(result);
        return;
    }

    if (cols > (int)(sizeof(widths) / sizeof(widths[0]))) {
        cols = (int)(sizeof(widths) / sizeof(widths[0]));
    }

    for (int col = 0; col < cols; col++) {
        widths[col] = text_width(PQfname(result, col));
    }

    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            int width = text_width(PQgetvalue(result, row, col));
            if (width > widths[col]) {
                widths[col] = width;
            }
        }
    }

    putchar('\n');
    print_table_separator(widths, cols);
    print_table_row(result, -1, widths, cols);
    print_table_separator(widths, cols);
    for (int row = 0; row < rows; row++) {
        print_table_row(result, row, widths, cols);
    }
    print_table_separator(widths, cols);

    PQclear(result);
}
