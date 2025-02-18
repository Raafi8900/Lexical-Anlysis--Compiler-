#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define MAX_TOK 100
#define MAX_OUT 1000

const char *kw[] = {"int", "float", "if", "else"};
#define KW_COUNT 4

int is_kw(char *str) {
    for (int i = 0; i < KW_COUNT; i++) {
        if (strcmp(str, kw[i]) == 0) return 1;
    }
    return 0;
}

int is_unique(char *output, char *token) {
    char temp[MAX_OUT];
    strcpy(temp, output);

    char *tok = strtok(temp, ", ");
    while (tok != NULL) {
        if (strcmp(tok, token) == 0) {
            return 0;
        }
        tok = strtok(NULL, ", ");
    }
    return 1;
}

int is_unique_others(char *output, char *token) {
    char temp[MAX_OUT];
    strcpy(temp, output);

    char *tok = strtok(temp, " ");
    while (tok != NULL) {
        if (strcmp(tok, token) == 0) {
            return 0;
        }
        tok = strtok(NULL, " ");
    }
    return 1;
}

void classify(char *tok, char *kw_out, char *id_out, char *math_out,
              char *log_out, char *num_out, char *oth_out) {
    if (is_kw(tok)) {
        if (is_unique(kw_out, tok)) {
            strcat(kw_out, tok);
            strcat(kw_out, ", ");
        }
        return;
    }

    int is_num = 1, dots = 0;
    for (int i = 0; tok[i]; i++) {
        if (!isdigit(tok[i]) && tok[i] != '.') {
            is_num = 0;
            break;
        }
        if (tok[i] == '.' && ++dots > 1) {
            is_num = 0;
            break;
        }
    }
    if (is_num) {
        if (is_unique(num_out, tok)) {
            strcat(num_out, tok);
            strcat(num_out, ", ");
        }
        return;
    }

    if (isalpha(tok[0])) {
        if (is_unique(id_out, tok)) {
            strcat(id_out, tok);
            strcat(id_out, ", ");
        }
        return;
    }

    if (strchr("+-=", tok[0])) {
        if (is_unique(math_out, tok)) {
            strcat(math_out, tok);
            strcat(math_out, ", ");
        }
        return;
    }

    if (strchr("><", tok[0])) {
        if (is_unique(log_out, tok)) {
            strcat(log_out, tok);
            strcat(log_out, ", ");
        }
        return;
    }

    if (is_unique_others(oth_out, tok)) {
        strcat(oth_out, tok);
        strcat(oth_out, " ");
    }
}

void trim_output(char *output) {
    size_t len = strlen(output);
    if (len >= 2 && output[len - 2] == ',' && output[len - 1] == ' ') {
        output[len - 2] = '\0';
    }
}

void parse(FILE *f) {
    char ch, tok[MAX_TOK];
    int tok_len = 0;

    char kw_out[MAX_OUT] = "Keywords: ";
    char id_out[MAX_OUT] = "Identifiers: ";
    char math_out[MAX_OUT] = "Math Operators: ";
    char log_out[MAX_OUT] = "Logical Operators: ";
    char num_out[MAX_OUT] = "Numerical Values: ";
    char oth_out[MAX_OUT] = "Others: ";

    while ((ch = fgetc(f)) != EOF) {
        if (isspace(ch)) {
            if (tok_len) {
                tok[tok_len] = '\0';
                classify(tok, kw_out, id_out, math_out, log_out, num_out, oth_out);
                tok_len = 0;
            }
            continue;
        }

        if (strchr(",;(){}[]", ch)) {
            if (tok_len) {
                tok[tok_len] = '\0';
                classify(tok, kw_out, id_out, math_out, log_out, num_out, oth_out);
                tok_len = 0;
            }
            char sym[2] = {ch, '\0'};
            classify(sym, kw_out, id_out, math_out, log_out, num_out, oth_out);
            continue;
        }

        tok[tok_len++] = ch;
        if (tok_len >= MAX_TOK - 1) {
            tok[tok_len] = '\0';
            classify(tok, kw_out, id_out, math_out, log_out, num_out, oth_out);
            tok_len = 0;
        }
    }

    if (tok_len) {
        tok[tok_len] = '\0';
        classify(tok, kw_out, id_out, math_out, log_out, num_out, oth_out);
    }

    trim_output(kw_out);
    trim_output(id_out);
    trim_output(math_out);
    trim_output(log_out);
    trim_output(num_out);

    size_t len = strlen(oth_out);
    if (len > 0 && oth_out[len - 1] == ' ') {
        oth_out[len - 1] = '\0';
    }

    printf("%s\n%s\n%s\n%s\n%s\n%s\n", kw_out, id_out, math_out, log_out, num_out, oth_out);
}

int main() {
    FILE *file = fopen("input.txt", "r"); // Using relative path

    if (!file) {
        perror("Error opening input.txt. Make sure the file exists in the same directory as the executable.");
        return 1;
    }

    parse(file);
    fclose(file);
    return 0;
}
