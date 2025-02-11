#include "alignment.h"

DP_cell** initTable(const char *str1, const char *str2, ScoreConfig scoreConfig) {
    int m = strlen(str1) + 1; // +1: null 0,0 cell
    int n = strlen(str2) + 1; // +1: null 0,0 cell

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
    traceback(table, seq1, seq2, m, n, scoreConfig, alignedStr1, alignedStr2);
    DP_cell optimal_cell = table[m][n];

    printf("Global Alignment Score: %d\n", getMaxScoreFromCell(optimal_cell));

    alignedStr1 = strrev(alignedStr1);
    alignedStr2 = strrev(alignedStr2);

    printf("Aligned S1: %s\n", alignedStr1);
    printf("Aligned S2: %s\n", alignedStr2);

    // Free memory
    free(alignedStr1);
    free(alignedStr2);
}

void traceback(DP_cell **table, const char *seq1, const char *seq2, int m, int n, ScoreConfig scoreConfig, char *alignedStr1, char *alignedStr2) {
    // start traceback from m,n cell
    DP_cell curr_cell = table[m][n];
    DP_cell prev_cell;
    CaseType next_case = getMaxCaseFromCell(curr_cell);

    size_t i = m; // current row position
    size_t j = n; // current column position
    size_t index = 0; // index for aligned strings

    Report tracebackStats = {0, 0, 0, 0};

    while (i != 0 && j != 0) {
        // not at the null cell in top-left corner
        if (next_case == S_CASE) {
            alignedStr1[index] = seq1[i-1];
            alignedStr2[index] = seq2[j-1];
            i--;
            j--;

            prev_cell = table[i][j];
            int matchMismatchScore = (seq1[i] == seq2[j]) ? scoreConfig.ma : scoreConfig.mi;
            int prevS_calculation = prev_cell.Sscore + matchMismatchScore;
            int prevD_calculation = prev_cell.Dscore + matchMismatchScore;
            int prevI_calculation = prev_cell.Iscore + matchMismatchScore;

            if (seq1[i] == seq2[j]) {
                tracebackStats.ma++; 
            } else {
                tracebackStats.mi++;
            }

            if (prevS_calculation == curr_cell.Sscore) {
                next_case = S_CASE;
            }
            else if (prevD_calculation == curr_cell.Sscore) {
                next_case = D_CASE;
                tracebackStats.g++;
            }
            else if (prevI_calculation == curr_cell.Sscore) {
                next_case = I_CASE;
                tracebackStats.g++;
            }
        }

        if (next_case == D_CASE) {
            alignedStr1[index] = seq1[i-1];
            alignedStr2[index] = '-';
            i--;

            // get case that contributed
            prev_cell = table[i][j];
            int prevD_calculation = prev_cell.Dscore + scoreConfig.g;
            int prevS_calculation = prev_cell.Sscore + scoreConfig.h + scoreConfig.g;
            int prevI_calculation = prev_cell.Iscore + scoreConfig.h + scoreConfig.g;

            if (prevD_calculation == curr_cell.Dscore) {
                next_case = D_CASE;
                tracebackStats.g++;
            }
            else if (prevS_calculation == curr_cell.Dscore) {
                next_case = S_CASE;
                tracebackStats.h++;
                tracebackStats.g++;
            }
            else if (prevI_calculation == curr_cell.Dscore) {
                next_case = I_CASE;
                tracebackStats.h++;
                tracebackStats.g++;
            }

        }

        if (next_case == I_CASE) {
            alignedStr1[index] = '-';
            alignedStr2[index] = seq2[j-1];
            j--;

            // get case that contributed
            prev_cell = table[i][j];
            int prevI_calculation = prev_cell.Iscore + scoreConfig.g;
            int prevS_calculation = prev_cell.Sscore + scoreConfig.h + scoreConfig.g;
            int prevD_calculation = prev_cell.Dscore + +scoreConfig.h + scoreConfig.g;   
            
            if (prevI_calculation == curr_cell.Iscore) {
                next_case = I_CASE;
                tracebackStats.g++;
            }
            else if (prevS_calculation == curr_cell.Sscore) {
                next_case = S_CASE;
                tracebackStats.h++;
                tracebackStats.g++;
            }
            else if (prevD_calculation == curr_cell.Dscore) {
                next_case = D_CASE;
                tracebackStats.h++;
                tracebackStats.g++;
            }
        }

        curr_cell = prev_cell;
        index++;
    }

    // at end of traceback
    alignedStr1[index] = '\0';
    alignedStr2[index] = '\0';

    // Print or return the traceback stats
    printf("Matches: %zu\n", tracebackStats.ma);
    printf("Mismatches: %zu\n", tracebackStats.mi);
    printf("Gap opens: %zu\n", tracebackStats.h);
    printf("Gap extensions: %zu\n", tracebackStats.g);
    

    return;
}

int getMaxScoreFromCell(DP_cell cell) {
    return fmax(fmax(cell.Sscore, cell.Dscore), cell.Iscore);
}

// get the max case from a cell (S, D, I)
CaseType getMaxCaseFromCell(DP_cell cell) {
    // init substitution as initial max value
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


