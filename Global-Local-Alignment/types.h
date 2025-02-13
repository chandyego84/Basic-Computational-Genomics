#ifndef TYPES_H
#define TYPES_H

// Struct to hold sequence names and data
typedef struct sequence {
    char *name;      // name of the sequence
    char *sequence;  // sequence data
 } Sequence;

 typedef struct position {
    size_t row;
    size_t col;
 } Position;

 // Struct to hold info of a cell in DP table
 typedef struct dp_cell {
    int Sscore; // substitution score
    int Dscore; // deletion score
    int Iscore; // insertion score
} DP_cell;

typedef enum {S_CASE, D_CASE, I_CASE} CaseType;

// Basic traceback stats (count of matches, mismatches, gap opens, gap extensions)
typedef struct tracebackStats {
    Sequence* aligned_Sequences; // list of aligned sequences
    int optimal_score; // optimal alignment score determined at m,n cell
    size_t ma; // # matches
    size_t mi; // # mismatches
    size_t h; // # gap opens
    size_t g; // # gap extensions
} TraceBackStats;

typedef struct scoreConfig {
    int ma; // match
    int mi; // mismatch
    int h; // gap open
    int g; // gap extension
} ScoreConfig;

#endif