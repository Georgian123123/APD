#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

/*
    schelet pentru exercitiul 5
*/
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y)) 

typedef struct Array{
    int *arr;
    int start;
    int end;
    long id;
} array;

int* arr;
int array_size;

void *increment(void *args) {
    struct Array myArray = *(struct Array*)args;
    printf("Thread-ul %ld se ocupa de intervalul %d -> %d \n", myArray.id, myArray.start, myArray.end);
    for (int florin = 0; florin < 100000; ++florin) {
        for (int iter = myArray.start; iter < myArray.end; ++iter) {
            myArray.arr[iter] = iter;
            myArray.arr[iter] += 100;   
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        perror("Specificati dimensiunea array-ului\n");
        exit(-1);
    }
    // utilites for threads
    // long NUM_THREADS = sysconf(_SC_NPROCESSORS_CONF);
    long NUM_THREADS = (long)8;
    pthread_t threads[NUM_THREADS];
  	int r;
  	long id;
  	void *status;


    array_size = atoi(argv[1]);

    arr = malloc(array_size * sizeof(int));

    for (int i = 0; i < array_size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
    // TODO: aceasta operatie va fi paralelizata
    struct Array myArray[NUM_THREADS];

  	for (int id = 0; id < NUM_THREADS; ++id) {
          int start = id * (double)array_size / NUM_THREADS;
          int end = MIN((id + 1) * (double)array_size / NUM_THREADS, array_size);
          myArray[id].arr = arr;
          myArray[id].start = start;
          myArray[id].end = end;
          myArray[id].id = id;
          r = pthread_create(&threads[id], NULL, increment, &myArray[id]);
          if (r) {
            printf("Eroare la asteptarea thread-ului %d\n", id);
  		    exit(-1);
          }
    }
    
    for (int id = 0; id < NUM_THREADS; ++id) {
        r = pthread_join(threads[id], &status);
        if (r) {
            printf("Eroare la asteptarea thread-ului %d\n", id);
  		    exit(-1);
        }
    }
    for (int i = 0; i < array_size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
  	pthread_exit(NULL);
}
