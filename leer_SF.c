#include "bloques.h"
#include "fitcheros_basico.h"
#include "fitcheros_basico.c"

static int iden;
unsigned  char buffer [blocksize];

char nombre_fitchero[40];  


main(int argc, char **argv){   
	
	strcpy(nombre_fitchero,argv[1]);
	iden= bmount(argv[1]);
	
	visualizarSB();
	//visualizarMB();
	//visualizarAI();
	
}
