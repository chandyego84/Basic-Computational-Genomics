#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"

// initialize DP table
DP_cell** initTable(const char *str1, const char *str2);

// free table
void freeTable(DP_cell **table, int m);

// print table
void printTable(DP_cell **table, int m, int n);

#endif
