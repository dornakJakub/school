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
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>

#define ARGCOUNT 6
#define MAXSKIERS 20000
#define MAXBUSSTOPS 10
#define MAXCAPACITY 100
#define MAXWAITTIME 10000
#define MAXRIDETIME 1000

sem_t *awaiting;
sem_t** stops;
sem_t *final;
int *skiersLeft, *passangersWaiting, *counter;
FILE *file;
int busStops;

// Cleans up file and semaphore
void cleanup(FILE *file) {
    fclose(file);

    if (awaiting != NULL) {
        sem_destroy(awaiting);
        if (munmap(awaiting, sizeof(sem_t)) == -1) {
            fprintf(stderr, "Error unmapping semaphore\n");
        }
        awaiting = NULL;
    }

    if (final != NULL) {
        sem_destroy(final);
        if (munmap(final, sizeof(sem_t)) == -1) {
            fprintf(stderr, "Error unmapping semaphore\n");
        }
        final = NULL;
    }

    if (stops != NULL) {
        for (int i = 0; i < busStops; i++) {
            if (stops[i] != NULL) {
                sem_destroy(stops[i]);
                if (munmap(stops[i], sizeof(sem_t)) == -1) {
                    fprintf(stderr, "Error unmapping semaphore\n");
                }
                stops[i] = NULL;
            }
        }
        free(stops);
        stops = NULL;
    }

    if (skiersLeft != NULL) {
        if (munmap(skiersLeft, sizeof(int)) == -1) {
            fprintf(stderr, "Error unmapping shared variable for skiers left\n");
        }
        skiersLeft = NULL;
    }

    if (passangersWaiting != NULL) {
        if (munmap(passangersWaiting, 10 * sizeof(int)) == -1) {
            fprintf(stderr, "Error unmapping shared variable for passangers waiting\n");
        }
        passangersWaiting = NULL;
    }

    if (counter != NULL) {
        if (munmap(counter, sizeof(int)) == -1) {
            fprintf(stderr, "Error unmapping shared variable for counter\n");
        }
        counter = NULL;
    }
}

// Checks and converts arguments
int checkArgs(int argc, char *argv[], int *args) {
    if (argc != 6) {
        fprintf(stderr, "Invalid number of params: %d\n", argc);
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        args[i] = atoi(argv[i]);
    }

    if (args[1] <= 0 || args[1] > MAXSKIERS) {
        fprintf(stderr, "Invalid value for number of skiers: %d\n", args[1]);
        return 1;
    }
    else if (args[2] <= 0 || args[2] > MAXBUSSTOPS) {
        fprintf(stderr, "Invalid value for number of bus stops: %d\n", args[2]);
        return 1;
    }
    else if (args[3] < 10 || args[3] > MAXCAPACITY) {
        fprintf(stderr, "Invalid value for skibus capacity: %d\n", args[3]);
        return 1;
    }
    else if (args[4] <= 0 || args[4] > MAXWAITTIME) {
        fprintf(stderr, "Invalid value for maximum wait time before getting to bus stop: %d\n", args[4]);
        return 1;
    }
    else if (args[5] <= 0 || args[5] > MAXRIDETIME) {
        fprintf(stderr, "Invalid value for maximum time riding between two bus stops: %d\n", args[5]);
        return 1;
    }

    return 0;
}

// Opens output file, initializes semaphore, and shared variable for skiers left
void init(int totalSkiers) {
    file = fopen("proj2.out", "w");
    setbuf(file, NULL);

    if (file == NULL) {
        fprintf(stderr, "Error opening file\n");
        cleanup(file);
        exit(1);
    }

    skiersLeft = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (skiersLeft == MAP_FAILED) {
        fprintf(stderr, "Error mapping shared variable for skiers left\n");
        cleanup(file);
        exit(1);
    }
    *skiersLeft = totalSkiers;

    passangersWaiting = mmap(NULL, busStops * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (passangersWaiting == MAP_FAILED) {
        fprintf(stderr, "Error mapping shared variable for passangers waiting\n");
        cleanup(file);
        exit(1);
    }

    counter = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (counter == MAP_FAILED) {
        fprintf(stderr, "Error mapping shared variable for counter\n");
        cleanup(file);
        exit(1);
    }
    *counter = 1;
}

int main(int argc, char *argv[]) {

    busStops = atoi(argv[2]);
    int args[6];
    if(checkArgs(argc, argv, args)) {
        return 1;
    }

    // Init
    init(args[1]);

    awaiting = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (awaiting == MAP_FAILED) {
        fprintf(stderr, "Error mapping semaphore\n");
        cleanup(file);
        exit(1); 
    }
    sem_init(awaiting, 1, 0);
    
    final = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (final == MAP_FAILED) {
        fprintf(stderr, "Error mapping semaphore\n");
        cleanup(file);
        exit(1); 
    }
    sem_init(final, 1, 0);

    //init sem for each bus stop
    stops = malloc(args[2] * sizeof(sem_t*));
    for (int i = 0; i < args[2]; i++) {
        stops[i] = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        if (stops[i] == MAP_FAILED) {
            fprintf(stderr, "Error mapping semaphore\n");
            cleanup(file);
            exit(1); 
        }
        sem_init(stops[i], 1, 0);
    }

    srand(time(NULL));
    pid_t pid;
    
    // Skibus process
    pid = fork();
    if (pid == 0) {
        fprintf(file, "%d: BUS: started\n", (*counter)++);
        //Start of skibus road
        while (*skiersLeft > 0) {
            int capacity = args[3];
            int currentBusStop = 1;
            //bus stop
            while (currentBusStop <= args[2]) {
                usleep(rand() % args[5] * 1000);
                fprintf(file, "%d: BUS: arrived to %d\n", (*counter)++, currentBusStop);
                int passangers = passangersWaiting[currentBusStop - 1];
                int maxPassengers = capacity > passangers ? passangers : capacity;
                //boarding
                for (int i = 0; i < maxPassengers; i++) {
                    sem_post(stops[currentBusStop - 1]);
                    sem_wait(awaiting);
                    capacity--;
                }
                fprintf(file, "%d: BUS: leaving %d\n", (*counter)++, currentBusStop);
                (currentBusStop)++;
            }
            usleep(rand() % args[5] * 1000);
            //final
            fprintf(file, "%d: BUS: arrived to final\n", (*counter)++);
            for (int i = 0; i < args[3] - capacity; i++) {
                sem_post(final);
                sem_wait(awaiting);
            }
            fprintf(file, "%d: BUS: leaving final\n", (*counter)++);
        }
        fprintf(file, "%d: BUS: finish\n", (*counter)++);
        exit(0);
    }

    // Skiers processes
    for (int i = 0; i < args[1]; i++) {
        pid = fork();
        if (pid == 0) {
            fprintf(file, "%d: L %d: started\n", (*counter)++, i + 1);
            int busStop = rand() % args[2] + 1;
            usleep(rand() % args[4] * 1000);
            passangersWaiting[busStop - 1]++;
            fprintf(file, "%d: L %d: arrived to %d\n", (*counter)++, i + 1, busStop);
            sem_wait(stops[busStop - 1]);
            fprintf(file, "%d: L %d: boarding\n", (*counter)++, i + 1);
            passangersWaiting[busStop - 1]--;
            sem_post(awaiting);
            sem_wait(final);
            fprintf(file, "%d: L %d: going to ski\n", (*counter)++, i + 1);
            (*skiersLeft)--;
            sem_post(awaiting);
            exit(0);
        }
    }

    while(wait(NULL) > 0);

    cleanup(file);

    return 0;
}