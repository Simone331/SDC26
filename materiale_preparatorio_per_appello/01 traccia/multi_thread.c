#include <pthread.h>
#include <semaphore.h>
#include "common.h"

#define BUFFER_LENGTH 5
#define THREAD_COUNT 4
#define ITERATION_COUNT 10
#define PROD_ROLE 0
#define CONS_ROLE 1

typedef struct thread_args_s {
    int idx;
    int role;
} thread_args_t;

int buffer[BUFFER_LENGTH];
int write_index, read_index;

/**
 * COMPLETARE QUI
 *
 * Obiettivi:
 * - dichiarare i semafori necessari
 *
 */


/***/
sem_t sp,sc,sf, se;



void producer_routine(int idx) {

    /**
     * COMPLETARE QUI
     *
     * Obiettivi:
     * - completare l'implementazione secondo il paradigma
     *   multi-prod/multi-cons inserendo operazioni sui
     *   semafori ove necessario
     * - gestire gli errori
     *
     */
    
    

     int i;
     for (i = 0; i < ITERATION_COUNT; i++) {
        if (sem_wait(&se))handle_error ("");
        if (sem_wait(&sp))handle_error ("");
        int value = idx*ITERATION_COUNT + i;

        buffer[write_index] = value;
        write_index = (write_index + 1) % BUFFER_LENGTH;

        printf("[Thread #%d] inserito %d nel buffer\n", idx, value);
        if (sem_post(&sp))handle_error ("");
        if (sem_post(&sf))handle_error ("");
     
     }
     
}

void consumer_routine(int idx) {

    /**
     * COMPLETARE QUI
     *
     * Obiettivi:
     * - completare l'implementazione secondo il paradigma
     *   multi-prod/multi-cons inserendo operazioni sui
     *   semafori ove necessario
     * - gestire gli errori
     *
     */

     int i;
     for (i = 0; i < ITERATION_COUNT; i++) {
        if (sem_wait(&sf))handle_error ("");
        if (sem_wait(&sc))handle_error ("");

        int value = buffer[read_index];
        read_index = (read_index + 1) % BUFFER_LENGTH;

        printf("[Thread #%d] letto %d dal buffer\n", idx, value);
        if (sem_post(&sc))handle_error ("");
        if (sem_post(&se))handle_error ("");
     }
}


void *thread_routine(void *args) {

    thread_args_t *thread_args = (thread_args_t*)args;
    if (thread_args->role == PROD_ROLE) {
        producer_routine(thread_args->idx);
    } else if (thread_args->role == CONS_ROLE) {
        consumer_routine(thread_args->idx);
    } else {
        printf("[Thread #%d] ruolo sconosciuto: %d\n", thread_args->idx, thread_args->role);
    }

    pthread_exit(NULL);
}

int main(int argc, char* argv[]) {

    int ret,i;

    // inizializzazioni
    read_index = write_index = 0;

    /**
     * COMPLETARE QUI
     *
     * Obiettivi
     * - inizializzare i semafori dichiarati
     * - gestire gli errori
     */
    ret= sem_init(&sc,0,1);
    if(ret) handle_error("sem_init");
    ret= sem_init(&sp,0,1);
    if(ret) handle_error("sem_init");
    ret= sem_init(&se,0,BUFFER_LENGTH);
    if(ret) handle_error("sem_init");
    ret= sem_init(&sf,0,0);
    if(ret) handle_error("sem_init");

    /***/

    /**
     * COMPLETARE QUI
     *
     * Obiettivi: creazione thread
     * - ogni thread deve invocare la funzione thread_routine()
     * - gli argomenti per il thread devono essere specificati
     *   all'interno di una struttura thread_args_t
     * - il campo role può assumere come valori PROD_ROLE o CONS_ROLE
     * - ogni thread ha un valore di idx diverso tra 0 e THREAD_COUNT-1
     * - ci dovranno essere THREAD_COUNT/2 thread con ruolo produttore
     *   ed altrettanti con ruolo consumatore (THREAD_COUNT è pari)
     * - gestire glie errori
     * - N.B.: si tenga presente che successivamente il programma
     *   dovrà attendere la terminazione di ogni thread
     * 
     */
    
    pthread_t threads[THREAD_COUNT];
    for (i=0;i<THREAD_COUNT;i++){
        thread_args_t* args = calloc(1, sizeof(thread_args_t));
        args->idx=i;
        if(i%2){
            args->role=PROD_ROLE;
        }else{
            args->role=CONS_ROLE;
        }
        ret=pthread_create(&threads[i], NULL,  thread_routine, args);
        if(ret) handle_error("error create");

    }



    /***/


    /**
     * COMPLETARE QUI
     *
     * Obiettivi:
     * - attendere la terminazione dei thread lanciati in precedenza
     * - gestire gli errori
     */

    /***/
    for(i=0;i<THREAD_COUNT;i++){
        ret=pthread_join(threads[i], NULL);
        if(ret) handle_error("join");
    }

    /**
     * COMPLETARE QUI
     *
     * Obiettivi:
     * - rilasciare i semafori inizializzati in precedenza
     * - gestire gli errori
     */
    ret=sem_destroy(&sp);
    if(ret) handle_error("destroy");
    ret=sem_destroy(&sc);
    if(ret) handle_error("destroy");
    ret=sem_destroy(&se);
    if(ret) handle_error("destroy");
    ret=sem_destroy(&sf);
    if(ret) handle_error("destroy");


    /***/

    printf("[Main Thread] operazioni completate\n");

    return EXIT_SUCCESS;
}
