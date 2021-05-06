#include<mpi/mpi.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#define N 1000
#define MASTER 0

int min(int a, int b) {
	int result = a < b ? a : b;
	return result;
}

void compareVectors(int * a, int * b) {
	// DO NOT MODIFY
	int i;
	for(i = 0; i < N; i++) {
		if(a[i]!=b[i]) {
			printf("Sorted incorrectly\n");
			return;
		}
	}
	printf("Sorted correctly\n");
}

void displayVector(int * v) {
	// DO NOT MODIFY
	int i;
	int displayWidth = 2 + log10(v[N-1]);
	for(i = 0; i < N; i++) {
		printf("%*i", displayWidth, v[i]);
	}
	printf("\n");
}

int cmp(const void *a, const void *b) {
	// DO NOT MODIFY
	int A = *(int*)a;
	int B = *(int*)b;
	return A-B;
}
 
int main(int argc, char * argv[]) {
	int rank, i, j;
	int nProcesses;
	MPI_Init(&argc, &argv);
	int pos[N];
	int recv_pos[N];
	int sorted = 0;
	int *v = (int*)malloc(sizeof(int)*N);
	int *u = (int*)malloc(sizeof(int)*N);
	int *vQSort = (int*)malloc(sizeof(int)*N);

	for (i = 0; i < N; i++)
		pos[i] = 0;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nProcesses);
	printf("Hello from %i/%i\n", rank, nProcesses);

    if (rank == MASTER) {
		srand(42);
		for (int i = 0; i < N; i++) {
			v[i] = rand() % 10000;
		}
        // generate random vector
    }

	MPI_Bcast(v, N, MPI_INT, MASTER, MPI_COMM_WORLD);
    // send the vector to all processes

	if(rank == 0) {
		// DO NOT MODIFY
		displayVector(v);

		// make copy to check it against qsort
		// DO NOT MODIFY
		for(i = 0; i < N; i++)
			vQSort[i] = v[i];
		qsort(vQSort, N, sizeof(int), cmp);

		// sort the vector v		
		for(i = 1; i < nProcesses; i++) {
			MPI_Recv(recv_pos, N, MPI_INT, i, 0, MPI_COMM_WORLD, NULL);
			for (int j = 0; j < N; j++) {
				pos[j] += recv_pos[j];
			}
		}
		for (i = 0; i < N; i++)
			u[pos[i]] = v[i];
        // recv the new pozitions

		displayVector(u);
		compareVectors(u, vQSort);
	} else {
		
		int P = nProcesses - 1;
		int start = ((rank - 1) * (double) N / P);
		int end = min(rank * (double) N / P, N);

        // compute the positions
		for (int i = start; i < end; i++) {
			for (int j = 0; j < i; j++) {
				if (v[j] <= v[i]) {
					pos[i] += 1;
				} else {
					pos[j] += 1;
				}
			}
		}

		// send the new positions to process MASTER
		MPI_Send(pos, N, MPI_INT, MASTER, 0, MPI_COMM_WORLD);
	}

	MPI_Finalize();
	return 0;
}
