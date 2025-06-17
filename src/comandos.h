#ifndef COMANDOS_H
#define COMANDOS_H

/* Flag que indica quando um arquivo está "inline" no sistema de arquivos
   (ou seja, os dados estão guardados direto no inode, sem usar blocos separados)
   Usado quando verificamos como o arquivo está armazenado fisicamente */
#define FIEMAP_EXTENT_INLINE 0x00000080

/* Número máximo de extents que vamos verificar quando analisamos */
#define MAX_EXTENTS 512

/* Funções de comandos implementadas em comandos.c */
void mostrar_info_arquivo(const char *diretorio_atual, const char *nome_arquivo); /* Tecla "I" */
void criar_arquivo_fragmentado(const char *diretorio_atual);                      /* Tecla "F" */
void limpar_arquivos_teste(const char *diretorio_atual);                          /* Usada na tecla "Q" */
void atualizar_rodape_comandos();

#endif