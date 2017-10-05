#include "montador.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#ifndef NPALAVRAS
#define NPALAVRAS 1024
#endif

typedef struct Simbolo {
	char * chave;
	int valor;
} Simbolo;

typedef struct Rotulo {
	char chave[65];
	int valor;
	int alinhamento;
} Rotulo;

int palavraDecimalParaDecimal(char * palavra);
int palavraHexadecimalParaDecimal(char * palavra);
int potencia(int base,int expoente);

/* Retorna:
 *  1 caso haja erro na montagem; 
 *  0 caso não haja erro.
 */
int emitirMapaDeMemoria() {
	char mapaMemoria[NPALAVRAS][10];
	int flagPreenchido[NPALAVRAS];
	int i;
	int qtdeSimb = 0;
	int qtdeRot = 0;
	int nTokens = getNumberOfTokens();

	Simbolo * simbolos;
	simbolos = (Simbolo *) malloc((nTokens/3) * sizeof(Simbolo)); 
	
	Rotulo * rotulos;
	rotulos = (Rotulo *) malloc(nTokens * sizeof(Rotulo));

	int pontoMontagem = 0;
	int flagDireita = 0;

	// Inicializa a matriz do mapa de memoria e a flag de preenchimento 
	for (i = 0; i < NPALAVRAS; ++i) {
		flagPreenchido[i] = 0;

		for(int j = 0; j < 10; j++) {
			mapaMemoria[i][j] = '0';
		}		
	}

	for (i = 0; i < getNumberOfTokens(); i++){
		Token tk = recuperaToken(i);
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
					if(recuperaToken(i+1).tipo == Hexadecimal) {
						pontoMontagem = palavraHexadecimalParaDecimal(tk.palavra);
					} else if(recuperaToken(i+1).tipo == Decimal) {
						pontoMontagem = palavraDecimalParaDecimal(tk.palavra);
					}
					flagDireita = 0;
					i++;
				} else if(!strcmp(".set",tk.palavra)) {
					simbolos[qtdeSimb].chave = recuperaToken(i+1).palavra;
					if(recuperaToken(i+2).tipo == Hexadecimal) {
						simbolos[qtdeSimb].valor = palavraHexadecimalParaDecimal(recuperaToken(i+2).palavra);
					} else {
						simbolos[qtdeSimb].valor = palavraDecimalParaDecimal(recuperaToken(i+2).palavra);
					}
					qtdeSimb++;
					i += 2;
				} else if (!strcmp(".word",tk.palavra)) { 
					pontoMontagem++;
					flagDireita = 0;
					i++;
				} else if (!strcmp(".wfill",tk.palavra)) {
					int n = palavraDecimalParaDecimal(recuperaToken(i+1).palavra);
					pontoMontagem += n;
					flagDireita = 0;
					i += 2;
				} else if (!strcmp(".align",tk.palavra)) {
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
				memcpy(rotulos[qtdeRot].chave,tk.palavra,strlen(tk.palavra)-1);
				rotulos[qtdeRot].valor = pontoMontagem;
				rotulos[qtdeRot].alinhamento = flagDireita;
				//printf("%s 0x%x %d\n",rotulos[qtdeRot].chave,rotulos[qtdeRot].valor,rotulos[qtdeRot].alinhamento);
				qtdeRot++;
				break;
			case Hexadecimal:
			case Decimal:
			case Nome:	
				break;
		}
	}

	pontoMontagem = 0;
	flagDireita = 0;

	for (i = 0; i < getNumberOfTokens(); i++) {
		Token tk = recuperaToken(i);
		switch(tk.tipo) {
			case Instrucao:
				if(!flagPreenchido[pontoMontagem] || (flagDireita && !strncmp("00000",&mapaMemoria[pontoMontagem][5],5))) {
					int flagRecebeEndereco = 0;
					char dig1,dig2;
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

						if(recuperaToken(i+1).tipo == Nome) {
							int flagSymRot = 0;
							for (int m = 0; m < qtdeRot && !flagSymRot; m++) {
								if (!strcmp(rotulos[m].chave,recuperaToken(i+1).palavra)) {
									
									if(rotulos[m].alinhamento) {
										dig1 = '0';
										dig2 = 'E';
									} 

									flagSymRot = 1;
								}
							}

							if(!flagSymRot) {
								fprintf(stderr,"USADO MAS NÃO DEFINIDO: %s!\n",recuperaToken(i+1).palavra);
								return 1;
							}
						}
						
					} else if(!strcmp("JMP+",tk.palavra)) {
						flagRecebeEndereco = 1;
						
						dig1 = '0';
						dig2 = 'F';

						if(recuperaToken(i+1).tipo == Nome) {
							int flagSymRot = 0;
							for (int m = 0; m < qtdeRot && !flagSymRot; m++) {
								if (!strcmp(rotulos[m].chave,recuperaToken(i+1).palavra)) {
									
									if(rotulos[m].alinhamento) {
										dig1 = '1';
										dig2 = '0';
									} 

									flagSymRot = 1;
								}
							}

							if(!flagSymRot) {
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

						if(recuperaToken(i+1).tipo == Nome) {
							int flagSymRot = 0;
							for (int m = 0; m < qtdeRot && !flagSymRot; m++) {
								if (!strcmp(rotulos[m].chave,recuperaToken(i+1).palavra)) {
									
									if(rotulos[m].alinhamento) {
										dig1 = '1';
										dig2 = '3';
									} 

									flagSymRot = 1;
								}
							}

							if(!flagSymRot) {
								fprintf(stderr,"USADO MAS NÃO DEFINIDO: %s!\n",recuperaToken(i+1).palavra);
								return 1;
							}
						}
					}

					if(flagDireita) {
						mapaMemoria[pontoMontagem][5] = dig1;
						mapaMemoria[pontoMontagem][6] = dig2;
					} else {
						mapaMemoria[pontoMontagem][0] = dig1;
						mapaMemoria[pontoMontagem][1] = dig2;
					}

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

							if(!flagSymRot) {
								fprintf(stderr,"USADO MAS NÃO DEFINIDO: %s!\n",recuperaToken(i+1).palavra);
								return 1;
							}
						}
						i++; 
					}


					flagPreenchido[pontoMontagem] = 1;
					if(flagDireita) {
						flagDireita = 0;
						pontoMontagem++;
					} else {
						flagDireita = 1;
					}
				} else {
					fprintf(stderr,"Impossível montar o código!\n");
					return 1;
				}

				break;
			case Diretiva:
				if(!strcmp(".org",tk.palavra)) {
					if(recuperaToken(i+1).tipo == Hexadecimal) {
						pontoMontagem = palavraHexadecimalParaDecimal(recuperaToken(i+1).palavra);
					} else if(recuperaToken(i+1).tipo == Decimal) {
						pontoMontagem = palavraDecimalParaDecimal(recuperaToken(i+1).palavra);
					} 
					i++;
				} else if (!strcmp(".word",tk.palavra)) { 
					if(flagDireita) {
						fprintf(stderr,"Impossível montar o código!\n");
						return 1;
					} else if (flagPreenchido[pontoMontagem]){
						fprintf(stderr,"Impossível montar o código!\n");
						return 1;
					} else {
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

							if(!flagSymRot) {
								fprintf(stderr,"USADO MAS NÃO DEFINIDO: %s!\n",recuperaToken(i+1).palavra);
								return 1;
							}
						} 

						flagPreenchido[pontoMontagem] = 1;
						pontoMontagem++;
					}
					i++;
				} else if (!strcmp(".wfill",tk.palavra)) {
					if(flagDireita) {
						fprintf(stderr,"Impossível montar o código!\n");
						return 1;
					} else {
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

							if(!flagSymRot) {
								fprintf(stderr,"USADO MAS NÃO DEFINIDO: %s!\n",recuperaToken(i+2).palavra);
								return 1;
							}
						}

						n = palavraDecimalParaDecimal(recuperaToken(i+1).palavra);

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
					i+=2;
				} else if (!strcmp(".align",tk.palavra)) {
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

int palavraDecimalParaDecimal(char * palavra) {
	int valor = 0;
	int len = strlen(palavra);

	for (int i = len-1; i >= 0; i--) {
		valor += potencia(10,len-1-i) * (palavra[i] - '0'); 
	}

	return valor;
}

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

int potencia(int base,int expoente) {
	int res = 1;
	for (int i = 0; i < expoente; i++) {
		res *= base; 	
	}
	return res;
}