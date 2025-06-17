#ifndef COMANDOS_H
#define COMANDOS_H

#define FIEMAP_EXTENT_INLINE 0x00000080 /* armazenado no próprio inode */
#define MAX_EXTENTS 512                 /* número máximo de extents */

/* Funções de comandos implementadas em comandos.c */
void mostrar_info_arquivo(const char *dir_atual,
                          const char *nome_arquivo);   /* Tecla "I" */
void criar_arquivo_fragmentado(const char *dir_atual); /* Tecla "F" */
void limpar_arquivos_teste(const char *dir_atual);     /* Usada na tecla "Q" */
void atualizar_rodape_comandos();

#endif