#include "wavStego.h"

void verifyWaveFile(FILE* waveFile){
	W_CHUNK chunk[MAX_CHUNKS];
    BYTE *pChunkData[MAX_CHUNKS];

	int x = readChunkHeader(waveFile, &chunk[0]);

	// check to make sure it is a RIFF file
	if(memcmp( &(chunk[0].chunkID), "RIFF", 4) != 0)
	{
		printf("\n\nError, file is NOT a RIFF file!\n\n");
		exit(-1);
	}

    // check to make sure it is a wave file
	pChunkData[0] = readChunkData(waveFile, 4);
    if(memcmp( pChunkData[0], "WAVE", 4) != 0){
		printf("\n\nError, file is not a WAVE file!\n\n");
		exit(-1);
	}
	
    return;
}

int hideData(FILE* coverFile, FILE*  messageFile, FILE* stegoFile, int threshold){
	//verify cover file is a wave file
	verifyWaveFile(coverFile);

    return 1;
}

int extractData(FILE* stegoFile, FILE* messageFile, int threshold){
    return 1;
}

int readChunkHeader(FILE *fptr, W_CHUNK *pChunk)
{
	int x;

	// size = 1, count = 8 bytes
	x = (int) fread(pChunk, 1, 8, fptr);
	if(x != 8) return(FAILURE);

	return(SUCCESS);
} // readChunkHeader

// reads in the data portion of a chunk
BYTE *readChunkData(FILE *fptr, int size)
{
	BYTE *ptr;
	int tmp, x;

	tmp = size%2;	// size MUST be WORD aligned
	if(tmp != 0) size = size + 1;

	ptr = (BYTE *) malloc(size);
	if(ptr == NULL)
	{
		printf("\n\nError, could not allocate %d bytes of memory!\n\n", size);
		exit(-1);
	}

	x = (int) fread(ptr, 1, size, fptr);
	if(x != size)
	{
		printf("\n\nError reading chunkd data!\n\n");
		exit(-1);
	}

	return(ptr);	
} // readChunkData