compiladores
============

Compiladores FPUNA

Para compilar: gcc anlex.c tablaSimbolos.c -o anlex
En este analizador lexico no reconoce carateres acentuados, solo 
cadenas de caracteres ascii.
Si en la fuente exite en un String caracter acentuado produce error
porque el lenguaje C solo reconoce caracteres ascii.
