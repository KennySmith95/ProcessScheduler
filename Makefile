OBJECTS =  thv1.o p1fxns.o

OBJECTS2 = thv2.o p1fxns.o

OBJECTS3 = thv3.o p1fxns.o

OBJECTS4 = thv4.o p1fxns.o

CFLAGS = -W -Wall

all : thv thv2 thv3 thv4

clean: 
	rm -f *.o *~ thv thv2 thv3 thv4

thv : $(OBJECTS)
	gcc $(CFLAGS) $(OBJECTS) -g -o thv

thv2: $(OBJECTS2)
	gcc $(CFLAGS) $(OBJECTS2) -g -o thv2

thv3: $(OBJECTS3)
	gcc $(CFLAGS) $(OBJECTS3) -g -o thv3

thv4: $(OBJECTS4)
	gcc  $(CFLAGS) $(OBJECTS4) -g -o thv4

thv1.o : thv1.c p1fxns.h
	gcc  $(CFLAGS) -c thv1.c

thv2.o : thv2.c p1fxns.h
	gcc $(CFLAGS) -c thv2.c
thv3.o : thv3.c p1fxns.h
	gcc $(CFLAGS) -c thv3.c
thv4.o : thv4.c p1fxns.h
	gcc $(CFLAGS) -c thv4.c
p1fxns.o : p1fxns.c
	gcc $(CFLAGS) -c p1fxns.c

