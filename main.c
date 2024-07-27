#include "wavStego.h"

#define HIDE 1
#define EXTRACT 2

int findInArgv(int argc, char* argv[], char* arg);

int main(int argc, char* argv[]) {

    if (argc < 4) {
        printf("\nERROR: Incorrect number of parameters.\n");
        exit(-1);
    }

    int action, result, flagCount = 0;
    char stegoFile[64] = {0};
    char coverFile[64] = {0};
    char messageFile[64] = {0};
    FILE* stegoFilePtr;
    FILE* coverFilePtr;
    FILE* messageFilePtr;
    int threshold = 40; // default threshold

    if (strcmp(argv[1], "-hide") == 0) {
        action = HIDE;
    } else if (strcmp(argv[1], "-extract") == 0) {
        action = EXTRACT;
    } else {
        printf("\nERROR: Unknown action.\n");
        exit(-1);
    }

    switch(action) {
        case HIDE:
            result = findInArgv(argc, argv, "-m");
            if (result && argc > result + 1){flagCount++; strcpy(messageFile, argv[result + 1]);}
            else {printf("\nERROR: Usage: StegoProject.exe -hide -c [cover file] -m [message file] (-o [stego file]) (-t [threshold])\n\n"); exit(-1);}
            result = findInArgv(argc, argv, "-c");
            if (result && argc > result + 1){flagCount++; strcpy(coverFile, argv[result + 1]);}
            else {printf("\nERROR: Usage: StegoProject.exe -hide -c [cover file] -m [message file] (-o [stego file]) (-t [threshold])\n\n"); exit(-1);}
            result = findInArgv(argc, argv, "-o");
            if (result && argc > result + 1){flagCount++; strcpy(stegoFile, argv[result + 1]);}
            else {
                strcat(stegoFile, "stego_");
                strcat(stegoFile, coverFile);
                strcat(stegoFile, ".bin");
            }
            result = findInArgv(argc, argv, "-t");
            if (result && argc > result + 1){flagCount++; threshold = atoi(argv[result + 1]);}
            if ((2*flagCount + 2) < argc){printf("\nERROR: Usage: StegoProject.exe -hide -c [cover file] -m [message file] (-o [stego file]) (-t [threshold])\n\n"); exit(-1);}

            //Open all input files
            coverFilePtr = fopen(coverFile, "rb");
            if( coverFilePtr == NULL){
                printf("\nERROR: Could not open %s.\n\n", coverFile);
                exit(-1);
            }
            messageFilePtr = fopen(messageFile, "rb");
            if( messageFilePtr == NULL){
                printf("\nERROR: Could not open %s.\n\n", messageFile);
                exit(-1);
            }
            stegoFilePtr = fopen(stegoFile, "wb");
            if( stegoFilePtr == NULL){
                printf("\nERROR: Could not open %s.\n\n", stegoFile);
                exit(-1);
            }

            result = hideData(coverFilePtr, messageFilePtr, stegoFilePtr, threshold);
            break;

        case EXTRACT:
            result = findInArgv(argc, argv, "-s");
            if (result && argc > result + 1){flagCount++; strcpy(stegoFile, argv[result + 1]);}
            result = findInArgv(argc, argv, "-o");
            if (result && argc > result + 1){flagCount++; strcpy(messageFile, argv[result + 1]);}
            else {
                strcat(messageFile, "message_");
                strcat(messageFile, stegoFile);
                strcat(messageFile, ".bin");
            }
            result = findInArgv(argc, argv, "-t");
            if (result && argc > result + 1){flagCount++; threshold = atoi(argv[result + 1]);}
            if ((2*flagCount + 2) != argc){printf("\nERROR: Usage: StegoProject.exe -extract -s [stego file] (-o [message file]) (-t [threshold])\n\n"); exit(-1);}

            //Open all input files
            messageFilePtr = fopen(messageFile, "wb");
            if( messageFilePtr == NULL){
                printf("\nERROR: Could not open %s.\n\n", messageFile);
                exit(-1);
            }
            stegoFilePtr = fopen(stegoFile, "rb");
            if( stegoFilePtr == NULL){
                printf("\nERROR: Could not open %s.\n\n", stegoFile);
                exit(-1);
            }

            result = extractData(stegoFilePtr, messageFilePtr, threshold);
            break;
        default:
            printf("\nERROR: Unknown action.\n\n");
            exit(-1);
    }

    if(action == HIDE) fclose(coverFilePtr);
    fclose(messageFilePtr);
    fclose(stegoFilePtr);
    printf("cover file: %s\nmessage file: %s\nstego file: %s\nthreshold = %d\n\n", coverFile, messageFile, stegoFile, threshold);

    return 0;
}

int findInArgv(int argc, char* argv[], char* arg){
    int i;
    for(i = 1; i < argc; i++){
        if(strcmp(argv[i], arg) == 0) return i;
    }
    return 0;
}