#include "comandos.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h> // Adicionado para readlink()
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/vfs.h>
#include <sys/ioctl.h>     // Para ioctl()
#include <sys/sysmacros.h> // Para major() e minor()
#include <linux/fs.h>      // Para FIBMAP
#include <linux/magic.h>   // Para os defines de magic numbers (opcional)
#include <linux/fiemap.h>

#define FIEMAP_EXTENT_INLINE 0x00000080
#define MAX_EXTENTS 512

static void mostrar_detalhes_estruturais(const char *caminho)
{
    struct stat file_stat;
    if (stat(caminho, &file_stat) < 0)
    {
        perror("Erro ao obter estatísticas");
        return;
    }

    printf("\n\033[1mESTRUTURA DO ARQUIVO:\033[0m\n");
    printf("Inode: %lu\n", file_stat.st_ino);
    printf("Número de links: %lu\n", (unsigned long)file_stat.st_nlink);
    printf("Tamanho: %ld bytes\n", file_stat.st_size);
    printf("Blocos alocados: %ld (blocos de 512 bytes)\n", file_stat.st_blocks);
    printf("Tamanho do bloco: %ld bytes\n", file_stat.st_blksize);
    printf("Dispositivo: %u,%u (major/minor)\n", // Alterado para %u
           major(file_stat.st_dev), minor(file_stat.st_dev));
}

static int verificar_fs_compativel(const char *caminho)
{
    struct statfs fs_info;
    if (statfs(caminho, &fs_info) != 0)
    {
        return 0;
    }

    // Magic numbers dos sistemas de arquivos suportados
    switch (fs_info.f_type)
    {
    case 0xEF53:     // EXT4_SUPER_MAGIC
    case 0x58465342: // XFS_SUPER_MAGIC
    case 0x9123683E: // BTRFS_SUPER_MAGIC
        return 1;
    default:
        return 0;
    }
}

void mostrar_detalhes_fisicos(const char *caminho)
{
    int fd = open(caminho, O_RDONLY);
    if (fd < 0)
    {
        perror("Erro ao abrir arquivo");
        return;
    }

    size_t fiemap_size = sizeof(struct fiemap) + MAX_EXTENTS * sizeof(struct fiemap_extent);
    struct fiemap *fiemap = malloc(fiemap_size);
    if (!fiemap)
    {
        perror("Erro de alocação");
        close(fd);
        return;
    }

    memset(fiemap, 0, fiemap_size);
    fiemap->fm_start = 0;
    fiemap->fm_length = FIEMAP_MAX_OFFSET;
    fiemap->fm_flags = 0;
    fiemap->fm_extent_count = MAX_EXTENTS;

    if (ioctl(fd, FS_IOC_FIEMAP, fiemap) == 0)
    {
        if (fiemap->fm_mapped_extents > 0)
        {
            printf("Extents físicos (%u encontrados):\n", fiemap->fm_mapped_extents);
            for (unsigned int i = 0; i < fiemap->fm_mapped_extents; i++)
            {
                printf("  Extent %u: offset físico %llu, comprimento %llu bytes\n",
                       i,
                       (unsigned long long)fiemap->fm_extents[i].fe_physical,
                       (unsigned long long)fiemap->fm_extents[i].fe_length);
            }
        }
        else
        {
            printf("Nenhum extent físico mapeado\n");
        }
    }
    else
    {
        perror("Erro ioctl FIEMAP");
        printf("Tente executar como root para mais detalhes\n");
    }

    free(fiemap);
    close(fd);
}

static void mostrar_detalhes_temporais(const char *caminho)
{
    struct stat file_stat;
    if (stat(caminho, &file_stat) < 0)
        return;

    printf("\n\033[1mTEMPOS DE ACESSO:\033[0m\n");
    printf("Último acesso: %s", ctime(&file_stat.st_atime));
    printf("Última modificação: %s", ctime(&file_stat.st_mtime));
    printf("Última alteração de status: %s", ctime(&file_stat.st_ctime));
}

static void mostrar_detalhes_permissoes(const char *caminho)
{
    struct stat file_stat;
    if (stat(caminho, &file_stat) < 0)
        return;

    printf("\n\033[1mPERMISSÕES E PROPRIEDADE:\033[0m\n");
    printf("Modo: %o\n", file_stat.st_mode & 07777);
    printf("Usuário: %d\n", file_stat.st_uid);
    printf("Grupo: %d\n", file_stat.st_gid);

    // Converter para formato rwxrwxrwx
    char perms[10];
    for (int i = 0; i < 9; i++)
    {
        perms[i] = (file_stat.st_mode & (1 << (8 - i))) ? "rwx"[i % 3] : '-';
    }
    perms[9] = '\0';
    printf("Permissões: %s\n", perms);
}

static void mostrar_detalhes_especiais(const char *caminho)
{
    struct stat file_stat;
    if (stat(caminho, &file_stat) < 0)
        return;

    if (S_ISLNK(file_stat.st_mode))
    {
        char link_target[1024];
        ssize_t len = readlink(caminho, link_target, sizeof(link_target) - 1);
        if (len != -1)
        {
            link_target[len] = '\0';
            printf("\n\033[1mLINK SIMBÓLICO:\033[0m\n");
            printf("Aponta para: %s\n", link_target);
        }
    }
}

void mostrar_info_arquivo(const char *diretorio_atual, const char *nome_arquivo)
{
    char caminho[1024];
    if (snprintf(caminho, sizeof(caminho), "%s/%s", diretorio_atual, nome_arquivo) >= (int)sizeof(caminho))
    {
        printf("\nCaminho muito longo!\n");
        return;
    }

    printf("\n\033[1;34m=== INFORMAÇÕES COMPLETAS DO ARQUIVO ===\033[0m\n");

    struct stat file_stat;
    if (stat(caminho, &file_stat) < 0)
    {
        perror("Erro ao obter informações");
        return;
    }

    printf("\n\033[1mINFORMAÇÕES BÁSICAS:\033[0m\n");
    printf("Nome: %s\n", nome_arquivo);
    printf("Tipo: %s\n",
           S_ISDIR(file_stat.st_mode) ? "Diretório" : S_ISREG(file_stat.st_mode) ? "Arquivo Regular"
                                                  : S_ISLNK(file_stat.st_mode)   ? "Link Simbólico"
                                                                                 : "Tipo Especial");

    mostrar_detalhes_estruturais(caminho);
    mostrar_detalhes_fisicos(caminho);
    mostrar_detalhes_temporais(caminho);
    mostrar_detalhes_permissoes(caminho);
    mostrar_detalhes_especiais(caminho);

    printf("\n\033[1mPressione qualquer tecla para continuar...\033[0m");
    getchar();
}

void criar_arquivo_fragmentado(const char *diretorio_atual)
{
    char caminho[1024];
    snprintf(caminho, sizeof(caminho), "%s/arquivo_fragmentado", diretorio_atual);

    printf("\nCriando arquivo fragmentado em: %s\n", caminho);

    // Criação de arquivos temporários para forçar fragmentação
    char temp1[1024], temp2[1024];
    snprintf(temp1, sizeof(temp1), "%s/temp1", diretorio_atual);
    snprintf(temp2, sizeof(temp2), "%s/temp2", diretorio_atual);

    // Cria arquivos temporários
    printf("Criando arquivos temporários para forçar fragmentação...\n");
    system("dd if=/dev/zero of=temp1 bs=1M count=50 status=none");
    system("dd if=/dev/zero of=temp2 bs=1M count=50 status=none");

    // Remove o primeiro temporário
    printf("Removendo primeiro arquivo temporário...\n");
    remove(temp1);

    // Cria o arquivo fragmentado
    printf("Criando arquivo fragmentado (70MB)...\n");
    system("dd if=/dev/zero of=arquivo_fragmentado bs=1M count=70 status=none");

    // Remove o segundo temporário
    printf("Limpando arquivos temporários...\n");
    remove(temp2);

    printf("\nArquivo fragmentado criado com sucesso!\n");
    printf("Use a opção 'I' para visualizar os extents físicos.\n");
    printf("\nPressione qualquer tecla para continuar...");
    getchar(); // Para capturar o ENTER
}

void limpar_arquivos_teste(const char *diretorio_atual)
{
    char caminho[1024];
    snprintf(caminho, sizeof(caminho), "%s/arquivo_fragmentado", diretorio_atual);
    remove(caminho);
    printf("Arquivo fragmentado removido: %s\n", caminho);
}

void atualizar_rodape_comandos()
{
    printf("\n\033[1mComandos:\033[0m ");
    printf("<↑/↓> Navegar | <ENTER> Entrar no Diretório | ");
    printf("<I> Informações do Arquivo | <F> Criar Arquivo Fragmentado | <Q> Sair\n");
}