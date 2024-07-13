StegoProject.exe : main.o
	gcc main.o -o StegoProject.exe
main.o : main.c
	gcc -c main.c
clean :
	rm *.o StegoProject.exe