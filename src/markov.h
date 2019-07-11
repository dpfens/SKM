#include <stdlib.h>


struct Markov {
  int initial_state;
  unsigned long int * numerator;
  unsigned long int * denominator;
  long double * transition_matrix;
};

void dealloc_markov(struct Markov * markov) {
  free(markov->numerator);
  free(markov->denominator);
  free(markov->transition_matrix);
  free(markov);
}


void print_matrix(unsigned long long int * matrix, int rows, int columns){
  printf("[\n");
  for(int i = 0; i<rows; i++) {
    printf("[");
    for(int j = 0; j<columns; j++){
      printf("%llu, ", matrix[i * columns + j]);
    }
    printf("],\n");
  }
  printf("]\n");
}


void print_markov(long double * matrix, int rows, int columns){
  printf("[\n");
  for(int i = 0; i<rows; i++) {
    printf("[");
    for(int j = 0; j<columns; j++){
      printf("%Lf, ", matrix[i * columns + j]);
    }
    printf("],\n");
  }
  printf("]\n");
}



void print_array(unsigned long long int * arr, int length) {
  printf("[");
  for(int i = 0; i<length; ++i){
    printf("%llu, ", arr[i]);
  }
  printf("]\n");
}

void update_matrix(struct Markov * matrix, unsigned long long int rows, unsigned long long int columns, unsigned long long ** sequences, unsigned long long int sequence_count, unsigned long long int * sequence_lengths) {
    unsigned long long int sequence_length;
    for(unsigned long long int i = 0; i<sequence_count; ++i) {
        sequence_length = sequence_lengths[i];
      for(unsigned long long int j = 0; j<sequence_length; ++j) {
        unsigned long int previous_state = 0;
        if (j > 0) {
          previous_state = *(sequences[i] + j - 1) + 1;
        }
        unsigned long int current_state = *(sequences[i] + j);
        matrix->numerator[current_state * columns + previous_state] += 1;
        matrix->denominator[previous_state] += 1;
      }
    }

    unsigned long long int i;
    #pragma omp parallel for private(i)
    for(i = 0; i<columns; ++i){
      for(unsigned long long int j = 0; j<rows; ++j) {
        matrix->transition_matrix[j * columns + i] = (long double)matrix->numerator[j * columns + i] / (long double)matrix->denominator[i];
      }
    }
}


struct Markov * build_markov_matrix(unsigned long long ** sequences, unsigned long long int sequence_count, unsigned long long int * sequence_lengths, unsigned long state_count) {
  unsigned long long int rows = state_count;
  unsigned long long int columns = state_count + 1;
  unsigned long long int matrix_size = rows * columns;
  unsigned long int * numerator_counts = calloc(matrix_size, sizeof(unsigned long int));

  if (numerator_counts == NULL) {
      printf("Unable to allocate memory %llu for numerator_counts", matrix_size);
  }

  unsigned long int * denominator_counts = calloc(columns, sizeof(unsigned long int));

  if (denominator_counts == NULL) {
      printf("Unable to allocate memory %llu for denominator_counts", columns);
  }
  long double * transition_matrix = calloc(matrix_size, sizeof(long double));

  if (transition_matrix == NULL) {
      printf("Unable to allocate memory %llu for transition_matrix", matrix_size);
  }

  //printf("Matrix size: %llu(%llu * %llu)\n", matrix_size, rows, columns);
  struct Markov * markov_matrix = malloc(sizeof(struct Markov));
  markov_matrix->initial_state = 0;
  markov_matrix->numerator = numerator_counts;
  markov_matrix->denominator = denominator_counts;
  markov_matrix->transition_matrix = transition_matrix;

  update_matrix(markov_matrix, rows, columns, sequences, sequence_count, sequence_lengths);

  //print_markov(transition_matrix, rows, columns);
  return markov_matrix;
}


long double markov_probability(struct Markov * matrix, int state_count, unsigned long int * previous_state, unsigned long int next_state) {
  // increment the column count to account for the initial state
  int column_count = state_count + 1;
  if (previous_state == NULL) {
    // return the probability of next_state being the initial state
    return matrix->transition_matrix[next_state * column_count + matrix->initial_state];
  }
  // skip the first column as it is the initial state
  //printf("markov_probability[%i][%i] = %Lf\n", *previous_state, next_state, matrix->transition_matrix[next_state * column_count + *previous_state + 1]);
  return matrix->transition_matrix[next_state * column_count + *previous_state + 1];
}
