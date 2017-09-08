#include "montador.h"
#include <stdio.h>
#include <strings.h>
#include <ctype.h>

/* 
 * Argumentos:
 *  entrada: cadeia de caracteres com o conteudo do arquivo de entrada.
 *  tamanho: tamanho da cadeia.
 * Retorna:
 *  1 caso haja erro na montagem; 
 *  0 caso não haja erro.
 */
int processarEntrada(char* entrada, unsigned tamanho) 
{
	unsigned index = 0;
	unsigned flagComment = 0;

	char read[64];
	for (int i = 0; i < tamanho; i++)
	{
		if(entrada[i] == '\n') {
			flagComment = 0;
		} else if(entrada[i] == '#') {
			flagComment = 1;
		}
	}
  	printf("Você deve implementar esta função para a parte 1.\n");
  	return 0; 
}

