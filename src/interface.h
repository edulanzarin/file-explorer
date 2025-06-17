#ifndef INTERFACE_H
#define INTERFACE_H

#include "navegacao.h"

void limpar_tela();

void exibir_cabecalho(const char *diretorio_atual);

/* Mostra a lista de arquivos/pastas do diretório
 * Recebe as entradas, quantas tem e qual tá selecionada pra destacar
 */
void exibir_entradas(DirEntry *entries, int num_entradas, int selecionado);

void exibir_rodape();

/* Muda o terminal pro modo raw, pra pegar entrada na hora, sem esperar Enter */
void configurar_terminal();

/* Volta o terminal pro jeito normal depois que a gente mexeu */
void restaurar_terminal();

#endif // INTERFACE_H
