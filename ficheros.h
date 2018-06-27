#include "bloques.h"



int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes);
int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes);
int mi_chmod_f(unsigned int ninodo, unsigned char modo);
int mi_truncar_f(unsigned int ninodo, unsigned int nbytes);
//int mi_stat_f(unsigned int ninodo, struct STAT *p_stat);
