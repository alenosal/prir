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

  int i = 0;
  for (i; i < N; i++) {
    table[i] = rand() % (100000 - 10000) + 10000;
  }
  return table;
}

int basicAverage(int min, int max, int setValue, int* table) {

  int sum = 0, count = 0, average;
  int num, m, tmp = 0;

  clock_t start = clock();

  int i = 0;
  for (i; i < N; i++) {
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

  int i = 0;
  for (i = 0; i < N; i++) {
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
  int num, m, tmp = 0, number = 0;
  int rank, size, length;
  char name[80];

  MPI_Status status;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  clock_t start = MPI_Wtime();
  int i = 0;
  if (rank == 0)
  {
    start = MPI_Wtime();

    printf("rank = 0\n");

    for (i = 1; i < size; i++)
      MPI_Send(table, sizeof(table) / sizeof(int), MPI_INT, i, 0, MPI_COMM_WORLD);
  }

  else
  {
    printf("else\n");

    MPI_Get_count(&status, MPI_INT, &number);
    MPI_Recv(table, sizeof(table) / sizeof(int), MPI_INT, 0, 0, MPI_COMM_WORLD, &status);

    printf("rank: %d\n", rank);

    for (i; i < N; i++) {
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

    printf("suma: %d\n", sum);
  }

  average = sum / count;

  float seconds = (float)(MPI_Wtime() - start) / CLOCKS_PER_SEC;
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
