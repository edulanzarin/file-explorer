#ifndef COMANDOS_H
#define COMANDOS_H

/* Mostra informações detalhadas sobre um arquivo
   - Permissões, tamanho, datas de acesso
   - Detalhes físicos como blocos e fragmentação
   - Informações do inode e dono do arquivo
*/
void mostrar_info_arquivo(const char *diretorio_atual, const char *nome_arquivo);

/* Atualiza a mensagem de ajuda na parte inferior da tela
   Exibe os comandos disponíveis e suas teclas correspondentes
*/
void atualizar_rodape_comandos();

/* Cria um arquivo propositalmente fragmentado no disco
   Usado para demonstrar como o sistema gerencia arquivos fragmentados
*/
void criar_arquivo_fragmentado(const char *diretorio_atual);

/* Remove os arquivos criados para testes
   Limpeza dos arquivos temporários e fragmentados criados
*/
void limpar_arquivos_teste(const char *diretorio_atual);

#endif // COMANDOS_H