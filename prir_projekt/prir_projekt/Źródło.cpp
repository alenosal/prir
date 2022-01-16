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

int mpiAverage(int min, int max, int setValue, int* table, int argc, char* argv[]) {
  printf("mpiAverage\n");

  unsigned long long int sum = 0, count = 0;
  int num, m, tmp = 0, number = 0, tmplength = 0, * tmptable, average = 0, tmpaverage;
  int rank, size, length;
  char name[80];

  MPI_Status status;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  tmplength = N / size;
  tmptable = (int*)malloc(tmplength * sizeof(int));

  MPI_Scatter(table, tmplength, MPI_INT, tmptable, tmplength, MPI_INT, 0, MPI_COMM_WORLD);

  double start = MPI_Wtime();

  int i = 0;

  if (rank != 0){

    for (i = 0; i < tmplength; i++) {
      num = tmptable[i];
      while (num > 0) {
        m = num % 10;
        tmp = tmp + m;
        num = num / 10;
      }
      if (min < tmptable[i] && tmptable[i]<max && tmp > setValue) {
        sum = sum + tmptable[i];
        count++;
      }
      tmp = 0;
    }

    tmpaverage = sum / count;
    printf("rank: %d\t tmpaverage: %d\n", rank, tmpaverage);

    MPI_Send(&tmpaverage, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
  }

  else{
    
    for (i = 0; i < tmplength; i++) {
      num = tmptable[i];
      while (num > 0) {
        m = num % 10;
        tmp = tmp + m;
        num = num / 10;
      }
      if (min < tmptable[i] && tmptable[i]<max && tmp > setValue) {
        sum = sum + tmptable[i];
        count++;
      }
      tmp = 0;
    }

    printf("%d += %d\n", average, sum / count);
    average += sum / count;

    for (i = 1; i < size; i++) {
      MPI_Recv(&tmpaverage, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
      
      printf("%d += %d\n", average, tmpaverage);
      average += tmpaverage;
    }
    
    average /= size;
  }

  MPI_Finalize();
  
  if( rank == 0 )
  {
    return average;
  }

  exit(0);
}

int main(int argc, char* argv[]) {
  int* table = createTable();
  printf("%d\n", mpiAverage(10000, 34000, 6, table, argc, argv));

  return 0;
}
