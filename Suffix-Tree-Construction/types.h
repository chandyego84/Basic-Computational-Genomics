#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

typedef enum bool {
    false,
    true
} bool;

// Struct to hold a sequence name and corresponding string
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
    int length;
    int* positions;
    int count;
} LongestRepeat;


#endif 