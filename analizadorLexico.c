// Librerias utilizadas 
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<ctype.h>
#include "tableSymbols.c"


// Variables globales 

int consumir; 
char cadena[5*TAMANHOLEXICO];		// string utilizado para cargar mensajes de error
token t;							// token global para recibir componentes del Analizador Lexico
FILE *archivo;						// Fuente json
FILE *ptrOutput;					// output archivo de salida de tokens json
char buffer[2*TAMANHOBUFFER];		// Buffer para lectura de archivo fuente
char id[TAMANHOLEXICO];				// Utilizado por el analizador lexico
int delantero=-1;					// Utilizado por el analizador lexico
int fin=0;							// Utilizado por el analizador lexico
int numeroLinea=1;					// Numero de Linea
int auxNumeroLinea;



// Funciones y procedimientos

void error(const char* mensaje)
{
	printf("Lin %d: Error Lexico. %s.\n",numeroLinea,mensaje);	
}

void sigLex()
{
	int i=0;
	char c=0;
	int acepto=0;
	int estado=0;
	char msg[41];
	input e;

	while((c=fgetc(archivo))!=EOF)
	{	
		if (c==' ' || c=='\t')
			continue;	//eliminar espacios en blanco
		else if(c=='\n')
		{
			//incrementar el numero de linea
			numeroLinea++;
			continue;
		}
		else if (isalpha(c))
		{
			//palabra reservada true false null
			i=0;
			do{
				id[i]=c;
				i++;
				c=fgetc(archivo);
				if (i>=TAMANHOLEXICO)
					error("Longitud de Identificador excede tamaño de buffer");
			}while(isalpha(c));
			id[i]='\0';
			if (c!=EOF)
				ungetc(c,archivo);
			else
				c=0;
			if(strcmp(id,"true")==0){
				t.componenteLexico=PR_TRUE;
				t.pe=buscar("true");
			}else if(strcmp(id,"false")==0){
				t.componenteLexico=PR_FALSE;
				t.pe=buscar("false");
			}
			else if(strcmp(id,"false")==0){
				t.componenteLexico=PR_FALSE;
				t.pe=buscar("false");
			}
			else if(strcmp(id,"null")==0){
				t.componenteLexico=PR_NULL;
				t.pe=buscar("null");
			}
			if (t.pe->componenteLexico==-1)
			{
				error("Error lexico");
			}
			break;
		}
		else if (isdigit(c))
		{
				//es digito
				i=0;
				estado=0;
				acepto=0;
				id[i]=c;
				
				while(!acepto)
				{
					switch(estado){
					case 0: //una secuencia netamente de digitos, puede ocurrir . o e
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=0;
						}
						else if(c=='.'){
							id[++i]=c;
							estado=1;
						}
						else if(tolower(c)=='e'){
							id[++i]=c;
							estado=3;
						}
						else{
							estado=6;
						}
						break;
					
					case 1://un punto, debe seguir un digito (caso especial de array, puede venir otro punto)
						c=fgetc(archivo);						
						if (isdigit(c))
						{
							id[++i]=c;
							estado=2;
						}
						else if(c=='.')
						{
							i--;
							fseek(archivo,-1,SEEK_CUR);
							estado=6;
						}
						else{
							sprintf(msg,"No se esperaba '%c'",c);
							estado=-1;
						}
						break;
					case 2://la fraccion decimal, pueden seguir los digitos o e
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=2;
						}
						else if(tolower(c)=='e')
						{
							id[++i]=c;
							estado=3;
						}
						else
							estado=6;
						break;
					case 3://una e, puede seguir +, - o una secuencia de digitos
						c=fgetc(archivo);
						if (c=='+' || c=='-')
						{
							id[++i]=c;
							estado=4;
						}
						else if(isdigit(c))
						{
							id[++i]=c;
							estado=5;
						}
						else{
							sprintf(msg,"No se esperaba '%c'",c);
							estado=-1;
						}
						break;
					case 4://necesariamente debe venir por lo menos un digito
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=5;
						}
						else{
							sprintf(msg,"No se esperaba '%c'",c);
							estado=-1;
						}
						break;
					case 5://una secuencia de digitos correspondiente al exponente
						c=fgetc(archivo);
						if (isdigit(c))
						{
							id[++i]=c;
							estado=5;
						}
						else{
							estado=6;
						}break;
					case 6://estado de aceptacion, devolver el caracter correspondiente a otro componente lexico
						if (c!=EOF)
							ungetc(c,archivo);
						else
							c=0;
						id[++i]='\0';
						acepto=1;
						strcpy(e.lexema,id);
						e.componenteLexico=NUMBER;
						insertar(e);
						t.pe=buscar(id);
						t.componenteLexico=NUMBER;
						break;
					case -1:
						if (c==EOF)
							error("No se esperaba el fin de archivo");
						else
							error(msg);
						exit(1);
					}
				}
			break;
		}
		else if (c==':')
		{
			// un operador de asignacion
			t.componenteLexico=DOS_PUNTOS;
			t.pe=buscar(":");
			break;
		}
		else if (c==',')
		{
			t.componenteLexico=COMA;
			t.pe=buscar(",");
			break;
		}
		
		else if (c=='[')
		{
			t.componenteLexico=L_CORCHETE;
			t.pe=buscar("[");
			break;
		}
		else if (c==']')
		{
			t.componenteLexico=R_CORCHETE;
			t.pe=buscar("]");
			break;
		}
		else if (c=='{')
		{
			t.componenteLexico=L_LLAVE;
			t.pe=buscar("{");
			break;
		}
		else if (c=='}')
		{
			t.componenteLexico=R_LLAVE;
			t.pe=buscar("}");
			break;
		}
		else if (c=='\'' || c == '\"')
		{//un caracter o una cadena de caracteres
			i=0;
			id[i]=c;
			i++;
			do{
				c=fgetc(archivo);
				if (c=='\'')
				{
					c=fgetc(archivo);
					if (c=='\'')
					{
						id[i]=c;
						i++;
						id[i]=c;
						i++;	
					}
					else
					{
						id[i]='\'';
						i++;
						break;
					}
				}
				else if (c=='\"')
				{
					c=fgetc(archivo);
					if (c=='\"')
					{
						
						id[i]=c;
						i++;
						id[i]=c;
						i++;
					}
					else
					{
						id[i]='\"';
						i++;
						break;
					}
				}
				else if(c==EOF)
				{
					error("Se llego al fin de archivo sin finalizar un literal");
				}
				else{
					id[i]=c;
					i++;
				}
			}while(isascii(c));
			id[i]='\0';
			if (c!=EOF)
				ungetc(c,archivo);
			else
				c=0;
			strcpy(e.lexema,id);
			e.componenteLexico=STRING;
			insertar(e);
			t.pe=buscar(id);
			t.componenteLexico=STRING;
			break;
		}
		else if (c!=EOF)
		{
			sprintf(msg,"%c no esperado",c);
			error(msg);
		}
	}
	if (c==EOF)
	{
		t.componenteLexico=EOF;
		strcpy(e.lexema,"EOF");
		sprintf(e.lexema,"EOF");
		t.pe=&e;
	}
	
}

int main(int argc,char* args[])
{
	
	// inicializar analizador lexico

	initTabla();
	initTablaSimbolos();
	
	if(argc > 1)
	{
		if (!(archivo=fopen(args[1],"rt")))
		{
			printf("Archivo no encontrado.\n");
			exit(1);
		}
		
		if ( ( ptrOutput = fopen( "output.txt", "w" ) ) == NULL ) {
 			printf( "El archivo no pudo abrirse\n" );
 		}else{
 			while (t.componenteLexico!=EOF){
				sigLex();
			
				if(t.pe->componenteLexico==L_CORCHETE){
					fprintf( ptrOutput, "L_CORCHETE\n");
				}else if(t.pe->componenteLexico==R_CORCHETE){
					fprintf( ptrOutput, "R_CORCHETE\n");
				}else if(t.pe->componenteLexico==L_LLAVE){
					fprintf( ptrOutput, "L_LLAVE\n");
				}else if(t.pe->componenteLexico==R_LLAVE){
					fprintf( ptrOutput, "R_LLAVE\n");
				}else if(t.pe->componenteLexico==COMA){
					fprintf( ptrOutput, "COMA\n");
				}else if(t.pe->componenteLexico==DOS_PUNTOS){
					fprintf( ptrOutput, "DOS_PUNTOS\n");
				}else if(t.pe->componenteLexico==STRING){
					fprintf( ptrOutput, "STRING\n");
				}else if(t.pe->componenteLexico==DOS_PUNTOS){
					fprintf( ptrOutput, "DOS_PUNTOS\n");
				}else if(t.pe->componenteLexico==NUMBER){
					fprintf( ptrOutput, "NUMBER\n");
				}else if(t.pe->componenteLexico==PR_TRUE){
					fprintf( ptrOutput, "PR_TRUE\n");
				}else if(t.pe->componenteLexico==PR_FALSE){
					fprintf( ptrOutput, "PR_FALSE\n");
				}else if(t.pe->componenteLexico==PR_NULL){
					fprintf( ptrOutput, "PR_NULL\n");
				}
				
				//fprintf( ptrOutput, "%s\n",t.pe->lexema);
			}
			fclose(archivo);
			fclose(ptrOutput);
		}
		
	}else{
		printf("Debe pasar como parametro el path al archivo fuente.\n");
		exit(1);
	}
	return 0;
}
