.org 0x0
.section .iv,"a"

_start:		

interrupt_vector:

    b RESET_HANDLER
.org 0x18
    b IRQ_HANDLER

.org 0x100
.text

RESET_HANDLER:

    @ Zera o contador
    ldr r2, =CONTADOR  @lembre-se de declarar esse contador em uma secao de dados! 
    mov r0, #0
    str r0, [r2]

    @Faz o registrador que aponta para a tabela de interrupções apontar para a tabela interrupt_vector
    ldr r0, =interrupt_vector
    mcr p15, 0, r0, c12, c0, 0

    @ Ajustar a pilha do modo IRQ.
    @ Você deve iniciar a pilha do modo IRQ aqui. Veja abaixo como usar a instrução MSR para chavear de modo.
    @ ...

    @ Configura o GPT

    @ GPT_CR
    mov r1, #0x53
    mov r2, #0xFA
    orr r1, r2, r1, LSL #8
    mov r1, r1, LSL #16
    mov r3, #0x00000041
    str r3, [r1]		

    @ GPT_PR
    add r1, r1, #4
    mov r3, #0 
    str r3, [r1]

    @ GPT_OCR1
    add r1, r1, #12
    mov r3, #100 
    str r3, [r1]

    @ GPT_IR
    sub r1, r1, #4
    mov r3, #1	 
    str r3, [r1]

.data

CONTADOR:
	.skip 4	