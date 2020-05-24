all: main.c ip_lib.o 
	gcc main.c -o main ip_lib.o  -lm

main: main_iplib.c ip_lib.o bmp.o
	gcc main_iplib.c -o main_iplib ip_lib.o bmp.o -lm

bmp.o: bmp.h
	gcc bmp.c -o bmp.o -c -lm
	
ip_lib.o: ip_lib.h
	gcc ip_lib.c -o ip_lib.o -c -lm

clean: 
	rm main ip_lib.o