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


typedef struct args {
    unsigned NZ; // Pocet Zakazniku
    unsigned NU; // Pocet Uredniku
    unsigned TZ; // Max cas v ms, po ktery zakaznik po svem vytcor ceka, nez vyjde na pocty
    unsigned TU; // Max cas prestavky    
    unsigned F;  // Max cas v ms, po kterem je uzavrena posta pro nove prichozi     
} args_t;