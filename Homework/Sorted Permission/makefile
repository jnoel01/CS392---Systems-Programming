####################################################################################
 # Name        : makefile.sh
 # Date        : June , 2021
 # Description : permission header.
 # Pledge      : I pledge my honor that I have abided by the Stevens Honor System.
####################################################################################

sspfind: spfind.o permission.o
		gcc spfind.o permission.o -o spfind
sspfind.o: spfind.c
		gcc -Wall -Werror -pedantic-errors -c spfind.c
permission.o: permission.c permission.h
		gcc -Wall -Werror -pedantic-errors -c permission.c
clean:
		rm -f *.o spfind spfind.exe

