/*
 * IOS - Projekt 2
 * Author: Danyleyko Kirill
 * xlogin: xdanyl00
 * Date: 20.04.2023
 * proj2.h
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>

// Shared memory
int *line_log = NULL;
int *idService = NULL;

// Semaphores
sem_t *mutex = NULL;
sem_t *customerSem = NULL;

// Arguments
typedef struct args {
    unsigned NZ; // Pocet Zakazniku
    unsigned NU; // Pocet Uredniku
    unsigned TZ; // Max cas v ms, po ktery zakaznik po svem vytcor ceka, nez vyjde na pocty
    unsigned TU; // Max cas prestavky    
    unsigned F;  // Max cas v ms, po kterem je uzavrena posta pro nove prichozi     
} args_t;

// Declaration prototype function
bool cleanup(FILE *f);
bool init_semaphore();
void error_messages(FILE *f, int error);
bool parse_args(int argc, char** argv, args_t *args);
bool init_mem();
void processCustomer(FILE *file, int TZ, int process_index);
void processWorker(FILE *file, int TU, int process_index);
void processMain(int F);
void randomIdService();


// Enum Errors
enum errors{
    err_open,
    err_args,
    err_sem_init,
    err_map,
    err_clean,
    err_fork
};
