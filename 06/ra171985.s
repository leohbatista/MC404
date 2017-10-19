.globl _start

.data

input_buffer:   .skip 32
output_buffer:  .skip 32
    
.text
.align 4

@ Funcao inicial
_start:
    @ Chama a funcao "read" para ler 4 caracteres da entrada padrao
    ldr r0, =input_buffer
    mov r1, #5             @ 4 caracteres + '\n'
    bl  read
    mov r4, r0             @ copia o retorno para r4.

    @ Chama a funcao "atoi" para converter a string para um numero
    ldr r0, =input_buffer
    mov r1, r4
    bl  atoi

    @ Chama a funcao "encode" para codificar o valor de r0 usando
    @ o codigo de hamming.
    bl  encode
    mov r4, r0             @ copia o retorno para r4.
	
    @ Chama a funcao "itoa" para converter o valor codificado
    @ para uma sequencia de caracteres '0's e '1's
    ldr r0, =output_buffer
    mov r1, #7
    mov r2, r4
    bl  itoa

    @ Adiciona o caractere '\n' ao final da sequencia (byte 7)
    ldr r0, =output_buffer
    mov r1, #'\n'
    strb r1, [r0, #7]

    @ Chama a funcao write para escrever os 7 caracteres e
    @ o '\n' na saida padrao.
    ldr r0, =output_buffer
    mov r1, #8         @ 7 caracteres + '\n'
    bl  write

    @--------------------------------------------------------------

    @ Chama a funcao "read" para ler 7 caracteres da entrada padrao
    ldr r0, =input_buffer
    mov r1, #8             @ 7 caracteres + '\n'
    bl  read
    mov r4, r0             @ copia o retorno para r4.

    @ Chama a funcao "atoi" para converter a string para um numero
    ldr r0, =input_buffer
    mov r1, r4
    bl  atoi

    @ Chama a funcao "encode" para codificar o valor de r0 usando
    @ o codigo de hamming.
    bl  decode
    mov r4, r0             @ copia o retorno em r0 para r4.
    mov r5, r1             @ copia o retorno em r1 para r5.
    
    @ Chama a funcao "itoa" para converter o valor codificado
    @ para uma sequencia de caracteres '0's e '1's
    ldr r0, =output_buffer
    mov r1, #4
    mov r2, r4
    bl  itoa

    @ Adiciona o caractere '\n' ao final da sequencia (byte 7)
    ldr r0, =output_buffer
    mov r1, #'\n'
    strb r1, [r0, #4]

    @ Chama a funcao write para escrever os 7 caracteres e
    @ o '\n' na saida padrao.
    ldr r0, =output_buffer
    mov r1, #5         @ 4 caracteres + '\n'
    bl  write

    @ Chama a funcao "itoa" para converter o valor codificado
    @ para uma sequencia de caracteres '0's e '1's
    ldr r0, =output_buffer
    mov r1, #1
    mov r2, r5
    bl  itoa

    @ Adiciona o caractere '\n' ao final da sequencia (byte 7)
    ldr r0, =output_buffer
    mov r1, #'\n'
    strb r1, [r0, #1]

    @ Chama a funcao write para escrever os 7 caracteres e
    @ o '\n' na saida padrao.
    ldr r0, =output_buffer
    mov r1, #2         @ 1 caractere + '\n'
    bl  write

    @ Chama a funcao exit para finalizar processo.
    mov r0, #0
    bl  exit

@ Codifica o valor de entrada usando o codigo de hamming.
@ parametros:
@  r0: valor de entrada (4 bits menos significativos)
@ retorno:
@  r0: valor codificado (7 bits como especificado no enunciado).
encode:    
    push {r4-r11, lr}
    
    mov r5, r0    @ Copia a entrada para r5
    and r5, r5, #0b1000 @ Armazena apenas o bit d1 em r5

    mov r6, r0    @ Copia a entrada para r6
    and r6, r6, #0b0100 @ Armazena apenas o bit d2 em r6

    mov r7, r0    @ Copia a entrada para r7
    and r7, r7, #0b0010 @ Armazena apenas o bit d3 em r7

    mov r8, r0    @ Copia a entrada para r8
    and r8, r8, #0b0001 @ Armazena apenas o bit d4 em r8
    
    orr r0, r0, r5, lsl #1
    and r0, r0, #0b1110111

    @ Calcula o primeiro bit de paridade (p1) e armazena em r0
    eor r9, r5, r6, lsl #1
    eor r9, r9, r8, lsl #3
    orr r0, r0, r9, lsl #3

    @ Calcula o primeiro bit de paridade (p2) e armazena em r0
    eor r9, r5, r7, lsl #2
    eor r9, r9, r8, lsl #3
    orr r0, r0, r9, lsl #2

    @ Calcula o primeiro bit de paridade (p3) e armazena em r0
    eor r9, r6, r7, lsl #1
    eor r9, r9, r8, lsl #2
    orr r0, r0, r9, lsl #1
    
    pop  {r4-r11, lr}
    mov  pc, lr

@ Decodifica o valor de entrada usando o codigo de hamming.
@ parametros:
@  r0: valor de entrada (7 bits menos significativos)
@ retorno:
@  r0: valor decodificado (4 bits como especificado no enunciado).
@  r1: 1 se houve erro e 0 se nao houve.
decode:    
    push {r4-r11, lr}
    
    mov r2, #0

    mov r5,r0   @ Copia a entrada para r5
    and r5, r5, #0b1000000 @ Armazena apenas o bit p1 em r5
    mov r6,r0    @ Copia a entrada para r6
    and r6, r6, #0b0100000 @ Armazena apenas o bit p2 em r6
    mov r7,r0    @ Copia a entrada para r6
    and r7, r7, #0b0001000 @ Armazena apenas o bit p3 em r7

    and r2, r5, r6, lsl #1 @ Calcula d1 e armazena em r2
    mov r2, r2, lsr #3 @ Ajusta d1 para a posicao correta

    and r3, r5, r7, lsl #3 @ Calcula d2 e armazena em r3 (auxiliar)
    orr r2, r2, r3, lsr #4 @ Armazena d2 em r2

    and r3, r6, r7, lsl #2 @ Calcula d3 e armazena em r3 (auxiliar)
    orr r2, r2, r3, lsr #4 @ Armazena d3 em r2

    eor r3, r5, r6, lsl #1 @ Calcula d4 e armazena em r3 (auxiliar)
    eor r3, r3, r7, lsl #3 @ Calcula d4 e armazena em r3 (auxiliar)
    orr r2, r2, r3, lsr #6 @ Armazena d4 em r2

    
    mov r8, r0    @ Copia a entrada para r8
    eor r8, r8, r8, lsr #2 @ Realiza a validacao
    eor r8, r8, r8, lsr #3 @    dos digitos, armazenando o resultado
    eor r8, r8, r5, lsr #6 @        de interesse no bit menos significativo
    and r8, r8, #1 @ Mantem apenas o bit com o resultado
    mov r1, r8 @ Move a flag para a saída

    mov r9, #1

    cmp r1,r9
    bne return
        mov r8, r0    @ Copia a entrada para r8
        eor r8, r8, r8, lsr #1 @ Realiza a validacao
        eor r8, r8, r8, lsr #3 @    dos digitos, armazenando o resultado
        eor r8, r8, r6, lsr #5 @        de interesse no bit menos significativo
        and r8, r8, #1 @ Mantem apenas o bit com o resultado
        mov r1, r8 @ Move a flag para a saída

        cmp r1,r9
        bne return
            mov r8, r0    @ Copia a entrada para r8
            eor r8, r8, r8, lsr #1 @ Realiza a validacao
            eor r8, r8, r8, lsr #2 @    dos digitos, armazenando o resultado
            eor r8, r8, r6, lsr #4 @        de interesse no bit menos significativo
            and r8, r8, #1 @ Mantem apenas o bit com o resultado
            mov r1, r8 @ Move a flag para a saída

    return:
    mov r0, r2 

    pop  {r4-r11, lr}
    mov  pc, lr

@ Le uma sequencia de bytes da entrada padrao.
@ parametros:
@  r0: endereco do buffer de memoria que recebera a sequencia de bytes.
@  r1: numero maximo de bytes que pode ser lido (tamanho do buffer).
@ retorno:
@  r0: numero de bytes lidos.
read:
    push {r4,r5, lr}
    mov r4, r0
    mov r5, r1
    mov r0, #0         @ stdin file descriptor = 0
    mov r1, r4         @ endereco do buffer
    mov r2, r5         @ tamanho maximo.
    mov r7, #3         @ read
    svc 0x0
    pop {r4, r5, lr}
    mov pc, lr

@ Escreve uma sequencia de bytes na saida padrao.
@ parametros:
@  r0: endereco do buffer de memoria que contem a sequencia de bytes.
@  r1: numero de bytes a serem escritos
write:
    push {r4,r5, lr}
    mov r4, r0
    mov r5, r1
    mov r0, #1         @ stdout file descriptor = 1
    mov r1, r4         @ endereco do buffer
    mov r2, r5         @ tamanho do buffer.
    mov r7, #4         @ write
    svc 0x0
    pop {r4, r5, lr}
    mov pc, lr

@ Finaliza a execucao de um processo.
@  r0: codigo de finalizacao (Zero para finalizacao correta)
exit:    
    mov r7, #1         @ syscall number for exit
    svc 0x0

@ Converte uma sequencia de caracteres '0' e '1' em um numero binario
@ parametros:
@  r0: endereco do buffer de memoria que armazena a sequencia de caracteres.
@  r1: numero de caracteres a ser considerado na conversao
@ retorno:
@  r0: numero binario
atoi:
    push {r4, r5, lr}
    mov r4, r0         @ r4 == endereco do buffer de caracteres
    mov r5, r1         @ r5 == numero de caracteres a ser considerado 
    mov r0, #0         @ number = 0
    mov r1, #0         @ loop indice
atoi_loop:
    cmp r1, r5         @ se indice == tamanho maximo
    beq atoi_end       @ finaliza conversao
    mov r0, r0, lsl #1 
    ldrb r2, [r4, r1]  
    cmp r2, #'0'       @ identifica bit
    orrne r0, r0, #1   
    add r1, r1, #1     @ indice++
    b atoi_loop
atoi_end:
    pop {r4, r5, lr}
    mov pc, lr

@ Converte um numero binario em uma sequencia de caracteres '0' e '1'
@ parametros:
@  r0: endereco do buffer de memoria que recebera a sequencia de caracteres.
@  r1: numero de caracteres a ser considerado na conversao
@  r2: numero binario
itoa:
    push {r4, r5, lr}
    mov r4, r0
itoa_loop:
    sub r1, r1, #1         @ decremento do indice
    cmp r1, #0          @ verifica se ainda ha bits a serem lidos
    blt itoa_end
    and r3, r2, #1
    cmp r3, #0
    moveq r3, #'0'      @ identifica o bit
    movne r3, #'1'
    mov r2, r2, lsr #1  @ prepara o proximo bit
    strb r3, [r4, r1]   @ escreve caractere na memoria
    b itoa_loop
itoa_end:
    pop {r4, r5, lr}
    mov pc, lr    
