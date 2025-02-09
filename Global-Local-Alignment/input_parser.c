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

Sequence* read_sequence_inputs(const char *filename, const size_t num_seq) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(1);
    }

    Sequence *sequences = (Sequence*)malloc(num_seq * sizeof(Sequence)); // array of sequence strings
    size_t *allocated_sizes = (size_t*)malloc(num_seq * sizeof(size_t)); // track allocated memory for each sequence

    if (!sequences || !allocated_sizes) {
        perror("Memory allocation failed");
        fclose(file);
        exit(1);
    }

    // init sequences
    for (size_t i = 0; i < num_seq; i++) {
        size_t initial_seq_size = INITIAL_MAX_SEQ_LEN * sizeof(char); // in BYTES

        sequences[i].name = NULL;
        sequences[i].sequence = (char*)malloc(initial_seq_size);
        allocated_sizes[i] = initial_seq_size;

        if (!sequences[i].sequence) {
            perror("Failed to allocate sequence memory");
            fclose(file);
            exit(1);
        }

        sequences[i].sequence[0] = '\0';
    }

    int curr_seq = -1; // current sequence being processed
    char line[256]; // buffer to hold current line being read

    while (fgets(line, sizeof(line), file)) {
        if (line[0] == '>') {
            curr_seq++;
            if (curr_seq >= num_seq) break; // ignore extra sequences, if any

            // extract and store the sequence name
            char *newline = strchr(line, '\n');
            if (newline) *newline = '\0'; // remove newline with null terminator

            sequences[curr_seq].name = strdup(line + 1); // skip '>' part of the header -- allocate memory for name
            if (!sequences[curr_seq].name) {
                perror("Failed to allocate name memory");
                fclose(file);
                exit(1);
            }
        } 
        else if (curr_seq >= 0 && curr_seq < num_seq) {
            // process sequence data lines
            size_t line_len = strlen(line);
            if (line_len > 0 && line[line_len - 1] == '\n') {
                line[line_len - 1] = '\0'; // remove newline
            }

            size_t curr_len = strlen(sequences[curr_seq].sequence);
            size_t curr_size = allocated_sizes[curr_seq];
            size_t needed_size = (curr_len + line_len) * sizeof(char);

            // realloc if buffer is too small
            if (curr_size < needed_size) {
                size_t new_size = needed_size * 2;
                char *temp = realloc(sequences[curr_seq].sequence, new_size);
                if (!temp) {
                    perror("Failed to realloc sequence");
                    fclose(file);
                    exit(1);
                }
                sequences[curr_seq].sequence = temp;
                allocated_sizes[curr_seq] = new_size;
            }

            strcat(sequences[curr_seq].sequence, line);
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
