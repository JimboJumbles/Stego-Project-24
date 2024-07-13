#include <stdio.h>
#include <string.h>
#define HIDE 1
#define EXTRACT 2

int main(int argc, char* argv[]){

    //check for correct number of arguments
    if (argc < 4){
        printf("ERROR: Incorrect number of parameters.\n");
        return -1;
    }

    int action = 0;
    char stegoFile[64];
    char coverFile[64];
    char messageFile[64];

    //read first flag in arguments
    if (strcmp(argv[1], "-hide") == 0){
        action = HIDE;
    }
    else if (strcmp(argv[1], "-extract") == 0){
        action = EXTRACT;
    }
    switch(action){
        //read input files for hiding data
        case HIDE:
            //read message file
            if (strcmp(argv[2], "-m") == 0){
                strcpy(messageFile, argv[3]);
            }
            else{
                printf("ERROR: Unknown action.");
                return -1;
            }
            //read cover file
            if (argc >= 6 && strcmp(argv[4], "-c") == 0){
                strcpy(coverFile, argv[5]);
            }
            else{
                printf("ERROR: Unknown action.");
                return -1;
            }
            //read stego file if specified, 
            if (argc == 8 && strcmp(argv[6],  "-o") == 0){
                strcpy(stegoFile, argv[7]);
            }
            else if (argc == 6){
                strcat(stegoFile, "stego_");
                strcat(stegoFile, coverFile);
                strcat(stegoFile, ".bin");
                break;
            }
            else{
                printf("ERROR: Unknown action.");
                return -1;
            }
            break;
        //read input data for extracting data
        case EXTRACT:
            //read stego file
            if (strcmp(argv[2], "-s") == 0){
                strcpy(stegoFile, argv[3]);
            }
            else{
                printf("ERROR: Unknown action.");
                return -1;
            }
            //read message file if specified
            if (argc == 6 && strcmp(argv[4],  "-o") == 0){
                strcpy(messageFile, argv[5]);
            }
            else if (argc == 6){
                strcat(messageFile, "message_");
                strcat(messageFile, stegoFile);
                strcat(messageFile, ".bin");
                break;
            }
            else{
                printf("ERROR: Unknown action.");
                return -1;
            }
            break;
        default:
            printf("ERROR: Unknown action.");
            return -1;
    }
    printf("cover file: %s\nmessage file: %s\nstego file: %s\n", coverFile, messageFile, stegoFile);
    
    return 0;
}