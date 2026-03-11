all:
	gcc keyValue.c server.c -o keyValue

clean:
	rm -f keyValue store.txt