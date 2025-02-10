#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "types.h"

#define NEG_INF -1000;

// initialize DP table
DP_cell** initTable(const char *str1, const char *str2, ScoreConfig scoreConfig);

// fill in values of the cells of the global table
void fillGlobalTable(DP_cell **table, const char *str1, const char *str2, ScoreConfig scoreConfig);

// perform traceback to get aligned strings
void performTraceback(DP_cell **table, const char *seq1, const char *seq2, ScoreConfig scoreConfig);

// recursive traceback algo
void traceback(DP_cell **table, const char *str1, const char *str2, int i, int j, ScoreConfig scoreConfig, char *alignedStr1, char *alignedStr2, int index);

// get the max score value from a cell
int getMaxScoreFromCell(DP_cell cell);

// get the max case from a cell(S, D, I)
CaseType getMaxCaseFromCell(DP_cell cell);

// free table
void freeTable(DP_cell **table, int m);

// print table
void printTable(DP_cell **table, int m, int n);

#endif
