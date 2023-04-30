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
    (*line_log) = 1;
    if(process == 0) {
        processMain(file, args.F);
    }
    else if(process == -1) {
        error_messages(file, err_fork);
    }
    else {
        // Generator
        pid_t generator;
        for (unsigned process_index = 0; process_index != (args.NU + args.NZ) ; process_index++) {
            generator = fork();

            if(generator == 0) {
                if(process_index < args.NZ)
                    processCustomer(file, args.TZ, (process_index + 1));
            }
            else if(generator == -1) {
                error_messages(file, err_fork);
            }
            else {
                if(process_index < args.NU)
                    processWorker(file, args.TU, (process_index + 1));
            }

            if((process_index > args.NU && process_index == args.NZ))
                break;
        }
    }


    // Clean MMAP
    if(cleanup(file) == false)
        error_messages(file, err_clean);
    return 0;
}

// Process for Customers
void processCustomer(FILE *file, int TZ, int idZ)
{
    // Random time in interval <0 , TZ>
    int ran = 0;
    if(TZ != 0) {
        srand(time(NULL) * getpid());
        ran = (rand() % TZ) + 1;
        ran *= 1000;
    }

    // Z Start
    sem_wait(print_sem);
        fprintf(file, "%d: Z %d: started\n", (*line_log), idZ);
        (*line_log)++;
        fflush(file);
    sem_post(print_sem);

    usleep(ran); // @ran: Random time in interval <0 , TZ>

    if((*postOpened))
    {
        // Z pick service
        sem_wait(print_sem);
            randomIdService();
            fprintf(file, "%d: Z %d: entering office for a service %d\n", (*line_log), idZ, (*idService));
            (*line_log)++;
            fflush(file);
            (queueList[(*idService)])++;
        sem_post(print_sem);

        // Switch for queue
        switch ((*idService)) {
            case 1:
                sem_wait(first_queue);
                break;
            case 2:
                sem_wait(second_queue);
                break;
            case 3:
                sem_wait(third_queue);
                break;
        }

        // Z called
        sem_wait(print_sem);
            fprintf(file, "%d: Z %d: called by office worker\n", (*line_log), idZ);
            (*line_log)++;
            fflush(file);
            randomWaitingTime(); // Random time usleep in interval <0 , 10>
        sem_post(print_sem);
    }

    // Z go home
    sem_wait(print_sem);
        fprintf(file, "%d: Z %d: going home\n", (*line_log), idZ);
        (*line_log)++;
        fflush(file);
    sem_post(print_sem);

    // End process with 0
    exit(0);
}

// Process for Worker
void processWorker(FILE *file, int TU, int idU)
{
    bool CustomerServed = false;
    int queueServe = 0;
    int ran = 0;
    if(TU != 0) {
        srand(time(NULL) * getpid());
        ran = (rand() % TU); // Random time in interval <0,TU>
        ran *= 1000;
    }

    // START
    sem_wait(print_sem);
        fprintf(file, "%d: U %d: started\n", (*line_log), idU);
        fflush(file);
        (*line_log)++;
    sem_post(print_sem);

    while(true) {
        // Queue is not NULL
        if(!queueIsNULL()){

            queueServe = randomQueuePick();

            // SERVE START
            sem_wait(print_sem);
                randomIdService(); // void function for picking random idService
                fprintf(file, "%d: U %d: serving a service of type %d\n", (*line_log), idU, queueServe);
                (*line_log)++;
                fflush(file);
            sem_post(print_sem);

            // Switch for queue
            switch ((*idService)) {
                case 1:
                    sem_post(first_queue);
                    (queueList[(queueServe)])--;
                    break;
                case 2:
                    sem_post(second_queue);
                    (queueList[(queueServe)])--;

                    break;
                case 3:
                    sem_post(third_queue);
                    (queueList[(queueServe)])--;
                    break;
            }

            // SERVE END
            sem_wait(print_sem);
                randomWaitingTime(); // Random time usleep in interval <0 , 10>
                fprintf(file, "%d: U %d: service finished\n", (*line_log), idU);
                (*line_log)++;
                fflush(file);
                CustomerServed = true;
            sem_post(print_sem);
        }
        // if Queue is NULL, then break
        else if(queueIsNULL() && (*postOpened) && !CustomerServed ){
            // BREAK START
            sem_wait(print_sem);
                fprintf(file, "%d: U %d: taking break\n", (*line_log), idU);
                (*line_log)++;
                fflush(file);
            sem_post(print_sem);

            usleep(ran); // @ran: Random time in interval <0 , TU>

            // BREAK END
            sem_wait(print_sem);
                fprintf(file, "%d: U %d: break finished\n", (*line_log), idU);
                (*line_log)++;
                fflush(file);
            sem_post(print_sem);
        }
        else if(!(*postOpened)) {
            sem_wait(print_sem);
                fprintf(file, "%d: U %d: going home\n", (*line_log), idU);
                (*line_log)++;
                fflush(file);
            sem_post(print_sem);

            break;
        }
    }
}

// Process Main
void processMain(FILE *file, int F)
{
    // Random time in interval <F/2 , F>
    int ran;
    if(F != 0){
        srand(time(NULL) * getpid());
        ran = (rand() % ((F - (F/2) + 1) + (F/2)));
        ran *= 1000;
    }

    usleep(ran); // @ran: Random time in interval <F/2 , F>

    printf("POST time working - %d microseconds\n", ran);

    // POST CLOSING
    sem_wait(print_sem);
        fprintf(file, "%d: closing\n", (*line_log));
        fflush(file);
        (*line_log)++;
        (*postOpened) = false;
    sem_post(print_sem);

    // End process with 0
    exit(0);
}

// Initialization Memory function
bool init_mem()
{
    // SM for global variables
    if(
    ((line_log = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED) ||
    ((idService = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED) ||
    ((postOpened = mmap(NULL, sizeof(bool), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0)) == MAP_FAILED) ||
    ((queueList = mmap(NULL, sizeof(int) * 3, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0) ) == MAP_FAILED) //||
    ) {
        return false;
    }

    // SM for semaphores
    if (
    ((print_sem = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0)) == MAP_FAILED) ||
    ((first_queue = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0)) == MAP_FAILED) ||
    ((second_queue = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0)) == MAP_FAILED) ||
    ((third_queue = mmap(NULL, sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, 0, 0)) == MAP_FAILED) //||
    ) {
        return false;
    }

    // Post open
    (*postOpened) = true;
    // Generate NULL queue
    for (unsigned i = 0; i < sizeof(*queueList); ++i) {
        (queueList[i]) = 0;
    }

    return true;
}

// Initialization Semaphore function
bool init_semaphore()
{
    print_sem = sem_open("/xdanyl00.ios.proj2.printSem", O_CREAT | O_EXCL, 0666, 1);
    if (print_sem == SEM_FAILED)
        return false;

    first_queue = sem_open("/xdanyl00.ios.proj2.firstQueueSem", O_CREAT | O_EXCL, 0666, 0);
    if (first_queue == SEM_FAILED)
        return false;

    second_queue = sem_open("/xdanyl00.ios.proj2.secondQueueSem", O_CREAT | O_EXCL, 0666, 0);
    if (second_queue == SEM_FAILED)
        return false;

    third_queue = sem_open("/xdanyl00.ios.proj2.thirdQueueSem", O_CREAT | O_EXCL, 0666, 0);
    if (third_queue == SEM_FAILED)
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
    (munmap((print_sem), sizeof(print_sem)) == -1) ||
    (munmap((first_queue), sizeof(first_queue)) == -1) ||
    (munmap((second_queue), sizeof(second_queue)) == -1) ||
    (munmap((third_queue), sizeof(third_queue)) == -1) //||
    ) {
        return false;
    }
    else {
        sem_unlink("/xdanyl00.ios.proj2.printSem");
        sem_close(print_sem);

        sem_unlink("/xdanyl00.ios.proj2.firstQueueSem");
        sem_close(first_queue);

        sem_unlink("/xdanyl00.ios.proj2.secondQueueSem");
        sem_close(second_queue);

        sem_unlink("/xdanyl00.ios.proj2.thirdQueueSem");
        sem_close(third_queue);
    }

    if(
    (munmap((line_log), sizeof(line_log)))  ||
    (munmap((idService), sizeof(idService)))  ||
    (munmap((postOpened), sizeof(postOpened)))  ||
    (munmap((queueList), sizeof(queueList)))  //||
    ){
        return false;
    }

    if (file != NULL) {
        fclose(file);
    }

    return true;
}

// Parsing args function
bool parse_args(int argc, char** argv, args_t *args)
{
    //@todo: Check if argv[] are digit()

    if (argc != 6) {
        return false;
    }
    char *end;
    long arg;


    // Checking if arhuments isdigit()
    for (int i = 1; i < argc; ++i) {
        for (int j = 0; argv[i][j] != '\0'; ++j) {
            if(!(isdigit(argv[i][j]))) {
                return false;
            }
        }
    }

    arg = strtol(argv[1], &end, 10);
    args->NZ = arg;
    if(arg == 0)
        exit(1);

    arg = strtol(argv[2], &end, 10);
    args->NU = arg;
    if(arg == 0)
        exit(1);

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
    srand(time(NULL) * getpid());
    int ran = (rand() % 3) + 1;
    (*idService) = ran;
}

void randomWaitingTime(){
    srand(time(NULL) * getpid());
    int ran = (rand() % 10) + 1;
    usleep(ran * 1000);
}

// Queue is NULL
bool queueIsNULL(){
    for (unsigned i = 0; i != sizeof(queueList); ++i) {
        if((queueList[i]) == 1)
        {
            return false;
        }
    }

    return true;
}

int randomQueuePick(){
    int len = (sizeof(*queueList)/sizeof(queueList[0]));
    int arr[len];
    int count = 0;
    for (int i = 0; i != sizeof(*queueList); ++i) {
        if(queueList[i] == 1)
        {
            arr[count] = i;
            count++;
        }
    }
    int random_index = arr[rand() % count];

    return random_index;
}