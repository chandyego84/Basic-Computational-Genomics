#ifndef INPUT_PARSER_H
#define INPUT_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "types.h"

#define MAX_ALPHABET_SIZE 256
#define INITIAL_MAX_SEQ_LEN 1000000

// Prints command prompt guide for inputting params and configs.
void print_usage();

// Reads a single string sequence
Sequence* read_single_sequence(const char *filename);

char* read_alphabet(const char* filename);

#endif