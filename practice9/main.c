#include <stdio.h>
#include <string.h>
#include <ctype.h>

int is_palindrome(const char *str, int *case_sensitive) {
    int left = 0, right = strlen(str) - 1;
    int case_mismatch = 0;

    while (left < right) {
        while (left < right && isspace((unsigned char)str[left])) left++;
        while (left < right && isspace((unsigned char)str[right])) right--;

        if (left < right) {
            if (tolower((unsigned char)str[left]) != tolower((unsigned char)str[right])) return 0;
            if (str[left] != str[right]) case_mismatch = 1;
            left++;
            right--;
        }
    }
    *case_sensitive = !case_mismatch;
    return 1;
}

int main(void) {
    char line[10000];
    int line_number = 0;

    printf("Enter string:\n");
    while (fgets(line, sizeof(line), stdin)) {
        line_number++;

        size_t len = strlen(line);
        if (len == 0 || (len == 1 && line[0] == '\n')) {
            printf("Invalid input.\n");
            return 1;
        }

        if (line[len - 1] != '\n') {
            printf("Invalid input.\n");
            return 1;
        }
        line[len - 1] = '\0';
        char *start = line;
        while (*start && isspace((unsigned char)*start)) start++;
        char *end = line + strlen(line) - 1;
        while (end >= start && isspace((unsigned char)*end)) *end-- = '\0';

        if (*start == '\0') {
            printf("Invalid input.\n");
            return 1;
        }

        int case_sensitive = 0;
        if (is_palindrome(start, &case_sensitive)) {
            if (case_sensitive) printf("String is a palindrome (case-sensitive).\n");
            else printf("String is a palindrome (case-insensitive).\n");
        }
        else printf("String is not a palindrome.\n");

    }

    return 0;
}

