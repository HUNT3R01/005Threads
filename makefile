CC      = gcc
CFLAGS  = -Wall -Wextra -O2
LIBS    = -lpthread
BIN     = bin

all: $(BIN)/log_analyzer

$(BIN)/log_analyzer: main_linux.c log_processor.c
	mkdir -p $(BIN)
	$(CC) $(CFLAGS) -o $@ main_linux.c log_processor.c $(LIBS)

clean:
	rm -rf $(BIN)

.PHONY: all clean