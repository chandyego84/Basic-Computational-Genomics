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

typedef struct scoreConfig {
    int ma; // match
    int mi; // mismatch
    int h; // gap open
    int g; // gap extension
} ScoreConfig;

#endif