#include "input_parser.h"

void print_usage() {
    printf("Usage: <executable> <input file containing sequence s> <input alphabet file>\n");
}

Sequence* read_single_sequence(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        return NULL;
    }

    Sequence *seq = malloc(sizeof(Sequence));
    if (!seq) {
        fclose(file);
        return NULL;
    }

    seq->name = NULL;
    seq->sequence = malloc(INITIAL_MAX_SEQ_LEN);
    if (!seq->sequence) {
        free(seq);
        fclose(file);
        return NULL;
    }
    seq->sequence[0] = '\0';

    char line[600];
    size_t allocated = INITIAL_MAX_SEQ_LEN;
    size_t length = 0;

    // read name from first line
    if (fgets(line, sizeof(line), file)) {
        if (line[0] == '>') {
            // use the filename (without path) as the sequence name
            const char *base = strrchr(filename, '/');
            base = base ? base + 1 : filename;

            char *dot = strrchr(base, '.');
            size_t name_len = dot ? (size_t)(dot - base) : strlen(base);

            seq->name = (char *)malloc(name_len + 1);
            if (seq->name) {
                strncpy(seq->name, base, name_len);
                seq->name[name_len] = '\0';
            }
        } 
    }

    // read sequence data
    while (fgets(line, sizeof(line), file)) {
        size_t line_len = strlen(line);
        if (line_len > 0 && line[line_len-1] == '\n') {
            line[line_len-1] = '\0';
            line_len--;
        }

        if (length + line_len + 1 > allocated) {
            allocated = (length + line_len + 1) * 2;
            char *temp = realloc(seq->sequence, allocated);
            if (!temp) {
                free(seq->name);
                free(seq->sequence);
                free(seq);
                fclose(file);
                return NULL;
            }
            seq->sequence = temp;
        }

        strcat(seq->sequence, line);
        length += line_len;
    }

    // add $
    if (length + 2 > allocated) {
        char *temp = realloc(seq->sequence, length + 2);
        if (!temp) {
            free(seq->name);
            free(seq->sequence);
            free(seq);
            fclose(file);
            return NULL;
        }
        seq->sequence = temp;
    }
    seq->sequence[length] = '$';
    seq->sequence[length+1] = '\0';

    fclose(file);
    return seq;
}

char* read_alphabet(const char* filename) {
    FILE* file = fopen(filename, "r");
    size_t alphabet_n = 1; // every alphabet contains "$"
    int seen_char[MAX_ALPHABET_SIZE] = {0};
    char c;

    if (!file) {
        perror("Error opening alphabet file");
        exit(1);
    }

    // count unique characters
    while ((c = fgetc(file)) != EOF) {
        if (isalnum(c) && !seen_char[(int)c]) {
            seen_char[(int)c] = 1;
            alphabet_n++;
        }
    }
    rewind(file);
    
    // allocate and populate alphabet array
    char* alpha_arr = malloc(alphabet_n + 1);
    if (!alpha_arr) {
        perror("Could not allocate memory for alphabet array");
        fclose(file);
        exit(1);
    }

    alpha_arr[0] = '$';
    size_t alpha_arr_index = 1;
    while ((c = fgetc(file)) != EOF) {
        if (isalnum(c) && seen_char[(int)c]) {
            alpha_arr[alpha_arr_index++] = c;
            seen_char[(int)c] = 0; // avoid duplicates
        }
    }

    alpha_arr[alphabet_n] = '\0';
    fclose(file);
    return alpha_arr;
}
