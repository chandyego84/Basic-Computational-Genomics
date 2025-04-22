#ifndef SIMILARITY_H
#define SIMILARITY_H

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "types.h"
#include "suffix_tree.h"

#define DNA_ALPHABET_SIZE 4
#define MATCH_SCORE 1
#define MISMATCH_PENALTY -2
#define GAP_OPEN -5
#define GAP_EXTEND -1

// FOR COLORS ON STRING OUTPUTS :)
#define RESET   "\x1b[0m"
#define BLUE    "\x1b[34m"
#define GREEN   "\x1b[32m"


extern double total_suffix_tree_time;
extern double total_alignment_time;

SimilarityCell** compute_similarity_matrix(Sequence *sequences, int count, const char *alphabet);

SimilarityCell compute_pair_similarity(const char *s1, const char *s2, const char *alphabet);

SubtreeColor dfs_lcs(Node *node, int len1, int len2, LongestRepeat *result);

LongestRepeat find_longest_common_substring(Node *root, const char *s1, const char *s2, const char *alphabet);

AlignmentResult global_align(const char *s1, const char *s2);

void free_alignment_result(AlignmentResult result);

void free_similarity_matrix(SimilarityCell **matrix, int size);

void free_suffix_tree(Node *node, const char *alphabet);

#endif