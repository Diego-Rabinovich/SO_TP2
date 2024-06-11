#include "include/philosophers.h"
#include "../Testing/test_util.h"
#include "../User/include/userLib.h"

#define MAX_PHILOS 17
#define MIN_PHILO 5
#define SEM_NAME 12
#define PHILO_NAME 8
#define INITIAL_PHILOS 7
#define PHILOS_STATE "philos_state"
#define PHILOS_AMOUNT "philos_amount"
#define THINKING 0
#define EATING 1
#define EOFILE 1

char palillos[MAX_PHILOS][SEM_NAME] = {"sem_philo00\0", "sem_philo01\0",
                                       "sem_philo02\0", "sem_philo03\0",
                                       "sem_philo04\0", "sem_philo05\0",
                                       "sem_philo06\0", "sem_philo07\0",
                                       "sem_philo08\0", "sem_philo09\0",
                                       "sem_philo10\0", "sem_philo11\0",
                                       "sem_philo12\0", "sem_philo13\0",
                                       "sem_philo14\0", "sem_philo15\0"};

char philos[MAX_PHILOS][PHILO_NAME] = {"philo00\0", "philo01\0",
                                       "philo02\0", "philo03\0",
                                       "philo04\0", "philo05\0",
                                       "philo06\0", "philo07\0",
                                       "philo08\0", "philo09\0",
                                       "philo10\0", "philo11\0",
                                       "philo12\0", "philo13\0",
                                       "philo14\0", "philo15\0"};
int philos_pid[MAX_PHILOS];
char philo_state[MAX_PHILOS];
int philos_amount;

int16_t bg_fds[3] = {DEV_NULL, STDOUT, STDERR};
int16_t my_fds[3];

void initPhilosopher() {

    sys_sem_destroy(PHILOS_STATE);
    sys_sem_destroy(PHILOS_AMOUNT);

    if (sys_sem_new(PHILOS_STATE, 1) == -1 || sys_sem_new(PHILOS_AMOUNT, 1) == -1) {
        printErr("\nWe couldn't open the restaurant, an error occurred opening semaphore\n", 2);
        return;
    }

    sys_get_FDs(my_fds);

    philos_amount = INITIAL_PHILOS;

    char philosopher_num[4];
    char **argv = (char **) sys_malloc(4*sizeof (char *));
    for(int j=0;j<INITIAL_PHILOS;j++){
        philo_state[j]=THINKING;
    }

    for (int i = 0; i < INITIAL_PHILOS; i++) {
        argv[0] = philos[i];
        uintToBase(i, philosopher_num, 10);
        argv[1] = philosopher_num;
        argv[2] = 0;
        philos_pid[i] = sys_createProcess((Main) philosopher, argv, philos[i], 3, bg_fds);
    }

    sys_free(argv);

    char c;
    sys_read(my_fds[STDIN], &c, 1);
    while (c != EOFILE) {
        if (c == 'a') {
            addPhilosopher();
        } else if (c == 'r') {
            removePhilosopher();
        }
        sys_read(my_fds[STDIN], &c, 1);
    }

    sys_sem_wait(PHILOS_STATE);
    for (int i = 0; i < MAX_PHILOS; i++) {
        /* sem se cierran con el kill */
        if (i < philos_amount) {
            sys_kill(philos_pid[i]);
        }
        sys_sem_destroy(palillos[i]);
    }
    sys_sem_post(PHILOS_STATE);
    sys_sem_destroy(PHILOS_STATE);
    print("\nFin\n", 0xFFFFFF, 2);

}

int get_random_number_using_time() {
    int current_time ; sys_getCpuTime(&current_time);
    current_time=current_time* 1000000; // Assuming microseconds
    unsigned int random_number = (unsigned int) current_time % 100001;
    return (int)random_number;
}


void philosopher(int argc, char **argv) {
    int philosopher_num;
    strToInt(argv[1], &philosopher_num);

    if (sys_sem_new(palillos[philosopher_num], 1) == -1 ||
        sys_sem_new(palillos[(philosopher_num + 1) % philos_amount], 1) == -1) {
        printErr("\nYou can't, an error occurred creating a new semaphore\n", 2);
        return;
    }

    int right;
    while (1) {
        right=(philosopher_num+ 1) % philos_amount;
        if (philosopher_num % 2) { //odd philosopher
            sys_sem_wait(palillos[philosopher_num]);
            sys_sem_wait(palillos[right]);
        } else { //even philosopher
            sys_sem_wait(palillos[right]);;
            sys_sem_wait(palillos[philosopher_num]);
        }
        philo_state[philosopher_num] = EATING;
        eat(philosopher_num);
        sys_sem_post(palillos[philosopher_num]);
        sys_sem_post(palillos[right]);
        philo_state[philosopher_num] = THINKING;
        for (int i = 0; i < 9999*GetUint()*GetUint(); i++);
    }
}

void eat(int philosopher_num) {
    int idx=philos_amount;
    sys_sem_wait(PHILOS_STATE);
    for (int i = 0; i <idx; i++) {
        if (philo_state[i] == EATING) {
            print("E", 0xFFFFFF, 2);
        } else {
            print(".", 0xFFFFFF, 2);
        }
    }
    print("\n", 0xFFFFFF, 2);
    sys_sem_post(PHILOS_STATE);
}


void addPhilosopher() {
    sys_sem_wait(PHILOS_STATE);
    if (philos_amount == MAX_PHILOS) {
        print("\nThe table is full, no more philosophers can be added\n", 0xFFFFFF, 2);
        sys_sem_post(PHILOS_STATE);
        return;
    }
    int idx=philos_amount;
    philos_amount++;
    philo_state[idx] = THINKING;
    char **argv = (char **) sys_malloc(4*sizeof (char *));
    argv[0] = philos[idx];
    char philo_num[4];
    uintToBase(idx, philo_num, 10);
    argv[1] = philo_num;
    argv[2] = 0;
    philos_pid[idx] = sys_createProcess((Main) philosopher, argv, philos[idx], 3, bg_fds);
    sys_free(argv);
    sys_sem_post(PHILOS_STATE);
}

void removePhilosopher() {
    sys_sem_wait(PHILOS_STATE);
    int idx=philos_amount;
    if (idx == MIN_PHILO) {
        printErr("\nThere should be at least 5 philosophers\n", 2);
        return;
    }
    philos_amount--;
    philo_state[--idx] = THINKING;
    sys_kill(philos_pid[idx]);
    sys_sem_post(PHILOS_STATE);
}
