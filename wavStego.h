#ifndef WAVSTEGO_H
#define WAVSTEGO_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "wave.h"

int hideData(FILE* coverFilePtr, FILE*  messageFilePtr, FILE* stegoFilePtr, int threshold);
int extractData(FILE* stegoFilePtr, FILE* messageFilePtr, int threshold);
void verifyWaveFile(FILE* waveFile);
void locateDataChunk(FILE* waveFile);
int readChunkHeader(FILE *fptr, W_CHUNK *pChunk);
BYTE *readChunkData(FILE *fptr, int size);

#endif