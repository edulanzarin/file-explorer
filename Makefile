# Compilador que vamos usar
CC = gcc

# Opções de compilação:
# - Wall e Wextra: ativa avisos importantes
# - std=c11: usa o padrão C mais recente
# - D_GNU_SOURCE: habilita recursos extras do GNU/Linux
CFLAGS = -Wall -Wextra -std=c11 -D_GNU_SOURCE

# Bibliotecas adicionais (deixamos vazio por enquanto)
LIBS =  

# Lista dos arquivos fonte (.c) que compõem o projeto
SRC = src/main.c src/interface.c src/navegacao.c src/comandos.c

# Converte os .c em .o (arquivos objeto)
OBJ = $(SRC:.c=.o)

# Nome do programa executável final
EXEC = file_explorer

# Regra principal - compila tudo
all: $(EXEC)

# Como construir o executável final:
# Junta todos os .o e gera o programa
$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(EXEC) $(LIBS)

# Limpeza - remove os arquivos temporários e o executável
clean:
	rm -f $(OBJ) $(EXEC)

# Indica que 'all' e 'clean' são regras especiais, não arquivos
.PHONY: all clean