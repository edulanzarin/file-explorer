#ifndef COMANDOS_H
#define COMANDOS_H

#include <fcntl.h>         /* funções para abrir arquivos */
#include <linux/fiemap.h>  /* mapeamento de extents */
#include <linux/fs.h>      /* mapeamento de blocos */
#include <linux/magic.h>   /* números mágicos de sistemas de arquivos */
#include <stdio.h>         /* entrada e saída padrão */
#include <stdlib.h>        /* utilitários gerais */
#include <string.h>        /* manipulação de strings e memória */
#include <sys/ioctl.h>     /* controle avançado de arquivos */
#include <sys/stat.h>      /* obter metadados de arquivos */
#include <sys/sysmacros.h> /* major() e minor() */
#include <sys/vfs.h>       /* informações do sistema de arquivos */
#include <time.h>          /* manipulação de tempo e datas */
#include <unistd.h>        /* chamadas POSIX */

/* 0x00000080 = 10000000
 * só o oitavo bit tá ligado, o kernel usa isso pra saber que os dados são
 * inline
 */
#define FIEMAP_EXTENT_INLINE 0x00000080 /* armazenado no próprio inode */

/*
 * tamanho geralmente de 32 a 64 bytes
 * a quantidade de memória para 512 extents seria de aproximadamente
 * 512 * 64 = 32768 bytes
 */
#define MAX_EXTENTS 512 /* número máximo de extents */

/*
 * ~0ULL é o maior valor possível para um número 64-bit.
 * fala pro kernel mapear o arquivo do início até o fim
 */
#define FIEMAP_MAX_OFFSET (~0ULL)

/* funções de comandos implementadas em comandos.c */
void mostrar_info_arquivo(const char *dir_atual,
                          const char *nome_arquivo);   /* tecla "I" */
void criar_arquivo_fragmentado(const char *dir_atual); /* tecla "F" */
void limpar_arquivos_teste(const char *dir_atual);     /* usada na tecla "Q" */
void atualizar_rodape_comandos();

#endif