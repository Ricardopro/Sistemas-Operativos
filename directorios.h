#include "bloques.h"

int extraer_camino(const char *camino, char *inicial, char *final);
int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char modo);
int mi_creat(const char *camino, unsigned char modo);
int mi_dir(const char *camino, char *buffer);

