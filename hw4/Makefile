all: hw4 test

hw4: hw4.o myfunctions.o
	gcc -Wall -g hw4.o myfunctions.o -o hw4
	
hw4.o: hw4.c myfunctions.h
	gcc -Wall -g -c hw4.c

test: test.o myfunctions.o
	gcc -Wall -g test.o myfunctions.o -o test
	
test.o: test.c myfunctions.h
	gcc -Wall -g -c test.c

myfunctions.o: myfunctions.c myfunctions.h
	gcc -Wall -g -c myfunctions.c
	
clean:
	rm -rf *.o *.dSYM