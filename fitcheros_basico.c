#include "fitcheros_basico.h"

struct superbloque{
unsigned int posPrimerBloqueMB; //Posición del primer bloque del mapa de bits
unsigned int posUltimoBloqueMB; //Posición del último bloque del mapa de bits
unsigned int posPrimerBloqueAI; //Posición del primer bloque del array de inodos
unsigned int posUltimoBloqueAI; //Posición del último bloque del array de inodos
unsigned int posPrimerBloqueDatos; //Posición del primer bloque de datos
unsigned int posUltimoBloqueDatos; //Posición del último bloque de datos
unsigned int posInodoRaiz; //Posición del inodo del directorio raíz
unsigned int posPrimerInodoLibre; //Posición del primer inodo libre
unsigned int cantBloquesLibres; //Cantidad de bloques libres
unsigned int cantInodosLibres; //Cantidad de inodos libres
unsigned int totBloques; //Cantidad total de bloques
unsigned int totInodos; //Cantidad total de inodos
char padding[blocksize-12*sizeof(unsigned int)]; //Relleno
};

struct inodo{
unsigned char tipo; //Tipo (libre, directorio o fichero)
unsigned char permisos; //Permisos (lectura y/o escritura y/o ejecución)
unsigned char reservado_alineacion[2];
time_t atime; //Fecha y hora del último acceso a datos: atime
time_t mtime; //Fecha y hora de la última modificación de datos: mtime
time_t ctime; //Fecha y hora de la última modificación del inodo: ctime
unsigned int nlinks; //Cantidad de enlaces de entradas en directorio
unsigned int tamEnBytesLog; //Tamaño en bytes lógicos
unsigned int numBloquesOcupados; //Cantidad de bloques ocupados en la zona de datos
unsigned int punterosDirectos[12]; //12 punteros a bloques directos
unsigned int punterosIndirectos[3]; /*3 punteros a bloques indirectos:1 puntero indirecto simple, 1 puntero indirecto doble, 1 puntero indirecto triple */
char padding[T_INODO-88];
};

int tamMB(unsigned int nbloques){
	int a=(nbloques/8)/blocksize;
	//printf("tamaño MBB %d\n",a);

	if (((nbloques/8) % blocksize)==0){
		return(((nbloques/8)/blocksize));
	}else{
		return(((nbloques/8)/blocksize)+1);
	}
}

int tamAI(unsigned int ninodos){
	//printf("ninodos %d\n", ninodos);
	if (((ninodos * T_INODO) % blocksize)==0){
		//printf("Tamaño AI: %d\n", ((ninodos * T_INODO) / blocksize));
		return((ninodos * T_INODO) / blocksize);
	}else{
		//printf("Tamaño AI: %d\n", ((ninodos * T_INODO) / blocksize)+1);
		return(((ninodos * T_INODO) / blocksize)+1);
	}
}

int initSB(unsigned int nbloques, unsigned int inodos){
	struct superbloque SB;

	SB.posPrimerBloqueMB = posSB + tamSB;
	SB.posUltimoBloqueMB = SB.posPrimerBloqueMB + tamMB(nbloques) - 1;
	SB.posPrimerBloqueAI = SB.posUltimoBloqueMB + 1;
	SB.posUltimoBloqueAI = SB.posPrimerBloqueAI + tamAI(inodos) - 1;
	SB.posPrimerBloqueDatos = SB.posUltimoBloqueAI + 1;
	SB.posUltimoBloqueDatos = nbloques - 1;
	SB.posInodoRaiz = 0;
	SB.posPrimerInodoLibre = 1;
	SB.cantBloquesLibres = nbloques;
	SB.cantInodosLibres = inodos - 1; //inodo raiz no incluido

	SB.totBloques = nbloques;
	SB.totInodos = inodos;

	int i;
	for(i = 0; i < blocksize-12*sizeof(unsigned int); i++){
		SB.padding[i] = 0;
	}
	/*
	printf("\n**INFORMACION SUPERBLOQUE**\n");
        printf("Posición del primer bloque del mapa de bits: %d\n",SB.posPrimerBloqueMB);    
        printf("Posición del último bloque del mapa de bits: %d\n",SB.posUltimoBloqueMB);
        printf("Posición del primer bloque del array de inodos: %d\n",SB.posPrimerBloqueAI);
        printf("Posición del último bloque del array de inodos: %d\n",SB.posUltimoBloqueAI);
        printf("Posición del primer bloque de datos: %d\n",SB.posPrimerBloqueDatos);
        printf("Posición del último bloque de datos: %d\n",SB.posUltimoBloqueDatos);
        printf("Posición del inodo del directorio raíz: %d\n",SB.posInodoRaiz);
        printf("Posición del primer inodo libre: %d\n",SB.posPrimerInodoLibre);
        printf("Cantidad de bloques libres: %d\n",SB.cantBloquesLibres);
        printf("Cantidad de inodos libres: %d\n",SB.cantInodosLibres);
        printf("Cantidad total de bloques: %d\n",SB.totBloques);
        printf("Cantidad total de inodos: %d\n",SB.totInodos);	
	*/
	return (bwrite(posSB,&SB)); //unsigned char*
}

int initMB(unsigned int nbloques){
	unsigned  char buffer [blocksize];
	memset(buffer, 0, blocksize); //Llenar buffer con 0's.
	
	int completo = 0;
	if (((nbloques/8) % blocksize)==0){
		completo = -1;
	}
	int a= tamMB(nbloques);
	int i;
	for(i = 0; i < a-1; i++){
		bwrite(i+1,buffer);
	}
	if(completo == 0){
		bwrite(i+1,buffer);
	}else{
		memset(buffer, 255, blocksize);
		int x = nbloques % blocksize;
		int y = x % 8;
		int j;
		for(j = 0; j < x/8; j++){
			memset(&buffer[j], 0, sizeof(unsigned char));
		}
		if(0 < y < 8){
			memset(&buffer[j], (255 - 2^(y-1)), sizeof(unsigned char));
		}
		bwrite(i+1,buffer);
	}

	struct superbloque sb;
	bread(posSB,&sb);
	
	int p;
	int z;
	for(z = posSB; z < sb.posUltimoBloqueMB; z++){
		p = escribir_bit(z,1);
		//printf("Init MB posicion: %d\n",z);
		sb.cantBloquesLibres--;
	}
	//printf("Cantidad de bloques libres: %d\n",sb.cantBloquesLibres);
	bwrite(posSB,&sb);
}

int initAI(unsigned int ninodos){
	struct superbloque sb;
	char buffer[blocksize];
	bread(posSB,&sb);
	int primerBloqueAI = sb.posPrimerBloqueAI;
	int ultimoBloqueAI = sb.posUltimoBloqueAI;
	memset(buffer, 0, blocksize);

	int i;
	for(i=primerBloqueAI; i < ultimoBloqueAI; i++){
		bwrite(i,buffer);
	}

	int bloque;
	int inod;
	int punteroSiguiente = 0;
	struct inodo arrayIno[blocksize/T_INODO];


	for (bloque = sb.posPrimerBloqueAI ; bloque <= sb.posUltimoBloqueAI ; bloque++) {
		bread(bloque,&arrayIno);
		for (inod = 0 ; inod < blocksize/T_INODO ; inod++) {
			arrayIno[inod].punterosDirectos[1] = punteroSiguiente + 1;
			arrayIno[inod].tipo = 'l';
			arrayIno[inod].permisos = '7';
			
			arrayIno[inod].atime = time(NULL);
			arrayIno[inod].mtime = time(NULL);
			arrayIno[inod].ctime = time(NULL);

			punteroSiguiente++;
			//printf("Posicion: %d\n",punteroSiguiente);
		}
		bwrite(bloque,&arrayIno);
	}
	bread(sb.posPrimerBloqueAI,&arrayIno);
	arrayIno[0].tipo = 'd';
	bwrite(sb.posPrimerBloqueAI,&arrayIno);

	//Ultimo inodo apunta a maximo inodos	

	sb.cantInodosLibres = punteroSiguiente - 1;
	sb.totInodos = punteroSiguiente;
	//printf("Cantidad nueva de inodos libres: %d\n",sb.cantInodosLibres);
	//printf("Cantidad total nueva de inodos libres: %d\n",sb.totInodos);

	int p;
	int z;
	for(z = sb.posPrimerBloqueAI; z < sb.posUltimoBloqueAI; z++){
		p = escribir_bit(z,1);
		sb.cantBloquesLibres--;
	}

	bwrite(posSB,&sb);
	//printf("Cantidad de bloques libres: %d\n",sb.cantBloquesLibres);
}



int escribir_bit(unsigned int nbloque, unsigned int bit){
	struct superbloque sb;
	unsigned char buffer [blocksize];
	bread(posSB,&sb);

	int pos_byte = nbloque / 8;
	int pos_bit = nbloque % 8;
	int bloque = (pos_byte / blocksize) + sb.posPrimerBloqueMB;
	int pos_aux = pos_byte % blocksize;
	/*printf("pos_byte: %d\n",pos_byte);
	printf("pos_bit: %d\n",pos_bit);
	printf("bloque: %d\n",bloque);
	*/
	bread(bloque , buffer);
	unsigned char mascara = 128;
	unsigned char mascara2 = 255; //Todo 1's
	mascara >>= pos_bit;
	
	if(bit==1){
		buffer[pos_aux] |= mascara;
		//printf(": %d\n",mascara);
	}
	if(bit==0){
		mascara ^= mascara2;
		buffer[pos_byte] &= mascara;
	}

	bwrite(bloque , buffer);
	return(1);
}

char leer_bit(unsigned int nbloque){
	//nbloque--;
	struct superbloque sb;
	unsigned char buffer [blocksize];
	bread(posSB,&sb);

	int pos_byte = nbloque / 8;
	int pos_bit = nbloque % 8;
	int bloque = (pos_byte / blocksize) + sb.posPrimerBloqueMB;
	int pos_aux = pos_byte % blocksize;
	unsigned char resultado = 128;
	resultado >>= pos_bit;
	resultado &= buffer[pos_aux];
	resultado >>= (7 - pos_bit);

	//printf("Valor de bit leido: %u\n",resultado);
	return(resultado);
}
int liberar_bloque(unsigned int nbloque){
	struct superbloque sb;
	bread(posSB,&sb);
	if(nbloque !=0){     
		if(sb.posPrimerBloqueDatos <= nbloque <= sb.posUltimoBloqueDatos){
			if(leer_bit(nbloque) == '1'){
				escribir_bit(nbloque,0);
				struct superbloque sb;
				bread(posSB, &sb);
				sb.cantBloquesLibres++;
				bwrite(posSB, &sb);
			}	
		}
	}
}

int reservar_bloque(){
	struct superbloque sb;
	bread(posSB,&sb);
	char libre;
	if(sb.cantBloquesLibres > 0){
		int primerBloqueMB = sb.posPrimerBloqueMB;
		int ultimoBloqueMB = sb.posUltimoBloqueMB;
		unsigned char buffer[blocksize];
		memset (buffer, 0, blocksize);
		unsigned char bufferAUX[blocksize];
		memset (bufferAUX, 255, blocksize);
		int posBloque;
		for(posBloque = primerBloqueMB; (posBloque <= ultimoBloqueMB); posBloque++){
			
			bread(posBloque,buffer);
			
			if(0 < memcmp(bufferAUX, buffer, blocksize)){ //memcmp devuelve mayor que 0 si el primer buffer es mayor que el segundo
				
				unsigned char mascara = 128;
				int byte=0;
				for(byte = 0; buffer[byte] == 255; byte++){}
				int valor = buffer[byte]; //Cuando sale del for anterior, en 'valor' se encuentra un byte < 255 (no todos los bits son '1')
				int bit = 0;
				while(valor & mascara){
					bit++;
					valor<<=1;
				}
				sb.cantBloquesLibres--;
				bwrite(posSB,&sb);
				escribir_bit(((posBloque-primerBloqueMB)*blocksize*8)+(byte*8)+bit,1);
				/*printf("\nposBloque: %d\n",posBloque);
				printf("primerBloqueMB: %d\n",primerBloqueMB);
				printf("byte: %d\n",byte);
				printf("bit: %d\n",bit);
				*/return(((posBloque-primerBloqueMB)*blocksize*8)+(byte*8)+bit);
			}			
		}
	}
	return(-1); //No hay bloques libres
}

struct inodo leer_inodo(unsigned int ninodo){
	struct inodo arrayIno[blocksize/sizeof(struct inodo)];
	struct inodo inod;
	struct superbloque sb;
	bread(posSB , &sb);

	int primeraPosicion = sb.posPrimerBloqueAI;
	int ultimaPosicion = sb.posUltimoBloqueAI;
	int posicionBloque = ninodo / (blocksize/sizeof(struct inodo));
	int posicionInodo = ninodo % (blocksize/sizeof(struct inodo));

	if(posicionBloque < ultimaPosicion){ //El inodo que se quiere leer está dentro del array de inodos
		bread(primeraPosicion + posicionBloque , &arrayIno);
		inod = arrayIno[posicionInodo];	//(blocksize/sizeof(struct inodo)) *

		arrayIno[posicionInodo].atime = time(NULL);

		bwrite(primeraPosicion + posicionBloque , &arrayIno);			
	}	
	return(inod);	
}

int escribir_inodo(struct inodo inod, unsigned int ninodo){
	struct inodo arrayIno[blocksize/sizeof(struct inodo)];
	struct superbloque sb;
	bread(posSB , &sb);

	int primeraPosicion = sb.posPrimerBloqueAI;
	int ultimaPosicion = sb.posUltimoBloqueAI;
	int posicionBloque = ninodo / (blocksize/sizeof(struct inodo));
	int posicionInodo = ninodo % (blocksize/sizeof(struct inodo));

	if(posicionBloque < ultimaPosicion){ //El inodo que se quiere escribir está dentro del array de inodos
		bread(primeraPosicion + posicionBloque , &arrayIno);
			
		arrayIno[posicionInodo] = inod;

		arrayIno[posicionInodo].atime = time(NULL);
		arrayIno[posicionInodo].mtime = time(NULL);

		bwrite(primeraPosicion + posicionBloque , &arrayIno);
			
	}	
}

int reservar_inodo(unsigned char tipo, unsigned char permisos){
	struct inodo inod;
	struct superbloque sb;
	bread(posSB , &sb);
	int a = -1;
	if(sb.posPrimerInodoLibre != sb.totInodos){ 
		a = sb.posPrimerInodoLibre; //Nº inodo retornado		
		inod = leer_inodo(sb.posPrimerInodoLibre);
		
		inod.punterosDirectos[0] = 0;
		
		inod.tipo = tipo;
		inod.permisos = permisos;
		inod.nlinks = 1;
		inod.tamEnBytesLog = 0;

		//Modificar Time
		inod.atime = time(NULL);
		inod.mtime = time(NULL);
		inod.ctime = time(NULL);		
		
		inod.numBloquesOcupados = 0;
		
		int i;
		for(i = 0; i < 12; i++){
			inod.punterosDirectos[i]=0;
		}
		for(i = 0; i < 3; i++){
			inod.punterosIndirectos[i]=0;
		}

		escribir_inodo(inod , sb.posPrimerInodoLibre);
		
		sb.cantInodosLibres--;
		sb.posPrimerInodoLibre = inod.punterosDirectos[0];
		bwrite(posSB, &sb);	
	}

	return(a);
}

int liberar_inodo(unsigned int ninodo){
	struct inodo inod;
	struct superbloque sb;
	bread(posSB , &sb);
	inod = leer_inodo(ninodo);

	liberar_bloques_inodo(inod, 0);

	inod.tipo = 'l';
	inod.permisos = '7';
	inod.nlinks = 0; 
	inod.tamEnBytesLog = 0; 
	inod.numBloquesOcupados = 0;
	
	sb.cantInodosLibres++;
	inod.punterosDirectos[0] = sb.posPrimerInodoLibre;
	sb.posPrimerInodoLibre = ninodo;
	
	bwrite(posSB, &sb);
	escribir_inodo(inod, ninodo);
}

int liberar(int nivel, int nbloque){
	char buffer[blocksize];
	bread(nbloque , buffer);	
	
	if(nivel > 0){
		
		if(nbloque != 0){
			int i;
			for(i = 0; i < (blocksize/4); i++){
				liberar(nivel - 1 , (int)buffer[i*4]);
			}
			liberar_bloque(nbloque);	
		}
	}

	if(nivel == 0){
		if(nbloque != 0){
			liberar_bloque(nbloque);
		}	
	}
}

int liberar_bloques_inodo(unsigned int inodo, unsigned int blogico){
	struct inodo inod;	
	
	if(blogico == 0){
		inod = leer_inodo(inodo);
		int i;
		for(i=0;i<12;i++){
			liberar_bloque(inod.punterosDirectos[i]);
			inod.punterosDirectos[i]=0;
		}
	
		for(i=0;i<3;i++){
			liberar(i+1,inod.punterosIndirectos[i]);
			inod.punterosIndirectos[i]=0;
		}
		escribir_inodo(inod, inodo);
	}
}

int traducir_bloque_inodo(unsigned int ninodo, unsigned int blogico, unsigned int *bfisico, char reservar){
	//SUMAR 1 a numBloquesOcupados numtotal bloques SI hemos reservado bloques
	
	//Punteros directo 	--> 0-11
	//Punteros indirectos 0	--> 12-267
	//Punteros indirectos 1	--> 268-65.803
	//Punteros indirectos 2 --> 65.804-16.843.019
	int npunterosdirectos = 12;
	int npunteros0 = blocksize / 4;
	int npunteros1 = npunteros0 * npunteros0;
	int npunteros2 = npunteros0 * npunteros0 * npunteros0;

	int error = -1;
	struct inodo inod;
	inod = leer_inodo(ninodo);
	unsigned char buffer[blocksize];
	memset(buffer,0,blocksize);
	
	struct superbloque sb;
	memset(&sb,blocksize,0);
	bread(posSB,&sb);
	if(ninodo > sb.totInodos){
		return(-1);
	}
	else if(blogico < npunterosdirectos){ // 0-11
		switch(reservar){
			case '0':
				if(inod.punterosDirectos[blogico] != 0){
					*bfisico = inod.punterosDirectos[blogico];
					error = 0;
				} 
				break;
			case '1':
				if(inod.punterosDirectos[blogico] == 0){
					inod.punterosDirectos[blogico] = reservar_bloque();
					inod.numBloquesOcupados++;
					escribir_inodo(inod,ninodo);
					*bfisico = inod.punterosDirectos[blogico];
				}else{
					*bfisico = inod.punterosDirectos[blogico];
				}
				break;
		}
	}
	else if (blogico < npunterosdirectos+npunteros0){ // 12 - 267
		switch(reservar){
			case '0':
				if(inod.punterosIndirectos[0] != 0){	
					bread(inod.punterosIndirectos[0],buffer);
					if(buffer[blogico - npunterosdirectos] != 0){
						*bfisico = buffer[blogico - npunterosdirectos];
						error = 0;
					}
				} 
				break;
			case '1':
				if(inod.punterosIndirectos[0] != 0){	
					bread(inod.punterosIndirectos[0],buffer);
					if(buffer[blogico - npunterosdirectos] != 0){
						*bfisico = buffer[blogico - npunterosdirectos];
					}else{
						buffer[blogico - npunterosdirectos] = reservar_bloque();
						bwrite(inod.punterosIndirectos[0],buffer);
						inod.numBloquesOcupados++;
						escribir_inodo(inod,ninodo);
						*bfisico = buffer[blogico - npunterosdirectos];
					}
				}else{
					inod.punterosIndirectos[0] = reservar_bloque();
					inod.numBloquesOcupados++;
					bread(inod.punterosIndirectos[0],buffer);
					buffer[blogico - npunterosdirectos] = reservar_bloque();
					inod.numBloquesOcupados++;
					bwrite(inod.punterosIndirectos[0],buffer);					
					escribir_inodo(inod,ninodo);
					*bfisico = buffer[blogico - npunterosdirectos];
				}
				break;
		}
	}
	else if(blogico < npunterosdirectos + npunteros0 + npunteros1){ // 268 - 65.803
		int punt0 = (blogico-(npunterosdirectos + npunteros0))%npunteros0;//puntero nivel 0
		int punt1 = (blogico-(npunterosdirectos + npunteros0))/npunteros0;//puntero nivel 1
		switch(reservar){
			case '0':
				if(inod.punterosIndirectos[1] != 0){
					bread(inod.punterosIndirectos[1],buffer);
					if(buffer[punt1] != 0){
						bread(buffer[punt1],buffer);
						if(buffer[punt0] != 0){ 
							*bfisico = buffer[punt0];
							error = 0;
						}
					}
				} 
				break;
			case '1':
				if(inod.punterosIndirectos[1] != 0){
					bread(inod.punterosIndirectos[1],buffer);
					if(buffer[punt1] != 0){
						int b = buffer[punt1];
						bread(buffer[punt1],buffer);
						if(buffer[punt0] != 0){ 
							*bfisico = buffer[punt0];
						}else{
							buffer[punt0] = reservar_bloque();
							inod.numBloquesOcupados++;
							bwrite(b,buffer);
							escribir_inodo(inod,ninodo);
							*bfisico = buffer[punt0];
						}
					}else{
						buffer[punt1] = reservar_bloque();
						inod.numBloquesOcupados++;
						bwrite(inod.punterosIndirectos[1],buffer);
						int b = buffer[punt1];
						bread(b,buffer);
						buffer[punt0] = reservar_bloque();
						inod.numBloquesOcupados++;
						bwrite(b,buffer);
						escribir_inodo(inod,ninodo);
						*bfisico = buffer[punt0];	
					}
				}else{
					inod.punterosIndirectos[1] = reservar_bloque();
					inod.numBloquesOcupados++;
					bread(inod.punterosIndirectos[1],buffer);
					buffer[punt1] = reservar_bloque();
					inod.numBloquesOcupados++;
					bwrite(inod.punterosIndirectos[1],buffer);
					int b = buffer[punt1];
					bread(b,buffer);
					buffer[punt0] = reservar_bloque();
					inod.numBloquesOcupados++;
					bwrite(b,buffer);
					escribir_inodo(inod,ninodo);
					*bfisico = buffer[punt0];
				}
				break;
		}
	}
	else if(blogico < npunterosdirectos + npunteros0 + npunteros1 + npunteros2){ // 65.804 - 16.843.019
		int punt2 = (blogico - (npunterosdirectos + npunteros0 + npunteros1))/npunteros1;
		int punt1 = ((blogico - (npunterosdirectos + npunteros0 + npunteros1))%npunteros1)/npunteros0;
		int punt0 = ((blogico - (npunterosdirectos + npunteros0 + npunteros1))%npunteros1)%npunteros0;
		switch(reservar){
			case '0':
				if(inod.punterosIndirectos[2] != 0){
					bread(inod.punterosIndirectos[2],buffer);
					if(buffer[punt2] != 0){
						bread(buffer[punt2],buffer);
						if(buffer[punt1] != 0){
							bread(buffer[punt1],buffer);
							if(buffer[punt0] != 0){
								*bfisico = buffer[punt0];
								error = 0;
							}
						}
					}
				}
				break;
			case '1':
				if(inod.punterosIndirectos[2] != 0){
					bread(inod.punterosIndirectos[2],buffer);
					if(buffer[punt2] != 0){
						int b2 = buffer[punt2];
						bread(buffer[punt2],buffer);
						if(buffer[punt1] != 0){
							int b1 = buffer[punt1];
							bread(b1,buffer);
							if(buffer[punt0] != 0){
								*bfisico = buffer[punt0];
							}else{
								buffer[punt0] = reservar_bloque();
								inod.numBloquesOcupados++;
								bwrite(b1,buffer);
								escribir_inodo(inod,ninodo);
								*bfisico = buffer[punt0];
							}
						}else{
							buffer[punt1] = reservar_bloque();
							inod.numBloquesOcupados++;
							bwrite(b2,buffer);
							int b1 = buffer[punt1];
							bread(b1,buffer);
							buffer[punt0] = reservar_bloque();
							inod.numBloquesOcupados++;
							bwrite(b1,buffer);
							escribir_inodo(inod,ninodo);
							*bfisico = buffer[punt0];
						}
					}else{
						buffer[punt2] = reservar_bloque();
						inod.numBloquesOcupados++;
						bwrite(inod.punterosIndirectos[2],buffer);
						int b2 = buffer[punt2];
						bread(buffer[punt2],buffer);
						buffer[punt1] = reservar_bloque();
						inod.numBloquesOcupados++;
						bwrite(b2,buffer);
						int b1 = buffer[punt1];
						bread(b1,buffer);
						buffer[punt0] = reservar_bloque();
						inod.numBloquesOcupados++;
						bwrite(b1,buffer);
						escribir_inodo(inod,ninodo);
						*bfisico = buffer[punt0];
					}
				}else{
					inod.punterosIndirectos[2] = reservar_bloque();
					inod.numBloquesOcupados++;
					bread(inod.punterosIndirectos[2],buffer);
					buffer[punt2] = reservar_bloque();
					inod.numBloquesOcupados++;
					bwrite(inod.punterosIndirectos[2],buffer);
					int b2 = buffer[punt2];
					bread(buffer[punt2],buffer);
					buffer[punt1] = reservar_bloque();
					inod.numBloquesOcupados++;
					bwrite(b2,buffer);
					int b1 = buffer[punt1];
					bread(b1,buffer);
					buffer[punt0] = reservar_bloque();
					inod.numBloquesOcupados++;
					bwrite(b1,buffer);
					escribir_inodo(inod,ninodo);
					*bfisico = buffer[punt0];
				}				
				break;
		}
	}
	if((error == -1)&&(reservar == '0')){
		return(-1);	
	}
	else{
		return(*bfisico);
	}
}
visualizarSB(){
	struct superbloque SB;
	bread(posSB, &SB);
	printf("\n**INFORMACION SUPERBLOQUE**\n");
        printf("Posición del primer bloque del mapa de bits: %d\n",SB.posPrimerBloqueMB);    
        printf("Posición del último bloque del mapa de bits: %d\n",SB.posUltimoBloqueMB);
        printf("Posición del primer bloque del array de inodos: %d\n",SB.posPrimerBloqueAI);
        printf("Posición del último bloque del array de inodos: %d\n",SB.posUltimoBloqueAI);
        printf("Posición del primer bloque de datos: %d\n",SB.posPrimerBloqueDatos);
        printf("Posición del último bloque de datos: %d\n",SB.posUltimoBloqueDatos);
        printf("Posición del inodo del directorio raíz: %d\n",SB.posInodoRaiz);
        printf("Posición del primer inodo libre: %d\n",SB.posPrimerInodoLibre);
        printf("Cantidad de bloques libres: %d\n",SB.cantBloquesLibres);
        printf("Cantidad de inodos libres: %d\n",SB.cantInodosLibres);
        printf("Cantidad total de bloques: %d\n",SB.totBloques);
        printf("Cantidad total de inodos: %d\n",SB.totInodos);	
}

visualizarMB(){
	unsigned char buffer[blocksize];
	struct superbloque sb;
	bread(posSB, &sb);

	printf("\n**INFORMACION MAPA DE BITS**\n");

	int i;
	int j;
	for(i = sb.posPrimerBloqueMB; i <= sb.posUltimoBloqueMB; i++){
		bread(i, buffer);
		for(j = 0; j < blocksize; j++){
			printf("%d",(int)buffer[j]);
		}
	}
	printf("\n");
}

visualizarAI(){
	printf("\n**INFORMACION ARRAY DE INODOS**\n");
	unsigned char buffer[blocksize];
	struct superbloque sb;
	bread(posSB, &sb);

	int i;
	int j;
	for(i = sb.posPrimerBloqueAI; i <= sb.posUltimoBloqueAI; i++){
		bread(i, buffer);
		//write(1,buffer,blocksize);

		for(j = 0; j < blocksize; j++){
			printf("%d",(int)buffer[j]);
		}

		printf("\n\n");
	}
	printf("\n");
}









