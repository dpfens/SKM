#include <time.h>
#include "markov.h"


int transition_count(unsigned int * sequence, int sequence_length, int j, int k) {
    int count = 0;

    for (int i = 1; i < sequence_length; ++i) {
        if (sequence[i - 1] != j || sequence[i] != k) {
            continue;
        }
        count += 1;
    }
    return count;
}


int * sequential_kmeans(unsigned long long ** sequences, int sequence_count, int k, int maximum_iterations) {
    int cluster_assignment_count = sequence_count * k;

    // randomly assign initial clusters to sequences
    unsigned int * cluster_assignments = malloc(cluster_assignment_count * sizeof(unsigned int));
    int i = 0;
    for(i = 0; i < cluster_assignment_count; i += k) {
        int cluster_assignment = (rand() % k);
        for (int j = 0; j < k; ++j) {
            if(j == cluster_assignment) {
                cluster_assignments[i + j] = 1;
            } else {
                cluster_assignments[i + j] = 0;
            }
        }
    }

    int clusters_match = 0;
    int current_iteration = 0;
    unsigned int * new_cluster_assignments = malloc(cluster_assignment_count * sizeof(unsigned int));
    double * kl_distances = malloc(sequence_count * k * sizeof(double));
    while (!clusters_match && current_iteration < maximum_iterations) {
        // update centroids
        // compute kullback-liebler distances for each sequence

        // assign clusters based on min distance from centroids
        for(i = 0; i < sequence_count; ++i) {
        }

        // check if clusters have changed
        clusters_match = 1;
        for (int i = 0; i < cluster_assignment_count; ++i) {
            if(new_cluster_assignments[i] != cluster_assignments[i]) {
                clusters_match = 0;
                break;
            }
        }

        #pragma omp parallel for private(i)
        for (i = 0; i < cluster_assignment_count; ++i) {
          printf("Assigning %i to %i\n", new_cluster_assignments[i], i);
          cluster_assignments[i] = new_cluster_assignments[i];
        }
        current_iteration += 1;
    }

    // convert cluster assignment matrix to list of cluster assignments
    int * final_assignments = malloc(sequence_count * sizeof(int));
    #pragma omp parallel for private(i)
    for(i = 0; i < sequence_count; ++i) {
        for(int j = 0; j < k; ++j) {
            if (cluster_assignments[(i * k) + j] == 1) {
                final_assignments[i] = j % k;
                break;
            }
        }
    }

    // compute classification log likelihood

    free(kl_distances);
    free(cluster_assignments);
    free(new_cluster_assignments);
    return final_assignments;
}
