/*
    CODE AUTHORS:
        Janelle Wiggins
        Josh Graham
*/
#ifndef WAVSTEGO_H
#define WAVSTEGO_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "wave.h"
#define END_FLAG 0xff

void hideData(FILE* coverFilePtr, FILE*  messageFilePtr, FILE* stegoFilePtr, int bitcount);
void extractData(FILE* stegoFilePtr, FILE* messageFilePtr, int bitcount);
void verifyWaveFile(FILE* waveFile);
void locateDataChunk(FILE* waveFile);
int readChunkHeader(FILE *fptr, W_CHUNK *pChunk);
BYTE *readChunkData(FILE *fptr, int size);
char getNextBit(BYTE currentByte, int index);
BYTE readBuffer(int* buffer, int count);

#endif