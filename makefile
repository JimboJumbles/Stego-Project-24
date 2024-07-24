StegoProject.exe : main.o wavStego.o
	gcc main.o wavStego.o -o StegoProject.exe
main.o : main.c wavStego.h
	gcc -c main.c
wavStego.o : wavStego.c wavStego.h wave.h
	gcc -c wavStego.c
clean :
	rm *.o StegoProject.exe