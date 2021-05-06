/*
 * APD - Tema 1
 * Octombrie 2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <pthread.h>

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y)) 

char *in_filename_julia;
char *in_filename_mandelbrot;
char *out_filename_julia;
char *out_filename_mandelbrot;
int threads;
pthread_barrier_t barrier;
#define MAX_THREADS 3

// structura pentru un numar complex
typedef struct _complex {
	double a;
	double b;
} complex;

// structura pentru parametrii unei rulari
typedef struct _params {
	int is_julia, iterations;
	double x_min, x_max, y_min, y_max, resolution;
	complex c_julia;
} params;


// structura pentru a retine datele threadului
typedef struct thread_details {
	int id;
	int start;
	int end;
	int **result;
	int height;
	params *par;
} details;
// citeste argumentele programului
void get_args(int argc, char **argv)
{
	if (argc < 5) {
		printf("Numar insuficient de parametri:\n\t"
				"./tema1 fisier_intrare_julia fisier_iesire_julia "
				"fisier_intrare_mandelbrot fisier_iesire_mandelbrot\n");
		exit(1);
	}

	in_filename_julia = argv[1];
	out_filename_julia = argv[2];
	in_filename_mandelbrot = argv[3];
	out_filename_mandelbrot = argv[4];
	strcat(argv[5], "\0");
	threads = atoi(argv[5]);
}

// citeste fisierul de intrare
void read_input_file(char *in_filename, params* par)
{
	FILE *file = fopen(in_filename, "r");
	if (file == NULL) {
		printf("Eroare la deschiderea fisierului de intrare!\n");
		exit(1);
	}

	fscanf(file, "%d", &par->is_julia);
	fscanf(file, "%lf %lf %lf %lf",
			&par->x_min, &par->x_max, &par->y_min, &par->y_max);
	fscanf(file, "%lf", &par->resolution);
	fscanf(file, "%d", &par->iterations);

	if (par->is_julia) {
		fscanf(file, "%lf %lf", &par->c_julia.a, &par->c_julia.b);
	}

	fclose(file);
}

// scrie rezultatul in fisierul de iesire
void write_output_file(char *out_filename, int **result, int width, int height)
{
	int i, j;

	FILE *file = fopen(out_filename, "w");
	if (file == NULL) {
		printf("Eroare la deschiderea fisierului de iesire!\n");
		return;
	}

	fprintf(file, "P2\n%d %d\n255\n", width, height);
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			fprintf(file, "%d ", result[i][j]);
		}
		fprintf(file, "\n");
	}

	fclose(file);
}

// aloca memorie pentru rezultat
int **allocate_memory(int width, int height)
{
	int **result;
	int i;

	result = malloc(height * sizeof(int*));
	if (result == NULL) {
		printf("Eroare la malloc!\n");
		exit(1);
	}

	for (i = 0; i < height; i++) {
		result[i] = malloc(width * sizeof(int));
		if (result[i] == NULL) {
			printf("Eroare la malloc!\n");
			exit(1);
		}
	}

	return result;
}

// elibereaza memoria alocata
void free_memory(int **result, int height)
{
	int i;

	for (i = 0; i < height; i++) {
		free(result[i]);
	}
	free(result);
}


// ruleaza algoritmul Julia
void *run_julia(void *args)
{
	struct thread_details threadDetails = *(struct thread_details*) args;

	printf("[JULIA] start %d -> end %d\n", threadDetails.start, threadDetails.end);
	int w, h, i;

	for (w = threadDetails.start; w < threadDetails.end; w++) {
		for (h = 0; h < threadDetails.height; h++) {
			int step = 0;
			complex z = { .a = w * threadDetails.par->resolution + threadDetails.par->x_min,
							.b = h * threadDetails.par->resolution + threadDetails.par->y_min };

			while (sqrt(pow(z.a, 2.0) + pow(z.b, 2.0)) < 2.0 && step < threadDetails.par->iterations) {
				complex z_aux = { .a = z.a, .b = z.b };

				z.a = pow(z_aux.a, 2) - pow(z_aux.b, 2) + threadDetails.par->c_julia.a;
				z.b = 2 * z_aux.a * z_aux.b + threadDetails.par->c_julia.b;

				step++;
			}

			threadDetails.result[h][w] = step % 256;
		}
	}

	// asteptam threadurile sa termine tot
	pthread_barrier_wait(&barrier);

	if (threadDetails.id == 0) {
		// transforma rezultatul din coordonate matematice in coordonate ecran
		for (i = 0; i < threadDetails.height / 2; i++) {
			int *aux = threadDetails.result[i];
			threadDetails.result[i] =threadDetails.result[threadDetails.height - i - 1];
			threadDetails.result[threadDetails.height - i - 1] = aux;
		}
	}

	pthread_exit(NULL);
}

// ruleaza algoritmul Mandelbrot
void *run_mandelbrot(void *args)
{	
	struct thread_details threadDetails = *(struct thread_details*) args;
	printf("[MANDELBROT] start %d -> end %d\n", threadDetails.start, threadDetails.end);
	int w, h, i;

	for (w = threadDetails.start; w < threadDetails.end; w++) {
		for (h = 0; h < threadDetails.height; h++) {
			complex c = { .a = w * threadDetails.par->resolution + threadDetails.par->x_min,
							.b = h * threadDetails.par->resolution + threadDetails.par->y_min };
			complex z = { .a = 0, .b = 0 };
			int step = 0;

			while (sqrt(pow(z.a, 2.0) + pow(z.b, 2.0)) < 2.0 && step < threadDetails.par->iterations) {
				complex z_aux = { .a = z.a, .b = z.b };

				z.a = pow(z_aux.a, 2.0) - pow(z_aux.b, 2.0) + c.a;
				z.b = 2.0 * z_aux.a * z_aux.b + c.b;

				step++;
			}

			threadDetails.result[h][w] = step % 256;
		}
	}
	
	// asteptam threadurile sa termine tot
	pthread_barrier_wait(&barrier);

	if (threadDetails.id == 0) {
		// transforma rezultatul din coordonate matematice in coordonate ecran
		for (i = 0; i < threadDetails.height / 2; i++) {
			int *aux = threadDetails.result[i];
			threadDetails.result[i] =threadDetails.result[threadDetails.height - i - 1];
			threadDetails.result[threadDetails.height - i - 1] = aux;
		}
	}
	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	params par1;
	params par2;
	int widthJulia, heightJulia, widthM, heightM;
	int **result1;
	int **result2;

	// se citesc argumentele programului
	get_args(argc, argv);
	struct thread_details threadDetails[threads];
	pthread_t tid[threads];

	// initializam matricea
	pthread_barrier_init(&barrier, NULL, threads);

	// Julia:
	// - se citesc parametrii de intrare
	// - se aloca tabloul cu rezultatul
	// - se ruleaza algoritmul
	// - se scrie rezultatul in fisierul de iesire
	// - se elibereaza memoria alocata
	read_input_file(in_filename_julia, &par1);

	widthJulia = (par1.x_max - par1.x_min) / par1.resolution;
	heightJulia = (par1.y_max - par1.y_min) / par1.resolution;

	result1 = allocate_memory(widthJulia, heightJulia);

	// Mandelbrot:
	// - se citesc parametrii de intrare
	// - se aloca tabloul cu rezultatul
	// - se ruleaza algoritmul
	// - se scrie rezultatul in fisierul de iesire
	// - se elibereaza memoria alocata

	read_input_file(in_filename_mandelbrot, &par2);

	widthM = (par2.x_max - par2.x_min) / par2.resolution;
	heightM = (par2.y_max - par2.y_min) / par2.resolution;

	result2 = allocate_memory(widthM, heightM);

	for (int i = 0; i < threads; ++i) {

		if (threads % 2 != 0) {
			if (i <= (threads) / 2) {
			int thread_id = i % ((threads  + 1) / 2);

			// Calcul intre ce coloane va merge thread-ul
			int start_width = thread_id * (double)widthM / ((threads  + 1)/2);
			int end_width = MIN((thread_id + 1) * (double)widthM/ ((threads  + 1)/2), widthM);

			// Populam structura cu detaliile obtinute
			threadDetails[i].id = thread_id;
			threadDetails[i].start = start_width;
			threadDetails[i].end = end_width;

			threadDetails[i].result = result2;
			threadDetails[i].par = &par2;
			threadDetails[i].height = heightM;

			// cream threadul
			int r = pthread_create(&tid[i], NULL, run_mandelbrot, &threadDetails[i]);

			// defensive coding
			if (r) {
				printf("eroare la crearea threadului\n");
				exit(-1);
			}
			} else {
				int thread_id = i % ((threads  + 1)/ 2);
				// Calcul intre ce coloane va merge thread-ul
				int start_width = thread_id * (double)widthJulia / ((threads) / 2);
				int end_width = MIN((thread_id + 1) * (double)widthJulia / ((threads) / 2), widthJulia);

				// Populam structura cu detaliile obtinute
				threadDetails[i].id = thread_id;
				threadDetails[i].start = start_width;
				threadDetails[i].end = end_width;

				threadDetails[i].result = result1;
				threadDetails[i].par = &par1;
				threadDetails[i].height = heightJulia;

				// cream threadul
				int r = pthread_create(&tid[i], NULL, run_julia, &threadDetails[i]);

				// defensive coding
				if (r) {
					printf("eroare la crearea threadului\n");
					exit(-1);
				}
			}
		} else {
			if (i <= (threads) / 2) {
				int thread_id = i % ((threads) / 2);

				// Calcul intre ce coloane va merge thread-ul
				int start_width = thread_id * (double)widthM / ((threads)/2);
				int end_width = MIN((thread_id + 1) * (double)widthM/ ((threads)/2), widthM);

				// Populam structura cu detaliile obtinute
				threadDetails[i].id = thread_id;
				threadDetails[i].start = start_width;
				threadDetails[i].end = end_width;

				threadDetails[i].result = result2;
				threadDetails[i].par = &par2;
				threadDetails[i].height = heightM;

				// cream threadul
				int r = pthread_create(&tid[i], NULL, run_mandelbrot, &threadDetails[i]);

				// defensive coding
				if (r) {
					printf("eroare la crearea threadului\n");
					exit(-1);
				}
			} else {
				int thread_id = i % ((threads)/ 2);
				// Calcul intre ce coloane va merge thread-ul
				int start_width = thread_id * (double)widthJulia / ((threads) / 2);
				int end_width = MIN((thread_id + 1) * (double)widthJulia / ((threads) / 2), widthJulia);

				// Populam structura cu detaliile obtinute
				threadDetails[i].id = thread_id;
				threadDetails[i].start = start_width;
				threadDetails[i].end = end_width;

				threadDetails[i].result = result1;
				threadDetails[i].par = &par1;
				threadDetails[i].height = heightJulia;

				// cream threadul
				int r = pthread_create(&tid[i], NULL, run_julia, &threadDetails[i]);

				// defensive coding
				if (r) {
					printf("eroare la crearea threadului\n");
					exit(-1);
				}
			}
		}
	}

	for (int i = 0; i < threads; ++i) {
		// asteptam sa termine threadul
		int r = pthread_join(tid[i], NULL);

		// defensive coding
		if (r) {
			printf("eroare la terminarea threadului\n");
			exit(-1);
		}
	}

	write_output_file(out_filename_julia, result1, widthJulia, heightJulia);
	free_memory(result1, heightJulia);
	
	write_output_file(out_filename_mandelbrot, result2, widthM, heightM);
	free_memory(result2, heightM);

	pthread_barrier_destroy(&barrier);
	return 0;
}
