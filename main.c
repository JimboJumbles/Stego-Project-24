#include "wavStego.h"

#define HIDE 1
#define EXTRACT 2

int main(int argc, char* argv[]) {
    if (argc < 4) {
        printf("ERROR: Incorrect number of parameters.\n");
        return -1;
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
        printf("ERROR: Unknown action.\n");
        return -1;
    }

    switch(action) {
        case HIDE:
            if (strcmp(argv[2], "-m") == 0) {
                strcpy(messageFile, argv[3]);
            } else {
                printf("ERROR: Unknown action.\n");
                return -1;
            }
            if (argc >= 6 && strcmp(argv[4], "-c") == 0) {
                strcpy(coverFile, argv[5]);
            } else {
                printf("ERROR: Unknown action.\n");
                return -1;
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
                printf("ERROR: Unknown action.\n");
                return -1;
            }

            //Open all input files
            coverFilePtr = fopen(coverFile, "r");
            if( coverFilePtr == NULL){
                printf("ERROR: Could not open %s.\n", coverFile);
                return -1;
            }
            messageFilePtr = fopen(messageFile, "r");
            if( messageFilePtr == NULL){
                printf("ERROR: Could not open %s.\n", messageFile);
                return -1;
            }
            stegoFilePtr = fopen(stegoFile, "w");
            if( stegoFilePtr == NULL){
                printf("ERROR: Could not open %s.\n", stegoFile);
                return -1;
            }

            result = hideData(coverFile, messageFile, stegoFile, threshold);
            break;

        case EXTRACT:
            if (strcmp(argv[2], "-s") == 0) {
                strcpy(stegoFile, argv[3]);
            } else {
                printf("ERROR: Unknown action.\n");
                return -1;
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
                printf("ERROR: Unknown action.\n");
                return -1;
            }

            //Open all input files
            messageFilePtr = fopen(messageFile, "w");
            if( messageFilePtr == NULL){
                printf("ERROR: Could not open %s.\n", messageFile);
                return -1;
            }
            stegoFilePtr = fopen(stegoFile, "r");
            if( stegoFilePtr == NULL){
                printf("ERROR: Could not open %s.\n", stegoFile);
                return -1;
            }

            result = extractData(stegoFile, messageFile, threshold);
            break;
        default:
            printf("ERROR: Unknown action.\n");
            return -1;
    }

    if(action == HIDE) fclose(coverFilePtr);
    fclose(messageFilePtr);
    fclose(stegoFilePtr);
    printf("cover file: %s\nmessage file: %s\nstego file: %s\nthreshold = %d\n", coverFile, messageFile, stegoFile, threshold);

    return 0;
}