// Trabalho 1 - Montador para a arquitetura do computador IAS - Parte 1
// Autor: Leonardo Henrique Batista - 171985  

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

	// Tratamento do vetor de caracteres da entrada
	for (i = 0; i < tamanho; i++) {
		
		// Análise inicial dos possíveis tipos de tokens
		if(isspace(entrada[i])) {
			// Termina um comentario e reseta a flag de diretivas ao terminar uma linha
			if(entrada[i] == '\n') {
				flagComment = 0;
				flagDir = 0;
				line++;
			}
		} else if(flagComment) {
			// Ignora caso esteja dentro de um comentario
			continue;
		} else if(entrada[i] == '#') {
			// Habilita a flag de comentario
			flagComment = 1;
		} else if(entrada[i] == '.') {
			// Habilita a flag de diretiva
			flagDir = 1;

			// Le a diretiva
			j = 0;
			while(!isspace(entrada[i]) && entrada[i] != 0) {
				read[j++] = entrada[i++];
			}
			read[j] = '\0';
			
			if(entrada[i] == '\n') {
				i--;
			}

			// Verifica se eh uma diretiva valida
			if(!strcmp(".org",read) || !strcmp(".set",read) || !strcmp(".word",read) || !strcmp(".wfill",read) || !strcmp(".align",read)) {
				// Armazena o token da diretiva
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
				return 1;
			}

		} else if(isalpha(entrada[i]) || entrada[i] == '_') {
			Token tk;
			int countDoisPontos = 0;

			// Define o tipo default
			if(flagDir) {
				// Pos diretiva
				tk.tipo = Nome;
			} else if (isupper(entrada[i])) {
				tk.tipo = Instrucao;
			} else {
				tk.tipo = Nome;
			}
			
			// Verifica se eh argumento de instrucao e realiza a leitura do token
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

			// Trata erros de definicao de rotulo
			if(isdigit(read[0]) || countDoisPontos > 1) {
				fprintf(stderr,"ERRO LEXICO: palavra inválida na linha %d!\n",line);
				return 1;
			}

			// Verifica se de fato eh definicao de rotulo ou eh erro lexico
			if(read[j-1] == ':') {
				tk.tipo = DefRotulo;
			} else if (countDoisPontos > 0) {
				fprintf(stderr,"ERRO LEXICO: palavra inválida na linha %d!\n",line);
				return 1;
			}

			// Armazena o token
			char * palavra;
			palavra = malloc((j+1)*sizeof(char));
			strcpy(palavra,read);
			tk.palavra = palavra;
			tk.linha = line;
			adicionarToken(tk);
		} else if(isdigit(entrada[i])) {
			Token tk;
			tk.linha = line;

			// Verifica o tipo do numero, le e verifica se eh valido
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
						return 1;
					}					
				}
			} else {
				tk.tipo = Decimal;
				while(!isspace(entrada[i]) && entrada[i] != 0 && entrada[i] != '"') {
					if(isdigit(entrada[i])) {
						read[j++] = entrada[i++];
					} else {
						fprintf(stderr,"ERRO LEXICO: palavra inválida na linha %d!\n",line);
						return 1;
					}					
				}
			}
			read[j] = '\0';
			if(entrada[i] == '\n') {
				i--;
			}

			// Armazena o token
			char * palavra;
			palavra = malloc((j+1)*sizeof(char));
			strcpy(palavra,read);
			tk.palavra = palavra;
			adicionarToken(tk);
		} else if(entrada[i] != '"' || entrada[i] != 0) {
			fprintf(stderr,"ERRO LEXICO: palavra inválida na linha %d!\n",line);
			return 1;
		}
	}

	// Verificacao de erros gramaticais
	int tokenCount = getNumberOfTokens();
	Token code[4];
	j = 0;
	
	// Tratamento por linha de codigo
	for (i = 1; i < line; i++) {
		// Obtencao dos tokens de uma linha
		while(pos < tokenCount) {
			Token tk = recuperaToken(pos);
			if(tk.linha == i) {
				// Dispara erro se a quantidade maxima de tokens por linha eh ultrapassada
				if(j == 4) {
					fprintf(stderr,"ERRO GRAMATICAL: palavra na linha %d!\n",i);
					return 1;
				}
				code[j] = tk;
				j++;
				pos++;
			} else {
				j = 0;
				break;
			}	
		}
		
		// Analisa se os tokens estão numa ordem valida dentro da linha
		int k = 0;
		int flagInstDir = 0;
		int flagDefRot = 0;
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
						if(k+1 < j && (code[k+1].tipo == Hexadecimal || code[k+1].tipo == Decimal)) {
							k += 2;
						} else {
							fprintf(stderr,"ERRO GRAMATICAL: palavra na linha %d!\n",i);
							return 1;
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

	imprimeListaTokens();
  	return 0; 
}