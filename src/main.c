#include "comandos.h"
#include "interface.h"
#include "navegacao.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  // variáveis principais
  char dir_atual[MAX_PATH_LEN]; // guarda onde estamos
  DirEntry *entries = NULL;     // lista de arquivos/pastas
  int num_entradas = 0;         // quantos itens tem na lista
  int selecionado = 0;          // item selecionado na tela
  int sair = 0;                 // flag pra saber quando fechar

  // prepara o terminal pra ler teclas direto
  configurar_terminal();

  // começa no diretório atual do programa
  obter_dir_atual(dir_atual, sizeof(dir_atual));

  // pega a lista de coisas que tem nesse diretório
  num_entradas = listar_diretorio(dir_atual, &entries);

  // loop principal - roda até o usuário pedir pra sair
  while (!sair) {
    // limpa a tela e mostra tudo de novo
    limpar_tela();
    exibir_cabecalho(dir_atual);                         // título e caminho
    exibir_entradas(entries, num_entradas, selecionado); // lista de arquivos
    atualizar_rodape_comandos();                         // ajuda dos comandos

    // espera o usuário apertar alguma tecla
    char c = getchar();

    // verifica o que foi apertado
    switch (c) {
    case '\033': // tecla especial (setas)
      getchar(); // descarta o '['
      switch (getchar()) {
      case 'A': // seta pra cima
        if (selecionado > 0)
          selecionado--;
        break;
      case 'B': // seta pra baixo
        if (selecionado < num_entradas - 1)
          selecionado++;
        break;
      }
      break;

    case '\n': // enter - tenta entrar na pasta selecionada
      if (num_entradas > 0) {
        char caminho_selecionado[MAX_PATH_LEN];
        snprintf(caminho_selecionado, sizeof(caminho_selecionado), "%s/%s",
                 dir_atual, entries[selecionado].nome);

        // só entra se for mesmo uma pasta
        if (eh_diretorio(caminho_selecionado)) {
          // verifica se o caminho não é muito grande
          size_t needed =
              snprintf(NULL, 0, "%s/%s", dir_atual, entries[selecionado].nome);
          if (needed >= MAX_PATH_LEN) {
            printf("\nErro: Caminho muito longo (max %d caracteres)\n",
                   MAX_PATH_LEN - 1);
            break;
          }

          // atualiza pra nova pasta
          strncpy(dir_atual, caminho_selecionado, MAX_PATH_LEN);

          // pega os arquivos da nova pasta
          DirEntry *novas_entradas = NULL;
          int novo_num = listar_diretorio(dir_atual, &novas_entradas);

          if (novo_num >= 0) // se deu certo
          {
            free(entries); // limpa a lista antiga
            entries = novas_entradas;
            num_entradas = novo_num;
            selecionado = 0; // seleciona o primeiro item
          } else {
            free(novas_entradas); // se deu erro, limpa
          }
        } else {
          // avisa se tentou entrar em arquivo (não pasta)
          printf("\nNão é um diretório, pressione uma tecla para continuar...");
          getchar();
        }
      }
      break;

    case 'i': // mostra info do arquivo selecionado
      if (num_entradas > 0) {
        mostrar_info_arquivo(dir_atual, entries[selecionado].nome);
      }
      break;

    case 'f': // cria arquivo fragmentado (pra teste)
      if (num_entradas > 0) {
        criar_arquivo_fragmentado(dir_atual);
        // atualiza a lista de arquivos
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

    case 'q':                           // sai do programa
      limpar_arquivos_teste(dir_atual); // limpa arquivo de teste
      sair = 1;
      break;
    }
  }

  // limpeza antes de fechar
  if (entries != NULL) {
    free(entries); // libera a memória da lista
  }
  restaurar_terminal(); // volta o terminal ao normal

  return 0;
}