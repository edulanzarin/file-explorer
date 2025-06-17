#ifndef COMANDOS_H
#define COMANDOS_H

#include <fcntl.h>
#include <linux/fiemap.h> // Para mapeamento de extents
#include <linux/fs.h>     // Para FIBMAP (mapeamento de blocos)
#include <linux/magic.h>  // Números mágicos de sistemas de arquivos
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/vfs.h>
#include <time.h>
#include <unistd.h>

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