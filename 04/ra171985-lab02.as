.org 0x000
    LOAD MQ,M(entrada)
    MUL M(gravidade)
    LOAD MQ
    STOR M(gx)
    RSH
    STOR M(k)
laco_calculo:
    LOAD M(contador)
    ADD M(constante1)
    STOR M(contador)
    LOAD M(gx)	
    DIV M(k)
    LOAD MQ
    ADD M(k)
    RSH
    STOR M(k)
    LOAD M(gravidade)
    SUB M(contador)
    JUMP+ M(laco_calculo,0:19)
carrega_resultado:
    LOAD M(k)
    JUMP M(0x400,0:19)

.org 0x100
gravidade:
    .word 10
constante1:
    .word 1
contador:
    .word 1
gx:    
    .word 0
k:
    .word 0
    
.org 0x105
entrada:
    .word 0