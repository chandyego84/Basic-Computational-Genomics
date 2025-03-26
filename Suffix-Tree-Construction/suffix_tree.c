#include "suffix_tree.h"
#include <stdlib.h>
#include <string.h>

// CreateNode
/**
* Initializes a generic node without unique data or information.
* @alphabet_size: size of the alphabet (including $)
* Returns a generic node with 
*/
Node* create_node(unsigned int alphabet_size) {
    // allocate memory for new node
    Node* new_node = (Node*)malloc(sizeof(Node));
    if (!new_node) {
        perror("Could not allocate memory for a new node");
        exit(1);
    }

    // allocate memory for array of node ptrs (children)
    new_node->children = (Node**)calloc(alphabet_size, sizeof(Node*));
    if (!new_node -> children) {
        perror("Could not allocate memory for children ptrs of a node");
        free(new_node);
        exit(1);
    }

    // init node members
    new_node->id = 0;
    new_node->parent = NULL;
    new_node->suff_link = NULL;
    new_node->depth = 0;
    new_node->edge_label[0] = 0;
    new_node->edge_label[1] = 0;

    return new_node;
}


// GenerateNodeId
/**
 * Generates an ID for a node depending on if its a leaf or not
 * 
 */
unsigned int generate_id(bool is_leaf, unsigned int suff_index) {
    unsigned int id = 0;


    return id;
}

/**
 * Given a character in an alphabet, gets the corresponding index in the children array of a node
 * @c: character in an alphabet
 * @alphabet: alphabet over which a sequence string is comrpised of
 * @returns: corresponding index in children array (in a node) of a character
 */
int get_char_child_index(const char c, const char* alphabet) {
    int c_index = -1;
    for (int i = 0; i < strlen(alphabet); i++) {
        if (c == alphabet[i]) {
            c_index = i;
            break;
        }
    }

    return c_index;
}

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
void find_path(Node* root, const char* sequence_string, unsigned int suff_index, const char* alphabet) {
    // v <- u, x <- s[i...end]


    // repeat until leaf is inserted
        // find child branch starting with x[1]
        // if no branch exists:
            // insert new leaf for s under u
            // return
        
            // compare characters of edge label against x[1...]
            // until first mismatch or edgelabel is exhausted

            // if mismatch:
                // break edge
                // create new internal node
                // create new leaf for s under that node
                // return
            // else:
                // v <- next internal node
                // x <- x[i + depth processed so far]
    
    return;
}

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
Node* build_suffix_tree(const char* sequence_string, const char* alphabet) {
    // T <- root

    // iterate over each suffix in string
        // T <- T.insert(suff_i) -- findPath is naive approach
    
    // return T
}
