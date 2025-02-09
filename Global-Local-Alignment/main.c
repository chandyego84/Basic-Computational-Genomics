#include <stdio.h>
#include "input_parser.h"

#define DEFAULT_CONFIG_FILE "parameters.config"
#define MAX_SEQUENCES 2

int main(int argc, char* argv[]) {
    // <executable> <input_sequence_file> <0: global, 1: local> <optional: path_to_parameters_config>
   // if (argc < 3) {
      //  print_usage();
        //return 1;
    //}

    char *input_file = (argv[1] == NULL) ? "test.fasta" : argv[1];
//    int alignment_type = (argv[2] == NULL ) ? 0 : parse_alignment_type(argv[2]);
    int alignment_type = 0;
    char *config_file = (argc > 3) ? argv[3] : DEFAULT_CONFIG_FILE;
    int match_val, mismatch_val, gap_open_val, gap_extension_val;

    read_configs(config_file, &match_val, &mismatch_val, &gap_open_val, &gap_extension_val);

    char **sequences = read_sequence_inputs(input_file, MAX_SEQUENCES);
    if (!sequences) {
        fprintf(stderr, "Failed to read sequences from file.\n");
        return 1;
    }

    printf("input file: %s\n", input_file);
    printf("alignment type: %d\n", alignment_type);
    printf("ma: %d, mi: %d, gapOpen: %d, gapExtension: %d\n", match_val, mismatch_val, gap_open_val, gap_extension_val);

    printf("\nSequences:\n");
    for (size_t i = 0; i < MAX_SEQUENCES && sequences[i] != NULL; i++) {
        printf("Sequence %zu: %s\n", i + 1, sequences[i]);
    }

    // free memory
    for (size_t i = 0; i < MAX_SEQUENCES; i++) {
        free(sequences[i]);
    }
    free(sequences);
    
    return 0;
}