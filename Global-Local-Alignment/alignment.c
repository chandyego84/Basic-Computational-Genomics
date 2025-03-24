#include "alignment.h"

DP_cell** initTable(const char *str1, const char *str2, ScoreConfig scoreConfig) {
    int m_rows = strlen(str1) + 1; // +1: null 0,0 cell
    int n_cols = strlen(str2) + 1; // +1: null 0,0 cell

    // allocate memory for rows
    DP_cell **table = (DP_cell**)malloc(m_rows * sizeof(DP_cell*));
    if (!table) {
        perror("Failed to allocate memory for the table");
        exit(1);
    }

    // allocate memory for each column in the row
    for (int i = 0; i < m_rows; i++) {
        table[i] = (DP_cell*)malloc(n_cols * sizeof(DP_cell)); // segmentation fault
        if (!table[i]) {
            perror("Failed to allocate memory for columns in a row of the table");
            exit(1);
        }
    }

    return table;
}

void fillTable(DP_cell **table, const char *str1, const char *str2, ScoreConfig scoreConfig, bool isLocalAlignment) {
    size_t m_rows = strlen(str1) + 1; // +1: null 0,0 cell
    size_t n_cols = strlen(str2) + 1; // +1: null 0,0 cell

    // Initialize null cell
    table[0][0].Sscore = 0;
    table[0][0].Dscore = 0;
    table[0][0].Iscore = 0;

    // Initialize first column (s1 compared to null string)
    for (int i = 1; i < m_rows; i++) {
        table[i][0].Sscore = isLocalAlignment ? 0 : NEG_INF;
        table[i][0].Dscore = i * scoreConfig.g + scoreConfig.h;
        table[i][0].Iscore = isLocalAlignment ? 0 : NEG_INF;
    }

    // Initialize first row (s2 compared to null string)
    for (int j = 1; j < n_cols; j++) {
        table[0][j].Sscore = isLocalAlignment ? 0 : NEG_INF;
        table[0][j].Dscore = isLocalAlignment ? 0 : NEG_INF;
        table[0][j].Iscore = j * scoreConfig.g + scoreConfig.h;
    }

    // Fill the rest of the table
    for (int i = 1; i < m_rows; i++) {
        for (int j = 1; j < n_cols; j++) {
            DP_cell *cell = &table[i][j];

            // S(i,j): match/mismatch
            int matchMismatchScore = (str1[i - 1] == str2[j - 1]) ? scoreConfig.ma : scoreConfig.mi;
            cell->Sscore = fmax(
                fmax(table[i - 1][j - 1].Sscore + matchMismatchScore, table[i - 1][j - 1].Dscore + matchMismatchScore),
                table[i - 1][j - 1].Iscore + matchMismatchScore
            );

            // D(i,j): gap in string 2
            cell->Dscore = 
                fmax(table[i - 1][j].Sscore + scoreConfig.h + scoreConfig.g, // gap open
                table[i - 1][j].Dscore + scoreConfig.g);  // gap extension

            // I(i,j): gap in string 1
            cell->Iscore = 
                fmax(table[i][j - 1].Sscore + scoreConfig.h + scoreConfig.g, // gap open
                table[i][j - 1].Iscore + scoreConfig.g); // gap extension

            // condition for resetting scores to 0 for local alignment
            if (isLocalAlignment) {
                cell->Sscore = fmax(0, cell->Sscore);
                cell->Dscore = fmax(0, cell->Dscore);
                cell->Iscore = fmax(0, cell->Iscore);
            }
        }
    }
}

TraceBackStats traceback(DP_cell **table, Sequence* sequences, ScoreConfig scoreConfig, bool isLocalAlignment) {
    const char *seq1 = sequences[0].sequence;
    const char *seq2 = sequences[1].sequence;

    int m = strlen(seq1);
    int n = strlen(seq2);
    int maxAlignedSize = m + n + 1; // with null

    // Allocate memory for aligned strings
    char *alignedStr1 = (char *)malloc(maxAlignedSize * sizeof(char));
    char *alignedStr2 = (char *)malloc(maxAlignedSize * sizeof(char));
    Sequence *aligned_sequences = (Sequence*)malloc(NUM_SEQ_PAIRWISE * sizeof(Sequence));

    if (!alignedStr1 || !alignedStr2 || !aligned_sequences) {
        perror("Memory allocation for aligned strings failed");
        exit(1);
    }

    // Initialize aligned string sequences
    aligned_sequences[0].name = strdup(sequences[0].name);
    aligned_sequences[1].name = strdup(sequences[1].name);
    aligned_sequences[0].sequence = alignedStr1;
    aligned_sequences[1].sequence = alignedStr2;

    TraceBackStats tracebackStats = {aligned_sequences, 0, 0, 0, 0};

    // Start traceback from bottom-right corner (m, n)
    size_t i = m; // current row position
    size_t j = n; // current column position

    // for local alignment, start at the cell with the maximum score
    if (isLocalAlignment) {
        Position max_pos = getMaxPositionFromTable(table, m + 1, n + 1); // +1 for 0-based row/col indexing of table (size of table is [m+1][n+1])
        i = max_pos.row;
        j = max_pos.col;
    }

    DP_cell curr_cell = table[i][j];
    DP_cell prev_cell;

    // Last cell of the path is the max cell
    int curr_cell_score = getMaxScoreFromCell(curr_cell);
    tracebackStats.optimal_score = curr_cell_score;

    // Initiate case which got to max cell
    CaseType next_case = getMaxCaseFromCell(curr_cell);

    size_t index = 0; // index for aligned strings

    // traceback loop
    while ((i != 0 || j != 0)) {
        // for local alignment, stop if the score drops to 0
        if (isLocalAlignment && curr_cell_score == 0) {
            break;
        }

        if (next_case == S_CASE) {
            alignedStr1[index] = seq1[i - 1];
            alignedStr2[index] = seq2[j - 1];
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
            } else if (prevD_calculation == curr_cell.Sscore) {
                next_case = D_CASE;
            } else if (prevI_calculation == curr_cell.Sscore) {
                next_case = I_CASE;
            }
        } 
        else if (next_case == D_CASE) {
            alignedStr1[index] = seq1[i - 1];
            alignedStr2[index] = '-';
            i--;

            // get case that contributed
            prev_cell = table[i][j];
            int prevD_calculation = prev_cell.Dscore + scoreConfig.g;
            int prevS_calculation = prev_cell.Sscore + scoreConfig.h + scoreConfig.g;

            if (prevD_calculation == curr_cell.Dscore) {
                next_case = D_CASE;
                tracebackStats.g++;
            } else if (prevS_calculation == curr_cell.Dscore) {
                next_case = S_CASE;
                tracebackStats.h++;
                tracebackStats.g++;
            }
        } 
        else if (next_case == I_CASE) {
            alignedStr1[index] = '-';
            alignedStr2[index] = seq2[j - 1];
            j--;

            // get case that contributed
            prev_cell = table[i][j];
            int prevI_calculation = prev_cell.Iscore + scoreConfig.g;
            int prevS_calculation = prev_cell.Sscore + scoreConfig.h + scoreConfig.g;

            if (prevI_calculation == curr_cell.Iscore) {
                next_case = I_CASE;
                tracebackStats.g++;
            } else if (prevS_calculation == curr_cell.Iscore) {
                next_case = S_CASE;
                tracebackStats.h++;
                tracebackStats.g++;
            }
        }

        curr_cell = prev_cell;
        curr_cell_score = getMaxScoreFromCell(curr_cell);
        index++;
    }

    // end of traceback
    alignedStr1[index] = '\0';
    alignedStr2[index] = '\0';

    // reverse aligned strings
    strrev(alignedStr1);
    strrev(alignedStr2);

    return tracebackStats;
}

void runAlignment(Sequence* sequences, ScoreConfig scoreConfig, bool isLocalAlignment){
    const char *seq1 = sequences[0].sequence;
    const char *seq2 = sequences[1].sequence;
    int m_rows = strlen(seq1) + 1; // num of rows

    DP_cell** table = initTable(seq1, seq2, scoreConfig);

    fillTable(table, seq1, seq2, scoreConfig, isLocalAlignment);
    // printTable(table, 20, 20);
    TraceBackStats tracebackStats = traceback(table, sequences, scoreConfig, isLocalAlignment);
    Sequence *alignedSequences = tracebackStats.aligned_Sequences;

    printAlignmentResults(sequences, tracebackStats, scoreConfig, isLocalAlignment);
    
    freeTable(table, m_rows);
    free_sequences(alignedSequences, NUM_SEQ_PAIRWISE);

    return;
}

Position getMaxPositionFromTable(DP_cell **table, size_t m, size_t n) {
    Position max_pos = {0, 0};
    int max_score = table[0][0].Sscore;

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            int cellMaxScore = getMaxScoreFromCell(table[i][j]);
            if (cellMaxScore > max_score) {
                max_score = cellMaxScore;
                max_pos.row = i;
                max_pos.col = j;
            }
        }
    }

    return max_pos;
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

void free_sequences(Sequence *sequences, size_t num_seq) {
    if (!sequences) return;

    for (size_t i = 0; i < num_seq; i++) {
            free(sequences[i].name);      
            free(sequences[i].sequence);
    }

    free(sequences);  
}


void freeTable(DP_cell **table, size_t m) {
    if (!table) return;

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

void printAlignmentResults(Sequence* sequences, TraceBackStats tracebackStats, ScoreConfig scoreConfig, bool isLocalAlignment) {
    if (isLocalAlignment) {
        printf("\n======LOCAL ALIGNMENT RESULTS======\n");

    }
    else {
        printf("\n======GLOBAL ALIGNMENT RESULTS======\n");
    }

    // param values
    printf("Parameter values:\n");
    printf("  Match score: %d\n", scoreConfig.ma);
    printf("  Mismatch penalty: %d\n", scoreConfig.mi);
    printf("  Gap open penalty (h): %d\n", scoreConfig.h);
    printf("  Gap extension penalty (g): %d\n", scoreConfig.g);

    // sequence names
    printf("\nSequences aligned:\n");
    printf("  Sequence 1: %s, length = %zu\n", sequences[0].name, strlen(sequences[0].sequence));
    printf("  Sequence 2: %s, length = %zu\n", sequences[1].name, strlen(sequences[1].sequence));

    // final optimal score
    printf("\nOptimal score: %d\n", tracebackStats.optimal_score);

    // alignment
    printf("\nAlignment:\n");
    const char *alignedSeq1 = tracebackStats.aligned_Sequences[0].sequence;
    const char *alignedSeq2 = tracebackStats.aligned_Sequences[1].sequence;
    size_t alignmentLength = strlen(alignedSeq1);

    // wrap alignment to 60 characters per line
    size_t lineLength = 60;
    for (size_t i = 0; i < alignmentLength; i += lineLength) {
        // sequence 1
        for (size_t j = i; j < i + lineLength && j < alignmentLength; j++) {
            printf("%c", alignedSeq1[j]);
        }
        printf("\n");

        // separator
        for (size_t j = i; j < i + lineLength && j < alignmentLength; j++) {
            if (alignedSeq1[j] == alignedSeq2[j]) {
                printf("|"); // match
            } else if (alignedSeq1[j] == '-' || alignedSeq2[j] == '-') {
                printf(" "); // gap
            } else {
                printf("."); // mismatch
            }
        }        
        printf("\n");

        // sequence 2
        for (size_t j = i; j < i + lineLength && j < alignmentLength; j++) {
            printf("%c", alignedSeq2[j]);
        }

        printf("\n\n");
    }

    // alignment stats
    printf("\nAlignment statistics:\n");
    printf("  Alignment length: %zu\n", alignmentLength);
    printf("  Matches: %zu\n", tracebackStats.ma);
    printf("  Mismatches: %zu\n", tracebackStats.mi);
    printf("  Gap opens: %zu\n", tracebackStats.h);
    printf("  Gap extensions: %zu\n", tracebackStats.g);
    printf("  Total gaps: %zu\n", tracebackStats.h + tracebackStats.g);
    printf("  Percent identity: %.2f%%\n", (tracebackStats.ma / (double)alignmentLength) * 100);

    printf("=====================================\n");
}


