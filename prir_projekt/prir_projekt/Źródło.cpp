#include <stdio.h>
#include <omp.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"

//iloœæ liczb w tabeli
int N = 10000;

using namespace std;

int* createTable() {
	int* table = (int*)malloc(sizeof(int) * N);


	for (int i = 0; i < N; i++) {
		table[i] = rand() % (100000 - 10000) + 10000;
	}
	return table;
}

int basicAverage(int min, int max, int setValue, int* table) {

	int sum = 0, count = 0, average;
	int num, m, tmp = 0;

	clock_t start = clock();

	for (int i = 0; i < N; i++) {
		num = table[i];
		while (num > 0) {
			m = num % 10;
			tmp = tmp + m;
			num = num / 10;
		}
		if (min < table[i] && table[i]<max && tmp > setValue) {
			sum = sum + table[i];
			count++;
		}
		tmp = 0;
	}

	average = sum / count;
	clock_t end = clock();
	float seconds = (float)(end - start) / CLOCKS_PER_SEC;
	printf("%f\n", seconds);

	return average;
}

int ompAverage(int min, int max, int setValue, int* table) {
	int sum = 0, count = 0, average;
	int num, m, tmp = 0;

	clock_t start = clock();

#pragma omp parallel for num_threads(9)
	for (int i = 0; i < N; i++) {
		num = table[i];
		while (num > 0) {
			m = num % 10;
			tmp = tmp + m;
			num = num / 10;
		}
		if (min < table[i] && table[i]<max && tmp > setValue) {
			sum = sum + table[i];
			count++;
		}
		tmp = 0;
	}

	average = sum / count;
	clock_t end = clock();
	float seconds = (float)(end - start) / CLOCKS_PER_SEC;
	printf("%f\n", seconds);

	return average;
}

int mpiAverage(int min, int max, int setValue, int* table, int argc, char* argv[]) {
	printf("mpiAverage\n");

	int sum = 0, count = 0, average;
	int num, m, tmp = 0;
	int rank, size, length;
	char name[80];

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	clock_t start = clock();

	for (int i = 0; i < N; i++) {
		if (i % size != rank) continue;

		printf("rank %i working on element %i\n", rank, i);

		num = table[i];
		while (num > 0) {
			m = num % 10;
			tmp = tmp + m;
			num = num / 10;
		}
		if (min < table[i] && table[i]<max && tmp > setValue) {
			sum = sum + table[i];
			count++;
		}
		tmp = 0;
	}

	average = sum / count;
	clock_t end = clock();
	float seconds = (float)(end - start) / CLOCKS_PER_SEC;
	printf("%f\n", seconds);

	MPI_Finalize();

	return average;
}

int main(int argc, char* argv[]) {
	int* table = createTable();
	printf("%d\n", basicAverage(10000, 34000, 6, table));;
	printf("%d\n", ompAverage(10000, 34000, 6, table));
	printf("%d\n", mpiAverage(10000, 34000, 6, table, argc, argv));

	return 0;
}