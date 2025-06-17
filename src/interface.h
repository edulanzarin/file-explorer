#ifndef INTERFACE_H
#define INTERFACE_H

#include "navegacao.h"

// Limpa a tela do terminal
void limpar_tela();

// Exibe o cabeçalho da interface
void exibir_cabecalho(const char *diretorio_atual);

// Exibe as entradas do diretório
void exibir_entradas(DirEntry *entries, int num_entradas, int selecionado);

// Exibe a barra de ajuda na parte inferior
void exibir_rodape();

// Configura o terminal para modo raw (para entrada sem esperar por Enter)
void configurar_terminal();

// Restaura as configurações originais do terminal
void restaurar_terminal();

#endif // INTERFACE_H