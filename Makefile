# Compilador e flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -D_GNU_SOURCE

# Diretório de saída
OUTDIR = out

# Lista automaticamente todos os .c dentro de src/ e subpastas
SRC := $(wildcard src/**/*.c) $(wildcard src/*.c)


# Cria a lista de .o correspondentes em out/
OBJ := $(patsubst src/%, $(OUTDIR)/%, $(SRC:.c=.o))
OBJ := $(notdir $(OBJ))
OBJ := $(addprefix $(OUTDIR)/, $(OBJ))

# Nome do executável
EXEC = $(OUTDIR)/file_explorer

# Regra principal
all: $(EXEC)

# Linka os .o e gera o executável
$(EXEC): $(OBJ) | $(OUTDIR)
	$(CC) $(CFLAGS) $(OBJ) -o $(EXEC)

# Compila cada .c em .o dentro de out/
$(OUTDIR)/%.o: 
	@mkdir -p $(OUTDIR)
	$(CC) $(CFLAGS) -c $(firstword $(filter %/$*.c, $(SRC))) -o $@

# Garante que a pasta out exista
$(OUTDIR):
	mkdir -p $(OUTDIR)

# Limpa os binários
clean:
	rm -rf $(OUTDIR)

.PHONY: all clean
