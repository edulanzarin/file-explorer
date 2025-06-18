#ifndef COMANDOS_H
#define COMANDOS_H

#include <stdio.h>         /* entrada e saída padrão */
#include <stdlib.h>        /* utilitários gerais */
#include <string.h>        /* manipulação de strings e memória */
#include <fcntl.h>         /* funções para abrir arquivos */
#include <unistd.h>        /* chamadas POSIX */
#include <time.h>          /* manipulação de tempo e datas */
#include <sys/ioctl.h>     /* controle avançado de arquivos */
#include <sys/stat.h>      /* obter metadados de arquivos */
#include <sys/sysmacros.h> /* major() e minor() */
#include <sys/vfs.h>       /* informações do sistema de arquivos */
#include <linux/fiemap.h>  /* mapeamento de extents */
#include <linux/fs.h>      /* mapeamento de blocos */
#include <linux/magic.h>   /* números mágicos de sistemas de arquivos */

/*
 * define FIEMAP_EXTENT_INLINE como 0x00000080
 * binário: apenas o 8º bit está ligado
 *
 * representação dos 8 bits (1 byte):
 *
 *  bit:     7   6   5   4   3   2   1   0
 *         ┌───┬───┬───┬───┬───┬───┬───┬───┐
 *  valor: │ 1 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │ 0 │
 *         └───┴───┴───┴───┴───┴───┴───┴───┘
 *           ↑
 *           FIEMAP_EXTENT_INLINE = bit 7 ligado
 *
 * os dados estão armazenados dentro do próprio inode e não em blocos separados
 * usado para arquivos muito pequenos que cabem direto na estrutura do inode
 */
#define FIEMAP_EXTENT_INLINE 0x00000080

/*
 * define MAX_EXTENTS como 512
 * geralmente cada extent ocupa entre 32 a 64 bytes
 *  ┌───────────────────────────────────────────┐
 *  │ 512 extents × 64 bytes = 32 KB de memória │
 *  └───────────────────────────────────────────┘
 * usar muitos extents aumenta o consumo de memória
 * mas permite mapear arquivos maiores fragmentados em muitos blocos
 */

#define MAX_EXTENTS 512

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