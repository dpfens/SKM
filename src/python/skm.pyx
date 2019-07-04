from libc.stdlib cimport malloc, free


cdef extern from "../skm.h":
    unsigned long long int * sequential_kmeans(unsigned long long int ** sequences, unsigned long long sequence_count, unsigned long long * sequence_lengths, unsigned long state_count, unsigned long int k, unsigned long long int maximum_iterations, int verbose)


cpdef list skm(list sequences, int k, int maximum_iterations, int verbose=0):
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
            if state not in states:
                states.append(state)
                state_index = len(states) + 1
            else:
                state_index = states.index(state)
            c_sequence[j] = state_index
        c_sequence_lengths[i] = sequence_length
        c_sequences[i] = c_sequence
    cdef unsigned long long int state_count = len(states)
    print('executing clustering algorithm: %r' % states)
    cdef unsigned long long int * c_clusters = sequential_kmeans(c_sequences, c_sequences_length, c_sequence_lengths, state_count, k, maximum_iterations, verbose)
    cdef list clusters = []
    cdef unsigned long long int cluster;
    i = 0
    print("assigning cluster data to list")
    for i in range(c_sequences_length):
        cluster = c_clusters[i]
        c_sequence = c_sequences[i]
        free(c_sequences[i])
        clusters.append(cluster)
    free(c_sequences)
    free(c_clusters)
    free(c_sequence_lengths)
    return clusters
