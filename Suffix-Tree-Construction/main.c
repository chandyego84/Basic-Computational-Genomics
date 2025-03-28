#include "input_parser.h"
#include "suffix_tree.h"
#include <time.h>

#define NUM_SEQ_STRINGS ((size_t)1)

int main(int argc, char* argv[]) {
    // <executable> <input file containing sequence s> <input alphabet file>
    // if (argc < 4) {
    //     print_usage();
    //     return 1;
    // }

    // get sequence file
    char *sequence_file = (argc > 1) ? argv[1] : "Slyco.fas";
    printf("Sequence File: %s\n", sequence_file);
    Sequence* sequence = read_string_sequence(sequence_file, NUM_SEQ_STRINGS);
    const char* seq_name = sequence[0].name;
    const char* seq_str = sequence[0].sequence;
    printf("Sequence Name: %s\n", seq_name);

    // get alphabet file
    char *alphabet_file = (argc > 2) ? argv[2] : "DNA_alphabet.txt";
    printf("Alphabet File: %s\n", alphabet_file);
    const char* alphabet = read_alphabet(alphabet_file);
    puts(alphabet);
    printf("**************************************************\n");
    
    clock_t start = clock();
    Node* root = build_suffix_tree(seq_str, alphabet, false);
    clock_t end = clock();
    double construction_time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Suffix Tree Construction Time: %.4f seconds\n", construction_time);
    printf("**************************************************\n");

    report_space_usage(root, seq_str, alphabet);
    printf("**************************************************\n");

    //dfs_enumerate(root, seq_str, alphabet);
    compute_bwt_index(root, sequence_file, seq_str, alphabet);
    printf("**************************************************\n");

    // Find longest repeats
    LongestRepeat repeats = find_repeats(root, seq_str, alphabet);
    print_repeats(&repeats, seq_str);
    
    // Clean up
    free(repeats.positions);    

    return 0;
}