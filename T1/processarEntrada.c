#include "montador.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

/* 
 * Argumentos:
 *  entrada: cadeia de caracteres com o conteudo do arquivo de entrada.
 *  tamanho: tamanho da cadeia.
 * Retorna:
 *  1 caso haja erro na montagem; 
 *  0 caso não haja erro.
 */
int processarEntrada(char* entrada, unsigned tamanho) {
	unsigned index = 0;
	unsigned line = 1;
	unsigned flagComment = 0;
	
	unsigned flagDir = 0;

	char read[65];
	int i = 0;
	int j = 0;

	for (i = 0; i < tamanho; i++) {
		
		if(isspace(entrada[i])) {
			if(entrada[i] == '\n') {
				flagComment = 0;
				flagDir = 0;
				line++;
			}
		} else if(flagComment) {
			continue;
		} else if(entrada[i] == '#') {
			flagComment = 1;
		} else if(entrada[i] == '.') {
			flagDir = 1;

			j = 0;
			read[j++] = entrada[i++];
			while(!isspace(entrada[i]) && entrada[i] != 0) {
				read[j++] = entrada[i++];
			}

			read[j] = '\0';
			
			if(entrada[i] == '\n') {
				i--;
			}

			if(!strcmp(".org",read) || !strcmp(".set",read) || !strcmp(".word",read) || !strcmp(".wfill",read) || !strcmp(".align",read)) {
				char * palavra;
				palavra = malloc((j+1)*sizeof(char));
				strcpy(palavra,read);
				Token tk;
				tk.tipo = Diretiva;
				tk.palavra = palavra;
				tk.linha = line;
				adicionarToken(tk);
			} else {
				fprintf(stderr,"ERRO LEXICO: palavra inválida na linha %d!\n",line);
				return -1;
			}
		} else if(isalpha(entrada[i]) || entrada[i] == '_') {
			Token tk;
			int countDoisPontos = 0;

			if(flagDir) {
				tk.tipo = Nome;
			} else if (isupper(entrada[i])) {
				tk.tipo = Instrucao;
			} else {
				tk.tipo = Nome;
			}
			
			j = 0;
			if(i > 0 && entrada[i-1] == '"') {
				tk.tipo = Nome;
				while(entrada[i] != '"') {
					read[j++] = entrada[i++];
				}
			} else {
				while(!isspace(entrada[i]) && entrada[i] != 0) {
					if(entrada[i] == ':') {
						countDoisPontos++;
					}
					read[j++] = entrada[i++];
				}
			}

			read[j] = '\0';

			if(entrada[i] == '\n') {
				i--;
			}

			if(isdigit(read[0]) || countDoisPontos > 1) {
				fprintf(stderr,"ERRO LEXICO: palavra inválida na linha %d!\n",line);
				return -1;
			}

			if(read[j-1] == ':') {
				tk.tipo = DefRotulo;
			} else if (countDoisPontos > 0) {
				fprintf(stderr,"ERRO LEXICO: palavra inválida na linha %d!\n",line);
				return -1;
			}

			char * palavra;
			palavra = malloc((j+1)*sizeof(char));
			strcpy(palavra,read);
			tk.palavra = palavra;
			tk.linha = line;
			adicionarToken(tk);
		} else if(isdigit(entrada[i])) {
			Token tk;
			tk.linha = line;

			j = 0;
			if(entrada[i+1] == 'x') {
				read[0] = '0';
				read[1] = 'x';
				j = 2;
				i += 2;
				tk.tipo = Hexadecimal;
				while(!isspace(entrada[i]) && entrada[i] != 0 && entrada[i] != '"') {
					if(isxdigit(entrada[i])) {
						read[j++] = entrada[i++];
					} else {
						fprintf(stderr,"ERRO LEXICO: palavra inválida na linha %d!\n",line);
						return -1;
					}					
				}
			} else {
				tk.tipo = Decimal;
				while(!isspace(entrada[i]) && entrada[i] != 0 && entrada[i] != '"') {
					if(isdigit(entrada[i])) {
						read[j++] = entrada[i++];
					} else {
						fprintf(stderr,"ERRO LEXICO: palavra inválida na linha %d!\n",line);
						return -1;
					}					
				}
			}

			read[j] = '\0';
			if(entrada[i] == '\n') {
				i--;
			}

			char * palavra;
			palavra = malloc((j+1)*sizeof(char));
			strcpy(palavra,read);
			tk.palavra = palavra;
			adicionarToken(tk);
		} else if(entrada[i] != '"' || entrada[i] != 0) {
			//fprintf(stderr,"ERRO LEXICO: palavra inválida na linha %d!\n",line);
			//return -1;
		}
	}

	int tokenCount = getNumberOfTokens();
	Token code[4];
	i = 0;
	int pos = 0;
	j = 0;
	for (i = 1; i < line; i++) {
		while(pos < tokenCount) {
			Token tk = recuperaToken(pos);
			//printf("%d %s\n",tk.linha,tk.palavra);
			if(tk.linha == i) {
				if(j == 4) {
					fprintf(stderr,"ERRO GRAMATICAL: palavra na linha %d!\n",i);
					return -1;
				}
				code[j] = tk;
				j++;
				pos++;
			} else {
				j = 0;
				break;
			}	
		}
		
		int k = 0;
		int flagInstDir = 0;
		int flagDefRot = 0;
		while(k < j) {
			//printf("%d %s %d %d %d\n",code[k].linha,code[k].palavra,code[k].tipo,j,flagInstDir);
			switch(code[k].tipo) {
				case Instrucao:
					if (flagInstDir) {
						fprintf(stderr,"ERRO GRAMATICAL: palavra na linha %d!\n",i);
						return -1;
					} else if((k+1 < j && (code[k+1].tipo == Hexadecimal || code[k+1].tipo == Decimal || code[k+1].tipo == Nome)) || k+1 == j) {
						k += 2;
					} else {
						fprintf(stderr,"ERRO GRAMATICAL: palavra na linha %d!\n",i);
						return -1;
					}
					flagInstDir = 1;
					break;

				case Diretiva:
					if (flagInstDir) {
						fprintf(stderr,"ERRO GRAMATICAL: palavra na linha %d!\n",i);
						return -1;
					} else if(!strcmp(".org",code[k].palavra)) {
						if(k+1 < j && (code[k+1].tipo == Hexadecimal || code[k+1].tipo == Decimal)) {
							k += 2;
						} else {
							fprintf(stderr,"ERRO GRAMATICAL: palavra na linha %d!\n",i);
							return -1;
						}
					} else if (!strcmp(".set",code[k].palavra)) {
						if(k+2 < j && (code[k+1].tipo == Nome) && (code[k+2].tipo == Hexadecimal || code[k+2].tipo == Decimal)) {
							k += 3;
						} else {
							fprintf(stderr,"ERRO GRAMATICAL: palavra na linha %d!\n",i);
							return -1;

						}
					} else if (!strcmp(".word",code[k].palavra)) { 
						if(k+1 < j && (code[k+1].tipo == Hexadecimal || code[k+1].tipo == Decimal || code[k+1].tipo == Nome)) {
							k += 2;
						} else {
							fprintf(stderr,"ERRO GRAMATICAL: palavra na linha %d!\n",i);
							return -1;
						}
					} else if (!strcmp(".wfill",code[k].palavra)) {
						if(k+2 < j && (code[k+1].tipo == Decimal) && (code[k+2].tipo == Hexadecimal || code[k+2].tipo == Decimal || code[k+2].tipo == Nome)) {
							k += 3;
						} else {
							fprintf(stderr,"ERRO GRAMATICAL: palavra na linha %d!\n",i);
							return -1;

						}
					} else if (!strcmp(".align",code[k].palavra)) {
						if(k+1 < j && (code[k+1].tipo == Decimal)) {
							k += 2;
						} else {
							fprintf(stderr,"ERRO GRAMATICAL: palavra na linha %d!\n",i);
							return -1;
						}
					} else {
						fprintf(stderr,"Foi nesse ERRO GRAMATICAL: palavra na linha %d!\n",i);
						return -1;
					}

					flagInstDir = 1;

					break;
				case DefRotulo:
					if (flagDefRot) {
						fprintf(stderr,"ERRO GRAMATICAL: palavra na linha %d!\n",i);
						return -1;
					} else if(k+1 < j && (code[k+1].tipo == DefRotulo || code[k+1].tipo == Hexadecimal || code[k+1].tipo == Decimal || code[k+1].tipo == Nome)) {
						fprintf(stderr,"ERRO GRAMATICAL: palavra na linha %d!\n",i);
						return -1;
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

	//imprimeListaTokens();
  	return 0; 
}