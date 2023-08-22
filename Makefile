#! /bin/bash
CC=gcc

sender : sender.o
	${CC}  -o sender sender.o -lrt

receiver : receiver.o 
	${CC}  -o receiver receiver.o -lrt

sender.o: sender.c 
	${CC} -c sender.c

receiver.o: receiver.c 
	${CC} -c receiver.c

clean:
	rm *.o
	rm sender
	rm receiver