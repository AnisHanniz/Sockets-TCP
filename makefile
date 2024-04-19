CC = gcc
CFLAGS = -c

all: serveur client

serveur: serveur.c
	$(CC) $(CFLAGS) serveur.c
	$(CC) -o serveur serveur.o

client: client.c
	$(CC) $(CFLAGS) client.c
	$(CC) -o client client.o

clean:
	rm -f serveur client
	rm *.o
