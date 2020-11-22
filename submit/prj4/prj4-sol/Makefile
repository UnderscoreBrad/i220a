TARGET = y86-sim

COURSE = cs220

SRC =		main.c\
				ysim.c\
				README\
				Makefile	
				
CFLAGS = -g -Wall -std=c11

CPPFLAGS = -I $$HOME/$(COURSE)/include

LDFLAGS = -L $$HOME/$(COURSE)/lib -l cs220 -l y86

$(TARGET):	main.o ysim.o
						$(CC) ysim.o main.o $(LDFLAGS) -o y86-sim
						
clean:
						rm -f *~ *.o y86-sim
