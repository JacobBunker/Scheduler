all: main.c 
	gcc -g -Wall -o myprog main.c -lm

clean: 
	$(RM) myprog
