#include "similarity.h"
#include "suffix_tree.h"
#include <stdlib.h>
#include <string.h>

double total_suffix_tree_time = 0.0;
double total_alignment_time = 0.0;

SimilarityCell** compute_similarity_matrix(Sequence *sequences, int count, const char *alphabet) {
    fprintf(stderr, "DEBUG: Entering compute_similarity_matrix with %d sequences\n", count);
    
    // allocate matrix of simCell structs
    fprintf(stderr, "DEBUG: Allocating similarity matrix (%d x %d)\n", count, count);
    SimilarityCell **D = malloc(count * sizeof(SimilarityCell *));
    if (!D) {
        fprintf(stderr, "ERROR: Failed to allocate matrix rows\n");
        return NULL;
    }
    
    for (int i = 0; i < count; i++) {
        D[i] = malloc(count * sizeof(SimilarityCell));
        if (!D[i]) {
            fprintf(stderr, "ERROR: Failed to allocate matrix column %d\n", i);
            for (int j = 0; j < i; j++) free(D[j]);
            free(D);
            return NULL;
        }
    }

    // compute pairwise similarities
    fprintf(stderr, "DEBUG: Computing pairwise similarities\n");
    for (int i = 0; i < count; i++) {               
        D[i][i] = (SimilarityCell){strlen(sequences[i].sequence) - 1, strlen(sequences[i].sequence) - 1}; // excluding '$'
        fprintf(stderr, "DEBUG: Self-similarity [%d][%d] = %d, LCS length = %d\n", 
                i, i, D[i][i].alignment_score, D[i][i].lcs_length);
        
        for (int j = i+1; j < count; j++) {
            fprintf(stderr, "DEBUG: Comparing sequence %d with %d\n", i, j);
            SimilarityCell result = compute_pair_similarity(sequences[i].sequence, 
                                                            sequences[j].sequence, 
                                                            alphabet);
            fprintf(stderr, "DEBUG: Similarity %d vs %d = %d, LCS length = %d\n", 
                    i, j, result.alignment_score, result.lcs_length);

            D[i][j] = result;
            D[j][i] = result; // symmetrical matrix
        }
    }

    fprintf(stderr, "DEBUG: Similarity matrix computation complete\n");
    return D;
}

SimilarityCell compute_pair_similarity(const char *s1, const char *s2, const char *alphabet) {
    fprintf(stderr, "DEBUG: Computing pair similarity between sequences\n");
    fprintf(stderr, "DEBUG: s1 length: %zu, s2 length: %zu\n", strlen(s1), strlen(s2));

    // build concat string for the suffix tree
    char *concat = (char *)malloc(strlen(s1) + strlen(s2) + 2); // +1 for separator +1 for null terminator
    if (!concat) {
        fprintf(stderr, "ERROR: Failed to allocate concatenated string\n");
        return (SimilarityCell){0, 0};
    }
    strcpy(concat, s1);
    strcat(concat, s2);

    // START TIMER FOR SUFF TREE CONSTRUCTION
    time_t start_time = time(NULL);
    
    Node *root = build_suffix_tree(concat, alphabet, false);
    if (!root) {
        fprintf(stderr, "ERROR: Failed to build suffix tree\n");
        free(concat);
        return (SimilarityCell){0, 0};
    }

    // END TIMER FOR ST CONSTRUCTION
    time_t end_time = time(NULL);
    double suffix_tree_time = difftime(end_time, start_time);
    printf(BLUE "DEBUG: Suffix tree construction took %.2f seconds\n" RESET, suffix_tree_time);

    // add to gloal ST TIME
    total_suffix_tree_time += suffix_tree_time;

    // START TIMER FOR ALIGNMENT
    time_t start_alignment_time = time(NULL);

    // find LCS
    fprintf(stderr, "DEBUG: Finding longest common substring\n");
    LongestRepeat lcs = find_longest_common_substring(root, s1, s2, alphabet);

    if (lcs.length == 0) {
        fprintf(stderr, "DEBUG: No common substring found\n");
        free(concat);
        free_suffix_tree(root, alphabet);
        return (SimilarityCell){0, 0}; // Return default struct if no LCS
    }

    fprintf(stderr, "DEBUG: Found LCS of length %d at positions (%d, %d)\n", 
            lcs.length, lcs.positions[0], lcs.positions[1]);

    // process prefixes (alignment score a)
    int x1 = lcs.positions[0];
    int x2 = lcs.positions[1];
    int y1 = x1 + lcs.length - 1;
    int y2 = x2 + lcs.length - 1;
    int b = lcs.length;

    // process prefixes
    fprintf(stderr, "DEBUG: Processing prefixes (lengths %d and %d)\n", x1, x2);
    char *prefix1 = (char *)malloc(x1 + 1);
    char *prefix2 = (char *)malloc(x2 + 1);
    if (!prefix1 || !prefix2) {
        fprintf(stderr, "ERROR: Failed to allocate prefixes\n");
        free(prefix1);
        free(prefix2);
        free(concat);
        free_suffix_tree(root, alphabet);
        free(lcs.positions);
        return (SimilarityCell){0, 0};
    }
    
    strncpy(prefix1, s1, x1);
    prefix1[x1] = '\0';
    strncpy(prefix2, s2, x2);
    prefix2[x2] = '\0';

    // revere prefixes
    char *prefix1_rev = (char *)malloc(x1 + 1);
    char *prefix2_rev = (char *)malloc(x2 + 1);
    if (!prefix1_rev || !prefix2_rev) {
        fprintf(stderr, "ERROR: Failed to allocate reversed prefixes\n");
        free(prefix1);
        free(prefix2);
        free(prefix1_rev);
        free(prefix2_rev);
        free(concat);
        free_suffix_tree(root, alphabet);
        free(lcs.positions);
        return (SimilarityCell){0, 0};
    }

    for (int i = 0; i < x1; i++) prefix1_rev[i] = prefix1[x1 - 1 - i];
    prefix1_rev[x1] = '\0';
    for (int i = 0; i < x2; i++) prefix2_rev[i] = prefix2[x2 - 1 - i];
    prefix2_rev[x2] = '\0';

    // align prefixes
    AlignmentResult prefix_align = global_align(prefix1_rev, prefix2_rev);
    int a = prefix_align.matches;

    // process suffixes (alignment score c)
    int len1 = strlen(s1);
    int len2 = strlen(s2);
    char *suffix1 = strdup(s1 + y1 + 1);
    char *suffix2 = strdup(s2 + y2 + 1);

    // align suffixes
    AlignmentResult suffix_align = global_align(suffix1, suffix2);
    int c = suffix_align.matches;

    // END TIMER FOR ALIGNMENT        
    time_t end_alignment_time = time(NULL);
    double alignment_time = difftime(end_alignment_time, start_alignment_time);
    printf(BLUE "DEBUG: Alignment computations took %.2f seconds\n" RESET, alignment_time);

    total_alignment_time += alignment_time;    

    // clean up memory
    free(concat);
    free(lcs.positions);
    free(prefix1);
    free(prefix2);
    free(prefix1_rev);
    free(prefix2_rev);
    free(suffix1);
    free(suffix2);
    free_alignment_result(prefix_align);
    free_alignment_result(suffix_align);

    int alignment_score = a + b + c;

    return (SimilarityCell){alignment_score, lcs.length};
}

// Finds the longest common substring between two strings using GST
SubtreeColor dfs_lcs(Node *node, int len1, int len2, LongestRepeat *result) {
    SubtreeColor color = {0, 0};

    // check if the current node is valid
    if (!node) return color;

    int is_leaf_flag = 1;

    // loop through each child of the current node
    for (int i = 0; i < DNA_ALPHABET_SIZE; i++) {
        if (node->children[i]) {
            is_leaf_flag = 0;
            SubtreeColor child_color = dfs_lcs(node->children[i], len1, len2, result);
            color.from_s1 |= child_color.from_s1;
            color.from_s2 |= child_color.from_s2;
        }
    }

    // if this node is a leaf, determine which string it belongs to
    if (is_leaf_flag) {
        if (node->id < len1) {
            color.from_s1 = 1;
        } 
        
        else {
            color.from_s2 = 1;
        }
        return color;
    }

    // if this is an internal node and both strings are represented in the subtree
    if (color.from_s1 && color.from_s2) {
        // check if the current depth is greater than the recorded result length
        if (node->depth > result->length) {
            result->length = node->depth;
            free(result->positions);
            result->positions = NULL;
            result->count = 0;

            // find representative leaf under this node from both strings
            int pos1 = -1, pos2 = -1;
            for (int i = 0; i < DNA_ALPHABET_SIZE && (pos1 == -1 || pos2 == -1); i++) {
                Node *child = node->children[i];
                if (child) {
                    // search to the leaf to find the representative
                    Node *leaf = child;
                    while (!is_leaf(leaf, len1 + len2)) {  // combined length of both strings
                        for (int j = 0; j < DNA_ALPHABET_SIZE; j++) {
                            if (leaf->children[j]) {
                                leaf = leaf->children[j];
                                break;
                            }
                        }
                    }

                    // reaching the leaf, calculate positions based on its ID
                    if (leaf->id < len1 && pos1 == -1) {
                        pos1 = leaf->id;
                    } 
                    
                    else if (leaf->id >= len1 && pos2 == -1) {
                        pos2 = leaf->id - len1;
                    }
                }
            }

            // if both positions are found, update the result
            if (pos1 != -1 && pos2 != -1) {
                result->positions = (int *)malloc(2 * sizeof(int));
                result->positions[0] = pos1;
                result->positions[1] = pos2;
                result->count = 2;
            }
        }
    }

    return color;
}

LongestRepeat find_longest_common_substring(Node *root, const char *s1, const char *s2, const char *alphabet) {
    LongestRepeat result = {0, NULL, 0};
    int len1 = strlen(s1);
    int len2 = strlen(s2); 

    dfs_lcs(root, len1, len2, &result);

    return result;
}

// global alignment with affine gap penalty that returns the maximum score in the matrix
AlignmentResult global_align(const char *s1, const char *s2) {
    int len1 = strlen(s1);
    int len2 = strlen(s2);

    if (len1 == 0 || len2 == 0) {
        return (AlignmentResult){0, 0, NULL, NULL};
    }

    // allocate DP matrix
    DP_cell **dp = (DP_cell **)malloc((len1 + 1) * sizeof(DP_cell *));
    for (int i = 0; i <= len1; i++) {
        dp[i] = (DP_cell *)malloc((len2 + 1) * sizeof(DP_cell));
    }

    // inits
    dp[0][0].Sscore = 0;
    dp[0][0].Dscore = dp[0][0].Iscore = GAP_OPEN;

    for (int i = 1; i <= len1; i++) {
        dp[i][0].Sscore = -1000000;
        dp[i][0].Dscore = GAP_OPEN + (i - 1) * GAP_EXTEND;
        dp[i][0].Iscore = -1000000;
    }

    for (int j = 1; j <= len2; j++) {
        dp[0][j].Sscore = -1000000;
        dp[0][j].Iscore = GAP_OPEN + (j - 1) * GAP_EXTEND;
        dp[0][j].Dscore = -1000000;
    }

    // Fill DP matrix
    int max_score = -1000000, max_i = 0, max_j = 0;
    char max_state = 'S';

    for (int i = 1; i <= len1; i++) {
        for (int j = 1; j <= len2; j++) {
            int sub_score = (s1[i - 1] == s2[j - 1]) ? MATCH_SCORE : MISMATCH_PENALTY;

            // S (substitution)
            int from_S = dp[i - 1][j - 1].Sscore + sub_score;
            int from_D = dp[i - 1][j - 1].Dscore + sub_score;
            int from_I = dp[i - 1][j - 1].Iscore + sub_score;
            if (from_S >= from_D && from_S >= from_I) {
                dp[i][j].Sscore = from_S;
                dp[i][j].from_S = 'S';
            } 
            else if (from_D >= from_I) {
                dp[i][j].Sscore = from_D;
                dp[i][j].from_S = 'D';
            } 
            else {
                dp[i][j].Sscore = from_I;
                dp[i][j].from_S = 'I';
            }

            // D (gap in s2 / deletion)
            int del_from_S = dp[i - 1][j].Sscore + GAP_OPEN;
            int del_from_D = dp[i - 1][j].Dscore + GAP_EXTEND;
            if (del_from_S >= del_from_D) {
                dp[i][j].Dscore = del_from_S;
                dp[i][j].from_D = 'S';
            } 
            else {
                dp[i][j].Dscore = del_from_D;
                dp[i][j].from_D = 'D';
            }

            // I (gap in s1 / insertion)
            int ins_from_S = dp[i][j - 1].Sscore + GAP_OPEN;
            int ins_from_I = dp[i][j - 1].Iscore + GAP_EXTEND;
            if (ins_from_S >= ins_from_I) {
                dp[i][j].Iscore = ins_from_S;
                dp[i][j].from_I = 'S';
            } 
            else {
                dp[i][j].Iscore = ins_from_I;
                dp[i][j].from_I = 'I';
            }

            // track max score
            if (dp[i][j].Sscore > max_score) {
                max_score = dp[i][j].Sscore;
                max_i = i;
                max_j = j;
                max_state = 'S';
            }
            if (dp[i][j].Dscore > max_score) {
                max_score = dp[i][j].Dscore;
                max_i = i;
                max_j = j;
                max_state = 'D';
            }
            if (dp[i][j].Iscore > max_score) {
                max_score = dp[i][j].Iscore;
                max_i = i;
                max_j = j;
                max_state = 'I';
            }
        }
    }

    // tracebaxck to count matches
    int i = max_i, j = max_j, matches = 0;
    char state = max_state;
    while (i > 0 && j > 0) {
        if (state == 'S') {
            if (s1[i - 1] == s2[j - 1]) matches++;
            state = dp[i][j].from_S;
            i--; j--;
        } 
        else if (state == 'D') {
            state = dp[i][j].from_D;
            i--;
        } 
        else {
            state = dp[i][j].from_I;
            j--;
        }
    }

    // Free memory
    for (int i = 0; i <= len1; i++) {
        free(dp[i]);
    }
    free(dp);

    return (AlignmentResult){.score = max_score, .matches = matches, .align1 = NULL, .align2 = NULL}; // we dont actually need alignmetns..
}

void free_alignment_result(AlignmentResult result) {
    if (result.align1) free(result.align1);
    if (result.align2) free(result.align2);
}

void free_similarity_matrix(SimilarityCell **matrix, int size) {
    if (matrix == NULL) return;
    
    for (int i = 0; i < size; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

void free_suffix_tree(Node *node, const char *alphabet) {
    if (!node) return;
    
    for (int i = 0; i < strlen(alphabet); i++) {
        if (node->children[i]) {
            free_suffix_tree(node->children[i], alphabet);
        }
    }
    
    free(node->children);
    free(node);
}