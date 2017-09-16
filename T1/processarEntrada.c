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

	char read[64];
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

			if(!strcmp(".org",read)) {
				char * palavra;
				palavra = malloc((j+1)*sizeof(char));
				strcpy(palavra,read);
				Token tk;
				tk.tipo = Diretiva;
				tk.palavra = palavra;
				tk.linha = line;
				adicionarToken(tk);

			} else if(!strcmp(".set",read)) {
				char * palavra;
				palavra = malloc((j+1)*sizeof(char));
				strcpy(palavra,read);
				Token tk;
				tk.tipo = Diretiva;
				tk.palavra = palavra;
				tk.linha = line;
				adicionarToken(tk);

			} else if(!strcmp(".word",read)) {
				char * palavra;
				palavra = malloc((j+1)*sizeof(char));
				strcpy(palavra,read);
				Token tk;
				tk.tipo = Diretiva;
				tk.palavra = palavra;
				tk.linha = line;
				adicionarToken(tk);

			} else if(!strcmp(".wfill",read)) {
				char * palavra;
				palavra = malloc((j+1)*sizeof(char));
				strcpy(palavra,read);
				Token tk;
				tk.tipo = Diretiva;
				tk.palavra = palavra;
				tk.linha = line;
				adicionarToken(tk);

			} else if(!strcmp(".align",read)) {
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
			} else {
				tk.tipo = Instrucao;
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
		} 
	}

	tokenCount = getNumberOfTokens();
	for (i = 0; i < tokenCount; i++) {
		
	}

	imprimeListaTokens();
  	return 0; 
}

