/*
* Author: xdorna06
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <time.h>
#include <stdarg.h>

#define ARG_COUNT 6
#define MAX_TRUCKS 9999
#define MAX_CARS 9999
#define MAX_SHIP_CAPACITY 100
#define MIN_SHIP_CAPACITY 3
#define MAX_CAR_TIME 10000
#define MAX_SHIP_TIME 1000

typedef struct {
    int cars;
    int trucks;
    int ship_capacity;
    int max_car_time;
    int max_ship_time;
} Config;

Config config;
FILE *file;
int *counter;
int *truck_id;
int *car_id;
int *cars_in_harbor;
int *cars_left;
int *ship_capacity;
sem_t *sem_ship;
sem_t **sem_leaves;
sem_t **sem_boards;


void cleanup() {
    fclose(file);

    if (counter != NULL) {
        if (munmap(counter, sizeof(int)) == -1) {
            fprintf(stderr, "Error unmapping shared variable\n");
        }
        counter = NULL;
    }

    if (truck_id != NULL) {
        if (munmap(truck_id, sizeof(int)) == -1) {
            fprintf(stderr, "Error unmapping shared variable\n");
        }
        truck_id = NULL;
    }

    if (car_id != NULL) {
        if (munmap(car_id, sizeof(int)) == -1) {
            fprintf(stderr, "Error unmapping shared variable\n");
        }
        car_id = NULL;
    }

    if (cars_in_harbor != NULL) {
        if (munmap(cars_in_harbor, sizeof(int)*2) == -1) {
            fprintf(stderr, "Error unmapping shared variable\n");
        }
        cars_in_harbor = NULL;
    }

    if (ship_capacity != NULL) {
        if (munmap(ship_capacity, sizeof(int)) == -1) {
            fprintf(stderr, "Error unmapping shared variable\n");
        }
        ship_capacity = NULL;
    }

    if (cars_left != NULL) {
        if (munmap(cars_left, sizeof(int)) == -1) {
            fprintf(stderr, "Error unmapping shared variable\n");
        }
        cars_left = NULL;
    }

    if (sem_ship != NULL) {
        sem_destroy(sem_ship);
        if (munmap(sem_ship, sizeof(sem_t)) == -1) {
            fprintf(stderr, "Error unmapping semaphore\n");
        }
        sem_ship = NULL;
    }

    if (sem_boards != NULL) {
        for (int i = 0; i < 2; i++) {
            if (sem_boards[i] != NULL) {
                sem_destroy(sem_boards[i]);
                if (munmap(sem_boards[i], sizeof(sem_t)) == -1) {
                    fprintf(stderr, "Error unmapping semaphore\n");
                }
                sem_boards[i] = NULL;
            }
        }
        free(sem_boards);
        sem_boards = NULL;
    }

    if (sem_leaves != NULL) {
        for (int i = 0; i < 2; i++) {
            if (sem_leaves[i] != NULL) {
                sem_destroy(sem_leaves[i]);
                if (munmap(sem_leaves[i], sizeof(sem_t)) == -1) {
                    fprintf(stderr, "Error unmapping semaphore\n");
                }
                sem_leaves[i] = NULL;
            }
        }
        free(sem_leaves);
        sem_leaves = NULL;
    }
}

int parse_args(int argc, char *argv[], Config *config) {
    if (argc != ARG_COUNT) {
        fprintf(stderr, "Wrong amount of arguments\n");
        return 1;
    }

    config->trucks = atoi(argv[1]);
    config->cars = atoi(argv[2]);
    config->ship_capacity = atoi(argv[3]);
    config->max_car_time = atoi(argv[4]);
    config->max_ship_time = atoi(argv[5]);

    if  (config->trucks > MAX_TRUCKS) {
        fprintf(stderr, "Wrong trucks value\n");
        return 1;
    }
    if (config->cars > MAX_CARS) {
        fprintf(stderr, "Wrong cars value\n");
        return 1;
    }
    if (config->ship_capacity < MIN_SHIP_CAPACITY || config->ship_capacity > MAX_SHIP_CAPACITY) {
        fprintf(stderr, "Wrong ship capacity value\n");
        return 1;
    }
    if (config->max_car_time < 0 || config->max_car_time > MAX_CAR_TIME) {
        fprintf(stderr, "Wrong car duration value\n");
        return 1;
    }
    if (config->max_ship_time < 0 || config->max_ship_time > MAX_SHIP_TIME) {
        fprintf(stderr, "Wrong ship duration value\n");
        return 1;
    }

    return 0;
}

int init() {
    if ((file = fopen("proj2.out", "w")) == NULL) {
        fprintf(stderr, "Error opening log file\n");
        return 1;
    }

    counter = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (counter == MAP_FAILED) {
        fprintf(stderr, "Error mapping shared variable\n");
        cleanup();
        return 1;
    }
    *counter = 1;

    truck_id = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (truck_id == MAP_FAILED) {
        fprintf(stderr, "Error mapping shared variable\n");
        cleanup();
        return 1;
    }
    *truck_id = 1;

    car_id = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (car_id == MAP_FAILED) {
        fprintf(stderr, "Error mapping shared variable\n");
        cleanup();
        return 1;
    }
    *car_id = 1;

    cars_left = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (cars_left == MAP_FAILED) {
        fprintf(stderr, "Error mapping shared variable\n");
        cleanup();
        return 1;
    }
    *cars_left = config.cars + config.trucks;

    ship_capacity = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (ship_capacity == MAP_FAILED) {
        fprintf(stderr, "Error mapping shared variable\n");
        cleanup();
        return 1;
    }
    *ship_capacity = config.ship_capacity;

    cars_in_harbor = mmap(NULL, sizeof(int) * 2, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (cars_in_harbor == MAP_FAILED) {
        fprintf(stderr, "Error mapping shared variable\n");
        cleanup();
        return 1;
    }
    cars_in_harbor[0] = 0;
    cars_in_harbor[1] = 0;

    sem_ship = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (sem_ship == MAP_FAILED) {
        fprintf(stderr, "Error mapping semaphore\n");
        cleanup();
        return 1; 
    }
    sem_init(sem_ship, 1, 0);

    sem_boards = malloc(2 * sizeof(sem_t*));
    for (int i = 0; i < 2; i++) {
        sem_boards[i] = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        if (sem_boards[i] == MAP_FAILED) {
            fprintf(stderr, "Error mapping semaphore\n");
            cleanup();
            return 1; 
        }
        sem_init(sem_boards[i], 1, 0);
    }

    sem_leaves = malloc(2 * sizeof(sem_t*));
    for (int i = 0; i < 2; i++) {
        sem_leaves[i] = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        if (sem_leaves[i] == MAP_FAILED) {
            fprintf(stderr, "Error mapping semaphore\n");
            cleanup();
            return 1; 
        }
        sem_init(sem_leaves[i], 1, 0);
    }

    return 0;
}

void print_log(const char *format, ...) {
    va_list args;
    va_start(args, format);
    
    vfprintf(file, format, args);

    va_end(args);
    fflush(file);
}

int next_harbor(int harbor) {
    return (++harbor % 2);
}

int main (int argc, char *argv[]) {
    if (parse_args(argc, argv, &config) != 0)
        return 1;
    if (init() != 0)
        return 1;

    pid_t id = fork();
    if (id == 0) {
        srand(time(NULL)*getpid());
        print_log("%d: P: started\n", (*counter)++);
        usleep(rand() % config.max_ship_time * 1000);
        int harbor = 0;
        while (*cars_left > 0) {
            print_log("%d: P: arrived %d\n", (*counter)++, harbor);
            usleep(rand() % config.max_ship_time * 1000);
            int boarded = config.ship_capacity - *ship_capacity;
            for (int i = 0; i < boarded; i++) {
                sem_post(sem_leaves[harbor]);
                sem_wait(sem_ship);
            }
            
            int awaiting = config.ship_capacity < cars_in_harbor[harbor] ? config.ship_capacity : cars_in_harbor[harbor];
            for (int i = 0; i < awaiting; i++) {
                sem_post(sem_boards[harbor]);
                sem_wait(sem_ship);
            }
            print_log("%d: P: leaving %d\n", (*counter)++, harbor);
            harbor = next_harbor(harbor);
        }
        usleep(rand() % config.max_ship_time * 1000);
        print_log("%d: P: finish\n", (*counter)++);
        return 0;
    }

    for (int i = 0; i < config.cars; i++) {
        pid_t id = fork();
        if (id == 0) {
            srand(time(NULL)*getpid());
            int c_id = (*car_id)++;
            int harbor = (rand()%2);
            
            print_log("%d: O %d: started\n", (*counter)++, c_id);
            usleep(rand() % config.max_car_time * 1000);
            (cars_in_harbor[harbor])++;
            print_log("%d: O %d: arrived to %d\n", (*counter)++, c_id, harbor);
            
            sem_wait(sem_boards[harbor]);
            (*ship_capacity)--;
            print_log("%d: O %d: boarding\n", (*counter)++, c_id);
            (cars_in_harbor[harbor])--;
            harbor = next_harbor(harbor);
            sem_post(sem_ship);
            
            sem_wait(sem_leaves[harbor]);
            (*ship_capacity)++;
            (*cars_left)--;
            print_log("%d: O %d: leaving in %d\n", (*counter)++, c_id, harbor);
            sem_post(sem_ship);
            
            return 0;
        }
    }

    for (int i = 0; i < config.trucks; i++) {
        pid_t id = fork();
        if (id == 0) {
            srand(time(NULL)*getpid());
            int t_id = (*truck_id)++;
            int harbor = (rand()%2);

            print_log("%d: N %d: started\n", (*counter)++, t_id);
            usleep(rand() % config.max_car_time * 1000);
            (cars_in_harbor[harbor])++;
            print_log("%d: N %d: arrived to %d\n", (*counter)++, t_id, harbor);
            
            sem_wait(sem_boards[harbor]);
            (*ship_capacity)--;
            print_log("%d: N %d: boarding\n", (*counter)++, t_id);
            (cars_in_harbor[harbor])--;
            harbor = next_harbor(harbor);
            sem_post(sem_ship);

            sem_wait(sem_leaves[harbor]);
            (*ship_capacity)++;
            (*cars_left)--;
            print_log("%d: N %d: leaving in %d\n", (*counter)++, t_id, harbor);
            sem_post(sem_ship);
            
            return 0;
        }
    }

    while (wait(NULL) > 0);

    cleanup();

    return 0;
}