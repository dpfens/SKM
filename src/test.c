#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "skm.h"


int main() {
  srand(time(0));
  unsigned long long int rows = 10000000;
  unsigned long long int columns = 100;
  unsigned long long int lower = 0;
  unsigned long long int states = 100;
  unsigned long long upper = states -1;

  // Create test data sequences
  unsigned long long * matrix = malloc(rows * columns * sizeof(unsigned long long));
  for(unsigned long long i=0; i<rows * columns; ++i) {
    matrix[i] = (rand() % (upper - lower + 1)) + lower;
  }
  unsigned long long ** matrix_rows = malloc(rows * sizeof(unsigned long long *));
  for(unsigned long long int i=0; i<rows; ++i) {
    matrix_rows[i] = &matrix[i * columns];
  }

  // create markov matrix
  struct Markov * markov_matrix = build_markov_matrix(matrix_rows, rows, columns, states);

  // verify columns of markov matrix sum to ~1.0
  unsigned long long int i;
  #pragma omp parallel for private(i)
  for(i=0; i<states; ++i){
      long double sum = 0.0;
      for(int j=0; j<states; ++j) {
          sum += markov_probability(markov_matrix, states, &i, j);
      }
      printf("%llu = %Lf\n", i, sum);
      assert(sum > 0.999999 && sum < 1.000001);
  }

  // test sequential_kmeans
  int * clusters = sequential_kmeans(matrix_rows, rows, 5, 0);

  // free memory
  free(clusters);
  free(matrix_rows);
  free(matrix);
  dealloc_markov(markov_matrix);
}
