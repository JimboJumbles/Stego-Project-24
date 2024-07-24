#ifndef WAVSTEGO_H
#define WAVSTEGO_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "wave.h"

int hideData(char* coverFile, char*  messageFile, char* stegoFile, int threshold);
int extractData(char* stegoFile, char* messageFile, int threshold);
int readChunkHeader(FILE *fptr, W_CHUNK *pChunk);
BYTE *readChunkData(FILE *fptr, int size);
#endif