#include "./include/comandos.h"
#include "./include/interface.h"
#include "./include/navegacao.h"

int main() {
  /* variáveis principais */
  char dir_atual[MAX_PATH_LEN]; /* guarda onde estamos */
  DirEntry *entries = NULL;     /* lista de arquivos/pastas */
  int num_entradas = 0;         /* quantos itens tem na lista */
  int selecionado = 0;          /* item selecionado na tela */
  int sair = 0;                 /* flag pra saber quando fechar */

  /*
   * começa no diretório atual
   * pega a lista de coisas que tem dentro
   */
  configurar_terminal();
  obter_dir_atual(dir_atual, sizeof(dir_atual));
  num_entradas = listar_diretorio(dir_atual, &entries);

  /* loop principal - roda até o usuário pedir pra sair "Q" */
  while (!sair) {

    limpar_tela();                                       /* limpa a tela */
    exibir_cabecalho(dir_atual);                         /* título e caminho */
    exibir_entradas(entries, num_entradas, selecionado); /* ista de arquivos */
    atualizar_rodape_comandos(); /* ajuda dos comandos */

    /* espera o apertar alguma tecla */
    char c = getchar();

    /*
     * verifica tecla a partir do código ASCII recebido
     * pressionar uma tecla de seta envia uma sequência de 3 caracteres:
     *
     *     ┌──────┬─────┬─────┐
     *     │'\033'│ '[' │ 'A' │ <- seta pra cima
     *     └──────┴─────┴─────┘
     *       ESC    CSI  Código
     *
     * - '\033' é o código ESC que sinaliza o início da sequência
     * - '[' é o "Control Sequence Introducer" (CSI) usado em ANSI
     * - 'A', 'B', 'C', 'D' identificam as setas:
     *
     *     ┌─────┬────────────┐
     *     │ cód │ seta       │
     *     ├─────┼────────────┤
     *     │ 'A' │ (cima)     │
     *     │ 'B' │ (baixo)    │
     *     │ 'C' │ (direita)  │
     *     │ 'D' │ (esquerda) │
     *     └─────┴────────────┘
     */
    switch (c) {
    case '\033':           /* ESC: início de sequência especial */
      getchar();           /* descarta o '[' */
      switch (getchar()) { /* pega o terceiro caractere */
      case 'A':            /* seta pra cima */
        if (selecionado > 0)
          selecionado--;
        break;
      case 'B': /* seta pra baixo */
        if (selecionado < num_entradas - 1)
          selecionado++;
        break;
      }
      break;

    case '\n': /* ENTER */
      if (num_entradas > 0) {
        char caminho_selecionado[MAX_PATH_LEN];
        snprintf(caminho_selecionado, sizeof(caminho_selecionado), "%s/%s",
                 dir_atual, entries[selecionado].nome);

        /* só entra se for mesmo uma pasta */
        if (eh_diretorio(caminho_selecionado)) {
          /* verifica se o caminho não é muito grande */
          size_t needed =
              snprintf(NULL, 0, "%s/%s", dir_atual, entries[selecionado].nome);
          if (needed >= MAX_PATH_LEN) {
            printf("\nErro: Caminho muito longo max %d caracteres\n",
                   MAX_PATH_LEN - 1);
            break;
          }

          /*
           * atualiza pra nova pasta
           * strncpy copia string com limite de segurança "MAX_PATH_LEN"
           * para evitar estouro de buffer
           */
          strncpy(dir_atual, caminho_selecionado, MAX_PATH_LEN);

          /*
           * cria um ponteiro para vetor DirEntry que começa NULL
           * pq a função listar_diretorio() preenche esse ponteiro
           */
          DirEntry *novas_entradas = NULL;
          int novo_num = listar_diretorio(dir_atual, &novas_entradas);

          if (novo_num >= 0) /* se deu certo >= 0 */
          {
            free(entries);            /* limpa lista antiga */
            entries = novas_entradas; /* atualiza ponteiro */
            num_entradas = novo_num;  /* atualiza qtd de entradas */
            selecionado = 0;          /* seleciona o primeiro item */
          } else {
            free(novas_entradas); /* se deu erro < 0 */
          }
        } else {
          /* caso não seja uma pasta */
          printf("\nNão é um diretório, pressione uma tecla para continuar...");
          getchar();
        }
      }
      break;

    /*
     * chama a função geral de "comandos.c" que mostra as informações
     * do arquivo selecionado
     */
    case 'i': /* tecla "I" */
      if (num_entradas > 0) {
        mostrar_info_arquivo(dir_atual, entries[selecionado].nome);
      }
      break;

    /* criar arquivo fragmentado para teste */
    case 'f': /* tecla "F" */
      if (num_entradas > 0) {
        criar_arquivo_fragmentado(dir_atual);
        /* atualiza a lista de arquivos novamente para mostrar o arquivo
         * fragmentado que foi criado
         */
        DirEntry *novas_entradas = NULL;
        int novo_num = listar_diretorio(dir_atual, &novas_entradas);
        if (novo_num >= 0) {
          free(entries);
          entries = novas_entradas;
          num_entradas = novo_num;
        } else {
          free(novas_entradas);
        }
      }
      break;

    case 'q':                           /* sai do programa */
      limpar_arquivos_teste(dir_atual); /* limpa arquivo de teste */
      sair = 1;
      break;
    }
  }

  /* verifica se a lista entries foi alocada e libera ela */
  if (entries != NULL) {
    free(entries);
  }
  restaurar_terminal();

  return 0;
}