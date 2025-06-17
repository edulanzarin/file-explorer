#include "comandos.h"

/* Mostra detalhes estruturais do arquivo
   - Número do inode, links, tamanho em bytes/blocos
   - Informações sobre o dispositivo onde está armazenado
*/
static void mostrar_detalhes_estruturais(const char *caminho) {
  struct stat file_stat;
  if (stat(caminho, &file_stat) < 0) {
    perror("Erro ao obter estatísticas");
    return;
  }

  printf("\n\033[1mESTRUTURA DO ARQUIVO:\033[0m\n");
  printf("Inode: %lu\n", file_stat.st_ino);
  printf("Número de links: %lu\n", (unsigned long)file_stat.st_nlink);
  printf("Tamanho: %ld bytes\n", file_stat.st_size);
  printf("Blocos alocados: %ld (blocos de 512 bytes)\n", file_stat.st_blocks);
  printf("Tamanho do bloco: %ld bytes\n", file_stat.st_blksize);
  printf("Dispositivo: %u,%u (major/minor)\n", major(file_stat.st_dev),
         minor(file_stat.st_dev));
}

/* Mostra como o arquivo está armazenado fisicamente no disco
   - Lista os extents (áreas contíguas de blocos)
   - Mostra a localização física e tamanho de cada extent
   Necessita de privilégios de root para funcionar
*/
void mostrar_detalhes_fisicos(const char *caminho) {
  int fd = open(caminho, O_RDONLY);
  if (fd < 0) {
    perror("Erro ao abrir arquivo");
    return;
  }

  // Prepara estrutura para mapear os extents do arquivo
  size_t fiemap_size =
      sizeof(struct fiemap) + MAX_EXTENTS * sizeof(struct fiemap_extent);
  struct fiemap *fiemap = malloc(fiemap_size);
  if (!fiemap) {
    perror("Erro de alocação");
    close(fd);
    return;
  }

  memset(fiemap, 0, fiemap_size);
  fiemap->fm_start = 0;
  fiemap->fm_length = FIEMAP_MAX_OFFSET;
  fiemap->fm_flags = 0;
  fiemap->fm_extent_count = MAX_EXTENTS;

  // Faz a chamada de sistema para obter o mapeamento físico
  if (ioctl(fd, FS_IOC_FIEMAP, fiemap) == 0) {
    if (fiemap->fm_mapped_extents > 0) {
      printf("Extents físicos (%u encontrados):\n", fiemap->fm_mapped_extents);
      for (unsigned int i = 0; i < fiemap->fm_mapped_extents; i++) {
        printf("  Extent %u: offset físico %llu, comprimento %llu bytes\n", i,
               (unsigned long long)fiemap->fm_extents[i].fe_physical,
               (unsigned long long)fiemap->fm_extents[i].fe_length);
      }
    } else {
      printf("Nenhum extent físico mapeado\n");
    }
  } else {
    perror("Erro ioctl FIEMAP");
    printf("Tente executar como root para mais detalhes\n");
  }

  free(fiemap);
  close(fd);
}

/* Mostra os tempos de acesso do arquivo
   - Último acesso
   - Última modificação
   - Última mudança de status
*/
static void mostrar_detalhes_temporais(const char *caminho) {
  struct stat file_stat;
  if (stat(caminho, &file_stat) < 0)
    return;

  printf("\n\033[1mTEMPOS DE ACESSO:\033[0m\n");
  printf("Último acesso: %s", ctime(&file_stat.st_atime));
  printf("Última modificação: %s", ctime(&file_stat.st_mtime));
  printf("Última alteração de status: %s", ctime(&file_stat.st_ctime));
}

/* Mostra permissões e dono do arquivo
   - Modo no formato rwxrwxrwx
   - IDs do usuário e grupo dono
*/
static void mostrar_detalhes_permissoes(const char *caminho) {
  struct stat file_stat;
  if (stat(caminho, &file_stat) < 0)
    return;

  printf("\n\033[1mPERMISSÕES E PROPRIEDADE:\033[0m\n");
  printf("Modo: %o\n", file_stat.st_mode & 07777);
  printf("Usuário: %d\n", file_stat.st_uid);
  printf("Grupo: %d\n", file_stat.st_gid);

  // Converte permissões para formato legível (rwx)
  char perms[10];
  for (int i = 0; i < 9; i++) {
    perms[i] = (file_stat.st_mode & (1 << (8 - i))) ? "rwx"[i % 3] : '-';
  }
  perms[9] = '\0';
  printf("Permissões: %s\n", perms);
}

/* Mostra detalhes especiais para links simbólicos
   - Mostra para onde o link aponta
*/
static void mostrar_detalhes_especiais(const char *caminho) {
  struct stat file_stat;
  if (stat(caminho, &file_stat) < 0)
    return;

  if (S_ISLNK(file_stat.st_mode)) {
    char link_target[1024];
    ssize_t len = readlink(caminho, link_target, sizeof(link_target) - 1);
    if (len != -1) {
      link_target[len] = '\0';
      printf("\n\033[1mLINK SIMBÓLICO:\033[0m\n");
      printf("Aponta para: %s\n", link_target);
    }
  }
}

/* Função principal que mostra todas as informações do arquivo
   Combina todos os detalhes em uma saída formatada
*/
void mostrar_info_arquivo(const char *dir_atual, const char *nome_arquivo) {
  char caminho[1024];
  if (snprintf(caminho, sizeof(caminho), "%s/%s", dir_atual, nome_arquivo) >=
      (int)sizeof(caminho)) {
    printf("\nCaminho muito longo!\n");
    return;
  }

  printf("\n\033[1;34m=== INFORMAÇÕES COMPLETAS DO ARQUIVO ===\033[0m\n");

  struct stat file_stat;
  if (stat(caminho, &file_stat) < 0) {
    perror("Erro ao obter informações");
    return;
  }

  printf("\n\033[1mINFORMAÇÕES BÁSICAS:\033[0m\n");
  printf("Nome: %s\n", nome_arquivo);
  printf("Tipo: %s\n", S_ISDIR(file_stat.st_mode)   ? "Diretório"
                       : S_ISREG(file_stat.st_mode) ? "Arquivo Regular"
                       : S_ISLNK(file_stat.st_mode) ? "Link Simbólico"
                                                    : "Tipo Especial");

  // Mostra todas as categorias de informação
  mostrar_detalhes_estruturais(caminho);
  mostrar_detalhes_fisicos(caminho);
  mostrar_detalhes_temporais(caminho);
  mostrar_detalhes_permissoes(caminho);
  mostrar_detalhes_especiais(caminho);

  printf("\n\033[1mPressione qualquer tecla para continuar...\033[0m");
  getchar();
}

/* Cria um arquivo fragmentado para demonstração
   - Cria arquivos temporários para ocupar espaço
   - Remove alguns para criar "buracos"
   - Cria um arquivo grande que ficará fragmentado
*/
void criar_arquivo_fragmentado(const char *dir_atual) {
  char caminho[1024];
  snprintf(caminho, sizeof(caminho), "%s/arquivo_fragmentado", dir_atual);

  printf("\nCriando arquivo fragmentado em: %s\n", caminho);

  // Cria arquivos temporários para forçar fragmentação
  char temp1[1024], temp2[1024];
  snprintf(temp1, sizeof(temp1), "%s/temp1", dir_atual);
  snprintf(temp2, sizeof(temp2), "%s/temp2", dir_atual);

  printf("Criando arquivos temporários para forçar fragmentação...\n");
  system("dd if=/dev/zero of=temp1 bs=1M count=50 status=none");
  system("dd if=/dev/zero of=temp2 bs=1M count=50 status=none");

  printf("Removendo primeiro arquivo temporário...\n");
  remove(temp1);

  printf("Criando arquivo fragmentado (70MB)...\n");
  system("dd if=/dev/zero of=arquivo_fragmentado bs=1M count=70 status=none");

  printf("Limpando arquivos temporários...\n");
  remove(temp2);

  printf("\nArquivo fragmentado criado com sucesso!\n");
  printf("Use a opção 'I' para visualizar os extents físicos.\n");
  printf("\nPressione qualquer tecla para continuar...");
  getchar();
}

/* Limpa os arquivos criados para testes
   Remove o arquivo fragmentado criado anteriormente
*/
void limpar_arquivos_teste(const char *dir_atual) {
  char caminho[1024];
  snprintf(caminho, sizeof(caminho), "%s/arquivo_fragmentado", dir_atual);
  remove(caminho);
  printf("Arquivo fragmentado removido: %s\n", caminho);
}

/* Atualiza a mensagem de ajuda na parte inferior da tela
   Mostra os comandos disponíveis para o usuário
*/
void atualizar_rodape_comandos() {
  printf("\n\033[1mComandos:\033[0m ");
  printf("<↑/↓> Navegar | <ENTER> Entrar no Diretório | ");
  printf("<I> Informações do Arquivo | <F> Criar Arquivo Fragmentado | <Q> "
         "Sair\n");
}