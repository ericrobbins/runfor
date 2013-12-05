all: runfor

OUTFILES=runfor

CFLAGS=-Wall -Wextra -Wno-unused-parameter
LDFLAGS=
LIBS=

runfor: runfor.o 
	gcc $(CFLAGS) -o runfor runfor.o $(LDFLAGS) $(LIBS)

clean:
	rm -f *.o $(OUTFILES)

.c.o:
	gcc -c $(CFLAGS) $<
