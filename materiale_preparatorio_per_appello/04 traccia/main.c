#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

#include "common.h"



void send_over_pipe(int write_desc, char* buf, size_t data_len) {

	/**
     * COMPLETARE QUI
     *
     * Obiettivi:
     * - scrivere sulla pipe tutti i data_len byte contenuti nel buffer buf
     * - gestire eventuali interruzioni ed errori
     * - assicurarsi che tutti i byte siano stati scritti
     */
    int w=0, ret;
    while(w<data_len){
        ret=write(write_desc, buf+w, data_len-w);
        if(ret==-1 && errno== EINTR) continue;
        if(ret==-1) handle_error("");
        w+=ret;
    }

}

int read_from_pipe(int read_desc, char* buf, size_t buf_len) {

	/**
	 * COMPLETARE QUI
     *
     * Obiettivi:
     * - leggere un messaggio dalla pipe e salvarlo nel buffer buf
     * - il valore di ritorno di questa funzione deve essere pari al numero di
     *   byte letto dalla pipe (incluso il delimitatore di fine messaggio)
     * - gestire eventuali chiusure della pipe da parte dell'altro processo come se fosse un errore
     * - gestire come un errore il caso in cui il messaggio sia troppo grande rispetto al buffer a disposizione
     * - gestire eventuali interruzioni ed errori
     *
     * Il delimitatore di fine messaggio è il terminatore di riga '\n'.
	 **/
    int r=0, ret;
    do{
        ret=read(read_desc, buf+r, 1);
        if(ret==-1 && errno== EINTR) continue;
        if(ret==-1) handle_error("");
        if(ret==0) handle_error("");
        if(r>buf_len) handle_error("messaggio maggiore buf");
    }while(buf[r++]!='\n');
    return r;

}

void child_loop(int write_desc, int read_desc) {
    char buf[BUF_SIZE];

    int i;
    for (i = 0; i < NUM_MSG; ++i) {
        int num = rand() % MAX_NUM;
        sprintf(buf, "%d\n", num);
        send_over_pipe(write_desc, buf, strlen(buf));

        int bytes_read = read_from_pipe(read_desc, buf, sizeof(buf));
        buf[bytes_read-1] = '\0';
        printf("[Figlio] ricevuto msg #%d: %s\n", i, buf);
    }
}

void parent_loop(int read_desc, int write_desc) {
    char buf[BUF_SIZE];

    int i;
    for (i = 0; i < NUM_MSG; ++i) {
        int bytes_read = read_from_pipe(read_desc, buf, sizeof(buf));
        buf[bytes_read-1] = '\0';
        printf("[Padre] ricevuto msg #%d: %s\n", i, buf);

        int num = atoi(buf);
        sprintf(buf, "%d\n", 2*num);
        send_over_pipe(write_desc, buf, strlen(buf));
    }
}

int main(int argc, char* argv[]) {

    srand(SEED);

	/**
     * COMPLETARE QUI
     *
     * Obiettivi:
	 * - creare due pipe
     * - creare un processo figlio
     * - chiudere i descrittori di pipe non utilizzati nel padre e nel figlio
     * - il processo figlio chiama child_loop(), che prende come argomenti i
     *   descrittori di pipe per scrivere/leggere (vedere signature funzione)
     * - il processo padre chiama parent_loop(), che prende come argomenti i
     *   descrittori di pipe per leggere/scrivere (vedere signature funzione)
	 * - il padre deve attendere il termine del figlio prima di terminare a sua
     *   volta
	 * - chiudere i descrittori di pipe in fase di uscita
     * - gestire eventuali errori
     */
    int ret;
    int p1[2];
    int p2[2];
    pid_t f;
    ret=pipe(p1);
    if(ret) handle_error("");
    ret=pipe(p2);
    if(ret) handle_error("");
    printf("\npipe create\n");

    f=fork();
    if(f==-1){
        handle_error("fork error");
    }else if (f==0){
        ret=close(p1[1]);
        if(ret) handle_error("close error");
        ret=close(p2[0]);
        if(ret) handle_error("close error");
        printf("lancio child_loop");
        child_loop(p2[1],p1[0]);
        ret=close(p1[0]);
        if(ret) handle_error("close error");
        ret=close(p2[1]);
        if(ret) handle_error("close error");
    } else{
        ret=close(p1[0]);
        if(ret) handle_error("close error");
        ret=close(p2[1]);
        if(ret) handle_error("close error");
        printf("lancio parent_loop");
        parent_loop(p2[0],p1[1]);
        ret=wait(0);
        if(ret==-1) handle_error("errore wait");
        ret=close(p1[1]);
        if(ret) handle_error("close error");
        ret=close(p2[0]);
        if(ret) handle_error("close error");
    }






    return 0;
}
