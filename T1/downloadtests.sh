#!/bin/bash
for i in `seq 0 9`;
do
	./montador.x testes/arq0$i.in &> testes/arq0$i.out;
	diff testes/arq0$i.out testes/arq0$i.res;
done    

for i in `seq 0 9`;
do
	./montador.x testes/arq1$i.in &> testes/arq1$i.out;
	diff testes/arq1$i.out testes/arq1$i.res;
done    

for i in `seq 0 2`;
do
	./montador.x testes/arq2$i.in &> testes/arq2$i.out;
	diff testes/arq2$i.out testes/arq2$i.res;
done    
