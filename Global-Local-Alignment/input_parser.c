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
