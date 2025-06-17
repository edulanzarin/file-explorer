#ifndef COMANDOS_H
#define COMANDOS_H

/* Mostra info do arquivo (permissões, tamanho, inode e tal)
 * Usado quando a gente aperta "i" num arquivo pra ver detalhes
 */
void mostrar_info_arquivo(const char *diretorio_atual, const char *nome_arquivo);

/* Atualiza o rodapé com os comandos que estão disponíveis.
 * Sempre que a tela muda, a gente chama isso pra deixar o rodapé certinho
 */
void atualizar_rodape_comandos();

/* Cria um arquivo fragmentado de propósito pra testar como o disco guarda ele */
void criar_arquivo_fragmentado(const char *diretorio_atual);

/* Apaga os arquivos de teste que foram criados (tipo o fragmentado) */
void limpar_arquivos_teste(const char *diretorio_atual);

#endif // COMANDOS_H
