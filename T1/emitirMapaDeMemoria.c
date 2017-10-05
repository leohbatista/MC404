// Trabalho 1 - Montador para a arquitetura do computador IAS - Parte 2
// Autor: Leonardo Henrique Batista - 171985  

#include "montador.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

// Definicao da constante com o numero de palavras de memoria
#ifndef NPALAVRAS
#define NPALAVRAS 1024
#endif

// Estrutura para armazenar dados de um simbolo
typedef struct Simbolo {
	char * chave;
	int valor;
} Simbolo;

// Estrutura para armazenar dados de um rotulo
typedef struct Rotulo {
	char chave[65];
	int valor;
	int alinhamento;
} Rotulo;

// Definicao das funcoes
int palavraDecimalParaDecimal(char * palavra);
int palavraHexadecimalParaDecimal(char * palavra);
int potencia(int base,int expoente);

/* Retorna:
 *  1 caso haja erro na montagem; 
 *  0 caso não haja erro.
 */
int emitirMapaDeMemoria() {
	// Matriz do mapa de memoria
	char mapaMemoria[NPALAVRAS][10];

	//Vetor de flags de preenchimento
	int flagPreenchido[NPALAVRAS];
	
	// Contadores e flags
	int i;
	int qtdeSimb = 0;
	int qtdeRot = 0;
	int pontoMontagem = 0;
	int flagDireita = 0;
	int nTokens = getNumberOfTokens();

	// Alocacao do vetor de simbolos
	Simbolo * simbolos;
	simbolos = (Simbolo *) malloc((nTokens/3) * sizeof(Simbolo)); 
	
	// Alocacao do vetor de rotulos
	Rotulo * rotulos;
	rotulos = (Rotulo *) malloc(nTokens * sizeof(Rotulo));

	// Inicializacao da matriz do mapa de memoria e das flags de preenchimento 
	for (i = 0; i < NPALAVRAS; ++i) {
		flagPreenchido[i] = 0;

		for(int j = 0; j < 10; j++) {
			mapaMemoria[i][j] = '0';
		}		
	}

	// Pre-montagem identificando os simbolos e definicoes de
	// rotulos e armazenando-os
	for (i = 0; i < getNumberOfTokens(); i++){
		Token tk = recuperaToken(i);
		
		// Avalia o tipo de token e processa endereco de montagem e alinhamento
		switch(tk.tipo) {
			
			case Instrucao:
				if(flagDireita) {
					flagDireita = 0;
					pontoMontagem++;
				} else {
					flagDireita = 1;
				}
				break;

			case Diretiva:
				if(!strcmp(".org",tk.palavra)) {
					// Avanca para o endereco especificado
					if(recuperaToken(i+1).tipo == Hexadecimal) {
						pontoMontagem = palavraHexadecimalParaDecimal(tk.palavra);
					} else if(recuperaToken(i+1).tipo == Decimal) {
						pontoMontagem = palavraDecimalParaDecimal(tk.palavra);
					}
					flagDireita = 0;
					i++;

				} else if(!strcmp(".set",tk.palavra)) {
					// Identifica e armazena o simbolo no vetor
					simbolos[qtdeSimb].chave = recuperaToken(i+1).palavra;
					if(recuperaToken(i+2).tipo == Hexadecimal) {
						simbolos[qtdeSimb].valor = palavraHexadecimalParaDecimal(recuperaToken(i+2).palavra);
					} else {
						simbolos[qtdeSimb].valor = palavraDecimalParaDecimal(recuperaToken(i+2).palavra);
					}
					qtdeSimb++;
					i += 2;

				} else if (!strcmp(".word",tk.palavra)) { 
					// Avanca uma posicao
					pontoMontagem++;
					flagDireita = 0;
					i++;

				} else if (!strcmp(".wfill",tk.palavra)) {
					// Avanca n posicoes
					int n = palavraDecimalParaDecimal(recuperaToken(i+1).palavra);
					pontoMontagem += n;
					flagDireita = 0;
					i += 2;

				} else if (!strcmp(".align",tk.palavra)) {
					// Alinha o ponto de montagem
					int align = palavraDecimalParaDecimal(recuperaToken(i+1).palavra);
					if (flagDireita) {
						pontoMontagem++;
						flagDireita = 0;
					}
					while(pontoMontagem % align) {
						pontoMontagem++;
					}
					i++;

				}
				break;

			case DefRotulo:
				// Identifica a definicao do rotulo e armazena os dados 
				memcpy(rotulos[qtdeRot].chave,tk.palavra,strlen(tk.palavra)-1);
				rotulos[qtdeRot].valor = pontoMontagem;
				rotulos[qtdeRot].alinhamento = flagDireita;
				qtdeRot++;
				break;

			case Hexadecimal:
			case Decimal:
			case Nome:	
				break;
		}
	}

	// Reinicializa as variaveis de montagem
	pontoMontagem = 0;
	flagDireita = 0;

	// Processa a montagem para os tokens
	for (i = 0; i < getNumberOfTokens(); i++) {
		Token tk = recuperaToken(i);
		
		// Identifica o tipo do token
		switch(tk.tipo) {
			case Instrucao:
				// Valida se nao ha sobrescricao
				if(!flagPreenchido[pontoMontagem] || (flagDireita && !strncmp("00000",&mapaMemoria[pontoMontagem][5],5))) {
					int flagRecebeEndereco = 0;
					char dig1,dig2;

					// Trata cada instrucao e armazena o codigo hexadecimal
					if(!strcmp("LOAD",tk.palavra)) {
						flagRecebeEndereco = 1;
						if(flagDireita) {
							dig1 = '0';
							dig2 = '1';
						} else {
							dig1 = '0';
							dig2 = '1';
						}
					} else if(!strcmp("LOAD-",tk.palavra)) {
						flagRecebeEndereco = 1;
						if(flagDireita) {
							dig1 = '0';
							dig2 = '2';
						} else {
							dig1 = '0';
							dig2 = '2';
						}
					} else if(!strcmp("LOAD|",tk.palavra)) {
						flagRecebeEndereco = 1;
						if(flagDireita) {
							dig1 = '0';
							dig2 = '3';
						} else {
							dig1 = '0';
							dig2 = '3';
						}
					} else if(!strcmp("LOADmq",tk.palavra)) {
						if(flagDireita) {
							dig1 = '0';
							dig2 = 'A';
						} else {
							dig1 = '0';
							dig2 = 'A';
						}
					} else if(!strcmp("LOADmq_mx",tk.palavra)) {
						flagRecebeEndereco = 1;
						if(flagDireita) {
							dig1 = '0';
							dig2 = '9';
						} else {
							dig1 = '0';
							dig2 = '9';
						}
					} else if(!strcmp("STOR",tk.palavra)) {
						flagRecebeEndereco = 1;
						if(flagDireita) {
							dig1 = '2';
							dig2 = '1';
						} else {
							dig1 = '2';
							dig2 = '1';
						}		
					} else if(!strcmp("JUMP",tk.palavra)) {
						flagRecebeEndereco = 1;

						dig1 = '0';
						dig2 = 'D';

						// Processa se o parametro esta alinhado a direita
						if(recuperaToken(i+1).tipo == Nome) {
							int flagRot = 0;
							for (int m = 0; m < qtdeRot && !flagRot; m++) {
								if (!strcmp(rotulos[m].chave,recuperaToken(i+1).palavra)) {
									
									if(rotulos[m].alinhamento) {
										dig1 = '0';
										dig2 = 'E';
									} 

									flagRot = 1;
								}
							}

							// Verifica se nao encontrou o rotulo
							if(!flagRot) {
								fprintf(stderr,"USADO MAS NÃO DEFINIDO: %s!\n",recuperaToken(i+1).palavra);
								return 1;
							}
						}
						
					} else if(!strcmp("JMP+",tk.palavra)) {
						flagRecebeEndereco = 1;
						
						dig1 = '0';
						dig2 = 'F';

						// Processa se o parametro esta alinhado a direita
						if(recuperaToken(i+1).tipo == Nome) {
							int flagRot = 0;
							for (int m = 0; m < qtdeRot && !flagRot; m++) {
								if (!strcmp(rotulos[m].chave,recuperaToken(i+1).palavra)) {
									
									if(rotulos[m].alinhamento) {
										dig1 = '1';
										dig2 = '0';
									} 

									flagRot = 1;
								}
							}

							// Verifica se nao encontrou o rotulo
							if(!flagRot) {
								fprintf(stderr,"USADO MAS NÃO DEFINIDO: %s!\n",recuperaToken(i+1).palavra);
								return 1;
							}
						}

					} else if(!strcmp("ADD",tk.palavra)) {
						flagRecebeEndereco = 1;
						if(flagDireita) {
							dig1 = '0';
							dig2 = '5';
						} else {
							dig1 = '0';
							dig2 = '5';
						}
					} else if(!strcmp("ADD|",tk.palavra)) {
						flagRecebeEndereco = 1;
						if(flagDireita) {
							dig1 = '0';
							dig2 = '7';
						} else {
							dig1 = '0';
							dig2 = '7';
						}
					} else if(!strcmp("SUB",tk.palavra)) {
						flagRecebeEndereco = 1;
						if(flagDireita) {
							dig1 = '0';
							dig2 = '6';
						} else {
							dig1 = '0';
							dig2 = '6';
						}
					} else if(!strcmp("SUB|",tk.palavra)) {
						flagRecebeEndereco = 1;
						if(flagDireita) {
							dig1 = '0';
							dig2 = '8';
						} else {
							dig1 = '0';
							dig2 = '8';
						}
					} else if(!strcmp("MUL",tk.palavra)) {
						flagRecebeEndereco = 1;
						if(flagDireita) {
							dig1 = '0';
							dig2 = 'B';
						} else {
							dig1 = '0';
							dig2 = 'B';
						}
					} else if(!strcmp("DIV",tk.palavra)) {
						flagRecebeEndereco = 1;
						if(flagDireita) {
							dig1 = '0';
							dig2 = 'C';
						} else {
							dig1 = '0';
							dig2 = 'C';
						}
					} else if(!strcmp("RSH",tk.palavra)) {
						if(flagDireita) {
							dig1 = '1';
							dig2 = '5';
						} else {
							dig1 = '1';
							dig2 = '5';
						}

					} else if(!strcmp("LSH",tk.palavra)) {
						if(flagDireita) {
							dig1 = '1';
							dig2 = '4';
						} else {
							dig1 = '1';
							dig2 = '4';
						}

					} else if(!strcmp("STORA",tk.palavra)) {
						flagRecebeEndereco = 1;

						dig1 = '1';
						dig2 = '2';

						// Processa se o parametro esta alinhado a direita
						if(recuperaToken(i+1).tipo == Nome) {
							int flagRot = 0;
							for (int m = 0; m < qtdeRot && !flagRot; m++) {
								if (!strcmp(rotulos[m].chave,recuperaToken(i+1).palavra)) {
									
									if(rotulos[m].alinhamento) {
										dig1 = '1';
										dig2 = '3';
									} 

									flagRot = 1;
								}
							}

							// Verifica se nao encontrou o rotulo
							if(!flagRot) {
								fprintf(stderr,"USADO MAS NÃO DEFINIDO: %s!\n",recuperaToken(i+1).palavra);
								return 1;
							}
						}
					}

					// Escreve no mapa de memoria de acordo com o alinhamento
					if(flagDireita) {
						mapaMemoria[pontoMontagem][5] = dig1;
						mapaMemoria[pontoMontagem][6] = dig2;
					} else {
						mapaMemoria[pontoMontagem][0] = dig1;
						mapaMemoria[pontoMontagem][1] = dig2;
					}

					// Processa e grava o parametro na memoria se existir
					if(flagRecebeEndereco) {
						int m,n;

						if(recuperaToken(i+1).tipo == Hexadecimal) {
							if(flagDireita) {
								for (m = strlen(recuperaToken(i+1).palavra)-1,n = 9; m >= 2 && n >= 7; m--,n--) {
									mapaMemoria[pontoMontagem][n] = toupper(recuperaToken(i+1).palavra[m]);
								}
							} else {
								for (m = strlen(recuperaToken(i+1).palavra)-1,n = 4; m >= 2 && n >= 2; m--,n--) {
									mapaMemoria[pontoMontagem][n] = toupper(recuperaToken(i+1).palavra[m]);
								}
							}
														
						} else if(recuperaToken(i+1).tipo == Decimal) {
							char hex[10];
							sprintf(hex, "%x", palavraDecimalParaDecimal(recuperaToken(i+1).palavra));
							if(flagDireita) {
								for (m = strlen(hex)-1,n = 9; m >= 0 && n >= 7; m--,n--) {
									mapaMemoria[pontoMontagem][n] = toupper(hex[m]);
								}
							} else {
								for (m = strlen(hex)-1,n = 4; m >= 0 && n >= 2; m--,n--) {
									mapaMemoria[pontoMontagem][n] = toupper(hex[m]);
								}
							}

						} else {
							int flagSymRot = 0;
							
							// Busca no vetor de simbolos
							for (m = 0; m < qtdeSimb && !flagSymRot; m++) {
								if (!strcmp(simbolos[m].chave,recuperaToken(i+1).palavra)) {
									char hex[10];
									sprintf(hex, "%x", simbolos[m].valor);
									
									if(flagDireita) {
										for (m = strlen(hex)-1,n = 9; m >= 0 && n >= 7; m--,n--) {
											mapaMemoria[pontoMontagem][n] = toupper(hex[m]);
										}
									} else {
										for (m = strlen(hex)-1,n = 4; m >= 0 && n >= 2; m--,n--) {
											mapaMemoria[pontoMontagem][n] = toupper(hex[m]);
										}
									}
									flagSymRot = 1;
								}
							}

							// Busca no vetor de rotulos
							for (m = 0; m < qtdeRot && !flagSymRot; m++) {
								if (!strcmp(rotulos[m].chave,recuperaToken(i+1).palavra)) {
									char hex[10];
									sprintf(hex, "%x", rotulos[m].valor);
									
									if(flagDireita) {
										for (m = strlen(hex)-1,n = 9; m >= 0 && n >= 7; m--,n--) {
											mapaMemoria[pontoMontagem][n] = toupper(hex[m]);
										}
									} else {
										for (m = strlen(hex)-1,n = 4; m >= 0 && n >= 2; m--,n--) {
											mapaMemoria[pontoMontagem][n] = toupper(hex[m]);
										}
									}
									flagSymRot = -1;
								}
							}

							// Verifica se nao encontrou o simbolo/rotulo
							if(!flagSymRot) {
								fprintf(stderr,"USADO MAS NÃO DEFINIDO: %s!\n",recuperaToken(i+1).palavra);
								return 1;
							}

						}

						i++;
					}

					// Atualiza as variaveis de montagem
					flagPreenchido[pontoMontagem] = 1;
					if(flagDireita) {
						flagDireita = 0;
						pontoMontagem++;
					} else {
						flagDireita = 1;
					}

				} else {
					// Dispara um erro caso nao encontre a instrucao
					fprintf(stderr,"Impossível montar o código!\n");
					return 1;
				}

				break;

			case Diretiva:
				if(!strcmp(".org",tk.palavra)) {
					// Avanca a montagem para o endereco especificado
					if(recuperaToken(i+1).tipo == Hexadecimal) {
						pontoMontagem = palavraHexadecimalParaDecimal(recuperaToken(i+1).palavra);
					} else if(recuperaToken(i+1).tipo == Decimal) {
						pontoMontagem = palavraDecimalParaDecimal(recuperaToken(i+1).palavra);
					} 
					i++;

				} else if (!strcmp(".word",tk.palavra)) { 
					if(flagDireita) {
						// Erro se o alinhamento eh invaildo
						fprintf(stderr,"Impossível montar o código!\n");
						return 1;
					} else if (flagPreenchido[pontoMontagem]){
						// Erro se ha sobrescricao
						fprintf(stderr,"Impossível montar o código!\n");
						return 1;
					} else {
						// Preenche o mapa de memoria com o valor 
						int m,n;
						if(recuperaToken(i+1).tipo == Hexadecimal) {
							for (m = strlen(recuperaToken(i+1).palavra)-1,n = 9; m >= 2 && n >= 0; m--,n--) {
								mapaMemoria[pontoMontagem][n] = toupper(recuperaToken(i+1).palavra[m]);
							}
							
						} else if(recuperaToken(i+1).tipo == Decimal) {
							char hex[10];
							sprintf(hex, "%x", palavraDecimalParaDecimal(recuperaToken(i+1).palavra));
							
							for (m = strlen(hex)-1,n = 9; m >= 0 && n >= 0; m--,n--) {
								mapaMemoria[pontoMontagem][n] = toupper(hex[m]);
							}

						} else {
							int flagSymRot = 0;

							// Busca no vetor de simbolos
							for (m = 0; m < qtdeSimb && !flagSymRot; m++) {
								if (!strcmp(simbolos[m].chave,recuperaToken(i+1).palavra)) {
									char hex[10];
									sprintf(hex, "%x", simbolos[m].valor);
									
									for (m = strlen(hex)-1,n = 9; m >= 0 && n >= 0; m--,n--) {
										mapaMemoria[pontoMontagem][n] = toupper(hex[m]);
									}
									flagSymRot = 1;
								}
							}

							// Busca no vetor de rotulos
							for (m = 0; m < qtdeRot && !flagSymRot; m++) {
								if (!strcmp(rotulos[m].chave,recuperaToken(i+1).palavra)) {
									char hex[10];
									sprintf(hex, "%x", rotulos[m].valor);
									
									for (m = strlen(hex)-1,n = 9; m >= 0 && n >= 0; m--,n--) {
										mapaMemoria[pontoMontagem][n] = toupper(hex[m]);
									}
									flagSymRot = -1;
								}
							}

							// Verifica se nao encontrou o simbolo/rotulo
							if(!flagSymRot) {
								fprintf(stderr,"USADO MAS NÃO DEFINIDO: %s!\n",recuperaToken(i+1).palavra);
								return 1;
							}
						} 

						// Atualiza as variaveis de montagem
						flagPreenchido[pontoMontagem] = 1;
						pontoMontagem++;
					}
					i++;

				} else if (!strcmp(".wfill",tk.palavra)) {
					if(flagDireita) {
						// Erro se o alinhamento eh invaildo
						fprintf(stderr,"Impossível montar o código!\n");
						return 1;
					} else {
						// Le o valor a ser salvo no mapa de memoria
						char temp[11] = {'0','0','0','0','0','0','0','0','0','0','\0'};
						int m,n;
						if(recuperaToken(i+2).tipo == Hexadecimal) {
							for (m = strlen(recuperaToken(i+2).palavra)-1,n = 9; m >= 2 && n >= 0; m--,n--) {
								temp[n] = toupper(recuperaToken(i+2).palavra[m]);
							}

						} else if(recuperaToken(i+2).tipo == Decimal) {
							char hex[10];
							sprintf(hex, "%x", palavraDecimalParaDecimal(recuperaToken(i+2).palavra));
							
							for (m = strlen(hex)-1,n = 9; m >= 0 && n >= 0; m--,n--) {
								temp[n] = toupper(hex[m]);
							}

						} else {
							int flagSymRot = 0;

							// Busca no vetor de simbolos
							for (m = 0; m < qtdeSimb && !flagSymRot; m++) {
								if (!strcmp(simbolos[m].chave,recuperaToken(i+2).palavra)) {
									char hex[10];
									sprintf(hex, "%x", simbolos[m].valor);
									
									for (m = strlen(hex)-1,n = 9; m >= 0 && n >= 0; m--,n--) {
										temp[n] = toupper(hex[m]);
									}
									flagSymRot = 1;
								}
							}

							// Busca no vetor de rotulos
							for (m = 0; m < qtdeRot && !flagSymRot; m++) {
								if (!strcmp(rotulos[m].chave,recuperaToken(i+2).palavra)) {
									char hex[10];
									sprintf(hex, "%x", rotulos[m].valor);
									
									for (m = strlen(hex)-1,n = 9; m >= 0 && n >= 0; m--,n--) {
										temp[n] = toupper(hex[m]);
									}
									flagSymRot = -1;
								}
							}

							// Verifica se nao encontrou o simbolo/rotulo
							if(!flagSymRot) {
								fprintf(stderr,"USADO MAS NÃO DEFINIDO: %s!\n",recuperaToken(i+2).palavra);
								return 1;
							}
						}

						// Obtem a quantidade de palavras a serem preenchidas
						n = palavraDecimalParaDecimal(recuperaToken(i+1).palavra);

						// Preenche a memoria e atualiza as variaveis de montagem,
						// verificando se ha sobrescricao
						for (m = 0; m < n; m++){
							if (flagPreenchido[pontoMontagem]){
								fprintf(stderr,"Impossível montar o código!\n");
								return 1;
							} else {
								strcpy(mapaMemoria[pontoMontagem],temp);
								flagPreenchido[pontoMontagem] = 1;
								pontoMontagem++;
							}								
						}
					}
					i += 2;

				} else if (!strcmp(".align",tk.palavra)) {
					// Alinha as variaveis de montagem
					int align = palavraDecimalParaDecimal(recuperaToken(i+1).palavra);
					if (flagDireita) {
						pontoMontagem++;
						flagDireita = 0;
					}
					while(pontoMontagem % align) {
						pontoMontagem++;
					}
					i++;
				}

				break;

			case DefRotulo:
			case Hexadecimal:
			case Decimal:
			case Nome:
				break;
		}
	}

	// Imprime o mapa de memoria
	for (i = 0; i < NPALAVRAS; i++) {
		if(flagPreenchido[i]) {
			printf("%03X ",(int)(i & 0xFFF));
			printf("%c%c ",mapaMemoria[i][0],mapaMemoria[i][1]);
			printf("%c%c%c ",mapaMemoria[i][2],mapaMemoria[i][3],mapaMemoria[i][4]);
			printf("%c%c ",mapaMemoria[i][5],mapaMemoria[i][6]);
			printf("%c%c%c\n",mapaMemoria[i][7],mapaMemoria[i][8],mapaMemoria[i][9]);
		}
	}

	return 0;
}

// Funcao de conversao da string com um valor decimal para um inteiro decimal
int palavraDecimalParaDecimal(char * palavra) {
	int valor = 0;
	int len = strlen(palavra);

	for (int i = len-1; i >= 0; i--) {
		valor += potencia(10,len-1-i) * (palavra[i] - '0'); 
	}

	return valor;
}

// Funcao de conversao da string com um valor hexadecimal para um inteiro decimal
int palavraHexadecimalParaDecimal(char * palavra) {
	int valor = 0;
	int len = strlen(palavra);

	for (int i = len-1; i >= 2; i--) {
		if(isdigit(palavra[i])) {
			valor += potencia(16,len-1-i) * (palavra[i] - '0'); 
		} else {
			valor += potencia(16,len-1-i) * (palavra[i] - 'A' + 10); 
		}
	}

	return valor;
}

// Funcao de potencia matematica
int potencia(int base,int expoente) {
	int res = 1;
	for (int i = 0; i < expoente; i++) {
		res *= base; 	
	}
	return res;
}