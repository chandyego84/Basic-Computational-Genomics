#include "global.h"

DP_cell** initTable(const char *str1, const char *str2) {
    int m = strlen(str1) + 1;
    int n = strlen(str2) + 1;

    printf("There are %d elements in the table (%d * %d)\n", m*n, m, n);

    // allocate memory for rows
    DP_cell **table = (DP_cell**)malloc(m * sizeof(DP_cell*));
    if (!table) {
        perror("Failed to allocate memory for the table");
        exit(1);
    }

    // allocate memory for each column in the row
    for (int i = 0; i < m; i++) {
        table[i] = (DP_cell*)malloc(n * sizeof(DP_cell)); // segmentation fault
        if (!table[i]) {
            perror("Failed to allocate memory for columns in a row of the table");
            exit(1);
        }
    }

    // init null cell
    table[0][0].Sscore = 0;
    table[0][0].Dscore = 0;
    table[0][0].Iscore = 0;

    // init s1 compared to null string: F(i,0)


    // init s2 compared to null string: F(0, j)


    return table;
}

void freeTable(DP_cell **table, int m) {
    for (int i = 0; i < m; i++) {
        free(table[i]);
    }
    free(table);
}

void printTable(DP_cell **table, int m, int n) {
    int count = 0;
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            printf("(%d, %d)", i, j);
            count++;
        }
        printf("\n");
    }
}


