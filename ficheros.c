#include "ficheros.h"
#include "fitcheros_basico.c"
struct STAT{
unsigned char tipo;
unsigned char permisos; 
unsigned char reservado_alineacion[2];
time_t atime;
time_t mtime; 
time_t ctime; 
unsigned int nlinks; 
unsigned int tamEnBytesLog; 
char padding[T_INODO-88];
};

int mi_write_f(unsigned int ninodo, const void *buf_original, unsigned int offset, unsigned int nbytes){
	if(nbytes > strlen(buf_original)){
		nbytes = strlen(buf_original);
	}
	struct inodo inod;
	inod = leer_inodo(ninodo);
	write(1,buf_original,strlen(buf_original));
	printf("\n");
	if((inod.permisos & 2) == 2){
		int primerBLogico = (offset/blocksize);
		int ultimoBLogico = ((offset + nbytes)/blocksize);
		int desp1 = offset % blocksize; //desplazamiento en el 1er bloque
		int desp2 = (offset + nbytes) % blocksize; //despl. en el último bloque
		
		int bfisico;
		unsigned char buf_bloque[blocksize];
		int b;
		int cont;
		memset(buf_bloque,0,blocksize);

		b=traducir_bloque_inodo(ninodo, primerBLogico, &bfisico, '1');	
		bread(b,buf_bloque);
		if(nbytes < blocksize - desp1){
			memcpy(buf_bloque + desp1, buf_original, nbytes);
		}
		else{
			memcpy(buf_bloque + desp1, buf_original, blocksize - desp1);
		}
		bwrite(b,buf_bloque);
		if(primerBLogico < ultimoBLogico){
			buf_original = buf_original + blocksize - desp1;
			if(desp2 == 0){
				ultimoBLogico++;
			}
			int i;
			for(i = primerBLogico + 1; i < ultimoBLogico; i++){	
				b=traducir_bloque_inodo(ninodo, i, &bfisico, '1');	
				bread(b,buf_bloque);
				memcpy (buf_bloque, buf_original, blocksize);
				bwrite(b,buf_bloque);
				buf_original=buf_original+blocksize;
			}
			if(desp2 != 0){
				b=traducir_bloque_inodo(ninodo, ultimoBLogico, &bfisico, '1');	
				bread(b, buf_bloque);
				memcpy (buf_bloque, buf_original, desp2);
				bwrite(b,buf_bloque);
				buf_original=buf_original+blocksize - desp2;
			}
		}
		if(inod.tamEnBytesLog < offset + nbytes){
			inod.tamEnBytesLog = offset + nbytes;
			escribir_inodo(inod,ninodo);
		}
		
		return(nbytes);
		//si ha augmentado el archivo modificar inodo.
	}	
}

int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes){
/*
	if(nbytes > sizeof(buf_original)){
		nbytes = sizeof(buf_original);
	}
*/
	struct inodo inod;
	inod = leer_inodo(ninodo);
	if((inod.permisos & 4) == 4){
		if(inod.tamEnBytesLog < offset + nbytes){
			nbytes= inod.tamEnBytesLog- offset;
		}
		int primerBLogico = (offset/blocksize);
		int ultimoBLogico = ((offset + nbytes)/blocksize);
		int desp1 = offset % blocksize; //desplazamiento en el 1er bloque
		int desp2 = (offset + nbytes) % blocksize; //despl. en el último bloque

		int bfisico;
		unsigned char buf_bloque[blocksize];
		int b;
		int cont;
		b=traducir_bloque_inodo(ninodo, primerBLogico, &bfisico, '1');	
		bread(b,buf_bloque);
		if(nbytes < blocksize - desp1){
			memcpy (buf_original, buf_bloque + desp1, nbytes);
		}
		else{
			memcpy (buf_original, buf_bloque + desp1, blocksize - desp1);
		}
		memcpy (buf_original, buf_bloque + desp1, (nbytes % blocksize));//blocksize - desp1
		if(primerBLogico != ultimoBLogico){
			buf_original = buf_original + blocksize - desp1;
			if(desp2 == 0){
				ultimoBLogico++;
			}
			int i;
			for(i = primerBLogico + 1; i < ultimoBLogico; i++){		
				b=traducir_bloque_inodo(ninodo, i, &bfisico, '1');	
				bread(b,buf_bloque);
				memcpy (buf_original, buf_bloque, blocksize);
				buf_original = buf_original + blocksize - desp1;
			}
			if(desp2 != 0){
				b=traducir_bloque_inodo(ninodo, ultimoBLogico, &bfisico, '1');	
				bread(b, buf_bloque);
				memcpy(buf_original, buf_bloque, desp2);
				buf_original = buf_original + blocksize - desp2;
			}
		}
		return(nbytes);
	}
	return(-1);	
}

int mi_chmod_f(unsigned int ninodo, unsigned char modo){
	struct inodo inod;
	inod = leer_inodo(ninodo);
	inod.permisos = modo;
	inod.ctime = time(NULL);
	escribir_inodo(inod,ninodo);
}

int mi_truncar_f(unsigned int ninodo, unsigned int nbytes){
	
}

int mi_stat_f(unsigned int ninodo, struct STAT *p_stat){
	struct inodo inod;
	inod = leer_inodo(ninodo);
	struct STAT stats;
	stats.tipo = inod.tipo;
	stats.permisos = inod.permisos;
	stats.atime = inod.atime;
	stats.mtime = inod.mtime;
	stats.ctime = inod.ctime;
	stats.nlinks = inod.nlinks;
	stats.tamEnBytesLog = inod.tamEnBytesLog;
	printf("Tipo de inodo: %c\n",stats.tipo);
        printf("Permiso de inodo: %c\n",stats.permisos);
        printf("atime: %ld\n",stats.atime);
        printf("mtime: %ld\n",stats.mtime);
        printf("ctime: %ld\n",stats.ctime);
        printf("nlinks: %d\n",stats.nlinks);
        printf("tamEnBytesLog: %d\n",stats.tamEnBytesLog);
	*p_stat = stats;
}
