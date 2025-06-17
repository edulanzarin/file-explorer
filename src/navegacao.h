#ifndef NAVEGACAO_H
#define NAVEGACAO_H

/* Tamanho máximo do caminho (string) que a gente vai armazenar */
#define MAX_PATH_LEN 1024

/* Número máximo de entradas que o programa vai conseguir listar de um diretório.
 * Se o diretório tiver mais que isso a lista vai ser cortada no limite.
 * Ajuda a evitar usar memória demais e manter a coisa mais rápida.
 */
#define MAX_ENTRIES 1000

/* Representa uma entrada do diretório, pode ser arquivo ou pasta */
typedef struct
{
    char nome[MAX_PATH_LEN]; // nome do arquivo ou pasta
    int is_dir;              // 1 se for pasta, 0 se for arquivo
} DirEntry;

/* Lista o conteúdo do diretório que a gente passar
 * Preenche o ponteiro com as entradas encontradas e retorna quantas tem
 */
int listar_diretorio(const char *caminho, DirEntry **entries);

/* Diz se o caminho passado é um diretório ou não */
int eh_diretorio(const char *caminho);

/* Pega o caminho do diretório atual e coloca no buffer que a gente passar */
void obter_diretorio_atual(char *buffer, size_t tamanho);

#endif // NAVEGACAO_H
