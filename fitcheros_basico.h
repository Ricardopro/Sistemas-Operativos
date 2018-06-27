#include "bloques.h"

int tamMB (unsigned int nbloques);
int tamAI (unsigned int ninodos);
int initSB(unsigned int nbloques, unsigned int ninodos);
int initMB(unsigned int nbloques);
int initAI(unsigned int ninodos);

int escribir_bit(unsigned int nbloque, unsigned int bit);
char leer_bit(unsigned int nbloque);
int reservar_bloque();
int liberar_bloque(unsigned int nbloque);
//int escribir_inodo(struct inodo inod, unsigned int ninodo);
struct inodo leer_inodo(unsigned int ninodo);
int reservar_inodo(unsigned char tipo, unsigned char permisos);
int liberar_inodo(unsigned int inodo);
