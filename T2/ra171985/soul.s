@ MC404 - Trabalho 2 - Sistema Operacional UoLi (SOUL)
@ - Giovani de Oliveira Coutinho  173156
@ - Leonardo Henrique Batista     171985

@ **************************
@ CONSTANTES DO PROGRAMA
@ **************************

@ Constantes das Pilhas
.set TAMANHO_PILHAS, 1024

@ Constantes de Alarms e Callbacks
.set MAX_CALLBACKS, 8
.set MAX_ALARMS,    8

@ Constantes de Modo de Execucao
.set SUPERVISOR_MODE_I,        0x13
.set SUPERVISOR_MODE_NO_I,     0xD3
.set IRQ_MODE_I,               0x12
.set IRQ_MODE_NO_I,            0xD2
.set USER_MODE_I,              0x10
.set USER_MODE_NO_I,           0xD0
.set SYS_MODE_NO_I,            0xDF

@ Constantes GPT
.set GPT_CR,    0x53FA0000
.set GPT_PR,    0x53FA0004
.set GPT_SR,    0x53FA0008
.set GPT_IR,    0x53FA000C
.set GPT_OCR1,  0x53FA0010
.set TIME_SZ,   0x0000004F

@ Constantes GPIO
.set GPIO_BASE,  0x53F84000
.set GPIO_DR,    0x53F84000
.set GPIO_GDIR,  0x53F84004
.set GPIO_PSR,   0x53F84008
.set GPIO_GDIR_CONFIG,  0b11111111111111000000000000111110

@ Constantes TZIC
.set TZIC_BASE,             0x0FFFC000
.set TZIC_INTCTRL,          0x0
.set TZIC_INTSEC1,          0x84
.set TZIC_ENSET1,           0x104
.set TZIC_PRIOMASK,         0xC
.set TZIC_PRIORITY9,        0x424

@ Constantes Auxiliares
.set USER_CODE_START,            0x77812000
.set MOTOR0_BITWISE_CLEAR,       0b00000001111111000000000000000000
.set MOTOR1_BITWISE_CLEAR,       0b11111110000000000000000000000000
.set SONAR_MUX_BITWISE_CLEAR,    0b00000000000000000000000000111100
.set SONAR_DATA_BITWISE_CLEAR,   0b00000000000000111111111111000000


@ **************************
@ INICIO DO PROGRAMA
@ **************************
.org 0x0
.section .iv,"a"

_start:

interrupt_vector:
    b RESET_HANDLER
.org 0x08
    b SVC_HANDLER
.org 0x18
    b IRQ_HANDLER

.org 0x100
.text

@ **************************
@ TRATADORES DE INTERUPCOES
@ **************************

@ Tratamento do Reset
RESET_HANDLER:
    @ Zera o SYS_TIME
    ldr r2, =SYS_TIME  @lembre-se de declarar esse SYS_TIME em uma secao de dados!
    mov r0, #0
    str r0, [r2]

    @Faz o registrador que aponta para a tabela de interrupções apontar para a tabela interrupt_vector
    ldr r0, =interrupt_vector
    mcr p15, 0, r0, c12, c0, 0

    @ Zera os contadores de alarms e callbacks
    mov r0, #0

    ldr r2, =Q_ALARMS
    str r0, [r2]
    ldr r2, =Q_CALLBACKS
    str r0, [r2]

    @ Inicializacao das pilhas
    ldr r0, =SUPERVISOR_MODE_NO_I
    msr CPSR_c, r0           @ SUPERVISOR mode, IRQ/FIQ disabled
    ldr sp, =PILHA_SUPERVISOR_START

    ldr r0, =SYS_MODE
    msr CPSR_c, r0           @ SYS mode, IRQ/FIQ disabled
    ldr sp, =PILHA_USER_START

    ldr r0, =IRQ_MODE_NO_I
    msr CPSR_c, r0           @ IRQ mode, IRQ/FIQ disabled
    ldr sp, =PILHA_IRQ_START

    @ Configuracao do GPT
    SET_GPT:
        @ Habilitando clock_src
        ldr  r2, =GPT_CR
        mov r0, #0x00000041
        str r0, [r2]

        @ Zerando prescaler
        ldr r2, =GPT_PR
        mov r0, #0x0
        str r0, [r2]

        @ Setando o valor de da constante que deseja contar
        ldr r2, =GPT_OCR1
        ldr r0, =TIME_SZ
        str r0, [r2]

        @ Habilitando interrupção do ocr1
        ldr r2, =GPT_IR
        mov r0, #1
        str r0, [r2]

    @ Configuracao do GPIO
    SET_GPIO:
        ldr r0, =GPIO_GDIR_CONFIG
        ldr r1, =GPIO_GDIR
        str r0, [r1]

    @ Configuracao do TZIC
    SET_TZIC:
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

        @ Muda o modo de execucao para usuario
        ldr r0, =USER_MODE_I
        msr CPSR_c, r0
        ldr pc, =USER_CODE_START


@ Tratamento de interrupcoes SVC
SVC_HANDLER:
    stmfd sp!, {lr}

    mrs r1, SPSR
    stmfd sp!, {r1}

    cmp r7,#16
    beq read_sonar

    cmp r7,#17
    beq register_proximity_callback

    cmp r7,#18
    beq set_motor_speed

    cmp r7,#19
    beq set_motors_speed

    cmp r7,#20
    beq get_time

    cmp r7,#21
    beq set_time

    cmp r7,#22
    beq set_alarm

    cmp r7, #25
    bne RETORNO_SVC
    add sp, sp, #4
    ldmfd sp!, {lr}
    mov pc, lr

    RETORNO_SVC:
        @ Restaura o modo de execucao
        ldmfd sp!, {r1}
        msr SPSR, r1

        @ Restaura o estado do sistema
        ldmfd sp!, {lr}
        movs pc, lr


@ Tratamento de interrupcoes IRQ
IRQ_HANDLER:
    stmfd sp!, {r0-r7, lr}
    mrs r0, SPSR
    stmfd sp!, {r0}
    ldr r1, =IRQ_MODE_NO_I
    msr CPSR_c, r1

    @ Seta GPT_SR como 0x1
  	ldr r2, =GPT_SR
  	mov r0, #1
  	str r0, [r2]

    @ Incrementa um no valor do SYS_TIME
  	ldr r0, =SYS_TIME
  	ldr r1, [r0]
  	add r1, r1, #1
  	str r1, [r0]

    ldr r0, =USER_MODE_I
    msr CPSR_c, r0

    @ Verifica callbacks
    ldr r2, =Q_CALLBACKS
    ldr r2, [r2]
    ldr r3, =V_CALLBACKS
    mov r5, #0  @ Contador de callbacks verificadas

    VERIFICA_CALLBACKS:
        cmp r5, r2
        bge FIM_VERIFICA_CALLBACKS
        mov r4, #12
        mul r6, r5, r4
        mov r4, r3
        add r4, r4, r6

        stmfd sp!, {r1-r3}
        ldr r0, [r4]
        mov r3,#16
        mov r7, r3
        svc 0x0
        ldmfd sp!, {r1-r3}
        add r4, r4, #4
        ldr r6, [r4]
        cmp r0, r6
        ble TRATA_CALLBACKS

        add r5, r5, #1

        b VERIFICA_CALLBACKS

    TRATA_CALLBACKS:
        mov r4, r3
        mov r3, #12
        mul r6, r5, r3
        add r4, r4, r6
        add r4, r4, #8
        ldr r0, [r4]

        @ Executa o callback
        stmfd sp!, {r1-r3,lr}
        blx r0
        ldmfd sp!, {r1-r3,lr}

        add r7, r4, #4
        sub r8, r4, #8

        sub r2, r2, #1

        @ Ajusta o vetor de alarms
        stmfd sp!, {r5}
        AJUSTA_CALLBACKS:
            cmp r5, r2
            bge FIM_AJUSTA_CALLBACKS
            ldr r4, [r7]
            str r4, [r8]
            add r8, r8, #4
            add r9, r9, #4
            ldr r4, [r7]
            str r4, [r8]
            add r8, r8, #4
            add r9, r9, #4
            ldr r4, [r7]
            str r4, [r8]
            add r8, r8, #4
            add r9, r9, #4
            add r5, r5, #1
            b AJUSTA_ALARMS

        FIM_AJUSTA_CALLBACKS:
            ldmfd sp!, {r5}
            b TRATA_CALLBACKS

    FIM_VERIFICA_CALLBACKS:

    @ Verifica alarms
    ldr r2, =Q_ALARMS
    ldr r2, [r2]
    ldr r3, =V_ALARMS
    mov r5, #0  @ Contador de callbacks verificadas

    VERIFICA_ALARMS:
        cmp r5, r2
        bge FIM_VERIFICA_ALARMS
        mov r6, r5, LSL #3
        mov r4, r3
        add r4, r4, r6
        ldr r4, [r4]

        cmp r4, r1
        bge TRATA_ALARMS

        add r5, r5, #1

        b VERIFICA_ALARMS

    TRATA_ALARMS:
        mov r4, r3
        add r4, r4, r6
        add r4, r4, #4
        ldr r0, [r4]

        @ Executa o callback do alarm
        stmfd sp!, {r1-r3,lr}
        blx r0
        ldmfd sp!, {r1-r3,lr}

        add r8, r4, #4
        sub r9, r4, #4

        sub r2, r2, #1

        @ Ajusta o vetor de alarms
        stmfd sp!, {r5}
        AJUSTA_ALARMS:
            cmp r5, r2
            bge FIM_AJUSTA_ALARMS
            ldr r4, [r8]
            str r4, [r9]
            add r8, r8, #4
            add r9, r9, #4
            ldr r4, [r8]
            str r4, [r9]
            add r8, r8, #4
            add r9, r9, #4
            add r5, r5, #1
            b AJUSTA_ALARMS

        FIM_AJUSTA_ALARMS:
            ldmfd sp!, {r5}
            b TRATA_ALARMS

    FIM_VERIFICA_ALARMS:

    @ Chama a syscall para voltar ao modo supervisor
    mov r7, #25
    svc 0x0

    ldr r0, =IRQ_MODE_NO_I
    msr CPSR_c, r0

    @ Restaura o modo de execucao
    ldmfd sp!, {r0}
    msr SPSR, r0

    @ Restaura o estado do sistema
    ldmfd sp!, {r0-r7, lr}

    @ Subtrai 4 do PC e retorna para o valor antes da interrupcao
  	sub lr, lr, #4
  	movs pc, lr


@ **************************
@ SYSCALLS
@ **************************

read_sonar:
    stmfd sp!, {r5, lr}

    @ Tratando identificador do sonar
    cmp r0, #0
    blt erro1
    cmp r0, #15
    bgt erro1

    @ Seta o sonar a ser lido em SONAR_MUX[3:0]
    ldr r1, =GPIO_PSR
    ldr r5, [r1]
    ldr r2, =SONAR_MUX_BITWISE_CLEAR
    bic r5, r5, r2
    orr r5, r5, r0, LSL #2
    bic r5, r5, #2
    ldr r1, =GPIO_DR
    str r5, [r1]

    @ TRIGGER = 0 + delay aprox. 15ms
    mov r5, #200
    bl delay

    @ TRIGGER = 1 + delay aprox. 15ms
    orr r5, r5, #2
    str r5, [r1]
    mov r5, #200
    bl delay

    @ TRIGGER = 0
    bic r5, r5, #2
    str r5, [r1]

    ldr r1, =GPIO_PSR
    read_flag:
        ldr r5, [r1]
        and r5, #1
        cmp r5, #1
        beq read_sonar_data
        mov r5, #140
        bl delay
        b read_flag

    read_sonar_data:
        ldr r0, [r1]
        ldr r2, =SONAR_DATA_BITWISE_CLEAR
        bic r0, r0, r2
        mov r0, r0, LSR #6

        ldmfd sp!, {r5, lr}
        b RETORNO_SVC


register_proximity_callback:
    stmfd sp!, {r4,r5,r6, lr}

    @tratando identificador do sonar
    cmp r0, #0
    blt erro2
    cmp r0, #15
    bgt erro2

    @ tratando quantidade maxima de callbacks
    ldr r4, =Q_CALLBACKS
    ldr r3, =MAX_CALLBACKS
    cmp r4, r3
    bge erro1

    @ Salva SENSOR + THRESHOLD + FUNC no vetor de callbacks
    ldr r3, =V_CALLBACKS
    mov r5, #12
    mov r6, r4
    mul r4, r6, r5
    add r3, r3, r4
    str r0, [r3]

    add r3, r3, #4
    str r1, [r3]

    add r3, r3, #4
    str r2, [r3]

    @ Adicionando um callback na quantidade
    ldr r4, =Q_CALLBACKS
    add r5, r4, #1
    str r5, [r4]

    @ Retorna 0 quando registra o callback
    mov r0, #0

    ldmfd sp!, {r4,r5,r6, lr}
    b RETORNO_SVC


set_motor_speed:
    stmfd sp!, {r5, lr}

    @tratando velocidade
    cmp r1, #0
    blt erro2
    cmp r1, #0b111111
    bgt erro2
    orr r1, r1, r1, LSL #1

    @tratando identificador do motor
    cmp r0, #0
    blt erro1
    cmp r0, #1
    bgt erro1
    beq set_motor1

    set_motor0:
        ldr r2, =GPIO_PSR
        ldr r5, [r2]
        ldr r3, =MOTOR0_BITWISE_CLEAR
        bic r5, r5, r3
        orr r5, r5, r1, LSL #18
        ldr r2, =GPIO_DR
        str r5, [r2]
        ldmfd sp!, {r5, lr}
        b RETORNO_SVC

    set_motor1:
        ldr r2, =GPIO_PSR
        ldr r5, [r2]
        ldr r3, =MOTOR1_BITWISE_CLEAR
        bic r5, r5, r3
        orr r5, r5, r1, LSL #25
        ldr r2, =GPIO_DR
        str r5, [r2]
        ldmfd sp!, {r5, lr}
        b RETORNO_SVC


set_motors_speed:
    stmfd sp!, {r5, lr}

    @ Tratamento da velocidade do motor 0
    cmp r0, #0
    blt erro1
    cmp r0, #0b111111
    bgt erro1
    orr r0, r0, r0, LSL #1

    @ Tratamento da velocidade do motor 1
    cmp r1, #0
    blt erro2
    cmp r1, #0b111111
    bgt erro2
    orr r1, r1, r1, LSL #1

    @ Define o novo valor para os pinos GPIO
    ldr r2, =GPIO_PSR
    ldr r5, [r2]
    ldr r3, =MOTOR0_BITWISE_CLEAR
    bic r5, r5, r3
    ldr r3, =MOTOR1_BITWISE_CLEAR
    bic r5, r5, r3
    orr r5, r5, r0, LSL #18
    orr r5, r5, r1, LSL #25
    ldr r2, =GPIO_DR
    str r5, [r2]

    ldmfd sp!, {r5, lr}
    b RETORNO_SVC


get_time:
    @ Retorna o tempo do sistema armazenado
    ldr r1, =SYS_TIME
    ldr r0, [r1]
    b RETORNO_SVC


set_time:
    @ Sobrescreve o tempo do sistema pelo valor em r0
    ldr r1, =SYS_TIME
    str r0, [r1]
    b RETORNO_SVC


set_alarm:
    stmfd sp!, {r4,lr}

    @ Tratando tempo do alarm se e menor que o do sistema
    ldr r3, =SYS_TIME
    ldr r2, [r3]
    cmp r1, r2
    bgt erro2

    @ Tratando quantidade maxima de alarms
    ldr r3, =Q_ALARMS
    ldr r2, [r3]
    ldr r4, =MAX_ALARMS
    cmp r2, r4
    bge erro1

    @ Salva TIME + FUNC no vetor de alarms
    ldr r3, =V_ALARMS
    add r3, r3, r2, lsl #3
    str r1, [r3]

    add r3, r3, #4
    str r0, [r3]

    @ Adicionando um alarm na quantidade
    ldr r3, =Q_ALARMS
    add r2, r2, #1
    str r2, [r3]

    ldmfd sp!, {r4,lr}
    b RETORNO_SVC


@ **************************
@ ROTINAS AUXILIARES
@ **************************

@ Retorno de erro -1
erro1:
    mov r0, #-1
    b RETORNO_SVC

@ Retorno de erro -2
erro2:
    mov r0, #-2
    b RETORNO_SVC

@ Rotina de delay - Recebe em R5 o num de instrucoes a realizar
delay:
    cmp r5, #0
    moveq pc, lr
    sub r5, r5, #1
    b delay


@ **************************
@ SEÇÃO DE DADOS
@ **************************
.data

@ Dados dos Alarms
Q_ALARMS:
    .word 0x0
V_ALARMS:
    .fill 64

@ Dados dos Callbacks
Q_CALLBACKS:
    .word 0x0
V_CALLBACKS:
    .fill 96

@ Contador de tempo do sistema
SYS_TIME:
    .skip 4

@ Pilhas do sistema
PILHA_USER_END:
    .skip 1024
PILHA_USER_START:

PILHA_IRQ_END:
    .skip 1024
PILHA_IRQ_START:

PILHA_SUPERVISOR_END:
    .skip 1024
PILHA_SUPERVISOR_START:
