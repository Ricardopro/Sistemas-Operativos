#include "directorios.c"


static int iden;

unsigned  char buffer [blocksize];

char nombre_fitchero[40]; 


main(int argc, char **argv){    
	
	strcpy(nombre_fitchero,argv[1]);	
	iden= bmount(argv[1]);
	//mi_write_f(1, "Esto es una prueba\0",50000,50);
/*
	unsigned char inicial[20];
	memset(inicial, 0, 20);
	unsigned char final[20];
	memset(final, 0, 20);
*/
	char inicial[20];
	char final[20];
	memset(inicial,0,20);
	memset(final,0,20);
	int x = extraer_camino("/dir1/dir2/fichero",(char *)inicial,(char *)final);
	printf("\n\n");
	write(1,(char *)inicial,strlen((char *)inicial));
	printf("\n\n");
	write(1,(char *)final,strlen((char *)final));
	printf("\n");	
	if(x == 0){
		printf("\nDirectorio\n");
	}else if(x == 1){
		printf("\nFichero\n");
	}
}
