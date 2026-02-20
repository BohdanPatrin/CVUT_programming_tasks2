#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// recursive function for splitting sequence
long long split_seq(const char *sequence, long long start, long long length, long long only_count, char ***results,
                    int *number_of_res, int was_prev_even, const char *prev_num_str) { // prev_num_str - stores previous number as string, was_prev_even - 1 if is previous number was even; 0 - if odd
    if (start == length) { // edge case (entire sequence split)
        if (!only_count) {
            *results = (char **)realloc(*results, sizeof(char *) * (*number_of_res + 1));
            (*results)[*number_of_res] = strdup("");
            (*number_of_res)++;
        }
        return 1;
    }

    long long total_ways = 0; // total number of all combinations
    for (long long i = 1; i <= length - start; ++i) {
        char curr_num_str[i + 1];
        strncpy(curr_num_str, sequence + start, i); // extract substring
        curr_num_str[i] = '\0';

        int curr_num_even = (curr_num_str[i - 1] - '0') % 2 == 0;

        if (was_prev_even && strcmp(prev_num_str, curr_num_str) > 0) // rule of odd and even
            continue;

        char **sub_results = NULL;
        int num_of_sub_res = 0;

        // use recursion for splitting the remaining sequence
        long long ways = split_seq(sequence, start + i, length, only_count, &sub_results,
                                   &num_of_sub_res, curr_num_even, curr_num_str); // update prev_num_str and was_prev_even
        total_ways += ways;

        if (!only_count) { // append the current split to all sub-results
            for (int j = 0; j < num_of_sub_res; ++j) {
                char *newSplit;
                if (strlen(sub_results[j]) == 0) asprintf(&newSplit, "%s", curr_num_str);
                else asprintf(&newSplit, "%s,%s", curr_num_str, sub_results[j]);

                *results = (char **)realloc(*results, sizeof(char *) * (*number_of_res + 1));
                (*results)[*number_of_res] = newSplit;
                (*number_of_res)++;
            }
        }

        // free allocated memory
        for (int j = 0; j < num_of_sub_res; ++j)
            free(sub_results[j]);
        free(sub_results);
    }
    return total_ways;
}

void process_input(const char *sequence, int only_count) { // function to print result
    char **results = NULL;
    int num_of_res = 0;

    long long total_ways = split_seq(sequence, 0, strlen(sequence),
                                     only_count, &results, &num_of_res, 0, "");

    if (only_count) printf("Total: %lld\n", total_ways);
    else {
        for (int i = 0; i < num_of_res; ++i) {
            printf("* %s\n", results[i]);
            free(results[i]);
        }
        free(results);
        printf("Total: %lld\n", total_ways);
    }
}

int main() {
    char line[1000];
    printf("Sequence:\n");

    while (fgets(line, sizeof(line), stdin)) {
        char *trimmed = line; // trimmed is line without unnecessary spaces
        while (isspace((unsigned char)*trimmed)) trimmed++; // cutting spaces in the beginning
        char *end = trimmed + strlen(trimmed) - 1;
        while (end > trimmed && isspace((unsigned char)*end)) end--; // cutting in the end
        *(end + 1) = '\0';

        if (trimmed[0] != '?' && trimmed[0] != '#') {
            printf("Invalid input.\n");
            return 1;
        }

        char *sequence = trimmed + 1; // sequence - trimmed without '?' or '#', so just sequence of digits

        while (isspace((unsigned char)*sequence)) sequence++; // skip spaces between first char and sequence

        // checking for invalid input
        if (strlen(sequence) == 0 || !isdigit(sequence[0])) {
            printf("Invalid input.\n");
            return 1;
        }

        for (size_t i = 0; i < strlen(sequence); ++i)
            if (!isdigit(sequence[i])) {
                printf("Invalid input.\n");
                return 1;
            }

        if (trimmed[0] == '?') process_input(sequence, 0); // 0 = print splits
        else if (trimmed[0] == '#') process_input(sequence, 1); // 1 = only count splits
        else {
            printf("Invalid input.\n");
            return 1;
        }
    }
    return 0;
}
