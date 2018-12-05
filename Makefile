all: poker

poker: poker.c
	gcc -g poker.c -o poker
clean:
	rm -rf *.o
	rm -rf poker