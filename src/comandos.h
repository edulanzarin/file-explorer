#ifndef COMANDOS_H
#define COMANDOS_H

/* 0x00000080 = 10000000
 * só o oitavo bit tá ligado, o kernel usa isso pra saber que os dados são
 * inline
 */
#define FIEMAP_EXTENT_INLINE 0x00000080 /* armazenado no próprio inode */
#define MAX_EXTENTS 512                 /* número máximo de extents */

/* funções de comandos implementadas em comandos.c */
void mostrar_info_arquivo(const char *dir_atual,
                          const char *nome_arquivo);   /* tecla "I" */
void criar_arquivo_fragmentado(const char *dir_atual); /* tecla "F" */
void limpar_arquivos_teste(const char *dir_atual);     /* usada na tecla "Q" */
void atualizar_rodape_comandos();

#endif