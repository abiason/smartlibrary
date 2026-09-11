CC = gcc
PKG_CONFIG ?= pkg-config

TARGET := build/smartlibrary
ifeq ($(OS),Windows_NT)
TARGET := build/smartlibrary.exe
endif

SRC := src/main.c src/config/config.c src/database/postgres.c src/database/mongodb.c src/repositories/cadastro_repository.c src/repositories/emprestimo_repository.c src/repositories/reserva_repository.c src/repositories/self_checkout_repository.c src/repositories/relatorio_repository.c src/services/cadastro_service.c src/services/emprestimo_service.c src/services/reserva_service.c src/services/self_checkout_service.c src/services/relatorio_service.c src/events/event_service.c src/ui/main_ui.c src/ui/cadastro_ui.c src/ui/emprestimo_ui.c src/ui/reserva_ui.c src/ui/self_checkout_ui.c src/ui/nosql_ui.c src/ui/relatorio_ui.c src/utils/input.c src/models/usuario.c src/models/autor.c src/models/editora.c src/models/genero.c src/models/livro.c src/models/exemplar.c src/models/emprestimo.c src/models/reserva.c
OBJ := $(SRC:src/%.c=build/%.o)

CFLAGS := -std=c17 -Wall -Wextra -Wpedantic
CPPFLAGS := -Isrc -D__USE_MINGW_ANSI_STDIO=1 $(shell $(PKG_CONFIG) --cflags libpq mongoc2)
LDLIBS := $(shell $(PKG_CONFIG) --libs libpq mongoc2)

.PHONY: all clean run check-deps

all: check-deps $(TARGET)

check-deps:
	$(PKG_CONFIG) --cflags --libs libpq
	$(PKG_CONFIG) --cflags --libs mongoc2

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDLIBS)

build/%.o: src/%.c
ifeq ($(OS),Windows_NT)
	@if not exist "$(subst /,\,$(dir $@))" mkdir "$(subst /,\,$(dir $@))"
else
	@mkdir -p $(dir $@)
endif
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

run: all
	$(TARGET)

clean:
ifeq ($(OS),Windows_NT)
	@if exist build rmdir /s /q build
else
	rm -rf build
endif







