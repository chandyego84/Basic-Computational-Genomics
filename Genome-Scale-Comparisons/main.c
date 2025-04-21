#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "input_parser.h"
#include "similarity.h"

// Function to run with hardcoded parameters
int run_hardcoded() {
    printf("Running with hardcoded parameters...\n");
    const char *alphabet_file = "DNA_alphabet.txt";
    const char *similarity_output_file = "similarity_results.csv";
    const char *lcs_output_file = "lcs_results.csv";
    // Hardcoded list of genome file paths
    const char *genome_files[] = {
        "Covid_Australia.fasta",
        "Covid_Brazil.fasta",
        "Covid_India.fasta",
        "Covid_USA-CA4.fasta",
        "Covid_Wuhan.fasta",
        "MERS_2012_KF600620.fasta",
        "MERS_2014_KY581694.fasta",
        "MERS_2014_USA_KP223131.fasta",
        "SARS_2003_GU553363.fasta",
        "SARS_2017_MK062179.fasta"
    };
    const int num_genomes = 10;

    char *alphabet = read_alphabet(alphabet_file);
    if (!alphabet) {
        fprintf(stderr, "Error: Failed to read alphabet file '%s'\n", alphabet_file);
        return 1;
    }
    fprintf(stderr, "DEBUG: Alphabet loaded: %s\n", alphabet);

    Sequence *genomes = malloc(num_genomes * sizeof(Sequence));
    if (!genomes) {
        fprintf(stderr, "Error: Memory allocation failed for genome array\n");
        free(alphabet);
        return 1;
    }

    for (int i = 0; i < num_genomes; i++) {
        genomes[i].name = NULL;
        genomes[i].sequence = NULL;

        Sequence *seq = read_single_sequence(genome_files[i]);
        if (!seq || !seq->sequence) {
            fprintf(stderr, "Error: Failed to read genome file '%s'\n", genome_files[i]);
            for (int j = 0; j < i; j++) {
                free(genomes[j].name);
                free(genomes[j].sequence);
            }
            free(genomes);
            free(alphabet);
            if (seq) free(seq);
            return 1;
        }
        fprintf(stderr, "DEBUG: Read sequence %s, length %zu\n", seq->name, strlen(seq->sequence));
        genomes[i] = *seq;
        free(seq);
    }

    fprintf(stderr, "DEBUG: Computing similarity matrix...\n");
    SimilarityCell **similarity_matrix = compute_similarity_matrix(genomes, num_genomes, alphabet);
    if (!similarity_matrix) {
        fprintf(stderr, "Error: Failed to compute similarity matrix\n");
        for (int i = 0; i < num_genomes; i++) {
            free(genomes[i].name);
            free(genomes[i].sequence);
        }
        free(genomes);
        free(alphabet);
        return 1;
    }
    fprintf(stderr, "FINISHED computing similarity matrix.\n");

    // Output similarity matrix to CSV
    FILE *similarity_output = fopen(similarity_output_file, "w");
    if (!similarity_output) {
        fprintf(stderr, "Error: Could not open output file '%s'\n", similarity_output_file);
        free_similarity_matrix(similarity_matrix, num_genomes);
        for (int i = 0; i < num_genomes; i++) {
            free(genomes[i].name);
            free(genomes[i].sequence);
        }
        free(genomes);
        free(alphabet);
        return 1;
    }

    fprintf(similarity_output, "Genomes");
    for (int i = 0; i < num_genomes; i++) {
        fprintf(similarity_output, ",%s", genomes[i].name);
    }
    fprintf(similarity_output, "\n");

    for (int i = 0; i < num_genomes; i++) {
        fprintf(similarity_output, "%s", genomes[i].name);
        for (int j = 0; j < num_genomes; j++) {
            fprintf(similarity_output, ",%d", similarity_matrix[i][j].alignment_score);
        }
        fprintf(similarity_output, "\n");
    }
    fclose(similarity_output);

    // Output LCS matrix to CSV
    FILE *lcs_output = fopen(lcs_output_file, "w");
    if (!lcs_output) {
        fprintf(stderr, "Error: Could not open LCS output file '%s'\n", lcs_output_file);
        free_similarity_matrix(similarity_matrix, num_genomes);
        for (int i = 0; i < num_genomes; i++) {
            free(genomes[i].name);
            free(genomes[i].sequence);
        }
        free(genomes);
        free(alphabet);
        return 1;
    }

    fprintf(lcs_output, "Genomes");
    for (int i = 0; i < num_genomes; i++) {
        fprintf(lcs_output, ",%s", genomes[i].name);
    }
    fprintf(lcs_output, "\n");

    for (int i = 0; i < num_genomes; i++) {
        fprintf(lcs_output, "%s", genomes[i].name);
        for (int j = 0; j < num_genomes; j++) {
            fprintf(lcs_output, ",%d", similarity_matrix[i][j].lcs_length);
        }
        fprintf(lcs_output, "\n");
    }
    fclose(lcs_output);

    free_similarity_matrix(similarity_matrix, num_genomes);
    for (int i = 0; i < num_genomes; i++) {
        free(genomes[i].name);
        free(genomes[i].sequence);
    }
    free(genomes);
    free(alphabet);

    printf("Successfully wrote similarity matrix to '%s' and LCS matrix to '%s'\n", similarity_output_file, lcs_output_file);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc == 1) {
        return run_hardcoded();
    }

    if (argc < 4) {
        fprintf(stderr, "Usage: %s <alphabet file> <similarity output file> <lcs output file> <genome1.fa> [genome2.fa ...]\n", argv[0]);
        fprintf(stderr, "       Or run with no arguments to use hardcoded parameters:\n");
        fprintf(stderr, "       DNA_alphabet.txt similarity_results.csv lcs_results.csv Covid_Wuhan.fasta Covid_USA-CA4.fasta\n");
        return 1;
    }

    char *alphabet = read_alphabet(argv[1]);
    if (!alphabet) {
        fprintf(stderr, "Error: Failed to read alphabet file '%s'\n", argv[1]);
        return 1;
    }
    fprintf(stderr, "DEBUG: Alphabet loaded: %s\n", alphabet);

    int num_genomes = argc - 4;
    Sequence *genomes = malloc(num_genomes * sizeof(Sequence));
    if (!genomes) {
        fprintf(stderr, "Error: Memory allocation failed for genome array\n");
        free(alphabet);
        return 1;
    }

    for (int i = 0; i < num_genomes; i++) {
        genomes[i].name = NULL;
        genomes[i].sequence = NULL;

        Sequence *seq = read_single_sequence(argv[i + 4]);
        if (!seq || !seq->sequence) {
            fprintf(stderr, "Error: Failed to read genome file '%s'\n", argv[i + 4]);
            for (int j = 0; j < i; j++) {
                free(genomes[j].name);
                free(genomes[j].sequence);
            }
            free(genomes);
            free(alphabet);
            if (seq) free(seq);
            return 1;
        }
        fprintf(stderr, "DEBUG: Read sequence %s, length %zu\n", seq->name, strlen(seq->sequence));
        genomes[i] = *seq;
        free(seq);
    }

    fprintf(stderr, "DEBUG: Computing similarity matrix...\n");
    SimilarityCell **similarity_matrix = compute_similarity_matrix(genomes, num_genomes, alphabet);
    if (!similarity_matrix) {
        fprintf(stderr, "Error: Failed to compute similarity matrix\n");
        for (int i = 0; i < num_genomes; i++) {
            free(genomes[i].name);
            free(genomes[i].sequence);
        }
        free(genomes);
        free(alphabet);
        return 1;
    }
    fprintf(stderr, "FINISHED computing similarity matrix.\n");

    // Output similarity matrix to CSV
    FILE *similarity_output = fopen(argv[2], "w");
    if (!similarity_output) {
        fprintf(stderr, "Error: Could not open output file '%s'\n", argv[2]);
        free_similarity_matrix(similarity_matrix, num_genomes);
        for (int i = 0; i < num_genomes; i++) {
            free(genomes[i].name);
            free(genomes[i].sequence);
        }
        free(genomes);
        free(alphabet);
        return 1;
    }

    fprintf(similarity_output, "Genomes");
    for (int i = 0; i < num_genomes; i++) {
        fprintf(similarity_output, ",%s", genomes[i].name);
    }
    fprintf(similarity_output, "\n");

    for (int i = 0; i < num_genomes; i++) {
        fprintf(similarity_output, "%s", genomes[i].name);
        for (int j = 0; j < num_genomes; j++) {
            fprintf(similarity_output, ",%d", similarity_matrix[i][j].alignment_score);
        }
        fprintf(similarity_output, "\n");
    }
    fclose(similarity_output);

    // Output LCS matrix to CSV
    FILE *lcs_output = fopen(argv[3], "w");
    if (!lcs_output) {
        fprintf(stderr, "Error: Could not open LCS output file '%s'\n", argv[3]);
        free_similarity_matrix(similarity_matrix, num_genomes);
        for (int i = 0; i < num_genomes; i++) {
            free(genomes[i].name);
            free(genomes[i].sequence);
        }
        free(genomes);
        free(alphabet);
        return 1;
    }

    fprintf(lcs_output, "Genomes");
    for (int i = 0; i < num_genomes; i++) {
        fprintf(lcs_output, ",%s", genomes[i].name);
    }
    fprintf(lcs_output, "\n");

    for (int i = 0; i < num_genomes; i++) {
        fprintf(lcs_output, "%s", genomes[i].name);
        for (int j = 0; j < num_genomes; j++) {
            fprintf(lcs_output, ",%d", similarity_matrix[i][j].lcs_length);
        }
        fprintf(lcs_output, "\n");
    }
    fclose(lcs_output);

    free_similarity_matrix(similarity_matrix, num_genomes);
    for (int i = 0; i < num_genomes; i++) {
        free(genomes[i].name);
        free(genomes[i].sequence);
    }
    free(genomes);
    free(alphabet);

    printf("Successfully wrote similarity matrix to '%s' and LCS matrix to '%s'\n", argv[2], argv[3]);
    return 0;
}
