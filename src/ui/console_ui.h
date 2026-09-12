#ifndef SMARTLIBRARY_CONSOLE_UI_H
#define SMARTLIBRARY_CONSOLE_UI_H

#include <stddef.h>

#define UI_WIDTH 78

void ui_clear(void);
void ui_line(void);
void ui_header(const char *title, const char *context);
void ui_context(const char *label, const char *value);
void ui_menu_item(int option, const char *description);
void ui_menu_back(const char *description);
void ui_prompt_label(const char *label);
int ui_read_line(const char *label, char *buffer, size_t size);
int ui_read_int(const char *label, int *value);
void ui_success(const char *message);
void ui_error(const char *message);
void ui_info(const char *message);
void ui_warning(const char *message);

#endif
