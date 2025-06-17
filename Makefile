CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -D_GNU_SOURCE
LIBS =  # Removido -static -lmount
SRC = src/main.c src/interface.c src/navegacao.c src/comandos.c
OBJ = $(SRC:.c=.o)
EXEC = file_explorer

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(EXEC) $(LIBS)

clean:
	rm -f $(OBJ) $(EXEC)

.PHONY: all clean