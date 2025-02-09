#ifndef INPUT_PARSER_H
#define INPUT_PARSER_H

#include <stdio.h>
#include <stdlib.h>

#define INITIAL_MAX_SEQ_SIZE 100

// Prints command prompt guide for inputting params and configs.
void print_usage();

// Reads type of alignment from command prompt input - 0: global, 1: local
int parse_alignment_type(const char *arg);

// Read the input sequences from a file
/**
 * The format allows the file to contain any number of sequences, although in this program project you will have only two sequences as input.
 * Each sequence will first start with a HEADER line, which has the sequence name in it. 
 * This header line will always starts with the ">" symbol and is immediately followed (without any whitespace character) by a word that will serve as the unique identifier (or name) for that sequence. Whatever follows the first whitespace character after the identifier is a don't care and can be ignored in your program. 
 * The header line is followed by the actual DNA sequence which is a string over the alphabet {a,c,g,t}. 
 * The sequence can span multiple lines and each line can variable number of characters (but no whitespaces or any other special characters).
 */
char** read_sequence_inputs(const char *filename, const size_t num_seq);

// Read configs for processing alignment
void read_configs(const char *filename, int *match, int *mismatch, int *gap_open, int *gap_ext);

#endif
