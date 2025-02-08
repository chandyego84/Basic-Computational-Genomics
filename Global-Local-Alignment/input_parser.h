#ifndef INPUT_PARSER_H
#define INPUT_PARSER_H

#include <stdio.h>


// Prints command prompt guide for inputting params and configs.
void print_usage();

// Reads type of alignment from command prompt input - 0: global, 1: local
int parse_alignment_type(const char *arg);

// Read configs for processing alignment
void read_configs(const char *filename, int *match, int *mismatch, int *gap_open, int *gap_ext);

#endif
