#include "alignment.h"

DP_cell** initTable(const char *str1, const char *str2, ScoreConfig scoreConfig) {
    int m = strlen(str1) + 1; // +1: null 0,0 cell
    int n = strlen(str2) + 1; // +1: null 0,0 cell

    printf("There are %d elements in the table\n", m*n);

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

    return table;
}

void fillGlobalTable(DP_cell **table, const char *str1, const char *str2, ScoreConfig scoreConfig) {
    int m = strlen(str1) + 1; // +1: null 0,0 cell
    int n = strlen(str2) + 1; // +1: null 0,0 cell
    
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
}

void performTraceback(DP_cell **table, const char *seq1, const char *seq2, ScoreConfig scoreConfig) {
    int m = strlen(seq1);
    int n = strlen(seq2);
    int maxAlignedSize = m + n + 1; // with null

    // allocate memory for aligned strings
    char *alignedStr1 = (char *)malloc(maxAlignedSize * sizeof(char));
    char *alignedStr2 = (char *)malloc(maxAlignedSize * sizeof(char));

    if (!alignedStr1 || !alignedStr2) {
        perror("Memory allocation for aligned strings failed");
        exit(1);
    }

    // traceback starting from last call (m,n)
    traceback(table, seq1, seq2, m, n, scoreConfig, alignedStr1, alignedStr2, 0);

    // Free memory
    free(alignedStr1);
    free(alignedStr2);
}

void traceback(DP_cell **table, const char *str1, const char *str2, int i, int j, ScoreConfig scoreConfig, char *alignedStr1, char *alignedStr2, int index) {
    if (i == 0 && j == 0) {
        // at starting cell

    }

    // get the max case (S, D, I)
    DP_cell curr_cell = table[i][j];
    CaseType max_Case = getMaxCaseFromCell(curr_cell);
    
}

int getMaxScoreFromCell(DP_cell cell) {
    return fmax(fmax(cell.Sscore, cell.Dscore), cell.Iscore);
}

// get the max case from a cell (S, D, I)
CaseType getMaxCaseFromCell(DP_cell cell) {
    // set substitution as initial max value
    int max_value = cell.Sscore;
    CaseType case_index = S_CASE;

    if (cell.Dscore > max_value) {
        max_value = cell.Dscore;
        case_index = D_CASE;
    }

    if (cell.Iscore > max_value) {
        max_value = cell.Iscore;
        case_index = I_CASE;
    }

    return case_index;
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


