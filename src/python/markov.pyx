from libc.stdlib cimport calloc, malloc, free, srand

cdef extern from "time.h":
    long int time(int)


cdef extern from "../markov.h":
    struct Markov:
        int initial_state
        unsigned long int * numerator
        unsigned long int * denominator
        long double * transition_matrix

    void dealloc_markov(Markov * markov)

    Markov * build_markov_matrix(unsigned long long ** sequences, unsigned long long int sequence_count, unsigned long long int * sequence_lengths, unsigned long state_count)

    void update_matrix(Markov * matrix, unsigned long long int rows, unsigned long long int columns, unsigned long long ** sequences, unsigned long long int sequence_count, unsigned long long int * sequence_lengths)

    long double markov_probability(Markov * matrix, int state_count, unsigned long int * previous_state, unsigned long int next_state)

    unsigned long int markov_state(Markov * matrix, int state_count, unsigned long int * previous_state)


srand(time(0))


cdef class MarkovMatrix:
    cdef Markov * matrix
    cdef list states

    def __init__(self, list states):
        self.states = states

    def __repr__(self):
        cdef unsigned long int state_count = len(self.states)
        cdef str output = '[\n'
        cdef unsigned long int i = 0
        cdef unsigned long int j = 0
        cdef long double probability
        cdef str formatted_probability
        for i in range(state_count):
            j = 0
            for j in range(state_count):
                probability = markov_probability(self.matrix, state_count, &j, i)
                formatted_probability = str(round(probability, 5))
                output += ' ' + formatted_probability + ','
            output += ']\n'
        output += ']'
        return output


    cpdef void build(self, list sequences):
        cdef unsigned long long int c_sequences_length = <unsigned long long int>len(sequences)
        cdef list states = list()
        cdef unsigned long long int state_index
        cdef unsigned long long int sequence_length
        cdef unsigned long long int j
        cdef list sequence
        cdef unsigned long long int * c_sequence
        cdef unsigned long long int * c_sequence_lengths = <unsigned long long int *> malloc(c_sequences_length * sizeof(unsigned long long int))
        cdef unsigned long long int ** c_sequences = <unsigned long long int **> malloc(c_sequences_length * sizeof(unsigned long long int *))
        cdef unsigned long long int i = 0
        for i in range(c_sequences_length):
            sequence = sequences[i]
            sequence_length = <unsigned long long int>len(sequence)
            c_sequence = <unsigned long long int *> malloc(sequence_length * sizeof(unsigned long long int))
            j = 0
            for j in range(sequence_length):
                state = sequence[j]
                state_index = self.states.index(state)
                c_sequence[j] = state_index
            c_sequence_lengths[i] = sequence_length
            c_sequences[i] = c_sequence
        cdef unsigned long int state_count = <unsigned long int>len(self.states)
        self.matrix = build_markov_matrix(c_sequences, c_sequences_length, c_sequence_lengths, state_count)

    cpdef void update(self, list sequences):
        cdef unsigned long long int c_sequences_length = <unsigned long long int>len(sequences)
        cdef list states = list()
        cdef unsigned long long int state_index
        cdef unsigned long long int sequence_length
        cdef unsigned long long int j
        cdef unsigned long long int * c_sequence
        cdef unsigned long long int * c_sequence_lengths = <unsigned long long int *> malloc(c_sequences_length * sizeof(unsigned long long int))
        cdef unsigned long long int ** c_sequences = <unsigned long long int **> malloc(c_sequences_length * sizeof(unsigned long long int *))
        cdef unsigned long long int i = 0
        for i in range(c_sequences_length):
            sequence = sequences[i]
            sequence_length = len(sequence)
            c_sequence = <unsigned long long int *> malloc(sequence_length * sizeof(unsigned long long int))
            j = 0
            for j in range(sequence_length):
                state = sequence[j]
                state_index = self.states.index(state)
                c_sequence[j] = state_index
            c_sequence_lengths[i] = sequence_length
            c_sequences[i] = c_sequence
        cdef long int state_count = len(self.states)
        cdef unsigned long int rows = state_count
        cdef unsigned long int columns = state_count + 1
        update_matrix(self.matrix, rows, columns, c_sequences, c_sequences_length, c_sequence_lengths)
        i = 0
        for i in range(c_sequences_length):
            c_sequence = c_sequences[i]
            free(c_sequences[i])
        free(c_sequences)
        free(c_sequence_lengths)

    cpdef list predict(self, initial_state, unsigned long int steps):
        cdef unsigned long int state_count = len(self.states)

        cdef unsigned long int * previous_state
        cdef unsigned long int index
        if initial_state in self.states:
            index = self.states.index(initial_state)
            previous_state = &index
        else:
            previous_state = NULL
        cdef unsigned long int next_state

        cdef list sequential_states = list()
        cdef unsigned long int i = 0
        for i in range(steps):
            next_state = markov_state(self.matrix, state_count, previous_state)
            sequential_states.append(self.states[next_state])
            previous_state = &next_state
        return sequential_states

    cdef __del__(self):
        dealloc_markov(self.matrix)
