#include <stdio.h>
#include<stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include<string.h>
#include <sys/types.h>
#include<sys/stat.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <pthread.h>
#include <semaphore.h>

int main(){

    const char * a1 = "fill";
    const char * a2 = "available";
    const char * a3 = "mutex";
    int loop = 10;

    sem_unlink(a1);
    sem_unlink(a2);
    sem_unlink(a3);
    
    int tbl = shm_open("table", O_CREAT | O_RDWR, 0666);
    ftruncate(tbl,sizeof(int));
    int* table = mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, tbl, 0);

    sem_t* fill = sem_open(a1, O_CREAT | O_RDWR, 0666, 0);
    sem_t* avail = sem_open(a2, O_CREAT | O_RDWR, 0666, 2);
    sem_t* mutex = sem_open(a3, O_CREAT | O_RDWR,0666, 1);

    //wait until space is available in shared buffer.

    
    printf("\nProdcuer ready to create %d items. \n", loop-1);

    for(int i=1; i < loop; ++i){
        sem_wait(avail);

        int ms=rand() % 2 + i;
        sleep(ms);

        sem_wait(mutex);
        ++(*table);
        sem_post(mutex);

        printf("Item produced, there are now %d item(s) in the table.\n", *table);

        sem_post(fill);
    }

    sem_close(fill);
    sem_close(avail);
    sem_close(mutex);

    sem_unlink(a1);
    sem_unlink(a2);
    sem_unlink(a3);

    munmap(table, sizeof(int));
    close(tbl);
    shm_unlink("table");
    
    printf("Producer's done.'\n");
    return 0;
}