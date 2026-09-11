#ifndef SMARTLIBRARY_INPUT_H
#define SMARTLIBRARY_INPUT_H

#include <stddef.h>

int input_read_line(const char *prompt, char *buffer, size_t size);
int input_read_int(const char *prompt, int *value);
int input_is_interactive(void);
void input_clear_screen(void);
void input_wait_enter(void);

#endif
