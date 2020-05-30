all: main_iplib.c ip_lib.o bmp.o
	gcc main_iplib.c -o main_iplib ip_lib.o bmp.o -Wall --ansi --pedantic -lm -g3 -O3 -fsanitize=address -fsanitize=undefined -std=gnu89 -Wextra  

bmp.o: bmp.h
	gcc bmp.c -o bmp.o -c -Wall

ip_lib.o: ip_lib.h
	gcc ip_lib.c -o ip_lib.o -c -Wall --ansi --pedantic -lm -g3 -O3 -fsanitize=address -fsanitize=undefined -std=gnu89 -Wextra  

clean: 
	rm ip_lib.o bmp.o main_iplib