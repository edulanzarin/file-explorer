#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "interface.h"
#include "navegacao.h"
#include "comandos.h"

int main()
{
    // Variáveis principais
    char diretorio_atual[MAX_PATH_LEN]; // Guarda onde estamos
    DirEntry *entries = NULL;           // Lista de arquivos/pastas
    int num_entradas = 0;               // Quantos itens tem na lista
    int selecionado = 0;                // Item selecionado na tela
    int sair = 0;                       // Flag pra saber quando fechar

    // Prepara o terminal pra ler teclas direto
    configurar_terminal();

    // Começa no diretório atual do programa
    obter_diretorio_atual(diretorio_atual, sizeof(diretorio_atual));

    // Pega a lista de coisas que tem nesse diretório
    num_entradas = listar_diretorio(diretorio_atual, &entries);

    // Loop principal - roda até o usuário pedir pra sair
    while (!sair)
    {
        // Limpa a tela e mostra tudo de novo
        limpar_tela();
        exibir_cabecalho(diretorio_atual);                   // Título e caminho
        exibir_entradas(entries, num_entradas, selecionado); // Lista de arquivos
        exibir_rodape();                                     // Ajuda dos comandos

        // Espera o usuário apertar alguma tecla
        char c = getchar();

        // Verifica o que foi apertado
        switch (c)
        {
        case '\033':   // Tecla especial (setas)
            getchar(); // Descarta o '['
            switch (getchar())
            {
            case 'A': // Seta pra cima
                if (selecionado > 0)
                    selecionado--;
                break;
            case 'B': // Seta pra baixo
                if (selecionado < num_entradas - 1)
                    selecionado++;
                break;
            }
            break;

        case '\n': // Enter - tenta entrar na pasta selecionada
            if (num_entradas > 0)
            {
                char caminho_selecionado[MAX_PATH_LEN];
                snprintf(caminho_selecionado, sizeof(caminho_selecionado),
                         "%s/%s", diretorio_atual, entries[selecionado].nome);

                // Só entra se for mesmo uma pasta
                if (eh_diretorio(caminho_selecionado))
                {
                    // Verifica se o caminho não é muito grande
                    size_t needed = snprintf(NULL, 0, "%s/%s",
                                             diretorio_atual, entries[selecionado].nome);
                    if (needed >= MAX_PATH_LEN)
                    {
                        printf("\nErro: Caminho muito longo (max %d caracteres)\n", MAX_PATH_LEN - 1);
                        break;
                    }

                    // Atualiza pra nova pasta
                    strncpy(diretorio_atual, caminho_selecionado, MAX_PATH_LEN);

                    // Pega os arquivos da nova pasta
                    DirEntry *novas_entradas = NULL;
                    int novo_num = listar_diretorio(diretorio_atual, &novas_entradas);

                    if (novo_num >= 0) // Se deu certo
                    {
                        free(entries); // Limpa a lista antiga
                        entries = novas_entradas;
                        num_entradas = novo_num;
                        selecionado = 0; // Seleciona o primeiro item
                    }
                    else
                    {
                        free(novas_entradas); // Se deu erro, limpa
                    }
                }
                else
                {
                    // Avisa se tentou entrar em arquivo (não pasta)
                    printf("\nNão é um diretório, pressione uma tecla para continuar...");
                    getchar();
                }
            }
            break;

        case 'i': // Mostra info do arquivo selecionado
            if (num_entradas > 0)
            {
                mostrar_info_arquivo(diretorio_atual, entries[selecionado].nome);
            }
            break;

        case 'f': // Cria arquivo fragmentado (pra teste)
            if (num_entradas > 0)
            {
                criar_arquivo_fragmentado(diretorio_atual);
                // Atualiza a lista de arquivos
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

        case 'q':                                   // Sai do programa
            limpar_arquivos_teste(diretorio_atual); // Limpa lixo de teste
            sair = 1;
            break;
        }
    }

    // Limpeza antes de fechar
    if (entries != NULL)
    {
        free(entries); // Libera a memória da lista
    }
    restaurar_terminal(); // Volta o terminal ao normal

    return 0;
}