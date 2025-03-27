#include "suffix_tree.h"

// CreateNode
/**
* Initializes a generic node without unique data or information.
* @alphabet_size: size of the alphabet (including $)
* Returns a generic node with 
*/
Node* create_node(int alphabet_size) {
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
 * IDs: 0...n-1 for leaves (suffix index), n...x for internal nodes
 * 
 */
int generate_id(bool is_leaf, int suff_index, int str_len) {
    static int internal_node_counter = 0;
    int id = suff_index;

    if (!is_leaf) {
        id = str_len + internal_node_counter;
        internal_node_counter++;
    }
    
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
void find_path(Node* root, const char* sequence_string, int suff_index, const char* alphabet) {
    Node* v = root;
    int curr_pos = suff_index;
    int alphabet_len = strlen(alphabet);
    int str_len = strlen(sequence_string);
    bool leaf_inserted = false;

    while (!leaf_inserted && curr_pos < str_len) {
        int branch_i = get_char_child_index(sequence_string[curr_pos], alphabet);
        Node* u = v->children[branch_i];

        if (u == NULL) {
            // no existing edge - create new leaf
            Node* new_leaf = create_node(alphabet_len);
            new_leaf->id = generate_id(true, suff_index, str_len);
            new_leaf->edge_label[0] = curr_pos;
            new_leaf->edge_label[1] = str_len - 1;
            new_leaf->parent = v;
            new_leaf->depth = v->depth + (new_leaf->edge_label[1] - new_leaf->edge_label[0] + 1);
            v->children[branch_i] = new_leaf;
            leaf_inserted = true;
        } 
        else {
            // existing edge - compare characters
            int edge_start = u->edge_label[0];
            int edge_end = u->edge_label[1];
            int edge_pos = edge_start;
            
            // Compare characters along the edge
            while (edge_pos <= edge_end && curr_pos < str_len && 
                   sequence_string[edge_pos] == sequence_string[curr_pos]) {
                edge_pos++;
                curr_pos++;
            }

            if (edge_pos > edge_end) {
                // Entire edge matched - move to child node
                v = u;
            } else {
                // mismatch -- split edge
                Node* new_internal = create_node(alphabet_len);
                new_internal->id = generate_id(false, suff_index, str_len);
                
                // set up the new internal node
                new_internal->edge_label[0] = edge_start;
                new_internal->edge_label[1] = edge_pos - 1;
                new_internal->parent = v;
                new_internal->depth = v->depth + (edge_pos - edge_start);
                
                // update the existing node
                u->edge_label[0] = edge_pos;
                u->parent = new_internal;
                
                // connect new internal node to existing node
                int u_branch = get_char_child_index(sequence_string[edge_pos], alphabet);
                new_internal->children[u_branch] = u;
                
                // update parent's child pointer to new internal
                v->children[branch_i] = new_internal;
                
                // create new leaf for current suffix
                Node* new_leaf = create_node(alphabet_len);
                new_leaf->id = generate_id(true, suff_index, str_len);
                new_leaf->edge_label[0] = curr_pos;
                new_leaf->edge_label[1] = str_len - 1;
                new_leaf->parent = new_internal;
                new_leaf->depth = new_internal->depth + (str_len - curr_pos);
                
                // add leaf to new internal node
                int leaf_branch = get_char_child_index(sequence_string[curr_pos], alphabet);
                new_internal->children[leaf_branch] = new_leaf;
                
                leaf_inserted = true;
            }
        }
    }
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
    int seq_len = strlen(sequence_string);
    int alphabet_size = strlen(alphabet);
    
    // create root node
    Node* root = create_node(alphabet_size);
    root->parent = root; // root points to itself
    root->id = generate_id(false, 0, seq_len);
    
    // insert all suffixes
    for (int suff_ind = 0; suff_ind < seq_len; suff_ind++) {
        find_path(root, sequence_string, suff_ind, alphabet);
    }

    return root;

}

/***************
 * PRINTING / TESTING CONSTRUCTION OF TREE FUNCTIONS
 */

 // Helper function to check if a node is a leaf
int is_leaf(Node* node, int str_len) {
    return node->id < str_len; // Assuming your leaf IDs are 0...n-1
}

void print_suffix_tree(Node* node, const char* sequence_string, const char* alphabet, int str_len, int depth) {
    // indentation
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }

    // print node information
    if (node->parent == node) {
        printf("[Root id=%d]", node->id);
    } 
    else {
        if (is_leaf(node, str_len)) {
            printf("[Leaf id=%d, suffix=%d, edge='", node->id, node->id);
        } 
        else {
            printf("[Internal id=%d, edge='", node->id);
        }

        // Print edge label
        if (node->edge_label[0] <= node->edge_label[1]) {
            for (int i = node->edge_label[0]; i <= node->edge_label[1]; i++) {
                printf("%c", sequence_string[i]);
            }
        }
        printf("']");
    }

    // print suffix link 
    if (node->suff_link != NULL) {
        printf(" --> [id=%d]", node->suff_link->id);
    }
    printf("\n");

    // recursively print children (only if they exist)
    for (int i = 0; i < strlen(alphabet); i++) {
        int branch_index = get_char_child_index(alphabet[i], alphabet);
        if (node->children[branch_index] != NULL) {
            print_suffix_tree(node->children[branch_index], sequence_string, alphabet, str_len, depth + 1);
        }
    }
}

void print_tree(Node* root, const char* sequence_string, const char* alphabet) {
    int str_len = strlen(sequence_string);
    printf("\nSuffix Tree for: '%s' (length=%d)\n", sequence_string, str_len);
    printf("Alphabet: '%s'\n", alphabet);
    printf("Tree structure (L=Leaf, I=Internal):\n");
    print_suffix_tree(root, sequence_string, alphabet, str_len, 0);
    printf("\n");
}
