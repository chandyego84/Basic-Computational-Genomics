#ifndef SUFFIX_TREE_H
#define SUFFIX_TREE_H

#include "types.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// CreateNode
/**
* Initializes a node.
* @alphabet_size: size of the alphabet (including $)
*/
Node* create_node(int alphabet_size);

// GenerateNodeId
/**
 * Generates an ID for a node depending on if its a leaf or not.
 * @is_leaf: true if node whose ID being generated is a leaf, otherwise False.
 * @suff_order: starting index of current suffix being processed.
 * @str_len: length of the whole sequence string being processed; 
 */
int generate_id(bool is_leaf, int suff_index, int str_len);

/**
 * Given a character in an alphabet, gets the corresponding index in the children array of a node
 * @c: character in an alphabet
 * @alphabet: alphabet over which a sequence string is comrpised of
 * @returns: corresponding index in children array (in a node) of a character
 */
int get_char_child_index(const char c, const char* alphabet);

// FindPath 
/**
 * Finds the path starting at a node arg that spells out
 * the longest possible prefix of the specified string arg,
 * and then inserts the next suffix.
 * i.e., inserts sufix S[i...] under some node u
 * @root: root node of tree to find path from
 * @string: full string
 * @index: starting index of string
 * @alphabet: alphabet that string is comprised of
 */
void find_path(Node* root, const char* sequence_string, int suff_index, const char* alphabet);


// NodeHops
/**
 * Does node hopping child to child until
 * string Beta (or Beta') is exhausted, depending on the case
 */

// ST Construction
/**
 * @sequence_string: input string to build ST of
 * @alphabet: alphabet related to input string to build ST with
 * @returns - root node of tree
 */
Node* build_suffix_tree(const char* sequence_string, const char* alphabet);

// PrintTree
void print_suffix_tree(Node* node, const char* sequence_string, const char* alphabet, int str_len, int depth);
void print_tree(Node* root, const char* sequence_string, const char* alphabet);
 
// Display children left to right
/**
 * Given a pointer to a specific node u in the tree, display u's children from left to right
 * @node_u: node whose children to display
 */

// Enumerate nodes using DFS
/**
* Using DFS, nodes will be enumerated in top-down fashion.
* As a result of this enumeration, displays STRING DEPTH info from each node.
* @node_r: starting/root node to enumerate the tree
*/

// BWT index
/**
 * Uses DFS procedure to print the BWT index for input string s. 
 * This can be done by enumerating ONLY the leaf node's id from left to right (lexicographically smallest->largest).
 * BWT index is an array B of size n, given by B[i] = s[leaf(i) - 1],
    * where leaf(i) is the suffix ID of the ith leaf in lexicographical order.
 * If i = 0, then B[0] = $ (i.e., cycling around from the end of the string)
 */

#endif 