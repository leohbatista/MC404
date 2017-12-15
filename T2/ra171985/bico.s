@ MC404 - Trabalho 2 - Biblioteca de Controle (BiCo)
@ Leonardo Henrique Batista - 171985

 Define os simbolos globais, tornando as rotinas
.global set_motor_speed
.global set_motors_speed
.global read_sonar
.global read_sonars
.global register_proximity_callback
.global add_alarm
.global get_time
.global set_time

.align 4

.text

set_motor_speed:
    @ Salva o estado de R7 e LR
    stmfd sp!, {r7, lr}

    @ Le o ID do motor e a velocidade desejada
    mov r2, r0
    ldrb r0, [r2]    
    ldrb r1, [r2, #1]

    @ Salva o estado antes de chamar a syscall (r0 nao, pois sera retorno)
    stmfd sp!, {r1,r2}

    @ Chama a syscall set_motor_speed (#18)
    mov r7, #18
    svc 0x0

    @ Restaura o estado ao voltar da syscall
    add sp, sp, #8

    @ Restaura o estado do sistema antes de retornar
    ldmfd sp!, {r7, pc}


set_motors_speed:
    @ Salva o estado de R4, R7 e LR
    stmfd sp!, {r4, r7, lr}

    @ Le os IDs dos motores, verifica qual eh qual e coloca os parametro na ordem correta
    mov r2, r0
    mov r3, r1
    ldrb r4, [r2]
    cmp r4, #0
    ldrbeq r0, [r2,#1]
    ldrbeq r1, [r3,#1]
    ldrbne r0, [r3,#1]
    ldrbne r1, [r2,#1]

    @ Salva o estado antes de chamar a syscall (r0 nao, pois sera retorno)
    stmfd sp!, {r1-r3}        

    @ Chama a syscall set_motors_speed (#19)
    mov r7, #19
    svc 0x0

    @ Restaura o estado ao voltar da syscall
    add sp, sp, #12

    @ Restaura o estado do sistema antes de retornar
    ldmfd sp!, {r4, r7, pc}


read_sonar:
    @ Salva o estado de R7 e LR
    stmfd sp!, {r7, lr}

    @ Nao eh necessario salvar o estado de r0 pois sera usado para receber o retorno da syscall

    @ Chama a syscall read_sonar (#16)
    mov r7, #16
    svc 0x0

    @ Restaura o estado do sistema antes de retornar
    ldmfd sp!, {r7, pc}


read_sonars:
    @ Salva o estado de R4, R5, R7 e LR
    stmfd sp!, {r4, r5, r7, lr}
    
    @ Salva o sonar de inicio e de fim     
    mov r4, r0
    mov r5, r1

    @ Armazena em R1 o deslocamento necessario para salvar o valor de retorno da syscall no vetor
    mov r1, #0

    @ Define a syscall a ser chamada - read_sonar (#16)
    mov r7, #16             

    @ Le o valor de cada sonar no range definido e salva no vetor
    read_each_sonar:
        @ Compara se ja leu todos os sonares
        cmp r4, r5
        bgt exit_read_sonars
        
        @ Salva o estado antes de chamar a syscall (r0 nao, pois sera retorno)
        stmfd sp!, {r1,r2}

        @ Chama a syscall read_sonar (#16)
        mov r0, r4
        svc 0x0  

        @ Restaura o estado ao voltar da syscall
        ldmfd sp!, {r1,r2} 

        @ Salva o retono no vetor
        str r0, [r2,r1]

        @ Aumenta uma posicao de deslocamento no vetor de retorno
        add r1, r1, #4

        @ Vai para o proximo sonar
        add r4, r4, #1
        b read_each_sonar
    exit_read_sonars:
        @ Restaura o estado do sistema antes de retornar
        ldmfd sp!, {r4, r5, r7, pc}


register_proximity_callback:
    @ Salva o estado de R7 e LR
    stmfd sp!, {r7, lr}

    @ Salva o estado antes de chamar a syscall (r0 nao, pois sera retorno)
    stmfd sp!, {r1-r2}

    @ Chama a syscall register_proximity_callback (#17)
    mov r7, #17
    svc 0x0
    
    @ Restaura o estado ao voltar da syscall
    ldmfd sp!, {r1,r2} 

    @ Restaura o estado do sistema antes de retornar
    ldmfd sp!, {r7, pc}


add_alarm:
    @ Salva o estado de R7 e LR
    stmfd sp!, {r7, lr}

    @ Salva o estado antes de chamar a syscall (r0 nao, pois sera retorno)
    stmfd sp!, {r1}

    @ Chama a syscall set_alarm (#22)
    mov r7, #22
    svc 0x0

    @ Restaura o estado ao voltar da syscall
    ldmfd sp!, {r1} 

    @ Restaura o estado do sistema antes de retornar
    ldmfd sp!, {r7, pc}


get_time:
    @ Salva o estado de R7 e LR
    stmfd sp!, {r7, lr}

    @ Chama a syscall get_time (#20)
    mov r7, #20
    svc 0x0

    @ Restaura o estado do sistema antes de retornar
    ldmfd sp!, {r7, pc}


set_time:
    @ Salva o estado de R7 e LR
    stmfd sp!, {r7, lr}

    @ Chama a syscall set_time (#21)
    mov r7, #21
    svc 0x0

    @ Restaura o estado do sistema antes de retornar
    ldmfd sp!, {r7, pc}