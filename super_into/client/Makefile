lib=../lib
CFLAGS=../include

client: client.o $(lib)/eles.o 
	gcc $^ -o $@ -I$(CFLAGS)

client.o: client.c
	gcc -c $^ -o  $@ -I$(CFLAGS)

$(lib)/eles.o: $(lib)/eles.c
	gcc -c $^ -o  $@ -I$(CFLAGS)

clean:
	rm client $(lib)/eles.o client.o
