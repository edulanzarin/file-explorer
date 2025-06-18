#include "comandos.h"

/*
 * mostra detalhes estruturais do arquivo
 * como número do inode, links, tamanho em bytes/blocos e
 * informações sobre o dispositivo onde está armazenado
 */
static void mostrar_detalhes_estruturais(const char *caminho) {

  /*
   * struct stat é uma estrutura pronta fornecida pelo <sys/stat.h>
   * ela serve basicamente para armazenar infos de um arquivo
   */
  struct stat file_stat;
  /* tenta armazenar as informações do arquivo passado no 'caminho' */
  if (stat(caminho, &file_stat) < 0) {
    /* se falhar, retorna -1 e dá erro */
    perror("Erro ao obter estatísticas");
    return;
  }

  printf("\n\033[1mESTRUTURA DO ARQUIVO:\033[0m\n");

  /* número único que identifica o arquivo no sistema de arquivos */
  printf("Inode: %lu\n", file_stat.st_ino);
  /* número de nomes (hard links) que apontam para o inode */
  printf("Número de links: %lu\n", (unsigned long)file_stat.st_nlink);
  /* tamanho do arquivo em bytes */
  printf("Tamanho: %ld bytes\n", file_stat.st_size);
  /* quantidade de blocos alocados no disco para armazenar o arquivo */
  printf("Blocos alocados: %ld (blocos de 512 bytes)\n", file_stat.st_blocks);
  /* tamanho preferido de bloco para I/O */
  printf("Tamanho do bloco: %ld bytes\n", file_stat.st_blksize);

  /* representam o dispositivo onde o arquivo está armazenado
   * major: identifica o driver do dispositivo ex: disco rígido, SSD, etc
   * minor: identifica o dispositivo específico controlado por esse driver
   */
  printf("Dispositivo: %u,%u (major/minor)\n", major(file_stat.st_dev),
         minor(file_stat.st_dev));
}

/*
 * - mostra como o arquivo tá armazenado fisicamente no disco
 * - lista os extents
 * - mostra a localização física e tamanho de cada extent
 * - necessita de privilégios de root para funcionar
 */
void mostrar_detalhes_fisicos(const char *caminho) {

  /* abre o arquivo só para leitura */
  int fd = open(caminho, O_RDONLY);
  /* se o arquivo abrir, o fd nunca será menor que 0
   * porém 0, 1 e 2 são reservados para o sistema
   * 0 - stdin
   * 1 - stdout
   * 2 - stderr
   * ou seja, arquivos abertos com 'open' começam no 3
   */
  if (fd < 0) {
    perror("Erro ao abrir arquivo");
    return;
  }

  /*
   * calcula o tamanho em bytes da memória necessária para armazenar a estrutura
   * fiemap com espaço para MAX_EXTENTS = 512 (definido em comandos.h)
   */
  size_t fiemap_size =
      sizeof(struct fiemap) + MAX_EXTENTS * sizeof(struct fiemap_extent);
  /*
   * aloca dinamicamente um bloco de memória com o tamanho calculado e guarda o
   * endereço no ponteiro fiemap.
   */
  struct fiemap *fiemap = malloc(fiemap_size);
  /* se a alocação falhar, malloc retorna NULL */
  if (!fiemap) {
    perror("Erro de alocação");
    close(fd);
    return;
  }

  memset(fiemap, 0, fiemap_size);        /* limpas todos os campos */
  fiemap->fm_start = 0;                  /* define o offset inicial */
  fiemap->fm_length = FIEMAP_MAX_OFFSET; /* mapear até o fim do arquivo */
  fiemap->fm_flags = 0;                  /* nenhuma opção especial */
  fiemap->fm_extent_count = MAX_EXTENTS; /* nmr máximo para obter na resposta */

  /*
   * ioctl serve para enviar comandos especiais para arquivos abertos
   * FS_IOC_FIEMAP é o comando que pede pro kernel preencher a estrutura
   * fiemap com o mapeamento físico dos blocos do arquivo
   */
  if (ioctl(fd, FS_IOC_FIEMAP, fiemap) == 0) {
    /* verifica se o kernel encontrou algum extent mapeado */
    if (fiemap->fm_mapped_extents > 0) {
      printf("Extents físicos (%u encontrados):\n", fiemap->fm_mapped_extents);
      /*
       * percorre cada extent encontrada e para cada extent imprime offset
       * físico em bytes e quantos bytes ele ocupa no disco
       */
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
  }

  free(fiemap); /* libera a memória alocada para o fiemap */
  close(fd);    /* fecha o fd aberto no começo da função */
}

/*
 * mostra os tempos de acesso
 * - último acesso
 * - última modificação
 * - última mudança de status
 */
static void mostrar_detalhes_temporais(const char *caminho) {

  /*
   * struct stat é uma estrutura pronta fornecida pelo <sys/stat.h>
   * ela serve basicamente para armazenar infos de um arquivo
   */
  struct stat file_stat;
  /* tenta armazenar as informações do arquivo passado no 'caminho' */
  if (stat(caminho, &file_stat) < 0) {
    /* se falhar, retorna -1 e dá erro */
    perror("Erro ao obter estatísticas");
    return;
  }

  printf("\n\033[1mTEMPOS DE ACESSO:\033[0m\n");

  printf("Último acesso: %s", ctime(&file_stat.st_atime));
  printf("Última modificação: %s", ctime(&file_stat.st_mtime));
  printf("Última alteração de status: %s", ctime(&file_stat.st_ctime));
}

/*
 * mostra permissões e dono do arquivo no formato rwxrwxrwx
 * 1 a 3 -  permissões do dono
 * 4 a 6 - permissões do grupo
 * 7 a 9 - permissões de outros
 */
static void mostrar_detalhes_permissoes(const char *caminho) {

  /*
   * struct stat é uma estrutura pronta fornecida pelo <sys/stat.h>
   * ela serve basicamente para armazenar infos de um arquivo
   */
  struct stat file_stat;
  /* tenta armazenar as informações do arquivo passado no 'caminho' */
  if (stat(caminho, &file_stat) < 0) {
    /* se falhar, retorna -1 e dá erro */
    perror("Erro ao obter estatísticas");
    return;
  }

  printf("\n\033[1mPERMISSÕES E PROPRIEDADE:\033[0m\n");

  /*
   * o campo 'st_mode' tem tipo do arquivo, permissões e atributos especiais
   * a máscara 07777 isola só as permissões e bits especiais (12 bits finais).
   *
   *    1   1   1   1   1   1   1   1   1   1   1   1
   *  ┌───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┬───┐
   *  │ S │ S │ S │ U │ U │ U │ G │ G │ G │ O │ O │ O │
   *  └───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┴───┘
   *    │   │   │   │   │   │   │   │   │   │   │   │
   *    │   │   │   │   │   │   │   │   │   └───└───└─── outros (rwx)
   *    │   │   │   │   │   │   └───└───└─────────────── grupo (rwx)
   *    │   │   │   └───└───└─────────────────────────── usuário (rwx)
   *    │   │   │
   *  bits especiais:
   *   - setuid -> executa como dono
   *   - setgid -> executa como grupo
   *   - sticky -> controle de escrita em pastas públicas
   *
   *  exemplo:
   *    0755 = rwxr-xr-x
   *    0644 = rw-r--r--
   *    0777 = rwxrwxrwx
   */
  printf("Modo: %o\n", file_stat.st_mode & 07777);

  printf("Usuário: %d\n", file_stat.st_uid); /* UID do dono */
  printf("Grupo: %d\n", file_stat.st_gid);   /* GID do grupo */

  char perms[10]; /* vetor para armazenar os 9 caracteres + '\0' */

  /*
   * i vai de 0 a 8 pois são 9 bits de permissão
   * (1 << (8 - i)): cria uma máscara binária que anda da esquerda até a direita
   * file_stat.st_mode & (1 << (8 - i)): testa se esse bit está ativado
   * "rwx"[i % 3]: decide qual letra mostrar i % 3 == 0 -> 'r',
   * 1 -> 'w', 2 -> 'x'
   *
   * ┌────┬──────────────┬──────────────┬───────────────┬───────────┐
   * │ i  │ (1 << (8-i)) │ verifica bit │ i % 3 ("rwx") │ resultado │
   * ├────┼──────────────┼──────────────┼───────────────┼───────────┤
   * │ 0  │  100000000   │    bit 8     │    0 -> 'r'   │ 1 -> 'r'  │
   * │ 1  │  010000000   │    bit 7     │    1 -> 'w'   │ 1 -> 'w'  │
   * │ 2  │  001000000   │    bit 6     │    2 -> 'x'   │ 1 -> 'x'  │
   * │ 3  │  000100000   │    bit 5     │    0 -> 'r'   │ 1 -> 'r'  │
   * │ 4  │  000010000   │    bit 4     │    1 -> 'w'   │ 1 -> 'w'  │
   * │ 5  │  000001000   │    bit 3     │    2 -> 'x'   │ 1 -> 'x'  │
   * │ 6  │  000000100   │    bit 2     │    0 -> 'r'   │ 1 -> 'r'  │
   * │ 7  │  000000010   │    bit 1     │    1 -> 'w'   │ 1 -> 'w'  │
   * │ 8  │  000000001   │    bit 0     │    2 -> 'x'   │ 1 -> 'x'  │
   * └────┴──────────────┴──────────────┴───────────────┴───────────┘
   */
  for (int i = 0; i < 9; i++) {
    perms[i] = (file_stat.st_mode & (1 << (8 - i))) ? "rwx"[i % 3] : '-';
  }
  perms[9] = '\0';
  printf("Permissões: %s\n", perms);
}

/*
 * verifica se arquivo é um link simbólico (exclusivo pra linux)
 * caso seja um link simbólico, mostra o destino desse link
 */
static void mostrar_detalhes_especiais(const char *caminho) {

  /*
   * struct stat é uma estrutura pronta fornecida pelo <sys/stat.h>
   * ela serve basicamente para armazenar infos de um arquivo
   */
  struct stat file_stat;
  /* tenta armazenar as informações do arquivo passado no 'caminho' */
  if (stat(caminho, &file_stat) < 0) {
    /* se falhar, retorna -1 e dá erro */
    perror("Erro ao obter estatísticas");
    return;
  }

  /*
   * S_ISLNK é macro do C que verifica se o arquivo é um link simbólico
   * checa os bits de tipo no campo st_mode da estrutura struct stat
   */
  if (S_ISLNK(file_stat.st_mode)) {
    /* array de caracteres que armazena caminho destino do link simbólico */
    char link_target[1024];
    /*
     * caminho: caminho do link simbólico
     * link_target: buffer onde o destino será armazenado
     * sizeof(link_target) - 1: tamanho máximo de leitura
     */
    ssize_t len = readlink(caminho, link_target, sizeof(link_target) - 1);
    /*
     * se len for diferente de -1 é pq teve sucesso
     * se falhar, não exibe o destino
     */
    if (len != -1) {
      link_target[len] = '\0';
      printf("\n\033[1mLINK SIMBÓLICO:\033[0m\n");
      printf("Aponta para: %s\n", link_target);
    }
  }
}

/*
 * função principal que mostra todas as informações do arquivo
 * combina todos os detalhes em uma saída formatada
 */
void mostrar_info_arquivo(const char *dir_atual, const char *nome_arquivo) {
  char caminho[1024];

  /*
   * concatena dir_atual, uma / e nome_arquivo respeitando [1024]
   * se o valor retornado for maior que o tamanho do buffer a string será
   * cortada pq não cabe inteira no 'caminho'
   */
  if (snprintf(caminho, sizeof(caminho), "%s/%s", dir_atual, nome_arquivo) >=
      (int)sizeof(caminho)) {
    /* imprime erro caso o caminho seja muito grande */
    printf("\nCaminho muito longo!\n");
    return;
  }

  printf("\n\033[1;34m=== INFORMAÇÕES COMPLETAS DO ARQUIVO ===\033[0m\n");

  /*
   * struct stat é uma estrutura pronta fornecida pelo <sys/stat.h>
   * ela serve basicamente para armazenar infos de um arquivo
   */
  struct stat file_stat;
  /* tenta armazenar as informações do arquivo passado no 'caminho' */
  if (stat(caminho, &file_stat) < 0) {
    /* se falhar, retorna -1 e dá erro */
    perror("Erro ao obter estatísticas");
    return;
  }

  printf("\n\033[1mINFORMAÇÕES BÁSICAS:\033[0m\n");

  /* verifica o tipo do arquivo */
  printf("Nome: %s\n", nome_arquivo);
  printf("Tipo: %s\n", S_ISDIR(file_stat.st_mode)   ? "Diretório"
                       : S_ISREG(file_stat.st_mode) ? "Arquivo Regular"
                       : S_ISLNK(file_stat.st_mode) ? "Link Simbólico"
                                                    : "Tipo Especial");

  /* chama todas as funções */
  mostrar_detalhes_estruturais(caminho);
  mostrar_detalhes_fisicos(caminho);
  mostrar_detalhes_temporais(caminho);
  mostrar_detalhes_permissoes(caminho);
  mostrar_detalhes_especiais(caminho);

  printf("\n\033[1mPressione qualquer tecla para continuar...\033[0m");
  getchar();
}

/*
 * cria um arquivo fragmentado para demonstração
 * - cria arquivos temporários para ocupar espaço
 * - remove alguns para criar "buracos"
 * - cria um arquivo grande que ficará fragmentado
 */
void criar_arquivo_fragmentado(const char *dir_atual) {
  char caminho[1024];

  /*
   * concatena dir_atual, uma / e arquivo_fragmentado respeitando [1024]
   * se o valor retornado for maior que o tamanho do buffer a string será
   * cortada pq não cabe inteira no 'caminho'
   */
  if (snprintf(caminho, sizeof(caminho), "%s/arquivo_fragmentado", dir_atual) >=
      (int)sizeof(caminho)) {
    /* imprime erro caso o caminho seja muito grande */
    printf("\nCaminho muito longo!\n");
    return;
  };

  printf("\nCriando arquivo fragmentado em: %s\n", caminho);

  /*
   * declara dois buffers temp1 e temp2 de tamanho 1024 para armazenar caminhos
   * dos arquivos temporários temp1 e temp2 dentro do diretório atual
   *
   * esses arquivos temporários serão criados pra ocupar espaço no disco,
   * quebrando a área livre em blocos dispersos
   *
   * passo 0: disco totalmente livre:
   * ┌───────┬───────┬───────┬───────┬───────┐
   * │ LIVRE │ LIVRE │ LIVRE │ LIVRE │ LIVRE │
   * └───────┴───────┴───────┴───────┴───────┘
   *
   * passo 1: criar temp1 e temp2 -> ocupa espaço
   * blocos no disco após criação:
   * ┌───────┬───────┬───────┬───────┬───────┐
   * │ TEMP1 │ TEMP1 │ TEMP2 │ TEMP2 │ LIVRE │
   * └───────┴───────┴───────┴───────┴───────┘
   *
   * passo 2: deletar temp1 -> cria buraco
   * blocos no disco após deleção:
   * ┌───────┬───────┬───────┬───────┬───────┐
   * │ LIVRE │ LIVRE │ TEMP2 │ TEMP2 │ LIVRE │
   * └───────┴───────┴───────┴───────┴───────┘
   *
   * passo 3: criar arquivo grande -> espalhado nos blocos livres
   * blocos no disco após criar arquivo_fragmentado:
   * ┌───────┬───────┬───────┬───────┬───────┐
   * │ FRAG1 │ FRAG2 │ TEMP2 │ TEMP2 │ FRAG3 │
   * └───────┴───────┴───────┴───────┴───────┘
   */
  char temp1[1024], temp2[1024];
  snprintf(temp1, sizeof(temp1), "%s/temp1", dir_atual);
  snprintf(temp2, sizeof(temp2), "%s/temp2", dir_atual);

  /*
   * system(...): executa comandos no shell
   * dd if=/dev/zero: fornece fluxo infinito de caracteres nulos (0x00)
   * of=temp1 e of=temp2: especificam os nomes dos arquivos a serem criados
   * bs=1M (block size): escreve em blocos de 1 megabyte
   * count=50: cria 50 MB em cada arquivo
   * status=none: oculta a saída padrão do dd.
   */
  printf("Criando arquivos temporários para forçar fragmentação...\n");
  system("dd if=/dev/zero of=temp1 bs=1M count=50 status=none");
  system("dd if=/dev/zero of=temp2 bs=1M count=50 status=none");

  /* deleta o arquivo temp1 */
  printf("Removendo primeiro arquivo temporário...\n");
  remove(temp1);

  /*
   * cria um novo arquivo chamado arquivo_fragmentado de 70 MB
   * só tinha 50 MB contíguos livres, os outros 20 estão depois de temp2
   * o sistema de arquivos vai dividir esse arquivo em blocos separados
   */
  printf("Criando arquivo fragmentado (70MB)...\n");
  system("dd if=/dev/zero of=arquivo_fragmentado bs=1M count=70 status=none");

  /*
   * deleta o temp2, pois o arquivo_fragmentado já tá gravado, então remover o
   * temp2 não muda a fragmentação
   */
  printf("Limpando arquivos temporários...\n");
  remove(temp2);

  printf("\nArquivo fragmentado criado com sucesso!\n");
  printf("\nPressione qualquer tecla para continuar...");
  getchar();
}

/*
 * concatena o diretório atual com /arquivo_fragmentado para remover o arquivo
 * de teste do diretório
 */
void limpar_arquivos_teste(const char *dir_atual) {
  char caminho[1024];

  /*
   * concatena dir_atual, uma / e arquivo_fragmentado respeitando [1024]
   * se o valor retornado for maior que o tamanho do buffer a string será
   * cortada pq não cabe inteira no 'caminho'
   */
  if (snprintf(caminho, sizeof(caminho), "%s/arquivo_fragmentado", dir_atual) >=
      (int)sizeof(caminho)) {
    /* imprime erro caso o caminho seja muito grande */
    printf("\nCaminho muito longo!\n");
    return;
  };

  remove(caminho);
  printf("Arquivo fragmentado removido: %s\n", caminho);
}

/* atualiza o rodapé */
void atualizar_rodape_comandos() {
  printf("\n\033[1mComandos:\033[0m ");
  printf("<↑/↓> Navegar | <ENTER> Entrar no Diretório | ");
  printf("<I> Informações do Arquivo | <F> Criar Arquivo Fragmentado | <Q> "
         "Sair\n");
}