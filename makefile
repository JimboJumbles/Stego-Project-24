StegoProject.exe : main.o wavStego.o WaveReader.o
	gcc main.o wavStego.o -o StegoProject.exe
main.o : main.c wavStego.h
	gcc -c main.c
wavStego.o : wavStego.c wavStego.h
	gcc -c wavStego.c
WaveReader.o : WaveReader.cpp wave.h
	gcc -c WaveReader.cpp
clean :
	rm *.o StegoProject.exe