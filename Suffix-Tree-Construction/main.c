#include "input_parser.h"
#include "suffix_tree.h"

#define NUM_SEQ_STRINGS ((size_t)1)

int main(int argc, char* argv[]) {
    // <executable> <input file containing sequence s> <input alphabet file>
    // if (argc < 4) {
    //     print_usage();
    //     return 1;
    // }

    // get sequence file
    char *sequence_file = (argc > 1) ? argv[1] : "s1.fas";
    printf("Sequence File: %s\n", sequence_file);
    Sequence* sequence = read_string_sequence(sequence_file, NUM_SEQ_STRINGS);
    const char* seq_str = sequence[0].sequence;
    printf("Sequence String: %s\n", seq_str);

    // get alphabet file
    char *alphabet_file = (argc > 2) ? argv[2] : "English_alphabet.txt";
    printf("Alphabet File: %s\n", alphabet_file);
    const char* alphabet = read_alphabet(alphabet_file);
    puts(alphabet);
    printf("**************************************************\n");
    
    Node* root = build_suffix_tree(seq_str, alphabet);
    print_tree(root, seq_str, alphabet);

    return 0;
}