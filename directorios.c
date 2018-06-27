#include "directorios.h"
#include "ficheros.c"

struct directorio{
char nombre[60]; //En el sistema de ficheros ext2 la longitud del nombre es 255
unsigned int ninodo;
};

int extraer_camino(const char *camino, char *inicial, char *final){
	int longitud = strlen(camino);	
	int i;
	for(i = 1; (camino[i]!='/')&&(i<=longitud); i++){
		inicial[i-1] = camino[i];
	}
	int x;
	printf("\nLogitud inicial: %d\n",strlen(inicial));
	printf("\n");
	write(1,inicial,strlen(inicial)); //Inicial
	if(i == longitud + 1){
		final[0] = ' ';
		return(1); //fichero
	}
	int j;
	for(j = i; j<=longitud; j++){
		final[j-i] = camino[j];
	}
	printf("\n\nLogitud final: %d\n",strlen(inicial));
	printf("\n");
	write(1,final,strlen(final)); //Final
	printf("\n");
	return(0); //directorio
}

int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir, unsigned int *p_inodo, unsigned int *p_entrada, char reservar, unsigned char modo){
	struct directorio dir;
	struct inodo inod;
	inod = leer_inodo(*p_inodo_dir);
	int ndir = inod.tamEnBytesLog/(sizeof(struct directorio));
	int i = 0;
	int permiso = mi_read_f(*p_inodo_dir, &dir, i*sizeof(struct directorio), sizeof(struct directorio));
	char inicial[20];
	char final[20];
	memset(inicial,0,20);
	memset(final,0,20);
	int tipo = extraer_camino(camino_parcial, (char *)inicial, (char *)final);
	i++;
	



	while ((i < ndir) && (final[0] != ' ') && (permiso != -1)){
		if(strcmp(dir.nombre, (char *)inicial) != 0){
			tipo = extraer_camino(camino_parcial, (char *)inicial, (char *)final);
		}
		permiso = mi_read_f(*p_inodo_dir, &dir, i*sizeof(struct directorio), sizeof(struct directorio));
		i++;
	}
	if((i < ndir) && (permiso != -1)){
		*p_inodo = dir.ninodo;
		*p_entrada = dir.ninodo;
	}
	return(-1); //Error. No se tiene permiso de lectura o no existe
}

int mi_creat(const char *camino, unsigned char modo){
	unsigned int p_inodo_dir = 0;
	unsigned int p_inodo = 0;
	unsigned int p_entrada = 0;
	char reservar = '1';
	buscar_entrada((const char *)camino, &p_inodo_dir, &p_inodo, &p_entrada, reservar, modo);
}

int mi_dir(const char *camino, char *buffer){
	
}
