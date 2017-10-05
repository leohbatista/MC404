#!/bin/bash
for i in `seq 0 9`;
do
	./montador.x testes/arq0$i.in &> testes/arq0$i.out;
	diff testes/arq0$i.out testes/arq0$i.res;
done    
