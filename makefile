CFLAGS = -lpthread -lrt
CC =gcc $(CFLAGS)

all: switchCosts

switchcosts: switchcosts.o
	$(CC) switchCosts.o -o switchCosts

switchcosts.o: switchCosts.c
	$(CC) switchCosts.c 

clean:
	rm -f switchCosts switchCosts.o

run: switchCosts
	./switchCosts
