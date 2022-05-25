all: p2unarchive p2crypt dirlist  p2archive hw2

p2unarchive: p2unarchive.o myfunctions.o
	gcc -Wall -g p2unarchive.o myfunctions.o -o p2unarchive
	
p2unarchive.o: p2unarchive.c myfunctions.h
	gcc -Wall -g -c p2unarchive.c
	
p2crypt: p2crypt.o myfunctions.o
	gcc -Wall -g p2crypt.o myfunctions.o -o p2crypt
	
p2crypt.o: p2crypt.c myfunctions.h
	gcc -Wall -g -c p2crypt.c
	
dirlist: dirlist.o myfunctions.o
	gcc -Wall -g dirlist.o myfunctions.o -o dirlist
	
dirlist.o: dirlist.c  myfunctions.h
	gcc -Wall -g -c dirlist.c
	
p2archive: p2archive.o myfunctions.o
	gcc -Wall -g p2archive.o myfunctions.o -o p2archive
	
p2archive.o: p2archive.c myfunctions.h
	gcc -Wall -g -c p2archive.c
	
hw2: hw2.o myfunctions.o
	gcc -Wall -g hw2.o myfunctions.o -o hw2
	
hw2.o: hw2.c myfunctions.h
	gcc -Wall -g -c hw2.c
	
myfunctions.o: myfunctions.c myfunctions.h
	gcc -Wall -g -c myfunctions.c
