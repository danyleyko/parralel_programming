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


    pid_t process = fork();
    if(process == 0)
    {
        processMain(args.F);
    }
    else
    {
        // Generator 1
        for (unsigned i = 0; i < args.NZ; ++i) {
            processCustomer();
        }

        // Generator 2
        for (unsigned i = 0; i < args.NU; ++i) {
            processWorker();
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
void processCustomer()
{

}

//
void processWorker()
{

}

//
void processMain(int F)
{
    int ran;
    srand(time(NULL) * getpid());
    ran = (rand() % ((F - (F/2) + 1) + (F/2)));
    ran *= 1000;

    printf("ran - %d\n", ran);
    printf("pid - %d\n", getpid());
}


/// Clean Up
/// @brief
/// @param f
bool cleanup(FILE *file)
{
    // Unmapping SM and Destroy semaphores
    if(munmap((semaphore), sizeof(semaphore)) == -1 || sem_destroy(semaphore) == -1)
    {
        return false;
    }
    else
    {
        sem_unlink("xdanyl00.ios.proj2.sem");
        sem_close(semaphore);
    }




    // Destroying semaphores
    //if(sem_destroy(semaphore) == -1)
    //{
    //    return false;
    //}

    if (file != NULL)
    {
        fclose(file);
    }

    return true;
}

// Initialization Memory function
bool init_mem()
{
    // SM for global variables
    if(
    ((line_log = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED) ||
    ((idZ = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED) ||
    ((idU = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED)
    )
    {
        return false;
    }

    // SM for semaphores
    if (
    ((semaphore = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0)) == MAP_FAILED)
    )
    {
        return false;
    }

    return true;
}

// Initialization Semaphore function
bool init_semaphore()
{
    //if ((sem_init(semaphore, 1, 0) == -1))
    //{
    //    return false;
    //}
    semaphore = sem_open("/xdanyl00.ios.proj2.sem", O_CREAT | O_EXCL, 0666, 1);
    if (semaphore == SEM_FAILED)
        return false;

    return true;
}


// Parsing args function
bool parse_args(int argc, char** argv, args_t *args)
{
    if (argc != 6)
    {
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
    }
}

//void setRandom
