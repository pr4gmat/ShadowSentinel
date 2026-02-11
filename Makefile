CC      = gcc
CFLAGS  = -Wall -Wextra -O2 -g
LDFLAGS = -ldl

# ---- Папки ----
COMMON_DIR  = common
WATCHER_DIR = watcher
SCANNER_DIR = scanner

# ---- Исходники ----
COMMON_SRC = \
    $(COMMON_DIR)/log.c \
    $(COMMON_DIR)/protection.c \
    $(COMMON_DIR)/utils.c

WATCHER_SRC = \
    $(WATCHER_DIR)/file_mapper.c \
    $(WATCHER_DIR)/library_monitor.c \
    $(WATCHER_DIR)/integrity.c \
    $(WATCHER_DIR)/main.c

SCANNER_SRC = \
    $(SCANNER_DIR)/va_scanner.c \
    $(SCANNER_DIR)/module_list.c \
    $(SCANNER_DIR)/memory_editor.c \
    $(SCANNER_DIR)/main.c

# ---- Объекты ----
COMMON_OBJ  = $(COMMON_SRC:.c=.o)
WATCHER_OBJ = $(WATCHER_SRC:.c=.o)
SCANNER_OBJ = $(SCANNER_SRC:.c=.o)

# ---- Цели ----
.PHONY: all clean rebuild

all: watcher_app scanner_app

# -------- WATCHER --------
watcher_app: $(COMMON_OBJ) $(WATCHER_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# -------- SCANNER --------
scanner_app: $(COMMON_OBJ) $(SCANNER_OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# -------- Общие правила --------
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# -------- Очистка --------
clean:
	rm -f \
		$(COMMON_DIR)/*.o \
		$(WATCHER_DIR)/*.o \
		$(SCANNER_DIR)/*.o \
		watcher_app scanner_app

rebuild: clean all
