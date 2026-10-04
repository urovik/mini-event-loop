CC      := gcc
CFLAGS  := -Wall -Wextra -Werror -std=c11 -D_GNU_SOURCE -g
LDFLAGS :=

SRC := $(wildcard src/*.c)
BIN := server

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(BIN) $(LDFLAGS)

clean:
	rm -f $(BIN)

.PHONY: all clean