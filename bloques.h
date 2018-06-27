#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> /* Modos de apertura y función open()*/
#include <stdlib.h> /* Funciones write() y close() */
#include <string.h>
#include <time.h>

#define blocksize 1024 //bytes
#define posSB 0 //el superbloque se escribe en el primer bloque de nuestro FS
#define tamSB 1
#define T_INODO 128 //tamaño en bytes de un inodo

int bmount(const char *camino);
int bumount();
int bwrite(unsigned int bloque,const void *buf);
int bread(unsigned int bloque,void *buf);
