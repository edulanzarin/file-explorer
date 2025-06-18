# Compilador que vamos usar
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -D_GNU_SOURCE

# não tem bibliotecas adicionais
LIBS =  

# lista dos arquivos fonte ".c"
SRC = src/main.c src/interface.c src/navegacao.c src/comandos.c

# Converte os .c em .o
OBJ = $(SRC:.c=.o)

# nome do programa executável
EXEC = file_explorer

# regra principal - compila tudo
all: $(EXEC)

# junta todos os .o e gera o programa
$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(EXEC) $(LIBS)

clean:
	rm -f $(OBJ) $(EXEC)

# indica que 'all' e 'clean' são regras especiais e não arquivos
.PHONY: all clean