all:
	gcc keyValue.c server.c -o keyValue -lpthread

clean:
	rm -f keyValue store.txt