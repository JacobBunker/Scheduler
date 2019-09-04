all: main.c 
	gcc -g -Wall -o myprog main.c

clean: 
	$(RM) myprog
