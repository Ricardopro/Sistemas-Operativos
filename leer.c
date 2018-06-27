#include "ficheros.c"

static int iden;

unsigned  char buffer [blocksize];

char nombre_fitchero[40]; 


main(int argc, char **argv){

	strcpy(nombre_fitchero,argv[1]);	
	iden= bmount(argv[1]);
	unsigned char buf_original[2000];
	memset(buf_original,0,2000);
	unsigned char buf_original2[2000];
	memset(buf_original2,0,2000);
	int i;
	for(i=0; i<2000; i++){
		buf_original[i] = i;
	}
	unsigned char buf_original3[2000];
	memset(buf_original,0,2000);
	mi_write_f(2,"pea bla bla blub", 2000, 2);
	mi_read_f(2,&buf_original2, 2000, 20);

	printf("\nHa salido\n");
	write(1,buf_original2,strlen(buf_original2));
	//for(i=0; i<10000; i++){
	//	printf(",%d", reservar_bloque());
	//}
/*
	for(i=0; i<10000; i++){
		//escribir_bit(i, 1);
		printf(",%d",leer_bit(i));
	}
*/
	printf("\n");

	//visualizarSB();

	//escribir_bit(8192, 1);
	//printf("bitpp: %d\n",leer_bit(3139));
	//printf("bitpp: %d\n",leer_bit(3138));
	//escribir_bit(8193, 1);
	//printf("bit: %d\n",leer_bit(8193));

/*
	strcpy(nombre_fitchero,argv[1]);	
	iden= bmount(argv[1]);

	struct STAT stats;
	mi_chmod_f(50, 'r');
	mi_stat_f(50, &stats);
	printf("Tipo de inodo: %c\n",stats.tipo);
        printf("Permiso de inodo: %c\n",stats.permisos);
        printf("atime: %ld\n",stats.atime);
        printf("mtime: %ld\n",stats.mtime);
        printf("ctime: %ld\n",stats.ctime);
        printf("nlinks: %d\n",stats.nlinks);
        printf("tamEnBytesLog: %d\n",stats.tamEnBytesLog);
*/
}
