#include "wavStego.h"

void hideData(FILE* coverFilePtr, FILE*  messageFilePtr, FILE* stegoFilePtr, int threshold){
	int nextByte, numRead, difference;
	float avg;
	BYTE messageByte, byteGroup[3];
	int endOfCoverFile = FALSE, endOfMessageFile = FALSE, bitWritten, index = 0;
	char nextBit;

	//verify cover file is a wave file
	verifyWaveFile(coverFilePtr);
	rewind(coverFilePtr);

	//Copy contents of cover file to stego file
	while((nextByte = fgetc(coverFilePtr)) != EOF) {fputc(nextByte, stegoFilePtr);}
	rewind(coverFilePtr);
	rewind(stegoFilePtr);

	//Navigate to data chunk in cover file
	verifyWaveFile(coverFilePtr);
	locateDataChunk(coverFilePtr);

	//LOOP THROIUGH DATA
	while (!endOfCoverFile){
		//Reset bitWritten flag
		bitWritten = FALSE;

		//Read a byte from the message file every 8 bits written
		if (index == 0 && !endOfMessageFile){
			if((numRead = fread(&messageByte, 1, 1, messageFilePtr)) == EOF) {messageByte = END_FLAG; endOfMessageFile = TRUE;}
		}

		//Read in 3 bytes
		if((numRead = fread(byteGroup, 1, 3, coverFilePtr)) == EOF) endOfCoverFile = TRUE;
		if(numRead != 3) break;

		//Compare inner value with outer values
		avg = (byteGroup[0] + byteGroup[2])/2.0;

		//If difference <= threshold
		difference = byteGroup[1] - (int)avg;
		if(abs(difference) <= threshold){
			nextBit = getNextBit(messageByte, index);
			//If message bit = 1
			if (nextBit == 1){
				//Change middle byte to 1 less than average if difference is negative
				if (difference < 0) byteGroup[1] = ceil(avg) - 1;
			}
			else{
			//If message bit = 0
				//Change middle byte to 1 greater than average if difference is positive
				if (difference >= 0) byteGroup[1] = floor(avg) + 1;
			}
			bitWritten = TRUE;
		}
		//Else, skip group

		//Update bit index if a bit was written
		if (bitWritten) index = (index + 1) % 8;
	}
    return;
}

void extractData(FILE* stegoFilePtr, FILE* messageFilePtr, int threshold){
	int nextByte, numRead, difference;
	float avg;
	BYTE byteGroup[3], messageByte;
	int endOfStegoFile = FALSE, endOfMessageFile = FALSE, endFlagRead = FALSE, bitRead, i, index = 0;
	int buffer[8];
	char nextBit;

	//verify stego file is a wave file
	verifyWaveFile(stegoFilePtr);

	//Navigate to data chunk in stego file
	locateDataChunk(stegoFilePtr);
	printf("%d, %d\n", endOfStegoFile, endFlagRead);
	//LOOP THROIUGH DATA
	while (!endOfStegoFile || !endFlagRead){
		//Reset bitRead flag
		bitRead = FALSE;

		//Read in 3 bytes
		if((numRead = fread(byteGroup, 1, 3, stegoFilePtr)) == EOF) endOfStegoFile = TRUE;
		if(numRead != 3) break;

		//Compare inner value with outer values
		avg = (byteGroup[0] + byteGroup[2])/2.0;

		//If difference <= threshold
		difference = byteGroup[1] - (int)avg;
		if(abs(difference) <= threshold){
			//Update bitRead Flag
			bitRead = TRUE;

			//If difference is positive, 0 written to buffer
			if (difference >= 0) buffer[index] = 0;
			//If difference is negative, 1 written to message file
			else buffer[index] = 1;
		}

		//Else, skip group

		//Read a byte from the bits in the buffer every 8 bits
		if (bitRead && index == 7){
			messageByte = readBuffer(buffer);
			printf("byte: %x\n", messageByte);

			//If last byte read is the end flag, stop extracting
			if (messageByte == END_FLAG) endFlagRead = TRUE;
		}

		//Flush buffer to message file at end of byte if the last byte wasn't the end flag
		if (bitRead && index == 7 && !endFlagRead){
			fwrite(&messageByte, 1, 1, messageFilePtr);
		}

		//Update bit index if a bit was read
		if (bitRead) index = (index + 1) % 8;
	}
    return;
}

void verifyWaveFile(FILE* waveFile){
	W_CHUNK chunk[MAX_CHUNKS];
    BYTE *pChunkData[MAX_CHUNKS];

	int x = readChunkHeader(waveFile, &chunk[0]);

	// check to make sure it is a RIFF file
	if(memcmp( &(chunk[0].chunkID), "RIFF", 4) != 0)
	{
		printf("\nERROR: file is NOT a RIFF file\n\n");
		exit(-1);
	}

    // check to make sure it is a wave file
	pChunkData[0] = readChunkData(waveFile, 4);
    if(memcmp( pChunkData[0], "WAVE", 4) != 0){
		printf("\nERROR: file is not a WAVE file\n\n");
		exit(-1);
	}
	
    return;
}

void locateDataChunk(FILE* waveFile){
	W_CHUNK chunk[MAX_CHUNKS];
    BYTE *pChunkData[MAX_CHUNKS];
	int i = 0;
	do{
		if(feof(waveFile)){
			printf("\nERROR: Data chunk not found\n\n");
			exit(-1);
		}
		i++;
		readChunkHeader(waveFile, &chunk[i]);
		if (memcmp( &(chunk[i].chunkID), "data", 4) == 0) return;
		pChunkData[i] = readChunkData(waveFile, chunk[i].chunkSize);
	}while(memcmp( &(chunk[i].chunkID), "data", 4) != 0);
	return;
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
		printf("\nERROR: could not allocate %d bytes of memory\n\n", size);
		exit(-1);
	}

	x = (int) fread(ptr, 1, size, fptr);
	if(x != size)
	{
		printf("\nERROR: Chunk data could not be read\n\n");
		exit(-1);
	}

	return(ptr);	
} // readChunkData

char getNextBit(BYTE currentByte, int index){
	char bit = 1 << (7 - index);
	return (bit & currentByte);
}

BYTE readBuffer(int* buffer){
	int i, total = 0;
	for (i = 0; i < 8; i++){
		if(buffer[i] == 1){
			total += (int) pow(2, (7-i));
		}
	}
	return total;
}