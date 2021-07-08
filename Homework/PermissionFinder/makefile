####################################################################################
 # Name        : makefile.sh
 # Date        : June 2, 2021
 # Description : permission header.
 # Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
####################################################################################

pfind: pfind.o permission.o
	gcc -g pfind.o permission.o -o pfind
pfind.o: pfind.c
	gcc -Wall -Werror -pedantic-errors -c pfind.c
permission.o: permission.c permission.h
	gcc -g -Wall -Werror -pedantic-errors -c permission.c
clean:
	rm -f *.o pfind pfind.exe

