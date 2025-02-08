#include <stdio.h>
#include "input_parser.h"

#define DEFAULT_CONFIG_FILE "parameters.config"

int main() {
    int match_val;
    int mismatch_val;
    int gap_open_val;
    int gap_extension_val;

    read_configs(DEFAULT_CONFIG_FILE, &match_val, &mismatch_val, &gap_open_val, &gap_extension_val);

    printf("ma: %d, mi: %d, gapOpen: %d, gapExtension: %d", match_val, mismatch_val, gap_open_val, gap_extension_val);
    
    return 0;
}