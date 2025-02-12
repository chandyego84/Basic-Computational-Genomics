#include <stdio.h>
#include "input_parser.h"
#include "alignment.h"

#define DEFAULT_CONFIG_FILE "parameters.config"

int main(int argc, char* argv[]) {
    // <executable> <input_sequence_file> <0: global, 1: local> <optional: path_to_parameters_config>
    // if (argc < 3) {
      //  print_usage();
        //return 1;
    //}

    char *input_file = (argv[1] == NULL) ? "test2.fasta" : argv[1];
    // int alignment_type = (argv[2] == NULL ) ? 0 : parse_alignment_type(argv[2]);
    int alignment_type = 0;
    char *config_file = (argc > 3) ? argv[3] : DEFAULT_CONFIG_FILE;
    ScoreConfig scoreConfig;

    read_configs(config_file, &scoreConfig);

    Sequence *sequences = read_sequence_inputs(input_file, NUM_SEQ_PAIRWISE);
    if (!sequences) {
        fprintf(stderr, "Failed to read sequences from file.\n");
        return 1;
    }

    printf("input file: %s\n", input_file);
    printf("alignment type: %d\n", alignment_type);
    printf("ma: %d, mi: %d, gapOpen: %d, gapExtension: %d\n", scoreConfig.ma, scoreConfig.mi, scoreConfig.h, scoreConfig.g);

    printf("\nSequences:\n");
    for (size_t i = 0; i < NUM_SEQ_PAIRWISE; i++) {
        printf("%s: %s (length = %zu)\n", sequences[i].name, sequences[i].sequence, strlen(sequences[i].sequence));
    }

    printf("\n======GLOBAL ALIGNMENT RESULTS======\n");
    runAlignment(sequences, scoreConfig);
    printf("=====================================\n");

    // free memory
    free_sequences(sequences, NUM_SEQ_PAIRWISE);
    
    return 0;
}