#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "interface.h"
#include "navegacao.h"
#include "comandos.h"

int main()
{
    char diretorio_atual[MAX_PATH_LEN];
    DirEntry *entries = NULL;
    int num_entradas = 0;
    int selecionado = 0;
    int sair = 0;

    // Configura o terminal para entrada direta
    configurar_terminal();

    // Obtém o diretório atual inicial
    obter_diretorio_atual(diretorio_atual, sizeof(diretorio_atual));

    // Lista o diretório atual
    num_entradas = listar_diretorio(diretorio_atual, &entries);

    // Loop principal
    while (!sair)
    {
        // Limpa a tela e exibe a interface
        limpar_tela();
        exibir_cabecalho(diretorio_atual);
        exibir_entradas(entries, num_entradas, selecionado);
        exibir_rodape();

        // Lê a entrada do usuário
        char c = getchar();

        // Processa o comando
        switch (c)
        {
        case '\033': // Tecla de escape (possivelmente seta)
            // Lê os próximos caracteres para identificar a seta
            getchar(); // Descarta o [
            switch (getchar())
            {
            case 'A': // Seta para cima
                if (selecionado > 0)
                    selecionado--;
                break;
            case 'B': // Seta para baixo
                if (selecionado < num_entradas - 1)
                    selecionado++;
                break;
            }
            break;

        case '\n': // Enter - abre o diretório/arquivo selecionado
            if (num_entradas > 0)
            {
                // Monta o caminho completo do item selecionado
                char caminho_selecionado[MAX_PATH_LEN];
                snprintf(caminho_selecionado, sizeof(caminho_selecionado), "%s/%s",
                         diretorio_atual, entries[selecionado].nome);

                // Só avança se for diretório
                if (eh_diretorio(caminho_selecionado))
                {
                    size_t needed = snprintf(NULL, 0, "%s/%s",
                                             diretorio_atual, entries[selecionado].nome);
                    if (needed >= MAX_PATH_LEN)
                    {
                        printf("\nErro: Caminho muito longo (max %d caracteres)\n", MAX_PATH_LEN - 1);
                        break;
                    }

                    // Atualiza caminho atual
                    strncpy(diretorio_atual, caminho_selecionado, MAX_PATH_LEN);

                    // Atualiza a lista do novo diretório
                    DirEntry *novas_entradas = NULL;
                    int novo_num = listar_diretorio(diretorio_atual, &novas_entradas);

                    if (novo_num >= 0)
                    {
                        free(entries);
                        entries = novas_entradas;
                        num_entradas = novo_num;
                        selecionado = 0; // Reseta seleção
                    }
                    else
                    {
                        free(novas_entradas);
                    }
                }
                else
                {
                    // Se quiser, avisa que não é diretório
                    printf("\nNão é um diretório, pressione uma tecla para continuar...");
                    getchar();
                }
            }
            break;

        case 'i': // Mostrar informações detalhadas do arquivo
            if (num_entradas > 0)
            {
                mostrar_info_arquivo(diretorio_atual, entries[selecionado].nome);
            }
            break;

        case 'f': // Criar arquivo fragmentado
            if (num_entradas > 0)
            {
                criar_arquivo_fragmentado(diretorio_atual);
                // Atualiza a lista de arquivos após criação
                DirEntry *novas_entradas = NULL;
                int novo_num = listar_diretorio(diretorio_atual, &novas_entradas);
                if (novo_num >= 0)
                {
                    free(entries);
                    entries = novas_entradas;
                    num_entradas = novo_num;
                }
                else
                {
                    free(novas_entradas);
                }
            }
            break;

        case 'q': // Sair
            limpar_arquivos_teste(diretorio_atual);
            sair = 1;
            break;
        }
    }

    // Limpeza antes de sair
    if (entries != NULL)
    {
        free(entries);
    }
    restaurar_terminal();

    return 0;
}