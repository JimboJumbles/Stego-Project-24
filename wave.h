#ifndef WAVE_H
#define WAVE_H
// wave.h
// 
// contains wave structures
//

#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <stdio.h>

// built-in windows types
//typedef unsigned char		BYTE;
//typedef unsigned short	WORD;
//typedef unsigned int		DWORD;

#define SUCCESS 0
#define FAILURE -1
#define MAX_CHUNKS 16

typedef struct
{
	DWORD	chunkID;
	DWORD	chunkSize;
} W_CHUNK;

typedef struct
{
	WORD	compCode;				
    WORD    numChannels;         
    DWORD   sampleRate;    
    DWORD   avgBytesPerSec;   //  avgBytesPerSec = sampleRate * blockAlign 
    WORD    blockAlign;       //  blockAlign = bitsPerSample / 8 * numChannels
    WORD    bitsPerSample;    
	// WORD extraFormat;		// do not worry about this
} W_FORMAT;

typedef struct
{

} W_DATA;

#endif