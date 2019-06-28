#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "skm.h"


int main() {
  srand(time(0));
  unsigned long long int rows = 1000000;
  unsigned long long int columns = 500;
  unsigned long long int lower = 0;
  unsigned long long int states = 7;
  unsigned long int upper = states -1;

  // Create test data sequences
  unsigned long long * matrix = malloc(rows * columns * sizeof(unsigned long long));
  for(unsigned long long i=0; i<rows * columns; ++i) {
    matrix[i] = (rand() % (upper - lower + 1)) + lower;
  }
  unsigned long long ** matrix_rows = malloc(rows * sizeof(unsigned long long *));
  for(unsigned long long int i=0; i<rows; ++i) {
    matrix_rows[i] = &matrix[i * columns];
  }

  unsigned long long * row_lengths = malloc(rows *sizeof(unsigned long long));
  for (unsigned long long i = 0; i <rows; ++i) {
      row_lengths[i] = columns;
  }

  // test sequential_kmeans
  printf("Testing Sequential KMeans function\n");
  unsigned long long int * clusters = sequential_kmeans(matrix_rows, rows, row_lengths, states, 8, 1000, 1);
  for(unsigned long int i = 0; i <rows; ++i) {
    printf("Cluster %llu\n", clusters[i]);
  }

  // free memory
  free(clusters);
  free(matrix_rows);
  free(matrix);
}
