#include "input_parser.h"
#include <stdlib.h>
#include <string.h>

void print_usage() {
    printf("Usage: <executable> <input_sequence_file> <0: global, 1: local> <optional: path_to_parameters_config>\n");
}

// Function to parse the alignment type (0 for global, 1 for local)
int parse_alignment_type(const char *arg) {
    return atoi(arg);
}

char** read_sequence_inputs(const char *filename, const size_t num_seq) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening sequence inputs file");
        exit(1);
    }

    char line[256]; // buffer to hold each line from file
    int curr_sequence = -1; // current sequence being processed
    char **sequences = (char**)malloc(num_seq * sizeof(char*)); // array of sequence strings
    size_t *allocated_sizes = (size_t*)malloc(num_seq * sizeof(size_t)); // track allocated memory for each sequence

    if (sequences == NULL) {
        perror("Failed to allocate memory for sequences array");
        fclose(file);
        exit(1);
    }

    if (allocated_sizes == NULL) {
        perror("Failed to allocate memory for allocated_sizes");
        fclose(file);
        exit(1);
    }
    
    // init all sequences in sequences array
    for (size_t i = 0; i < num_seq; i++) {
        size_t initial_size = INITIAL_MAX_SEQ_SIZE * sizeof(char);
        sequences[i] = (char*)malloc(initial_size);
        allocated_sizes[i] = initial_size;
        if (sequences[i] == NULL) {
            perror("Failed to allocate memory for a sequence");
            fclose(file);
            exit(1);
        }
        sequences[i][0] = '\0'; // empty string initialization
    }
    
    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '>') {
            // skip
            curr_sequence++;
            continue;
        }

        if (curr_sequence < num_seq) {
            size_t line_len = strlen(line);

            // check if there is a newline char and replace with null terminator
            if (line_len > 0 && line[line_len - 1] == '\n') {
                line[line_len - 1] = '\0';
            }
    
            size_t curr_seq_len = strlen(sequences[curr_sequence]);
            size_t curr_size = allocated_sizes[curr_sequence];
            size_t needed_size = (curr_seq_len + line_len) * sizeof(char);
            size_t realloc_size = needed_size * 2;

            // if current sequence does not have enough space, reallocate
            if (curr_size < needed_size) {
                char *seq = (char*)realloc(sequences[curr_sequence], realloc_size);
                if (seq == NULL) {
                    perror("Failed to realloc memory for a sequence");
                    fclose(file);
                    exit(1);
                }

                sequences[curr_sequence] = seq;
                allocated_sizes[curr_sequence] = realloc_size;
            }

            strcat(sequences[curr_sequence], line); // add new line to the sequence
        }
    }

    free(allocated_sizes);
    fclose(file);
    return sequences;
}

void read_configs(const char *filename, int *match, int *mismatch, int *gap_open, int *gap_ext) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening parameters config file");
        exit(1);
    }

    char param[50];
    int value;

    while (fscanf(file, "%s %d", param, &value) != EOF) {
        if (strcmp(param, "match") == 0) {
            *match = value;
        } else if (strcmp(param, "mismatch") == 0) {
            *mismatch = value;
        } else if (strcmp(param, "h") == 0) {  // gap opening
            *gap_open = value;
        } else if (strcmp(param, "g") == 0) {  // gap extension
            *gap_ext = value;
        }
    }

    fclose(file);
}
