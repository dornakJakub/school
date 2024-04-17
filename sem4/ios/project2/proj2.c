/*
* Project 2
* Authors: Jakub Dornak xdorna06
* Date: 17/04/2024
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/mman.h>

#define ARGCOUNT 6
#define MAXSKIERS 20000
#define MAXBUSSTOPS 10
#define MAXCAPACITY 100
#define MAXWAITTIME 10000
#define MAXRIDETIME 1000

sem_t *mutex;

void cleanup() {
    
}

int checkArgs(int argc, char *argv[]) {
    // Check if the number of arguments is correct
    if (argc != 6) {
        fprintf(stderr, "Invalid number of params: %d\n", argc);
        return 1;
    }

    // Convert arguments to integers
    int args[6];
    for (int i = 0; i < 6; i++) {
        args[i] = atoi(argv[i]);
    }

    // Check if the arguments are valid
    if (args[1] <= 0 || args[1] > MAXSKIERS) {
        fprintf(stderr, "Invalid value for number of skiers: %d\n", argc);
        return 1;
    }
    else if (args[2] <= 0 || args[2] > MAXBUSSTOPS) {
        fprintf(stderr, "Invalid value for number of bus stops: %d\n", argc);
        return 1;
    }
    else if (args[3] < 10 || args[3] > MAXCAPACITY) {
        fprintf(stderr, "Invalid value for skibus capacity: %d\n", argc);
        return 1;
    }
    else if (args[4] <= 0 || args[4] > MAXWAITTIME) {
        fprintf(stderr, "Invalid value for maximum wait time before getting to bus stop: %d\n", argc);
        return 1;
    }
    else if (args[5] <= 0 || args[5] > MAXRIDETIME) {
        fprintf(stderr, "Invalid value for maximum time riding between two bus stops: %d\n", argc);
        return 1;
    }

    return 0;
}

int main(int argc, char *argv[]) {

    if(checkArgs(argc, argv)) {
        return 1;
    }

    // while(wait(NULL) > 0);

    cleanup();

    return 0;
}