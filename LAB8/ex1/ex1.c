#include "mpi.h"
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[])
{
    int  numtasks, rank;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Comm_rank(MPI_COMM_WORLD,&rank);

    int recv_num;

    // First process starts the circle.
    if (rank == 0) {
        MPI_Status status;
        // First process starts the circle.
        // Generate a random number.
        // Send the number to the next process.
        srand(42);
        int random = rand();
        MPI_Send(&random, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        printf("Process with rank %d, sent %d\n\n",
                rank, random);
            
        MPI_Recv(&recv_num, 1, MPI_INT, numtasks - 1, 0, MPI_COMM_WORLD, &status);
        printf("Process with rank %d, received %d\n\n",
                rank, recv_num);

    } else if (rank == numtasks - 1) {
        // Last process close the circle.
        // Receives the number from the previous process.
        // Increments the number.
        // Sends the number to the first process.
        MPI_Status status;
        MPI_Recv(&recv_num, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
        printf("Process with rank %d, received %d\n\n",
                rank, recv_num);
        recv_num++;
        printf("Process with rank %d, sent %d\n\n",
                rank, recv_num);
        MPI_Send(&recv_num, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        
    } else {
        MPI_Status status;
        MPI_Recv(&recv_num, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
        printf("Process with rank %d, received %d\n\n",
                rank, recv_num);
        recv_num++;
        MPI_Send(&recv_num, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
        printf("Process with rank %d, sent %d\n\n",
                rank, recv_num);
        // Middle process.
        // Receives the number from the previous process.
        // Increments the number.
        // Sends the number to the next process.
    }

    MPI_Finalize();

}

