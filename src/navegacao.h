#ifndef NAVEGACAO_H
#define NAVEGACAO_H

#define MAX_PATH_LEN 1024
#define MAX_ENTRIES 1000

// Estrutura para representar uma entrada no diretório
typedef struct
{
    char nome[MAX_PATH_LEN]; // Nome do arquivo/diretório
    int is_dir;              // 1 se for diretório, 0 se for arquivo
} DirEntry;

// Lista as entradas do diretório atual
int listar_diretorio(const char *caminho, DirEntry **entries);

// Verifica se um caminho é um diretório
int eh_diretorio(const char *caminho);

// Obtém o diretório atual
void obter_diretorio_atual(char *buffer, size_t tamanho);

#endif // NAVEGACAO_H