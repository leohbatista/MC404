#!/usr/bin/python3

import os

for x in range(0,9):
	os.system("./montador.x testes-abertos/arq0"+str(x)+".in > testes-abertos/arq0"+str(x)+".out")
	passou = os.system("diff testes-abertos/arq0"+str(x)+".out testes-abertos/arq0"+str(x)+".parte1.res")
	print("Teste 0",x," - ")
	if(passou == 0):
		print("Passou")
	else:
		print("Erro")
		