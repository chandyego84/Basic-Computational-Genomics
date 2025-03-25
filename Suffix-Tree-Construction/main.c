#include <stdio.h>
#include "input_parser.h"

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
    printf("Sequence String: %s\n", sequence[0].sequence);

    // get alphabet file
    char *alphabet_file = (argc > 2) ? argv[2] : "English_alphabet.txt";
    printf("Alphabet File: %s\n", alphabet_file);
    char* alphabet = read_alphabet(alphabet_file);
    puts(alphabet);

    
    
    return 0;
}