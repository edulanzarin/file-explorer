#ifndef INTERFACE_H
#define INTERFACE_H

#include "navegacao.h"

/* Limpa a tela do terminal */
void limpar_tela();

/* Mostra o título e o diretório atual no topo */
void exibir_cabecalho(const char *diretorio_atual);

/* Mostra a lista de arquivos e pastas
   - entries: lista de itens do diretório
   - num_entradas: quantos itens tem
   - selecionado: qual item tá marcado (em destaque)
*/
void exibir_entradas(DirEntry *entries, int num_entradas, int selecionado);

/* Mostra a barra de comandos embaixo */
void exibir_rodape();

/* Liga o modo "raw" do terminal:
   - Teclas são lidas na hora (sem esperar Enter)
   - Não mostra o que digitamos
*/
void configurar_terminal();

/* Volta o terminal ao normal quando o programa fecha */
void restaurar_terminal();

#endif // INTERFACE_H