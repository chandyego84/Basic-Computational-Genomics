#ifndef TYPES_H
#define TYPES_H

typedef enum bool {
    false,
    true
} bool;

// Struct to hold sequence names and data
typedef struct sequence {
    char *name;      // name of the sequence
    char *sequence;  // sequence data
} Sequence;

typedef struct node {
    int id; // 0...n-1 (for leaves, the suffix order), n...x (internal nodes)
    struct node* suff_link; // ptr to suffix link node
    struct node* parent;
    struct node** children; // array of children of size dependent on size of alphabet
    int depth; // length of the string that leads from root to the node
    int edge_label[2]; // [start_index, end_index], i.e., label of incoming edge from parent
} Node;

typedef struct {
    int from_s1;
    int from_s2;
} SubtreeColor;

typedef struct {
    int Sscore; // substitution (match/mismatch)
    int Dscore; // deletion (gap in s2)
    int Iscore; // insertion (gap in s1)
    char from_S;
    char from_D;
    char from_I;
} DP_cell;

typedef struct {
    int score;
    int matches;
    char *align1;
    char *align2;
} AlignmentResult;

typedef struct {
    int length;
    int* positions;
    int count;
} LongestRepeat;

typedef struct {
    int alignment_score; // alignment score between the sequences
    int lcs_length; // length of the longest common substring (LCS)
} SimilarityCell;

 #endif