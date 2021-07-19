lib=../lib
CFLAGS=../include

service: service.o $(lib)/store_manege.o $(lib)/super_into.o
	gcc $^ -o $@ -I$(CFLAGS) -lsqlite3 -pthread

service.o: service.c
	gcc -c $^ -o  $@ -I$(CFLAGS)

$(lib)/store_manege.o: $(lib)/store_manege.c
	gcc -c $^ -o  $@ -I$(CFLAGS)

$(lib)/super_into.o: $(lib)/super_into.c
	gcc -c $^ -o  $@ -I$(CFLAGS)

clean:
	rm service $(lib)/store_manege.o service.o $(lib)/super_into.o
