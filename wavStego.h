#ifndef WAVSTEGO_H
#define WAVSTEGO_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "wave.h"
#define END_FLAG 0xFF

void hideData(FILE* coverFilePtr, FILE*  messageFilePtr, FILE* stegoFilePtr, int threshold);
void extractData(FILE* stegoFilePtr, FILE* messageFilePtr, int threshold);
void verifyWaveFile(FILE* waveFile);
void locateDataChunk(FILE* waveFile);
int readChunkHeader(FILE *fptr, W_CHUNK *pChunk);
BYTE *readChunkData(FILE *fptr, int size);
char getNextBit(BYTE currentByte, int index);
BYTE readBuffer(int* buffer);

#endif