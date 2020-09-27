hamming-encode:		main.o hamming.o
					#link main.o and hello.o to executable hello
					gcc main.o hamming.o -o hamming 

hamming.o:			hello.c hello.h
					#compile hello.c to object file hello.o
					gcc -std=c11 -g -Wall -c hamming.c

main.o:				main.c hello.h
					#compile main.c to object file main.o
					gcc -std=c11 -g -Wall -c main.c

hamming-decode: hamming-encode
                    ln -s -f $< $@

clean:
					rm -f *~ *.o hello