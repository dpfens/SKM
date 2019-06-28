#include <time.h>
#include <math.h>
#include <float.h>


unsigned long long int * sequential_kmeans(unsigned long long int ** sequences, unsigned long long sequence_count, unsigned long long * sequence_lengths, unsigned long state_count, unsigned long int k, unsigned int maximum_iterations, int verbose) {
    // randomly assign initial clusters to sequences
    unsigned long long int * cluster_assignments = malloc(sequence_count * sizeof(unsigned long long int));
    unsigned long int i;
    for(i = 0; i < sequence_count; ++i) {
        unsigned long int cluster_assignment = rand() % k;
        cluster_assignments[i] = cluster_assignment;
    }

    // cache sequence state change counts for use when computing centroids
    unsigned long int * sequence_transition_counts = calloc(sequence_count * state_count * state_count, sizeof(unsigned long int));
    if(sequence_transition_counts == NULL) {
      printf("Unsuccessful memory allocation\n");
      return NULL;
    }

    if (verbose > 0) {
        printf("Calculating %llu state transition counts for each of the %llu sequences\n", sequence_count * state_count * state_count, sequence_count);
    }
    #pragma omp parallel for private(i) shared(sequence_count)
    for (i = 0; i < sequence_count; ++i) {
      unsigned long int sequence_start = i * state_count * state_count;
      unsigned long long int sequence_length = sequence_lengths[i];
      for (unsigned long int j = 1; j < sequence_length; ++j) {
        unsigned long int previous_state = *(sequences[i] + j - 1);
        unsigned long int current_state = *(sequences[i] + j);
        #pragma omp critical
        sequence_transition_counts[sequence_start + previous_state * state_count + current_state] += 1;
      }
    }

    if (verbose > 0) {
        printf("Calculating %llu state transition probabilities for each of the %llu sequences\n", sequence_count * state_count * state_count, sequence_count);
    }
    long double * sequence_transition_probabilities = calloc(sequence_count * state_count * state_count, sizeof(long double));
    if(sequence_transition_probabilities == NULL) {
      printf("Unsuccessful memory allocation\n");
      return NULL;
    }

    #pragma omp parallel for private(i) collapse(1) shared(sequence_count, state_count)
    for (unsigned long int i = 0; i < sequence_count; ++i) {
      for (unsigned long int j = 0; j < state_count; ++j) {
        unsigned long int transition_probability_denominator = 0;
        for (unsigned long int l = 0; l < state_count; ++l) {
          transition_probability_denominator += sequence_transition_counts[i * state_count * state_count + j * state_count + l];
        }
        for (unsigned long int l = 0; l < state_count; ++l) {
          unsigned long int transition_probability_numerator = sequence_transition_counts[i * state_count * state_count + j * state_count + l];
          if (transition_probability_numerator == 0 || transition_probability_denominator == 0) {
              sequence_transition_probabilities[i * state_count * state_count + j * state_count + l] = 0;
          } else{
              sequence_transition_probabilities[i * state_count * state_count + j * state_count + l] = (long double)transition_probability_numerator / (long double)transition_probability_denominator;
          }
        }
      }
    }

    int clusters_match = 0;
    int current_iteration = 0;
    unsigned long long int * new_cluster_assignments = malloc(sequence_count * sizeof(unsigned long long int));
    unsigned long int centroid_count = k * state_count * state_count;
    long double * centroids = malloc(centroid_count * sizeof(long double));
    unsigned long int * centroid_numerators = malloc(centroid_count * sizeof(unsigned long int));
    unsigned long int * centroid_denominators = malloc(k * state_count * sizeof(unsigned long int));
    unsigned long long int kl_count = sequence_count * k;
    long double * kl_distances = malloc(kl_count * sizeof(long double));
    unsigned long long int cluster_assignment_changes = 0;
    while (!clusters_match && current_iteration < maximum_iterations) {
        // Update cached centroid numerators/denominator calculations for re-use in centroid calculations
        unsigned long int s;
        #pragma omp parallel for private(s) collapse(1) shared(k, state_count, sequence_count)
        for (unsigned long int s = 0; s < k; ++s) {
            for (unsigned long int j = 0; j < state_count; ++j) {
                unsigned long int denominator = 0;
                for(unsigned long int r = 0; r < state_count; ++r) {
                    unsigned long int numerator = 0;
                    for(unsigned long int i = 0; i < sequence_count; ++i) {
                        if (cluster_assignments[i] == s) {
                          numerator += sequence_transition_counts[i * state_count * state_count + j * state_count + r];
                          denominator += sequence_transition_counts[i * state_count * state_count + j * state_count + r];
                        }
                    }
                    centroid_numerators[s * state_count * state_count + j * state_count + r] = numerator;
                }
                centroid_denominators[s * state_count + j] = denominator;
            }
        }

        // updating centroids
        //printf("Calculating centroids\n");
        #pragma omp parallel for private(s) collapse(2) shared(k, state_count)
        for (unsigned long int s = 0; s < k; ++s) {
            for (unsigned long int j = 0; j < state_count; ++j) {
                for (unsigned long int r = 0; r < state_count; ++r) {
                  //printf("%ld * %ld + %ld * %ld + %ld = %ld\n", s, k, j, state_count, i, s * state_count * state_count + j * state_count + r);
                  unsigned long int centroid_numerator = centroid_numerators[s * state_count * state_count + j * state_count + r];
                  unsigned long int centroid_denominator = centroid_denominators[s * state_count + j];
                  centroids[s * state_count * state_count + j * state_count + r] = (long double)centroid_numerator / (long double)centroid_denominator;
                  //printf("Cluster %lu - Centroid[%lu][%lu](%lu): %lu/%lu\n", s, j, r, s * state_count * state_count + j * state_count + r, centroid_numerator, centroid_denominator);
                }
            }
        }

        //compute kullback-liebler distances for each sequence
        //printf("Calculating kullback-liebler distances\n");
        #pragma omp parallel for private(s) shared(k, state_count, sequence_count)
        for (s = 0; s < k; ++s) {
          for(unsigned long int i = 0; i < sequence_count; ++i) {
            long double distance = 0.0;
            for (unsigned long int j = 0; j < state_count; ++j) {
              for (unsigned long int l = 0; l < state_count; ++l) {
                long double transition_probability = sequence_transition_probabilities[i * state_count * state_count + j * state_count + l];
                long double centroid = centroids[s * state_count * state_count + j * state_count + l];
                distance += centroid * (long double)log(centroid / transition_probability);
                //printf("Cluster %lu, Distance: %Lf, Centroid[%lu][%lu][%lu]: %Lf, transition_probability: %Lf, log(%Lf/%Lf) = %f\n", s, distance, s, j, l, centroid, transition_probability, centroid, transition_probability, log(centroid / transition_probability));
              }
            }
            kl_distances[i * k + s] = distance;
          }
        }

        // assign clusters based on min distance from centroids
        #pragma omp parallel for private(i) shared(sequence_count, k)
        for(i = 0; i < sequence_count; ++i) {
            unsigned long int potential_assignment = cluster_assignments[i];
            long double min_kl_distance = DBL_MAX;
            for (unsigned long int j = 0; j < k; ++j) {
              long double kl_distance = kl_distances[i * k + j];
              //printf("Sequence #%lu: checking if %lu is possible cluster (%Lf < %Lf)\n", i, j, kl_distance, min_kl_distance);
              if (kl_distance < min_kl_distance) {
                //printf("sequence: %lu - new potential cluster: %li -> %lu\n", i, cluster_assignments[i], j);
                potential_assignment = j;
                min_kl_distance = kl_distance;
              }
            }
            //printf("Sequence #%lu = %lu", i, potential_assignment);
            new_cluster_assignments[i] = potential_assignment;

        }

        // check if clusters have changed
        clusters_match = 1;
        cluster_assignment_changes = 0;
        #pragma omp parallel for reduction(+: cluster_assignment_changes) shared(sequence_count, clusters_match)
        for (unsigned long int i = 0; i < sequence_count; ++i) {
            if(new_cluster_assignments[i] != cluster_assignments[i]) {
                if (clusters_match != 0) {
                    clusters_match = 0;
                }
                cluster_assignment_changes += 1;
            }
        }

        #pragma omp parallel for private(i)
        for (i = 0; i < sequence_count; ++i) {
          cluster_assignments[i] = new_cluster_assignments[i];
        }
        if(verbose > 0) {
            printf("Iteration: %i - %llu cluster assignment changes\n", current_iteration, cluster_assignment_changes);
        }
        current_iteration += 1;
    }
    free(kl_distances);
    free(sequence_transition_counts);
    free(sequence_transition_probabilities);
    free(centroids);
    free(centroid_numerators);
    free(centroid_denominators);
    free(new_cluster_assignments);

    long double * cluster_proportions = calloc(k, sizeof(long double));
    for(unsigned long int i = 0; i < sequence_count; ++i) {
      unsigned long int cluster_assignment = cluster_assignments[i];
      cluster_proportions[cluster_assignment] += 1;
    }
    for(unsigned long int s = 0; s < k; ++s) {
      cluster_proportions[s] = cluster_proportions[s] / (long double)sequence_count;
    }
    return cluster_assignments;
}
