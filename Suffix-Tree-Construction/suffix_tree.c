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
                
                last_internal = last_internal;
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
    if (v_prime == NULL) {
        fprintf(stderr, "Error: NULL v_prime parameter\n");
        exit(1);
    }

    Node* v = v_prime;
    int str_len = strlen(sequence_string);
    int alphabet_len = strlen(alphabet);
    int beta_counter = 0;
    int str_pos = beta_start;

    // validate beta_start position
    if (beta_start < 0 || beta_start >= str_len) {
        fprintf(stderr, "Error: Invalid beta_start position %d\n", beta_start);
        exit(1);
    }

    while (beta_counter < beta_len && str_pos < str_len) {
        // get next character
        char current_char = sequence_string[str_pos];
        int next_branch_index = get_char_child_index(current_char, alphabet);
        
        // validate branch index
        if (next_branch_index < 0 || next_branch_index >= alphabet_len) {
            fprintf(stderr, "Error: Invalid character %c in sequence (not in alphabet)\n", current_char);
            exit(1);
        }

        // get child node
        Node* next = v->children[next_branch_index];
        if (next == NULL) {
            fprintf(stderr, "Error in node_hops: No child for character '%c' at position %d\n", 
                    current_char, str_pos);
            fprintf(stderr, "Current node ID: %d, depth: %d\n", v->id, v->depth);
            fprintf(stderr, "Beta: len=%d, start=%d, counter=%d\n", beta_len, beta_start, beta_counter);
            exit(1);
        }

        // validate edge labels
        if (next->edge_label[0] < 0 || next->edge_label[1] >= str_len || 
            next->edge_label[0] > next->edge_label[1]) {
            fprintf(stderr, "Error: Invalid edge labels [%d,%d] for node %d\n",
                    next->edge_label[0], next->edge_label[1], next->id);
            exit(1);
        }

        int edge_len = next->edge_label[1] - next->edge_label[0] + 1;
        int remaining_beta = beta_len - beta_counter;

        if (edge_len > remaining_beta) {
            // split edge - with additional validation
            if (next->edge_label[0] + remaining_beta > next->edge_label[1]) {
                fprintf(stderr, "Error: Invalid edge split position\n");
                exit(1);
            }

            Node* new_internal = create_node(alphabet_len);
            new_internal->id = generate_id(false, suff_index, str_len);
            new_internal->edge_label[0] = next->edge_label[0];
            new_internal->edge_label[1] = next->edge_label[0] + remaining_beta - 1;
            new_internal->parent = v;
            new_internal->depth = v->depth + remaining_beta;

            // update existing node
            next->edge_label[0] += remaining_beta;
            next->parent = new_internal;

            // connect nodes
            int split_char_index = get_char_child_index(sequence_string[next->edge_label[0]], alphabet);
            if (split_char_index < 0 || split_char_index >= alphabet_len) {
                fprintf(stderr, "Error: Invalid split character\n");
                exit(1);
            }
            new_internal->children[split_char_index] = next;
            v->children[next_branch_index] = new_internal;

            v = new_internal;
            break;
        } 
        else {
            beta_counter += edge_len;
            str_pos += edge_len;
            v = next;
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
    if (v_prime == NULL) {
        printf("ERROR: V_prime is null @ suff_i %d\n", suff_index);
    }
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
    if (u_prime == NULL) {
        printf("ERROR: u_prime is null @ suff_i %d\n", suff_index);
    }
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
        Node* last_internal = NULL;  // tracks last node created (leaf)
        
        // insert suffixes
        for (int suff_ind = 0; suff_ind < seq_len; suff_ind++) {
            Node* u = (last_internal != NULL) ? last_internal : root;
            
            if (u->suff_link != NULL) {
                // case 1: SL(u) is known
                last_internal = suff_link_known(u, sequence_string, suff_ind, alphabet);
            } 
            else if (u != root) {
                // case 2: SL(u) is unknown and u is not root

                last_internal = suff_link_unknown_internal(u, sequence_string, suff_ind, alphabet);
                
                // set suffix link for the previous internal node if needed
                // if (last_internal != NULL && last_internal->suff_link == NULL) {
                //     printf("we are doing this?\n");
                //     last_internal->suff_link = u->suff_link;
                // }
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

// Display children left to right
/**
 * Given a pointer to a specific node u in the tree, display u's children from left to right
 * @node_u: node whose children to display
 */
void display_children(Node* u, const char* alphabet) {
    if (u == NULL) {
        printf("Node is NULL\n");
        return;
    }

    printf("Children of node %d: [", u->id);
    
    // Iterate through alphabet to maintain lexicographical order
    for (int i = 0; i < strlen(alphabet); i++) {
        int branch_index = get_char_child_index(alphabet[i], alphabet);
        if (u->children[branch_index] != NULL) {
            Node* child = u->children[branch_index];
            printf(" %d(%c..%c)", child->id, 
                   alphabet[i], 
                   alphabet[i]); // All children start with their branch character
        }
    }
    
    printf(" ]\n");
}

// Enumerate nodes using DFS
/**
* Using DFS, nodes will be enumerated in top-down fashion.
* As a result of this enumeration, displays STRING DEPTH info from each node.
* @node_r: starting/root node to enumerate the tree
*/
void dfs_enumerate(Node* node_r, const char* sequence_string, const char* alphabet) {
    if (node_r == NULL) return;
 
    // print current node info
    if (node_r->parent == node_r) {
        printf("[Root id=%d, depth=%d]\n", node_r->id, node_r->depth);
    } else {
        printf("[Node id=%d, depth=%d, edge='", node_r->id, node_r->depth);
        for (int i = node_r->edge_label[0]; i <= node_r->edge_label[1]; i++) {
            printf("%c", sequence_string[i]);
        }
        printf("']\n");
    }
 
    // recursively visit children in lexicographical order
    for (int i = 0; i < strlen(alphabet); i++) {
        int branch_index = get_char_child_index(alphabet[i], alphabet);
        if (node_r->children[branch_index] != NULL) {
            dfs_enumerate(node_r->children[branch_index], sequence_string, alphabet);
        }
    }
 }
 
/**
 * Uses DFS procedure to print the BWT index for input string s.
 * BWT index is an array B of size n, given by B[i] = s[leaf(i) - 1],
 * where leaf(i) is the suffix ID of the ith leaf in lexicographical order.
 * If i = 0, then B[0] = $ (i.e., cycling around from the end of the string)
 */
void compute_bwt_index(Node* root, const char* sequence_file, const char* sequence_string, const char* alphabet) {
    int n = strlen(sequence_string);
    char* BWT = (char*)malloc((n + 1) * sizeof(char)); // +1 for null terminator
    int bwt_index = 0;

    // stack for iterative DFS
    Node** stack = (Node**)malloc(n * sizeof(Node*));
    int stack_top = -1;
    stack[++stack_top] = root; // init stack with

    while (stack_top >= 0) {
        Node* curr = stack[stack_top--];

        // if leaf node, process it
        if (is_leaf(curr, n)) {
            int suffix_id = curr->id;
            int bwt_pos = (suffix_id == 0) ? n - 1 : suffix_id - 1;
            BWT[bwt_index++] = sequence_string[bwt_pos];
        }

        // push children in reverse lexicographical order (to process left-to-right when POPPING OFF!)
        for (int i = strlen(alphabet) - 1; i >= 0; i--) {
            int branch_index = get_char_child_index(alphabet[i], alphabet);
            if (curr->children[branch_index] != NULL) {
                stack[++stack_top] = curr->children[branch_index];
            }
        }
    }

    // generate output filename by appending "_BWT.txt" to the sequence file name
    char output_filename[256];
    snprintf(output_filename, sizeof(output_filename), "%.*s_bwt.txt",
             (int)(strrchr(sequence_file, '.') ? strrchr(sequence_file, '.') - sequence_file : strlen(sequence_file)),
             sequence_file);

    // Write to file
    FILE* file = fopen(output_filename, "w");
    if (file == NULL) {
        perror("Error opening file");
        free(BWT);
        free(stack);
        return;
    }

    for (int i = 0; i < n; i++) {
        fprintf(file, "%c\n", BWT[i]);
    }

    fclose(file);
    printf("BWT output written to: %s\n", output_filename);
    free(BWT);
    free(stack);
}

void report_space_usage(Node* root, const char* seq_str, const char* alphabet) {
    size_t input_bytes = strlen(seq_str);
    size_t alphabet_size = strlen(alphabet);
    
    size_t node_count = input_bytes * 2;
    
    size_t node_size = sizeof(Node) + (alphabet_size * sizeof(Node*));
    
    // total estimated memory
    size_t estimated_memory = node_count * node_size;
    double space_constant = (double)estimated_memory / input_bytes;
    
    printf("Space Usage Estimate:\n");
    printf("Input size: %zu bytes\n", input_bytes);
    printf("Estimated tree memory: %zu bytes (~%.2f MB)\n", 
           estimated_memory, estimated_memory/(1024.0*1024.0));
    printf("Space constant: ~%.1f bytes per input byte\n", space_constant);
}

// helper function
void find_longest_repeat(Node* node, const char* sequence, const char* alphabet, LongestRepeat* result, int current_depth) {
    if (!node) return;

    // update current depth with edge length
    int edge_length = node->edge_label[1] - node->edge_label[0] + 1;
    current_depth += edge_length;

    // check if this is an internal node (>= 2 children)
    int child_count = 0;
    for (int i = 0; i < strlen(alphabet); i++) {
        if (node->children[i]) child_count++;
    }

    if (child_count >= 2) {
        if (current_depth > result->length) {
            // found new longest repeat
            result->length = current_depth;
            free(result->positions);
            result->positions = NULL;
            result->count = 0;
            
            // collect all leaf positions under this node
            collect_leaf_positions(node, sequence, alphabet, result);
        }
    }

    // recurse check children
    for (int i = 0; i < strlen(alphabet); i++) {
        find_longest_repeat(node->children[i], sequence, alphabet, result, current_depth);
    }
}

// helper function for finding repeats
void collect_leaf_positions(Node* node, const char* sequence, const char* alphabet, LongestRepeat* result) {
    if (!node) return;

    if (is_leaf(node, strlen(sequence))) {
        // found a leaf - add its position to results
        result->positions = realloc(result->positions, (result->count + 1) * sizeof(int));
        result->positions[result->count] = node->id;
        result->count++;
    } 
    else {
        // internal node - check children
        for (int i = 0; i < strlen(alphabet); i++) {
            collect_leaf_positions(node->children[i], sequence, alphabet, result);
        }
    }
}

// actual function to call to find repeats
LongestRepeat find_repeats(Node* root, const char* sequence, const char* alphabet) {
    LongestRepeat result = {0, NULL, 0};
    find_longest_repeat(root, sequence, alphabet,  &result, 0);
    return result;
}

void print_repeats(const LongestRepeat* repeat, const char* sequence) {
    if (repeat->length == 0) {
        printf("No repeats found.\n");
        return;
    }

    printf("Longest exact repeat: '");
    // Print the repeat substring
    for (int i = 0; i < repeat->length; i++) {
        printf("%c", sequence[repeat->positions[0] + i]);
    }
    printf("' (length %d)\n", repeat->length);

    printf("Positions: ");
    for (int i = 0; i < repeat->count; i++) {
        printf("%d", repeat->positions[i]);
        if (i < repeat->count - 1) printf(", ");
    }
    printf("\n");
}
