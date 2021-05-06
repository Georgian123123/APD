#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#define MASTER 0

int main(int argc, char * argv[]) {
	int rank;
	int nProcesses;
	int fx, partial, coef;
	MPI_Init(&argc, &argv);
	MPI_Status status;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nProcesses);
	printf("Hello from %i/%i\n", rank, nProcesses);

	if (rank == MASTER) { // This code is run by a single process
		int polynomialSize, n;
		int x = 5; // valoarea cu care se calculeaza polinomul - f(5)

		/*
			in fisierul de intrare formatul este urmatorul:
			numarul_de_coeficienti
			coeficient x^0
			coeficient x^1
			etc.
		*/

		FILE * polFunctionFile = fopen(argv[1], "rt");
		fscanf(polFunctionFile, "%d", &polynomialSize);
		/*
			in array-ul a se vor salva coeficientii ecuatiei / polinomului
			de exemplu: a = {1, 4, 4} => 1 * (x ^ 2) + 4 * (x ^ 1) + 4 * (x ^ 0)
			----------invers-------------
		*/
		float *a = malloc(sizeof(float)*polynomialSize);
		for (int i = 0; i < polynomialSize; i++) {
			fscanf(polFunctionFile, "%f", &a[i]);
			printf("Read value %f\n", a[i]);
			/*
				Se trimit coeficientii pentru x^1, x^2 etc. proceselor 1, 2 etc.
				Procesul 0 se ocupa de x^0 si are valoarea coeficientului lui x^0
			*/
			MPI_Send(&a[i], 1, MPI_INT, i, 0, MPI_COMM_WORLD);
			printf("sends %d, having the value %f\n",i, a[i]);
		}

		fclose(polFunctionFile);

		// Se trimit valorile x si suma partiala (in acest caz valoarea coeficientului lui x^0)
	
		MPI_Send(&a[MASTER], 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
		printf("sends %d, having the partial sum =  %f\n", rank + 1, a[MASTER]);
		MPI_Send(&x, 1, MPI_INT, rank  + 1, 0, MPI_COMM_WORLD);
		printf("sends %d, having the value of x = %d\n", rank + 1, x);

	} else {
		float val, sum;
		int x;

		/*
			se primesc: 
			- coeficientul corespunzator procesului (exemplu procesul 1 primeste coeficientul lui x^1)
			- suma partiala
			- valoarea x din f(x)
			si se calculeaza valoarea corespunzatoare pentru c * x^r, r fiind rangul procesului curent
			si c fiind coeficientul lui x^r, si se aduna la suma
		*/
		MPI_Recv(&val, 1, MPI_FLOAT, MASTER, 0, MPI_COMM_WORLD, &status);
        printf("recieves from %d, having the value %f\n", MASTER, val);
		MPI_Recv(&sum, 1, MPI_FLOAT, rank - 1, 0, MPI_COMM_WORLD, &status);
        printf("recieves from %d the partial sum %f\n", rank, sum);
		MPI_Recv(&x, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
        printf("recieves from %d x %d\n", rank, x);
		sum += val * pow(x, rank);

		if (rank == nProcesses - 1) {
			printf("Polynom value is %f\n", sum);
		} else {
			// se trimit x si suma partiala catre urmatorul proces
			MPI_Send(&sum, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
			printf("sends to %d, having the sum =  %f\n", rank + 1, sum);
			MPI_Send(&x, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
			printf("sends to %d, having the value of x =  %d\n", rank + 1, x);

		}
	}

	MPI_Finalize();
	return 0;
}
