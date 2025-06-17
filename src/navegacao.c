#include "navegacao.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

/* Verifica se é uma pasta */
int eh_diretorio(const char *caminho) {
  struct stat statbuf;
  if (stat(caminho, &statbuf) != 0) {
    return 0; // Se der erro, assume que não é pasta
  }
  return S_ISDIR(statbuf.st_mode);
}

/* Lista tudo dentro de uma pasta */
int listar_diretorio(const char *caminho, DirEntry **entries) {
  DIR *dir;
  struct dirent *ent;
  int count = 0;
  int added_parent_dir = 0; // Flag pra saber se já adicionamos o ".."

  // Aloca memória pra guardar os itens
  *entries = malloc(MAX_ENTRIES * sizeof(DirEntry));
  if (*entries == NULL) {
    return -1; // Erro de alocação
  }

  // Tenta abrir a pasta
  if ((dir = opendir(caminho)) == NULL) {
    return -2; // Erro ao abrir diretório
  }

  // Verifica se precisa adicionar o ".." manualmente
  int needs_parent_dir = (strcmp(caminho, "/") != 0);

  // Lê cada item da pasta
  while ((ent = readdir(dir)) != NULL && count < MAX_ENTRIES) {
    // Ignora o diretório atual (.)
    if (strcmp(ent->d_name, ".") == 0) {
      continue;
    }

    // Se for ".." (pasta pai)
    if (strcmp(ent->d_name, "..") == 0) {
      if (needs_parent_dir && !added_parent_dir) {
        // Adiciona apenas uma vez
        strncpy((*entries)[count].nome, "..", MAX_PATH_LEN - 1);
        (*entries)[count].is_dir = 1;
        count++;
        added_parent_dir = 1;
      }
      continue;
    }

    // Monta o caminho completo
    char caminho_completo[MAX_PATH_LEN];
    snprintf(caminho_completo, sizeof(caminho_completo), "%s/%s", caminho,
             ent->d_name);

    // Adiciona na lista
    strncpy((*entries)[count].nome, ent->d_name, MAX_PATH_LEN - 1);
    (*entries)[count].is_dir = eh_diretorio(caminho_completo);
    count++;
  }

  // Se precisava do ".." mas não encontrou naturalmente
  if (needs_parent_dir && !added_parent_dir) {
    strncpy((*entries)[count].nome, "..", MAX_PATH_LEN - 1);
    (*entries)[count].is_dir = 1;
    count++;
  }

  closedir(dir);
  return count; // Retorna quantos itens encontrou
}

/* Pega a pasta atual onde estamos */
void obter_dir_atual(char *buffer, size_t tamanho) {
  getcwd(buffer, tamanho); // Função do sistema que faz isso
}