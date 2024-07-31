/*
    CODE AUTHORS:
        Janelle Wiggins
        Josh Graham
*/
#include "wavStego.h"

#define HIDE 1
#define EXTRACT 2

int findInArgv(int argc, char* argv[], char* arg);

int main(int argc, char* argv[]) {

    //Demonstrate program usage if exe is run alone
    if (argc == 1) {
        printf("RUN OPTIONS:\nHIDE:\t\tStegoProject.exe -hide -c [cover file] -m [message file] (-o [stego file]) (-b [bitcount])\nEXTRACT:\tStegoProject.exe -extract -s [stego file] (-o [message file]) (-b [bitcount])\n");
        return 0;
    }

    int temp, action, result, flagCount = 0;
    char stegoFile[64] = {0};
    char coverFile[64] = {0};
    char messageFile[64] = {0};
    FILE* stegoFilePtr;
    FILE* coverFilePtr;
    FILE* messageFilePtr;
    // int threshold = 40; //default threshold
    int bitcount = 1; //default bits written or read

    //Check first argument for desired action
    if (strcmp(argv[1], "-hide") == 0) {
        action = HIDE;
    } else if (strcmp(argv[1], "-extract") == 0) {
        action = EXTRACT;
    } else {
        printf("\nERROR: Unknown action.\n\n");
        exit(-1);
    }

    //Parse input files based on specified action
    switch(action) {
        case HIDE:
            //Parse message file, error if not found
            result = findInArgv(argc, argv, "-m");
            if (result && argc > result + 1){flagCount++; strcpy(messageFile, argv[result + 1]);}
            else {printf("\nERROR: Usage: StegoProject.exe -hide -c [cover file] -m [message file] (-o [stego file]) (-b [bitcount])\n\n"); exit(-1);}
            //Parse cover file, error if not found
            result = findInArgv(argc, argv, "-c");
            if (result && argc > result + 1){flagCount++; strcpy(coverFile, argv[result + 1]);}
            else {printf("\nERROR: Usage: StegoProject.exe -hide -c [cover file] -m [message file] (-o [stego file]) (-b [bitcount])\n\n"); exit(-1);}
            //Parse stego file
            result = findInArgv(argc, argv, "-o");
            if (result && argc > result + 1){flagCount++; strcpy(stegoFile, argv[result + 1]);}
            else {
                //Generate default stego file name
                strcat(stegoFile, "stego_");
                strcat(stegoFile, coverFile);
            }
            //Parse bitcount, assigns it to bitcount if 4 or less
            result = findInArgv(argc, argv, "-b");
            if (result && argc > result + 1){flagCount++; temp = atoi(argv[result + 1]); bitcount = (temp < 5) ? temp : 1;}
            //Error if extra arguments are read
            if ((2*flagCount + 2) != argc){printf("\nERROR: Usage: StegoProject.exe -hide -c [cover file] -m [message file] (-o [stego file]) (-b [bitcount])\n\n"); exit(-1);}

            //Open all input files
            //Open cover file as binary to be read
            coverFilePtr = fopen(coverFile, "rb");
            if( coverFilePtr == NULL){
                printf("\nERROR: Could not open %s.\n\n", coverFile);
                exit(-1);
            }
            //Open message file as binary to be read
            messageFilePtr = fopen(messageFile, "rb");
            if( messageFilePtr == NULL){
                printf("\nERROR: Could not open %s.\n\n", messageFile);
                fclose(coverFilePtr);
                exit(-1);
            }
            //Open stego file as binary to be read and written
            stegoFilePtr = fopen(stegoFile, "wb+");
            if( stegoFilePtr == NULL){
                printf("\nERROR: Could not open %s.\n\n", stegoFile);
                fclose(coverFilePtr);
                fclose(messageFilePtr);
                exit(-1);
            }

            //Call function to hide the message file data in a copy of the cover file at the desired threshold
            hideData(coverFilePtr, messageFilePtr, stegoFilePtr, bitcount);
            break;

        case EXTRACT:
            //Parse stego file, error if not found
            result = findInArgv(argc, argv, "-s");
            if (result && argc > result + 1){flagCount++; strcpy(stegoFile, argv[result + 1]);}
            else {printf("\nERROR: Usage: StegoProject.exe -extract -s [stego file] (-o [message file]) (-b [bitcount])\n\n"); exit(-1);}
            //Parse message file
            result = findInArgv(argc, argv, "-o");
            if (result && argc > result + 1){flagCount++; strcpy(messageFile, argv[result + 1]);}
            else {
                //Generate default message file name
                strcat(messageFile, "message_");
                strcat(messageFile, stegoFile);
                strcat(messageFile, ".bin");
            }
            //Parse bitcount, assigns it to bitcount if 4 or less
            result = findInArgv(argc, argv, "-b");
            if (result && argc > result + 1){flagCount++; temp = atoi(argv[result + 1]); bitcount = (temp < 5) ? temp : 1;}
            //Error if extra arguments are read
            if ((2*flagCount + 2) != argc){printf("\nERROR: Usage: StegoProject.exe -extract -s [stego file] (-o [message file]) (-b [bitcount])\n\n"); exit(-1);}

            //Open all input files
            //Open message file as binary to be written
            messageFilePtr = fopen(messageFile, "wb");
            if( messageFilePtr == NULL){
                printf("\nERROR: Could not open %s.\n\n", messageFile);
                exit(-1);
            }
            //Open stego file as binary to be read
            stegoFilePtr = fopen(stegoFile, "rb");
            if( stegoFilePtr == NULL){
                printf("\nERROR: Could not open %s.\n\n", stegoFile);
                fclose(messageFilePtr);
                exit(-1);
            }

            //Call function to read hidden data from the stego file into the message file at the desired threshold
            extractData(stegoFilePtr, messageFilePtr, bitcount);
            break;
        //Error if the action wasn't hide or extract
        default:
            printf("\nERROR: Unknown action.\n\n");
            exit(-1);
    }

    //Close all opened files and print all used files
    if(action == HIDE){fclose(coverFilePtr); printf("Cover File:\t%s", coverFile);}
    fclose(messageFilePtr);
    fclose(stegoFilePtr);
    printf("\nmessage file:\t%s\nstego file:\t%s\n\n", messageFile, stegoFile);

    return 0;
}

//Iterate through command-line arguments to find desired token
int findInArgv(int argc, char* argv[], char* arg){
    int i;
    for(i = 1; i < argc; i++){
        if(strcmp(argv[i], arg) == 0) return i;
    }
    return 0;
}