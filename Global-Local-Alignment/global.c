#include "global.h"

DP_cell** initTable(const char *str1, const char *str2, ScoreConfig scoreConfig) {
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
    for (int i = 1; i < m; i++) {
        table[i][0].Sscore = NEG_INF;
        table[i][0].Dscore = i * scoreConfig.g + scoreConfig.h;
        table[i][0].Iscore = NEG_INF;
    }

    // init s2 compared to null string: F(0, j)
    for (int j = 1; j < n; j++) {
        table[0][j].Sscore = NEG_INF;
        table[0][j].Dscore = NEG_INF;
        table[0][j].Iscore = j * scoreConfig.g + scoreConfig.h;
    }

    // fill the rest of the table
    for (int i = 1; i < m; i++) {
        for (int j = 1; j < n; j++) {
            DP_cell *cell = &table[i][j];
            
            // S(i,j)
            int matchMismatchScore = (str1[i-1] == str2[j-1]) ? scoreConfig.ma : scoreConfig.mi; // match or mismatch
            cell->Sscore = fmax(
                fmax(table[i-1][j-1].Sscore + matchMismatchScore, table[i-1][j-1].Dscore + matchMismatchScore),
                table[i-1][j-1].Iscore + matchMismatchScore
            );

            // D(i,j)
            cell->Dscore = fmax(
                fmax(table[i-1][j].Iscore + scoreConfig.h + scoreConfig.g, table[i-1][j].Sscore + scoreConfig.h + scoreConfig.g),
                table[i-1][j].Dscore + scoreConfig.g
            );

            // I(i,j)
            cell->Iscore = fmax(
                fmax(table[i][j-1].Iscore + scoreConfig.g, table[i][j-1].Iscore + scoreConfig.h + scoreConfig.g),
                table[i][j-1].Dscore + scoreConfig.h + scoreConfig.g
            );
        }
    }

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
            printf("(S: %d, D: %d, I: %d)", table[i][j].Sscore, table[i][j].Dscore, table[i][j].Iscore);
            count++;
        }
        printf("\n");
    }
}


