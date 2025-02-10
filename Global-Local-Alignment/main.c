#include <stdio.h>
#include "input_parser.h"
#include "global.h"

#define DEFAULT_CONFIG_FILE "parameters.config"
#define MAX_SEQUENCES 2

int main(int argc, char* argv[]) {
    // <executable> <input_sequence_file> <0: global, 1: local> <optional: path_to_parameters_config>
    // if (argc < 3) {
      //  print_usage();
        //return 1;
    //}

    char *input_file = (argv[1] == NULL) ? "test.fasta" : argv[1];
    // int alignment_type = (argv[2] == NULL ) ? 0 : parse_alignment_type(argv[2]);
    int alignment_type = 0;
    char *config_file = (argc > 3) ? argv[3] : DEFAULT_CONFIG_FILE;
    ScoreConfig scoreConfig;

    read_configs(config_file, &scoreConfig);

    Sequence *sequences = read_sequence_inputs(input_file, MAX_SEQUENCES);
    if (!sequences) {
        fprintf(stderr, "Failed to read sequences from file.\n");
        return 1;
    }

    printf("input file: %s\n", input_file);
    printf("alignment type: %d\n", alignment_type);
    printf("ma: %d, mi: %d, gapOpen: %d, gapExtension: %d\n", scoreConfig.ma, scoreConfig.mi, scoreConfig.h, scoreConfig.g);

    printf("\nSequences:\n");
    for (size_t i = 0; i < MAX_SEQUENCES; i++) {
        printf("%s: %s\n", sequences[i].name, sequences[i].sequence);
    }

    const char* seq1 = sequences[0].sequence;
    const char* seq2 = sequences[1].sequence;

    printf("\nGlobal Alignment Results!\n");
    DP_cell** table = initTable(seq1, seq2, scoreConfig);

    // free memory
    for (size_t i = 0; i < MAX_SEQUENCES; i++) {
        free(sequences[i].name);
        free(sequences[i].sequence);
    }
    free(sequences);
    freeTable(table, strlen(seq1));
    
    return 0;
}