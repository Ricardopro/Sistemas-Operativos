#include "mi_mkfs.h"
#include "fitcheros_basico.c"
	//argc=3 //comprobar si son 3
	//argv: vector de punteros a los parámetros
	//argv[0]="mi_mkfs"
	//argv[1]=nombre_fichero
	//argv[2]=cantidad_bloques (puede sernos útil la función atoi)  //convertir a entero con atoi
	//char *nombre_fitchero;
	//static int cantidad_bloques;

static int iden;
unsigned  char buffer [blocksize];

char nombre_fitchero[40];  //aqui mejor poner * haciendo reserva memoria dinamica
unsigned int cantidad_bloques;  //nbloques

main(int argc, char **argv){    //array de punteros, 4 valor vale null
	if (argc!=3){
		puts("error en paso de parametros,tiene que haber 3\n");
	 	//violacion de segmento??
	}else{

		cantidad_bloques=atoi(argv[2]);
		strcpy(nombre_fitchero,argv[1]);
	
		printf("El nombre de fitchero es: %s\n" ,nombre_fitchero);
		printf("El numero de bloques  es: %d\n", cantidad_bloques);
	
		iden= bmount(argv[1]);
		printf("El numero de identificador es: %d\n", iden);

	if( iden<0) {
		printf("error en abrir archivo \n");
	}
	int a;
	memset(buffer, 0, blocksize); //Llenar buffer con 0's.
	for (a=0; a < cantidad_bloques;a++){
		 bwrite(a,buffer); //Inicializar a 0 todos los bloques.
	}

	initSB(cantidad_bloques, cantidad_bloques/4);
	initMB(cantidad_bloques);
	initAI(cantidad_bloques/4);

	}
}
