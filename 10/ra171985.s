@ MC404 - Turma B - Lab 10
@ Leonardo Henrique Batista - 171985

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

    @ Faz o registrador que aponta para a tabela de interrupções apontar para a tabela interrupt_vector
    ldr r0, =interrupt_vector
    mcr p15, 0, r0, c12, c0, 0

    @ Ajustar a pilha do modo IRQ.
    @ Você deve iniciar a pilha do modo IRQ aqui. Veja abaixo como usar a instrução MSR para chavear de modo.
    @ ...

    @ Configura o GPT

    @ GPT_CR
    .set GPT_CR, 0x53FA0000
    ldr r1, =GPT_CR 
    mov r3, #0x41
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

SET_TZIC:
    @ Constantes para os enderecos do TZIC
    .set TZIC_BASE,             0x0FFFC000
    .set TZIC_INTCTRL,          0x0
    .set TZIC_INTSEC1,          0x84 
    .set TZIC_ENSET1,           0x104
    .set TZIC_PRIOMASK,         0xC
    .set TZIC_PRIORITY9,        0x424

    @ Liga o controlador de interrupcoes
    @ R1 <= TZIC_BASE

    ldr	r1, =TZIC_BASE

    @ Configura interrupcao 39 do GPT como nao segura
    mov	r0, #(1 << 7)
    str	r0, [r1, #TZIC_INTSEC1]

    @ Habilita interrupcao 39 (GPT)
    @ reg1 bit 7 (gpt)

    mov	r0, #(1 << 7)
    str	r0, [r1, #TZIC_ENSET1]

    @ Configure interrupt39 priority as 1
    @ reg9, byte 3

    ldr r0, [r1, #TZIC_PRIORITY9]
    bic r0, r0, #0xFF000000
    mov r2, #1
    orr r0, r0, r2, lsl #24
    str r0, [r1, #TZIC_PRIORITY9]

    @ Configure PRIOMASK as 0
    eor r0, r0, r0
    str r0, [r1, #TZIC_PRIOMASK]

    @ Habilita o controlador de interrupcoes
    mov	r0, #1
    str	r0, [r1, #TZIC_INTCTRL]

    @instrucao msr - habilita interrupcoes
    msr  CPSR_c, #0x13       @ SUPERVISOR mode, IRQ/FIQ enabled

@ Loop infinito até que uma interrupcao ocorra
laco:
	b laco

IRQ_HANDLER:
	@ Configura o GPT_SR
	.set GPT_SR, 0x53FA0008
	ldr r0, =GPT_SR
	mov r1, #1
	str r1, [r0]

	@ Incrementa o contador
	ldr r2, =CONTADOR
	ldr r1, [r2]
	add r1, r1, #1
	str r1, [r2]

	@ Corrige lr e retorna
	mov r3, lr
	sub lr, r3, #4
	movs  pc,lr


.data
CONTADOR:
	.skip 4	
