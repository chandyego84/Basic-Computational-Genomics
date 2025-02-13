#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "types.h"

#define NEG_INF -1000
#define NUM_SEQ_PAIRWISE ((size_t)2)

// initialize DP table
DP_cell** initTable(const char *str1, const char *str2, ScoreConfig scoreConfig);

// fill in values of the cells of the global table
void fillTable(DP_cell **table, const char *str1, const char *str2, ScoreConfig scoreConfig, bool isLocalAlignment);

// traceback algo
TraceBackStats traceback(DP_cell **table, Sequence* sequences, ScoreConfig scoreConfig, bool isLocalAlignment);

// run global alignment algorithm and return aligned sequences
void runAlignment(Sequence* sequences, ScoreConfig scoreConfig, bool isLocalAlignment);

// get the max i,j cell position from the table
Position getMaxPositionFromTable(DP_cell **table, size_t m, size_t n);

// get the max score value from a cell
int getMaxScoreFromCell(DP_cell cell);

// get the max case from a cell(S, D, I)
CaseType getMaxCaseFromCell(DP_cell cell);

// free sequences memory
void free_sequences(Sequence *sequences, size_t num_seq);

// free table memory
void freeTable(DP_cell **table, size_t m);

// print table
void printTable(DP_cell **table, int m, int n);

// print the alignment results
void printAlignmentResults(Sequence* sequences, TraceBackStats tracebackStats, ScoreConfig scoreConfig, bool isLocalAlignment);

#endif
