#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// #define NUM_THREADS 2


void *threadfirst(void *args) {
	int id = *(int*) args;
	printf("geo e cel mai... spune thread-ul %d!\n", id);
	pthread_exit(NULL);
}

void *threadsecond(void *args) {
	int id = *(int*) args;
	printf("smecher spune thread-ul %d!\n", id);
	pthread_exit(NULL);
}

void *f(void *arg) {
  	long id = *(long*) arg;
	for (int i = 0; i < 100; ++i) {
  		printf("Hello World din thread-ul %ld! la iteratia %d\n", id, i);
	}
	  pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	
	long cores = sysconf(_SC_NPROCESSORS_CONF);
	long NUM_THREADS = cores;

	

	pthread_t threads[NUM_THREADS];
  	int r;
  	long id;
  	void *status;
  	long arguments[NUM_THREADS];

	/* ex 2 si  3
  	// for (id = 0; id < NUM_THREADS; id++) {
  	// 	arguments[id] = id;
	// 	r = pthread_create(&threads[id], NULL, f, &arguments[id]);

	// 	if (r) {
	//   		printf("Eroare la crearea thread-ului %ld\n", id);
	//   		exit(-1);
	// 	}
  	// }

  	// for (id = 0; id < NUM_THREADS; id++) {
	// 	r = pthread_join(threads[id], &status);

	// 	if (r) {
	//   		printf("Eroare la asteptarea thread-ului %ld\n", id);
	//   		exit(-1);
	// 	}
  	// }
	  */
	arguments[0] = 0;
	arguments[1] = 1;
	r = pthread_create(&threads[0], NULL, threadfirst, &arguments[0]);
	if (r) {
		printf("Eroare la crearea thread-ului %d\n", 0);
	  	exit(-1);
	}
	r = pthread_create(&threads[1], NULL, threadsecond, &arguments[1]);
	if (r) {
		printf("Eroare la crearea thread-ului %d\n", 1);
	  	exit(-1);
	}

	r = pthread_join(threads[0], &status);
	if (r) {
  		printf("Eroare la asteptarea thread-ului %d\n", 0);
  		exit(-1);
	}
	r = pthread_join(threads[1], &status);
	if (r) {
  		printf("Eroare la asteptarea thread-ului %d\n", 1);
  		exit(-1);
	}
  	pthread_exit(NULL);
}
