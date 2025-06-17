#ifndef INTERFACE_H
#define INTERFACE_H

/* includes necessários que são usados em interface.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

/* navegacao.h importado para utilizar a estrutura DirEntry na função
 * exibir_entradas() para mostrar os arquivos/diretórios na interface
 */
#include "navegacao.h"

/* Funções de interface implementadas em interface.c */
void limpar_tela();
void exibir_cabecalho(const char *dir_atual);
void exibir_entradas(DirEntry *entries, int num_entradas, int selecionado);
void configurar_terminal();
void restaurar_terminal();

#endif