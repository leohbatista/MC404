#include "montador.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#ifndef NPALAVRAS
#define NPALAVRAS 1024
#endif

int palavraDecimalParaDecimal(char * palavra);
int palavraHexadecimalParaDecimal(char * palavra);

/* Retorna:
 *  1 caso haja erro na montagem; 
 *  0 caso não haja erro.
 */
int emitirMapaDeMemoria() {
	char mapaMemoria[NPALAVRAS][10];
	int flagPreenchido[NPALAVRAS][2];
	int i;
	int nTokens = getNumberOfTokens();

	int pontoMontagem = 0;
	int flagDireita = 0;

	// Inicializa a matriz do mapa de memoria e a flag de preenchimento 
	for (i = 0; i < NPALAVRAS; ++i) {
		flagPreenchido[i][0] = 0;
		flagPreenchido[i][1] = 0;

		for(int j = 0; j < 10; j++) {
			mapaMemoria[i][j] = '0';
		}		
	}

	Token code[4];
	int j = 0;
	int pos = 0;
	int ultimoToken = recuperaToken(nTokens-1).linha;

	// Tratamento por linha de codigo
	for (i = 1; i <= ultimoToken; i++) {
		// Obtencao dos tokens de uma linha
		while(pos < nTokens) {
			Token tk = recuperaToken(pos);
			if(tk.linha == i) {
				code[j] = tk;
				j++;
				pos++;
			} else {
				j = 0;
				break;
			}	
		}

		int k = 0;
		while(k < j) {
			switch(code[k].tipo) {
				case Instrucao:
					if (flagInstDir) {
						fprintf(stderr,"ERRO GRAMATICAL: palavra na linha %d!\n",i);
						return 1;
					} else if((k+1 < j && (code[k+1].tipo == Hexadecimal || code[k+1].tipo == Decimal || code[k+1].tipo == Nome)) || k+1 == j) {
						k += 2;
					} else {
						fprintf(stderr,"ERRO GRAMATICAL: palavra na linha %d!\n",i);
						return 1;
					}
					flagInstDir = 1;
					break;

				case Diretiva:
					if (flagInstDir) {
						fprintf(stderr,"ERRO GRAMATICAL: palavra na linha %d!\n",i);
						return 1;
					} else if(!strcmp(".org",code[k].palavra)) {
						if(code[k+1].tipo == Hexadecimal) {
							if (flagDireita){
								/* code */
							}
							pontoMontagem = palavraDecimalParaDecimal()
						} else {
							
						} 
					} else if (!strcmp(".set",code[k].palavra)) {
						if(k+2 < j && (code[k+1].tipo == Nome) && (code[k+2].tipo == Hexadecimal || code[k+2].tipo == Decimal)) {
							k += 3;
						} else {
							fprintf(stderr,"ERRO GRAMATICAL: palavra na linha %d!\n",i);
							return 1;

						}
					} else if (!strcmp(".word",code[k].palavra)) { 
						if(k+1 < j && (code[k+1].tipo == Hexadecimal || code[k+1].tipo == Decimal || code[k+1].tipo == Nome)) {
							k += 2;
						} else {
							fprintf(stderr,"ERRO GRAMATICAL: palavra na linha %d!\n",i);
							return 1;
						}
					} else if (!strcmp(".wfill",code[k].palavra)) {
						if(k+2 < j && (code[k+1].tipo == Decimal) && (code[k+2].tipo == Hexadecimal || code[k+2].tipo == Decimal || code[k+2].tipo == Nome)) {
							k += 3;
						} else {
							fprintf(stderr,"ERRO GRAMATICAL: palavra na linha %d!\n",i);
							return 1;

						}
					} else if (!strcmp(".align",code[k].palavra)) {
						if(k+1 < j && (code[k+1].tipo == Decimal)) {
							k += 2;
						} else {
							fprintf(stderr,"ERRO GRAMATICAL: palavra na linha %d!\n",i);
							return 1;
						}
					} else {
						fprintf(stderr,"Foi nesse ERRO GRAMATICAL: palavra na linha %d!\n",i);
						return 1;
					}

					flagInstDir = 1;
					break;

				case DefRotulo:
					if (flagDefRot) {
						fprintf(stderr,"ERRO GRAMATICAL: palavra na linha %d!\n",i);
						return 1;
					} else if(k+1 < j && (code[k+1].tipo == DefRotulo || code[k+1].tipo == Hexadecimal || code[k+1].tipo == Decimal || code[k+1].tipo == Nome)) {
						fprintf(stderr,"ERRO GRAMATICAL: palavra na linha %d!\n",i);
						return 1;
					} else {
						k++;
						flagDefRot = 1;
					}
					break;

				case Hexadecimal:
				case Decimal:
				case Nome:
					fprintf(stderr,"ERRO GRAMATICAL: palavra na linha %d!\n",i);
					return 1;
					break;
			}
		}
	}

	for (i = 0; i < nTokens; i++) {
		
	}

	for (i = 0; i < NPALAVRAS; ++i) {
		
	}

	return 0;
}

int palavraDecimalParaDecimal(char * palavra) {
	int valor = 0;
	int len = strlen(palavra);

	for (int i = len-1; i >= 0; i--) {
		valor += pow(10,len-1-i) * (palavra[i] - '0'); 
	}

	return valor;
}

int palavraHexadecimalParaDecimal(char * palavra) {
	int valor = 0;
	int len = strlen(palavra);

	for (int i = len-1; i >= 0; i--) {
		if(isdigit(palavra[i])) {
			valor += pow(16,len-1-i) * (palavra[i] - '0'); 
		} else {
			valor += pow(16,len-1-i) * (palavra[i] - 'A' + 10); 
		}
	}

	return valor;
}