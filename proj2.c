/*
 * IOS - Projekt 2
 * Author: Danyleyko Kirill
 * xlogin: xdanyl00
 * Date: 20.04.2023
 * proj2.c
 */
#include "proj2.h"


//Main function
int main (int argc, char *argv[])
{
    // Create file
    FILE *file = fopen("proj2.out", "w");
    if (file == NULL)
        error_messages(file, err_open);

    // Parsing Arguments
    args_t args;
    if(parse_args(argc, argv, &args) == false)
        error_messages(file, err_args);

    //-----------------------------------------------
    // Initialization memory
    if (init_mem() == false)
        error_messages(file, err_map);

    // Initialization semaphores
    if (init_semaphore() == false)
        error_messages(file, err_sem_init);
    //-----------------------------------------------

    processMain(args.F);

    pid_t process = fork();
    (*line_log) = 1;
    if(process == 0) {

        // Generator Customers
        for (unsigned process_index = 0; process_index < args.NZ; ++process_index) {
            pid_t generateCustomer[args.NZ];
            generateCustomer[args.NZ] = fork();
            printf("line_log - %d, generateCustomer[args.NZ] - [%d] \n", *line_log, generateCustomer[args.NZ]);

            if(generateCustomer[args.NZ] > 0) {
                processCustomer(file, args.TZ, (process_index + 1));
            }
            else if(generateCustomer[args.NZ] == -1) {
                error_messages(file, err_fork);
            }
            else{
                break;
            }
        }
    }
    else if(process == -1)
    {
        error_messages(file, err_fork);
    }
    else {
        printf("NZ - %d\n", args.NZ);
        printf("NU - %d\n", args.NU);
        printf("F - %d\n", args.F);

        // Generator Workers
        for (unsigned process_index = 0; process_index < args.NU; ++process_index)
        {
            pid_t generateWorkers[args.NU];
            generateWorkers[args.NU] = fork();

            if(generateWorkers[args.NU] == 0) {
                processWorker(file, args.TU, (process_index + 1));
            }
            else if(generateWorkers[args.NU] == -1) {
                error_messages(file, err_fork);
            }
            else{
                break;
            }
        }


    }


    // Waiting end of each child processes
    while(wait(NULL) > 0);

    // Clean MMAP
    if(cleanup(file) == false)
        error_messages(file, err_clean);
    return 0;
}

//
void processCustomer(FILE *file, int TZ, int idZ)
{
    int ran;

    sem_wait(mutex);
        fprintf(file, "%d: Z %d: started\n", (*line_log), idZ);
        (*line_log)++;
        fflush(file);
    sem_post(mutex);

    //while()
    //{
        if(TZ != 0) {
            srand(time(NULL) * getpid());
            ran = (rand() % TZ);
            ran *= 1000;
            usleep(ran);
        }

    randomIdService();
    sem_wait(mutex);
        fprintf(file, "%d: Z %d: entering office for a service %d\n", (*line_log), idZ, (*idService));
        (*line_log)++;
        fflush(file);
    sem_post(mutex);

    //}

}

//
void processWorker(FILE *file, int TU, int idU)
{
    sem_wait(mutex);
        fprintf(file, "%d: U %d: started\n", (*line_log), idU);
        fflush(file);
        (*line_log)++;
    sem_post(mutex);

    if(TU == 124) {
        printf("TU - %d\n", TU);
    }

}

//
void processMain(int F)
{
    int ran;
    srand(time(NULL) * getpid());
    ran = (rand() % ((F - (F/2) + 1) + (F/2)));
    ran *= 1000;

    printf("ran - %d\n", ran);
}

// Initialization Memory function
bool init_mem()
{
    // SM for global variables
    if(
    ((line_log = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED) ||
    ((idService = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED) //||
    ) {
        return false;
    }

    // SM for semaphores
    if (
    ((mutex = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0)) == MAP_FAILED) ||
    ((customerSem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0)) == MAP_FAILED)
    ) {
        return false;
    }

    return true;
}

// Initialization Semaphore function
bool init_semaphore()
{
    mutex = sem_open("/xdanyl00.ios.proj2.mut", O_CREAT | O_EXCL, 0666, 1);
    if (mutex == SEM_FAILED)
        return false;

    customerSem = sem_open("/xdanyl00.ios.proj2.customSem", O_CREAT | O_EXCL, 0666, 1);
    if (customerSem == SEM_FAILED)
        return false;

    return true;
}

/// Clean Up
/// @brief
/// @param f
bool cleanup(FILE *file)
{
    // Unmapping SM and Destroy semaphores
    if(
    (munmap((mutex), sizeof(mutex)) == -1 || sem_destroy(mutex) == -1) ||
    (munmap((customerSem), sizeof(customerSem)) == -1 || sem_destroy(mutex) == -1)
    ) {
        return false;
    }
    else {
        sem_unlink("xdanyl00.ios.proj2.mut");
        sem_close(mutex);

        sem_unlink("/xdanyl00.ios.proj2.customSem");
        sem_close(customerSem);
    }


    if (file != NULL) {
        fclose(file);
    }

    return true;
}

// Parsing args function
bool parse_args(int argc, char** argv, args_t *args)
{
    if (argc != 6) {
        return false;
    }
    char *end;
    long arg;

    arg = strtol(argv[1], &end, 10);
    args->NZ = arg;

    arg = strtol(argv[2], &end, 10);
    args->NU = arg;

    arg = strtol(argv[3], &end, 10);
    if (arg >= 0 && arg <= 10000)
        args->TZ = arg;
    else
        return false;

    arg = strtol(argv[4], &end, 10);
    if (arg >= 0 && arg <= 100)
        args->TU = arg;
    else
        return false;

    arg = strtol(argv[5], &end, 10);
    if (arg >= 0 && arg <= 10000)
        args->F = arg;
    else
        return false;

    return true;
}


//Error message
void error_messages(FILE *file, int error)
{
    switch (error) {
        case err_open:
            fprintf(stderr, "Error: Can't open the file \"proj2.out\"\n");
            exit(1);
        case err_args:
            fprintf(stderr, "Error: Invalid arguments\n");
            fclose(file);
            exit(1);
        case err_sem_init:
            fprintf(stderr, "Error: Can't initialization semaphores\n");
            cleanup(file);
            exit(1);
        case err_map:
            fprintf(stderr, "Error: Shared memory initialization FAILED\n");
            exit(1);
        case err_clean:
            fprintf(stderr, "Error: Clean FAILED\n");
            exit(1);
        case err_fork:
            fprintf(stderr, "Error: Fork failed\n");
            cleanup(file);
            exit(1);
    }
}

//
void randomIdService(){
    int ran = (rand() % 3) + 1;
    (*idService) = ran;
}