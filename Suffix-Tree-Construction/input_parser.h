#ifndef INPUT_PARSER_H
#define INPUT_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "types.h"

#define INITIAL_MAX_SEQ_LEN 10
#define MAX_ALPHABET_SIZE 256

// Prints command prompt guide for inputting params
void print_usage();

// Read the input sequences from a file
/**
 * Each sequence will first start with a HEADER line, which has the sequence name in it. 
 * This header line will always starts with the ">" symbol and is immediately followed (without any whitespace character) by a word that will serve as the unique identifier (or name) for that sequence. 
 * Whatever follows the first whitespace character after the identifier is a don't care and can be ignored in your program. 
 * The header line is followed by the actual string sequence.
 * The sequence can span multiple lines and each line can variable number of characters (but no whitespaces or any other special characters).
 */
Sequence* read_string_sequence(const char *filename, const size_t num_seq);

// Get alphabet from a file
/*
* Returns character array of ALPHANUMERIC alphabet in sorted order.
*/
char* read_alphabet(const char* filename);

#endif
