#ifndef WAVSTEGO_H
#define WAVSTEGO_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "wave.h"

void verifyWaveFile(FILE* waveFile);
int hideData(FILE* coverFile, FILE*  messageFile, FILE* stegoFile, int threshold);
c;
int readChunkHeader(FILE *fptr, W_CHUNK *pChunk);
BYTE *readChunkData(FILE *fptr, int size);
#endif