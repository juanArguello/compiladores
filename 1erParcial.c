# include <stdio.h>  	//si el array tiene "x" se comprueba que esta ocupado
# include <stdlib.h>
# include <string.h>
# define FILA 5
# define COL  19
int func(char array[FILA][COL],int i,int j,int nro_est);
int main(void){
	int k,z;
	char nro_aux[COL] ,est[FILA][COL];
	for(k=0;k<=FILA;k++){
		for(z=0;z<=COL;z++){
			scanf("%s",nro_aux);
			strcpy(est,nro_aux);
		}
	}
	int nro;
	nro=atoi(est[0][0]);
	printf("El numero de estacionamiento es %d \n ",func(est,FILA,COL,nro));
	return 0;
}
int func(char array[FILA][COL],int i,int j,int nro_est){
	if(i==0 && j==0){	// caso base
		return nro_est;
	}else{	// caso general
//convierte el "array[i][j]" a int para comparar con el "nro_int" en la primera expresion
//compara el "array[i][j]" si es distinto que x
		if(atoi(array[i][j])<nro_est && array[i][j] !='x')
			if((i<FILA-2) && (j>3 && j<COL-3))// comprueba por hilera y luego por las centrales
				nro_est=atoi(array[i][j]);//convierte el "array[i][j]"a int para la asignacion a "nro_est"
				return func(array,i-1,j-1,nro_est);
	}
}
