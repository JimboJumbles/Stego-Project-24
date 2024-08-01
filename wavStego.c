/*
    CODE AUTHORS:
        Janelle Wiggins
        Josh Graham
*/
#include "wavStego.h"

void hideData(FILE* coverFilePtr, FILE*  messageFilePtr, FILE* stegoFilePtr, int bitcount){
	int i, totalBytesUsed = 0, fileCapacity = 0, messageSize = 0, messageBytesWritten = 0, numRead, numWritten, difference, index = 0, endByteCount = bitcount, avg;
	int endOfCoverFile = FALSE, endOfMessageFile = FALSE;
	int bitArray[8], messageBuffer[8 * bitcount], sizeArray[MESSAGE_SIZE_BIT_COUNT];
	BYTE messageBytes[bitcount], byteGroup[2] ,stegoByteGroup[2], nextByte;
	char nextBit;

	//verify cover file is a wave file
	verifyWaveFile(coverFilePtr);

	//Get total modifiable bytes
	while(fread(&i, 1, 1, coverFilePtr) != 0){
		fileCapacity++;
	}

	//Get total message size
	while(fread(&i, 1, 1, messageFilePtr) != 0){
		messageSize++;
	}

	rewind(coverFilePtr);
	rewind(messageFilePtr);

	//Copy contents of cover file to stego file
	while((i = fgetc(coverFilePtr)) != EOF) {fputc(i, stegoFilePtr);}
	rewind(coverFilePtr);
	rewind(stegoFilePtr);

	//Navigate to data chunk in cover file and stego file
	verifyWaveFile(coverFilePtr);
	locateDataChunk(coverFilePtr);
	verifyWaveFile(stegoFilePtr);
	locateDataChunk(stegoFilePtr);

	//Embed message file size at beginning of data chunk
	decimalToBinary(messageSize, sizeArray, 0, MESSAGE_SIZE_BIT_COUNT);
	while (index < MESSAGE_SIZE_BIT_COUNT && !endOfCoverFile){
		if (MESSAGE_SIZE_BIT_COUNT - index < bitcount) endByteCount = MESSAGE_SIZE_BIT_COUNT - index;
		if(fread(&nextByte, 1, 1, coverFilePtr) != 1) endOfCoverFile = TRUE;
		decimalToBinary(nextByte, bitArray, 0, 8);
		for(i = 0; i < endByteCount; i++){
			bitArray[8 - (endByteCount - i)] = sizeArray[index + i];
			// printf("%d", sizeArray[index + i]);
		}
		nextByte = readBuffer(bitArray, 8, 0);
		fseek(stegoFilePtr, 0, SEEK_CUR);
		numWritten = fwrite(&nextByte, 1, 1, stegoFilePtr);
		fseek(stegoFilePtr, 0, SEEK_CUR);
		totalBytesUsed++;
		index += bitcount;
	}
	index = 0;

	//LOOP THROIUGH DATA
	while (!endOfCoverFile && messageBytesWritten <= messageSize){

		if (index % (8 * endByteCount) == 0 && endOfMessageFile) break;

		//Read a number of bytes equal to bitcount from the message file every 8 samples changed
		else if (index == 0 && !endOfMessageFile){
			numRead = fread(messageBytes, 1, bitcount, messageFilePtr);
			if (numRead == 0){endOfMessageFile = TRUE; break;}
			else if (numRead < bitcount) endByteCount = numRead;
			if (feof(messageFilePtr)) endOfMessageFile = TRUE;
			messageBytesWritten += numRead;
			//Translate bytes read into individual bits
			for(i = 0; i < endByteCount; i++){
				decimalToBinary(messageBytes[i], messageBuffer, 8 * i, 8);
			}
		}

		//Read in 2 bytes from the cover file
		numRead = fread(byteGroup, 1, 2, coverFilePtr);
		if(feof(coverFilePtr)) endOfCoverFile = TRUE;
		if(numRead != 2) break;

		//Generate average of the 2 bytes
		avg = (byteGroup[0] + byteGroup[1])/2;

		//Convert average to binary
		decimalToBinary(avg, bitArray, 0, 8);
		//Difference = [bitcount] bits of message data - [bitcount] LSBs of avg
		difference = readBuffer(messageBuffer, bitcount, index) - readBuffer(bitArray, bitcount, 8 - bitcount);

		//If the average in the cover file bytes is greater, lower the values of those 2 bytes
		if (difference < 0){
			for (i = 0; i < abs(difference); i++){
				byteGroup[0]--;
				byteGroup[1]--;
			}
		}
		//If the average in the cover file bytes is lower, raise the values of those 2 bytes
		else if (difference > 0){
			for (i = 0; i < difference; i++){
				byteGroup[0]++;
				byteGroup[1]++;
			}
		}

		//If no difference, skip to next bytes.

		//Write the byte (changed or not) to the stego file
		fseek(stegoFilePtr, 0, SEEK_CUR);
		numWritten = fwrite(&byteGroup, 1, 2, stegoFilePtr);
		fseek(stegoFilePtr, 0, SEEK_CUR);
		totalBytesUsed += 2;
		//Update bit index
		index = (index + bitcount) % (8 * bitcount);
	}
	//Print end statistics
	if (!endOfMessageFile)printf("\nWARNING: Could not fit all %d bytes of message data!\n", messageSize);
	printf("\nHiding Capacity of Cover File:\t\t%d bytes\n", fileCapacity);
	printf("Total Message File Data Written:\t%d bytes\n", messageBytesWritten);
	printf("Number of bytes modified:\t\t%d bytes\n", totalBytesUsed);
	printf("%% of Bytes Modified:\t\t\t%.3lf%%\n", (totalBytesUsed * 100.0)/fileCapacity);
	printf("Bits written per modified sample:\t%d\n\n", bitcount);
	fclose(coverFilePtr);
	fclose(messageFilePtr);
    fclose(stegoFilePtr);
    return;
}

void extractData(FILE* stegoFilePtr, FILE* messageFilePtr, int bitcount){
	int i, j, nextByte, numRead, avg, messageSize = 0, index = 0, totalRead = 0, endBitCount = bitcount;
	BYTE byteGroup[2], messageByte;
	int endOfStegoFile = FALSE, endOfMessageFile = FALSE;
	int messageBuffer[8 * bitcount], bitArray[8], sizeArray[MESSAGE_SIZE_BIT_COUNT];
	char nextBit;

	//verify stego file is a wave file
	verifyWaveFile(stegoFilePtr);

	//Navigate to data chunk in stego file
	locateDataChunk(stegoFilePtr);

	//Read message file size from beginning of data chunk
	decimalToBinary(messageSize, sizeArray, 0, MESSAGE_SIZE_BIT_COUNT);
	while (index < MESSAGE_SIZE_BIT_COUNT && !endOfStegoFile){
		if (MESSAGE_SIZE_BIT_COUNT - index < bitcount) endBitCount = MESSAGE_SIZE_BIT_COUNT - index;
		if(fread(&nextByte, 1, 1, stegoFilePtr) != 1){endOfStegoFile = TRUE; break;}
		decimalToBinary(nextByte, bitArray, 0, 8);
		for(i = 0; i < endBitCount; i++){
			sizeArray[index + i] = bitArray[8 - (endBitCount - i)];
		}
		index += bitcount;
	}
	messageSize = readBuffer(sizeArray, MESSAGE_SIZE_BIT_COUNT, 0);
	index = 0;
	//LOOP THROIUGH DATA
	while (!endOfStegoFile && !endOfMessageFile && totalRead < messageSize){

		//Read in 2 bytes
		numRead = fread(byteGroup, 1, 2, stegoFilePtr);
		if(feof(stegoFilePtr)) endOfStegoFile = TRUE;
		if(numRead != 2) break;

		//Compare inner value with outer values
		avg = (byteGroup[0] + byteGroup[1])/2;


		//Convert average to binary
		decimalToBinary(avg, bitArray, 0, 8);

		// Last [bitcount] bits are written to the message buffer
		for (i = 0; i < bitcount; i++){
			messageBuffer[index + i] = bitArray[8 - bitcount + i];
		}

		//Update bit index
		index = (index + bitcount) % (8 * bitcount);

		//Convert message buffer into byte values once buffer is full
		if(index == 0){
			for(i = 0; i < bitcount; i++){
				messageByte = readBuffer(messageBuffer, 8, 8 * i);
				fwrite(&messageByte, 1, 1, messageFilePtr);
				totalRead++;
			}
		}
	}
	printf("\nTotal bytes extracted:\t\t%d bytes\n", totalRead);
	fclose(messageFilePtr);
    fclose(stegoFilePtr);
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
	//Iterate through each chunk in the wav file until the header of the data chunk is read
	do{
		if(feof(waveFile)){
			printf("\nERROR: Data chunk not found\n\n");
			exit(-1);
		}
		i++;
		readChunkHeader(waveFile, &chunk[i]);
		if (memcmp( &(chunk[i].chunkID), "data", 4) == 0) return;
		//If the header wasn't for the data chunk, read until the next header
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
	//Return isolated bit at desired position
	char bit = 1 << (7 - index);
	//Effectively returns boolean for a 1 at the position specified
	return (bit & currentByte);
}

//Iterates through byte buffer and converts to decimal integer
int readBuffer(int* buffer, int numToRead, int offset){
	int i, total = 0;
	for (i = 0; i < numToRead; i++){
		if(buffer[offset + i] == 1){
			total += (int) pow(2, (numToRead - 1 - i));
		}
	}
	return total;
}

//Converts a decimal integer into a binary number stored as an array of bits
void decimalToBinary(int decimal, int* bitArray, int offset, int size){
	int i;
	for (i = (size - 1) + offset; i >= offset; i--){
		bitArray[i] = decimal % 2;
		decimal = (int) floor(decimal / 2);
	}
	return;
}