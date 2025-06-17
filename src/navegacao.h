#ifndef NAVEGACAO_H
#define NAVEGACAO_H

#include <dirent.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

/* tamanho máximo que um caminho de arquivo pode ter */
#define MAX_PATH_LEN 1024

/* número máximo de arquivos/pastas que listamos de uma vez
   (pra não travar se tiver muitos arquivos) */
#define MAX_ENTRIES 1000

/* estrutura para representar a entrada de um diretório, um arquivo
 * ou pasta listado dentro de uma pasta
 */
typedef struct {
  char nome[MAX_PATH_LEN]; /* array de caracteres para o nome */
  int is_dir;              /* indica se é diretório (1) ou arquivo (0) */
} DirEntry;

/* funções de navegação implementadas em navegacao.c */
int listar_diretorio(const char *caminho, DirEntry **entries);
int eh_diretorio(const char *caminho);
void obter_dir_atual(char *buffer, size_t tamanho);

#endif