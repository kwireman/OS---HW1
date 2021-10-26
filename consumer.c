#include <stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include<fcntl.h>
#include <sys/shm.h>
#include<sys/stat.h>
#include <sys/mman.h>
#include <pthread.h>
#include <semaphore.h>


int main(){

    const char * a1 = "fill";
    const char * a2 = "available";
    const char * a3 = "mutex";

    int shm_fd = shm_open("table", O_RDWR, 0666);
    int* table = mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    sem_t* fill = sem_open(a1, O_CREAT | O_RDWR, 0666, 0);
    sem_t* avail = sem_open(a2, O_CREAT | O_RDWR, 0666, 2);
    sem_t* mutex = sem_open(a3, O_CREAT | O_RDWR, 0666, 1);

    int loop = 10;
    printf("\nConsumer ready to receive %d items.\n", loop);

    for(int i=1; i<loop; ++i){
        sem_wait(fill);

        int ms = rand() % 2 + 1;
        sleep(ms);

        sem_wait(mutex);
        --(*table);
        sem_post(mutex);

        printf("Item consumed, %d remaining.\n", *table);

        sem_post(avail);
        sem_post(fill);
    }

    sem_close(fill);
    sem_close(avail);
    sem_close(mutex);
    sem_unlink(a1);
    sem_unlink(a2);
    sem_unlink(a3);

    munmap(table,sizeof(int));
    shm_unlink("table");
    printf("Consumer's done!\n");
    return 0;
}
//

