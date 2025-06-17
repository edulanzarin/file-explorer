#ifndef NAVEGACAO_H
#define NAVEGACAO_H

/* Tamanho máximo que um caminho de arquivo pode ter */
#define MAX_PATH_LEN 1024

/* Número máximo de arquivos/pastas que listamos de uma vez
   (pra não travar se tiver muitos arquivos) */
#define MAX_ENTRIES 1000

/* Cada entrada pode ser um arquivo ou pasta */
typedef struct
{
    char nome[MAX_PATH_LEN]; // Nome do arquivo/pasta
    int is_dir;              // 1 = pasta, 0 = arquivo
} DirEntry;

/* Lista tudo que tem dentro de uma pasta
   Retorna quantos itens encontrou ou -1 se der erro */
int listar_diretorio(const char *caminho, DirEntry **entries);

/* Verifica se um caminho é uma pasta ou não */
int eh_diretorio(const char *caminho);

/* Pega o caminho completo da pasta onde estamos agora */
void obter_diretorio_atual(char *buffer, size_t tamanho);

#endif // NAVEGACAO_H