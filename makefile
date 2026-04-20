all:
	gcc -std=c99 main.c keyValue.c -o server -lpthread

clean:
	rm -f server store.txt