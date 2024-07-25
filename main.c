#include "wavStego.h"

#define HIDE 1
#define EXTRACT 2

int main(int argc, char* argv[]) {

    if (argc < 4) {
        printf("\nERROR: Incorrect number of parameters.\n");
        exit(-1);
    }

    int action = 0;
    int result = 0;
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
            if (strcmp(argv[2], "-m") == 0) {
                strcpy(messageFile, argv[3]);
            } else {
                printf("\nERROR: Unknown action.\n");
                exit(-1);
            }
            if (argc >= 6 && strcmp(argv[4], "-c") == 0) {
                strcpy(coverFile, argv[5]);
            } else {
                printf("\nERROR: Unknown action.\n");
                exit(-1);
            }
            if (argc >= 8 && strcmp(argv[6],  "-o") == 0) {
                strcpy(stegoFile, argv[7]);
            } else {
                strcat(stegoFile, "stego_");
                strcat(stegoFile, coverFile);
                strcat(stegoFile, ".bin");
            }
            if (argc == 10 && strcmp(argv[8], "-t") == 0) {
                threshold = atoi(argv[9]);
            } else if (argc > 9) {
                printf("\nERROR: Unknown action.\n");
                exit(-1);
            }

            //Open all input files
            coverFilePtr = fopen(coverFile, "r");
            if( coverFilePtr == NULL){
                printf("\nERROR: Could not open %s.\n", coverFile);
                exit(-1);
            }
            messageFilePtr = fopen(messageFile, "r");
            if( messageFilePtr == NULL){
                printf("\nERROR: Could not open %s.\n", messageFile);
                exit(-1);
            }
            stegoFilePtr = fopen(stegoFile, "w");
            if( stegoFilePtr == NULL){
                printf("\nERROR: Could not open %s.\n", stegoFile);
                exit(-1);
            }

            result = hideData(coverFilePtr, messageFilePtr, stegoFilePtr, threshold);
            break;

        case EXTRACT:
            if (strcmp(argv[2], "-s") == 0) {
                strcpy(stegoFile, argv[3]);
            } else {
                printf("\nERROR: Unknown action.\n");
                exit(-1);
            }
            if (argc >= 6 && strcmp(argv[4], "-o") == 0) {
                strcpy(messageFile, argv[5]);
            } else {
                strcat(messageFile, "message_");
                strcat(messageFile, stegoFile);
                strcat(messageFile, ".bin");
            }
            if (argc == 8 && strcmp(argv[6], "-t") == 0) {
                threshold = atoi(argv[7]);
            } else if (argc > 7){
                printf("\nERROR: Unknown action.\n");
                exit(-1);
            }

            //Open all input files
            messageFilePtr = fopen(messageFile, "w");
            if( messageFilePtr == NULL){
                printf("\nERROR: Could not open %s.\n", messageFile);
                exit(-1);
            }
            stegoFilePtr = fopen(stegoFile, "r");
            if( stegoFilePtr == NULL){
                printf("\nERROR: Could not open %s.\n", stegoFile);
                exit(-1);
            }

            result = extractData(stegoFilePtr, messageFilePtr, threshold);
            break;
        default:
            printf("\nERROR: Unknown action.\n");
            exit(-1);
    }

    if(action == HIDE) fclose(coverFilePtr);
    fclose(messageFilePtr);
    fclose(stegoFilePtr);
    printf("cover file: %s\nmessage file: %s\nstego file: %s\nthreshold = %d\n", coverFile, messageFile, stegoFile, threshold);

    return 0;
}