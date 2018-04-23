// Librerias utilizadas 
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>


// Definicion de tokens con valores numericos simbolicos
#define L_CORCHETE		256
#define R_CORCHETE		257
#define L_LLAVE			258
#define R_LLAVE			259
#define COMA			300
#define DOS_PUNTOS		301
#define STRING			302
#define NUMBER			303
#define PR_TRUE			304
#define PR_FALSE		305
#define PR_NULL			306


// constantes de tamaÃ±os
#define TAMANHOBUFFER 				5		// tamaÃ±o del buffer
#define TAMANHOLEXICO 				50		// tamaÃ±o del lexema
#define TAMANHOHASH 				101		// tamaÃ±o de la tabla hash

// estructura de entrada
typedef struct input{
	int componenteLexico;			// componente lexico
	char lexema[TAMANHOLEXICO];		// lexema
	struct input *tipoDato; 		// null puede representar variable no declarada		
} input;

// estructura de tokens
typedef struct {
	int componenteLexico;			// componente lexico
	input *pe;
} token;

// prototipos de funcion y procedimientos
void insertar(input e);
input* buscar(const char *clave);
void initTabla();
void initTablaSimbolos();
void sigLex();
