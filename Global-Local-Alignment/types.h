#ifndef TYPES_H
#define TYPES_H

// Struct to hold sequence names and data
typedef struct sequence {
    char *name;      // name of the sequence
    char *sequence;  // sequence data
 } Sequence;

 // Struct to hold info of a cell in DP table
 typedef struct dp_cell {
    int Sscore; // substitution score
    int Dscore; // deletion score
    int Iscore; // insertion score
} DP_cell;

typedef enum {S_CASE, D_CASE, I_CASE} CaseType;

// Basic traceback stats (count of matches, mismatches, open gaps, gap extensions)
typedef struct report {
    size_t ma;
    size_t mi;
    size_t h;
    size_t g;
} Report;

typedef struct scoreConfig {
    int ma; // match
    int mi; // mismatch
    int h; // gap open
    int g; // gap extension
} ScoreConfig;

#endif