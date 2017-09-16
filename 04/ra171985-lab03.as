.org 0x000
    LOAD M(inicio1)
    STOR M(pi)
    STA M(for_vetor,8:19)
    LOAD M(inicio2)	
    STOR M(pj)
    STA M(for_vetor,28:39)
for_vetor:   
    LOAD MQ,M(pi)
    MUL M(pj)		
    LOAD MQ
    ADD M(soma)
    STOR M(soma)
    LOAD M(contador)	
    ADD M(constante1)
    STOR M(contador)
    LOAD M(pi)
    ADD M(constante1)
    STOR M(pi)
    LOAD M(pj)
    ADD M(constante1)
    STOR M(pj)
    STA M(for_vetor,28:39)
    LOAD M(pi)
    STA M(for_vetor,8:19)
    LOAD M(tamanho)
    SUB M(contador)
    JUMP+ M(for_vetor,0:19)
    
    LOAD M(soma)
    JUMP M(0x400,0:19)


.org 0x050
contador:
    .word 1
constante1:    
    .word 1
soma:
    .word 0
pi:
    .word 0
pj:
    .word 0


.org 0x3FD
inicio1:
    .word 0
inicio2:
    .word 0
tamanho:
    .word 0