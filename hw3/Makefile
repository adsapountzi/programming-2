all: server agent clean

server: server.o myfunctions.o
	gcc -Wall -g server.o myfunctions.o -o server
	
server.o: server.c myfunctions.h
	gcc -Wall -g -c server.c

agent: agent.o myfunctions.o
	gcc -Wall -g agent.o myfunctions.o -o agent
	
agent.o: agent.c myfunctions.h
	gcc -Wall -g -c agent.c

myfunctions.o: myfunctions.c myfunctions.h
	gcc -Wall -g -c myfunctions.c
	
clean:
	rm -rf *.o *.dSYM