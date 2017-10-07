disk.img: ra171985
	mksd.sh --so /home/specg12-1/mc404/simulador/dummyos.elf --user ra171985
ra171985: ra171985.o
	arm-eabi-ld ra171985.o -o ra171985 -Ttext=0x77802000 -Tdata=0x77803000
ra171985.o: ra171985.s
	arm-eabi-as ra171985.s -o ra171985.o
clean:
	rm -f ra171985 ra171985.o disk.img
