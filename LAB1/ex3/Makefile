CC:= gcc

CFLAGS:=-Wall -g

objs:=main.c libmymath.a
functions:=add.o sub.o

main: $(objs)
	$(CC) $(CFLAGS) -static main.c -o main_static -L . -lmymath

add.o:add.c
	$(CC) $(CFLAGS) -c add.c

sub.o:sub.c
	$(CC) $(CFLAGS) -c sub.c

libmymath.a: $(functions)	
	ar rc libmymath.a $(functions)

clean:
	rm -f *.o main_static libmymath.a
	
