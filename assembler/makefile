assembler: assembler.o wLServices.o mLServices.o pServices.o preAss.o parse.o imageServices.o labelServices.o parseServices.o fileServices.o
	gcc -ansi -Wall -pedantic -g assembler.o wLServices.o mLServices.o pServices.o preAss.o parse.o imageServices.o labelServices.o parseServices.o fileServices.o -o assembler
assembler.o: assembler.c
	gcc -c -ansi -Wall -pedantic -g assembler.c -o assembler.o
wLServices.o: wLServices.c
	gcc -c -ansi -Wall -pedantic -g  wLServices.c -o wLServices.o
mLServices.o: mLServices.c
	gcc -c -ansi -Wall -pedantic -g mLServices.c -o mLServices.o
pServices.o: pServices.c
	gcc -c -ansi -Wall -pedantic -g pServices.c -o pServices.o
preAss.o: preAss.c
	gcc -c -ansi -Wall -pedantic -g preAss.c -o preAss.o
imageServices.o: imageServices.c
	gcc -c -ansi -Wall -pedantic -g imageServices.c -o imageServices.o
labelServices.o: labelServices.c
	gcc -c -ansi -Wall -pedantic -g labelServices.c -o labelServices.o
parseServices.o: parseServices.c
	gcc -c -ansi -Wall -pedantic -g parseServices.c -o parseServices.o
parse.o: parse.c
	gcc -c -ansi -Wall -pedantic -g parse.c -o parse.o
fileServices.o: fileServices.c
	gcc -c -ansi -Wall -pedantic -g fileServices.c -o fileServices.o
