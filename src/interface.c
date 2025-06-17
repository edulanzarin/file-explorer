#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include "interface.h"
#include "comandos.h"

// Variáveis para armazenar configuração original do terminal
static struct termios oldt;

// Limpa a tela de forma portável
void limpar_tela()
{
    printf("\033[2J\033[H"); // Sequências de escape ANSI
}

// Exibe o cabeçalho com o diretório atual
void exibir_cabecalho(const char *diretorio_atual)
{
    printf("\033[1m"); // Fundo azul, texto em negrito
    printf("=== Explorador de Arquivos ===\n");
    printf("Diretório: %s\n", diretorio_atual);
    printf("\033[0m"); // Resetar cores
}

// Exibe as entradas do diretório com realce na seleção
void exibir_entradas(DirEntry *entries, int num_entradas, int selecionado)
{
    printf("\n");
    for (int i = 0; i < num_entradas; i++)
    {
        if (i == selecionado)
        {
            printf("\033[7m"); // Texto reverso para seleção
        }
        else if (entries[i].is_dir)
        {
            printf("\033[1;34m"); // Azul para diretórios
        }

        printf("%s", entries[i].nome);

        if (entries[i].is_dir)
        {
            printf("/");
        }

        printf("\033[0m\n"); // Reset sempre no final
    }
}

// Exibe a barra de ajuda
void exibir_rodape()
{
    atualizar_rodape_comandos(); // Usa a função de comandos
}
// Configura o terminal para modo raw
void configurar_terminal()
{
    struct termios newt;

    // Obtém configurações atuais
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;

    // Desabilita eco e modo canônico (leitura caractere a caractere)
    newt.c_lflag &= ~(ICANON | ECHO);

    // Aplica novas configurações
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

// Restaura configurações originais do terminal
void restaurar_terminal()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}