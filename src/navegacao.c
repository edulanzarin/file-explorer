#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include "navegacao.h"

// Função para verificar se um caminho é diretório
int eh_diretorio(const char *caminho)
{
    struct stat statbuf;
    if (stat(caminho, &statbuf) != 0)
    {
        return 0; // Se não conseguir acessar, assume que não é diretório
    }
    return S_ISDIR(statbuf.st_mode);
}

// Função para listar conteúdo do diretório
int listar_diretorio(const char *caminho, DirEntry **entries)
{
    DIR *dir;
    struct dirent *ent;
    int count = 0;
    int added_parent_dir = 0; // Flag para controlar se já adicionamos o ..

    *entries = malloc(MAX_ENTRIES * sizeof(DirEntry));
    if (*entries == NULL)
    {
        return -1;
    }

    if ((dir = opendir(caminho)) == NULL)
    {
        return -2;
    }

    // Verifica se precisamos adicionar o .. manualmente
    int needs_parent_dir = (strcmp(caminho, "/") != 0);

    while ((ent = readdir(dir)) != NULL && count < MAX_ENTRIES)
    {
        // Ignora o diretório atual (.)
        if (strcmp(ent->d_name, ".") == 0)
        {
            continue;
        }

        // Se encontrou um .. natural do sistema
        if (strcmp(ent->d_name, "..") == 0)
        {
            if (needs_parent_dir && !added_parent_dir)
            {
                // Adiciona apenas uma vez
                strncpy((*entries)[count].nome, "..", MAX_PATH_LEN - 1);
                (*entries)[count].is_dir = 1;
                count++;
                added_parent_dir = 1;
            }
            continue;
        }

        char caminho_completo[MAX_PATH_LEN];
        snprintf(caminho_completo, sizeof(caminho_completo), "%s/%s", caminho, ent->d_name);

        strncpy((*entries)[count].nome, ent->d_name, MAX_PATH_LEN - 1);
        (*entries)[count].is_dir = eh_diretorio(caminho_completo);
        count++;
    }

    // Se não encontrou .. natural mas precisamos adicionar
    if (needs_parent_dir && !added_parent_dir)
    {
        strncpy((*entries)[count].nome, "..", MAX_PATH_LEN - 1);
        (*entries)[count].is_dir = 1;
        count++;
    }

    closedir(dir);
    return count;
}

// Obtém o diretório atual
void obter_diretorio_atual(char *buffer, size_t tamanho)
{
    getcwd(buffer, tamanho);
}