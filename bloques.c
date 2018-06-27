#include "bloques.h"
static int fitchero = 0; //descritor de fitchero fd


int bmount(const char *camino){  
	fitchero = open(camino, O_RDWR|O_CREAT, 0777);
	if( fitchero ==-1) {
		printf("error en abrir archivo");			
	}
	return (fitchero);
}

int bumount(){
	return(close(fitchero));
}

int bwrite(unsigned int bloque,const void *buf){
	int b = lseek(fitchero, bloque*blocksize, SEEK_SET);
	int a = write(fitchero,buf, blocksize);

	return (a); // a=-1 si ha ocurrido un error de seek.
}

int bread(unsigned int bloque,void *buf){
	int b = lseek(fitchero, bloque*blocksize, SEEK_SET);
	int a = read(fitchero, buf, blocksize);

	return (a);
}
