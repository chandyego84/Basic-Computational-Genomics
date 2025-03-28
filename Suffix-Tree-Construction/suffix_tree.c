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
 * @index: starting index of suffix string
 * @start_pos: index position to start comparing in sequence string
 * @alphabet: alphabet that string is comprised of
 */
Node* find_path(Node* root, const char* sequence_string, int suff_index, int start_pos, const char* alphabet) {
    Node* v = root;
    Node* last_internal = root;
    int curr_pos = start_pos;
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

            // last internal node (non-leaf) inserted is still the root
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
                
                last_internal = new_internal;
                leaf_inserted = true;
            }
        }
    }

    return last_internal;
}

/**
 * Get beta for node hops
 */

// NodeHops
/**
 * Does node hopping child to child until
 * string Beta (or Beta') is exhausted, depending on the case
 * @v_prime: node start node hopping from
 * @sequence_string: full sequence string
 * @suff_index: starting index of suffing string to insert
 * @beta_len: if u' is not root: beta = u.stringdepth. otherwise, beta = c + alpha between u and root.
 * @beta_start: starting index position in the string according to beta edge from u.
 * @returns: node v - node reached from node hopping
 */
Node* node_hops(Node* v_prime, const char* sequence_string, int suff_index, const char* alphabet, int beta_len, int beta_start) {
    Node* v = v_prime; // curr node in hopping process
    int str_len = strlen(sequence_string);
    int beta_counter = 0;
    int str_pos = beta_start;

    // hop until beta exhausted
    while (beta_counter < beta_len && str_pos < str_len) {
        // find next node to hop to
        int next_branch_index = get_char_child_index(sequence_string[str_pos], alphabet);
        Node* next = v->children[next_branch_index];
        
        if (next != NULL) {
            int edge_start = next->edge_label[0];
            int edge_end = next->edge_label[1];
            int edge_len = edge_end - edge_start + 1;
            int remaining_beta = beta_len - beta_counter;

            // check if we need to break the edge
            if (edge_len > remaining_beta) {
                // split the edge
                Node* new_internal = create_node(strlen(alphabet));
                new_internal->id = generate_id(false, suff_index, str_len);
                
                // set up the new internal node
                new_internal->edge_label[0] = edge_start;
                new_internal->edge_label[1] = edge_start + remaining_beta - 1;
                new_internal->parent = v;
                new_internal->depth = v->depth + remaining_beta;
                
                // update the existing node
                next->edge_label[0] = edge_start + remaining_beta;
                next->parent = new_internal;
                
                // connect new internal node to existing node
                int next_branch = get_char_child_index(sequence_string[edge_start + remaining_beta], alphabet);
                new_internal->children[next_branch] = next;
                
                // update parent's child pointer to new internal
                v->children[next_branch_index] = new_internal;
                
                // found v
                v = new_internal;
                break;
            } 
            else {
                // take full edge and move to next node
                beta_counter += edge_len;
                str_pos += edge_len;            
                v = next;
            }
        } 
        else {
            // should not happen if beta is correct
            fprintf(stderr, "Error in node_hops: no child for character %c\n", sequence_string[str_pos]);
            exit(1);
        }
    }

    return v;
}

/**
 * Case: SL(u) is known.
 * Inserts suffix string into ST.
 * @u: parent of leaf i-1 during suffix index iteration of building ST
 * @string: full sequence string 
 * @suff_index: starting index of suffing string to insert
 * @alphabet: alphabet that string is comprised of
 */
Node* suff_link_known(Node* u, const char* sequence_string, int suff_index, const char* alphabet) {
    Node* v = u->suff_link;
    int str_len = strlen(sequence_string);
    int k = v->depth;

    if (suff_index + k <= str_len) {
        return find_path(v, sequence_string, suff_index, suff_index + k, alphabet);
    }

    return v;
}

/**
 * Case: SL(u) is unknown and u' (grandparent of leaf i-1) is not the root.
 * Inserts suffix string into ST.
 * @u: parent of leaf i-1 during suffix index iteration of building ST
 * @string: full sequence string 
 * @suff_index: starting index of suffing string to insert
 * @alphabet: alphabet that string is comprised of
 */
Node* suff_link_unknown_internal(Node* u, const char* sequence_string, int suff_index, const char* alphabet) {
    Node* u_prime = u->parent;
    Node* v_prime = u_prime->suff_link;
    int u_start_edge = u->edge_label[0];
    int beta_len = u->edge_label[1] - u->edge_label[0] + 1;

    // find v by hopping along beta path
    Node* v = node_hops(v_prime, sequence_string, suff_index, alphabet, beta_len, u_start_edge);
    
    // set suffix link for u
    u->suff_link = v;
    
    // insert remaining suffix
    int alpha = v->depth;
    return find_path(v, sequence_string, suff_index, suff_index + alpha, alphabet);
}

/**
 * Case: SL(u) is unknown and u' (grandparent of leaf i-1) is the root.
 * Inserts suffix string into ST.
 * @u: parent of leaf i-1 during suffix index iteration of building ST
 * @sequence_string: full sequence string 
 * @suff_index: starting index of suffix string to insert
 * @alphabet: alphabet that string is comprised of
 * @returns: last internal node created during insertion
 */
Node* suff_link_unknown_root(Node* u, const char* sequence_string, int suff_index, const char* alphabet) {
    int str_len = strlen(sequence_string);
    
    // Get u' (grandparent, which is root)
    Node* u_prime = u->parent;
    
    // Calculate beta (u's edge label minus first character)
    int beta_len = u->depth - 1;  // u.depth is length from root to u
    int beta_start = u->edge_label[0] + 1;  // skip first character
    
    // Node hop from root following beta
    Node* v = node_hops(u_prime, sequence_string, suff_index, alphabet, beta_len, beta_start);
    
    // Set u's suffix link to v
    u->suff_link = v;
    
    // Calculate remaining part to insert (alpha)
    int alpha = v->depth;
    
    // Insert remaining suffix starting at suff_index + alpha
    Node* last_internal = find_path(v, sequence_string, suff_index, suff_index + alpha, alphabet);
    
    return last_internal;
}

// ST Construction
/**
 * @sequence_string: input string to build ST of
 * @alphabet: alphabet related to input string to build ST with
 * @returns - root node of tree
 */
Node* build_suffix_tree(const char* sequence_string, const char* alphabet, bool is_naive) {
    int seq_len = strlen(sequence_string);
    int alphabet_size = strlen(alphabet);
    
    // create root node
    Node* root = create_node(alphabet_size);
    root->suff_link = root;  // root's suffix link points to itself
    root->parent = root;
    root->id = generate_id(false, 0, seq_len);

    if (is_naive) {
        // naive construction - insert all suffixes independently
        for (int suff_ind = 0; suff_ind < seq_len; suff_ind++) {
            find_path(root, sequence_string, suff_ind, suff_ind, alphabet);
        }
    } 
    else {
        // O(n) algorithm:
            // let u <- parent of leaf i-1
        Node* last_internal = NULL;  // tracks last internal node created
        
        // insert suffixes
        for (int suff_ind = 0; suff_ind < seq_len; suff_ind++) {
            Node* u = (last_internal != NULL) ? last_internal->parent : root;
            
            if (u->suff_link != NULL) {
                // case 1: SL(u) is known
                last_internal = suff_link_known(u, sequence_string, suff_ind, alphabet);
            } 
            else if (u != root) {
                // case 2: SL(u) is unknown and u is not root
                last_internal = suff_link_unknown_internal(u, sequence_string, suff_ind, alphabet);
                
                // set suffix link for the previous internal node if needed
                if (last_internal != NULL && last_internal->suff_link == NULL) {
                    last_internal->suff_link = u->suff_link;
                }
            } 
            else {
                // case 3: SL(u) is unknown and u is root
                last_internal = suff_link_unknown_root(u, sequence_string, suff_ind, alphabet);
            }
        }
    }

    return root;
}

/***************
 * PRINTING / TESTING CONSTRUCTION OF TREE FUNCTIONS
 ****************/

// Helper function to check if a node is a leaf
// Assumes leaf IDs are 0...n-1
 bool is_leaf(Node* node, int str_len) {
    return node->id < str_len; 
}

// Helper function to check if a node is the root
/**
 * Assumes the root node initialized its suff_link to itself already
 * @node: node to check
 */

bool is_root(Node* node) {
    return node->suff_link == node;
}

void print_suffix_tree(Node* node, const char* sequence_string, const char* alphabet, int str_len, int depth) {
    // indentation
    for (int i = 0; i < depth; i++) {
        printf("  ");
    }

    // print node information
    if (is_root(node)) {
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
