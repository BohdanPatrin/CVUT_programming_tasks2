#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct {
    int x, y;
} Direction;

// array of all possible directions
const Direction directions[] = {
        {0, 1}, {0, -1}, {1, 0}, {-1, 0},
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1}
};

//global variables which stores puzzle and struck-out words
int rows = 0, cols = 0;
char **puzzle = NULL;
int **struck_out = NULL;

void error_exit() { // function for invalid input cases
    printf("Invalid input.\n");
    exit(1);
}

void read_puzzle() { // function to read puzzle, check it for validity and store it
    char *line = NULL;
    size_t len = 0;

    // read lines until we get an empty line separating the puzzle and commands
    while (getline(&line, &len, stdin) != -1) {
        if (line[0] == '\n') break;

        // trim newline
        size_t line_len = strlen(line);
        if (line[line_len - 1] == '\n') line[--line_len] = '\0';

        // check for invalid input
        if (line_len == 0 || (rows > 0 && (int)line_len != cols)) error_exit();
        if (rows == 0) cols = line_len;

        for (size_t i = 0; i < line_len; i++)
            if (!islower(line[i]) && line[i] != '.') error_exit();

        // store new line in the puzzle
        puzzle = (char **)realloc(puzzle, (rows + 1) * sizeof(char *));
        puzzle[rows] = strdup(line);
        rows++;
    }
    if (rows == 0 || cols == 0) error_exit();

    // initialize struck out markers
    struck_out = (int **)malloc(rows * sizeof(int *));
    for (int i = 0; i < rows; i++) struck_out[i] = (int *)calloc(cols, sizeof(int));

    free(line);
}

// function to сheck if a word fits starting at (x, y) in direction (dx, dy)
int fits(const char *word, int x, int y, int dx, int dy) {
    int len = strlen(word);
    for (int i = 0; i < len; i++, x += dx, y += dy)
        if (x < 0 || y < 0 || x >= rows || y >= cols || puzzle[x][y] != word[i])
            return 0;

    return 1;
}

// function to mark word as struck out
void mark_word(const char *word, int x, int y, int dx, int dy) {
    int len = strlen(word);
    for (int i = 0; i < len; i++, x += dx, y += dy) struck_out[x][y] = 1;
}


int find_word(const char *word, int strike){ // function to find word in puzzle
    int count = 0;

    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++) {
            if(puzzle[i][j] == word[0]) // check word from first char
               for (int d = 0; d < 8; d++) // if they are equal check for every direction
                  if (fits(word, i, j, directions[d].x, directions[d].y)) {
                    count++;
                    if (strike) mark_word(word, i, j, directions[d].x, directions[d].y); // mark word if it is needed
                  }
        }
    return count;
}

void display_solution() { // function for prinitng '?' solution
    printf("Solution:\n");
    int printed = 0;

    for (int i = 0; i < rows; i++)
        for (int j = 0; j < cols; j++)
            if (!struck_out[i][j] && puzzle[i][j] != '.') {
                putchar(puzzle[i][j]);
                printed++;
                if (printed % 60 == 0) putchar('\n');
            }
    if (printed % 60 != 0) putchar('\n');
}

void process_commands() { // function to process input after we get puzzle
    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, stdin) != -1) {
        size_t line_len = strlen(line);
        if (line[line_len - 1] == '\n') line[--line_len] = '\0';
        //print the solution depending on the first input symbol
        if (line[0] == '?') {
            if (line_len != 1) error_exit();
            display_solution();
        }
        else if (line[0] == '-' || line[0] == '#') {
            char *word = line + 1;
            while (isspace(*word)) word++;
            if (strlen(word) < 2 ||
            strspn(word, "abcdefghijklmnopqrstuvwxyz") != strlen(word)) //check if every char are in alphabet
                error_exit();

            int count = find_word(word, line[0] == '-');
            printf("%s: %dx\n", word, count);
        }
        else error_exit();
    }
    free(line);
}

int main() {
    printf("Wordfind puzzle:\n");
    read_puzzle(); // read and store puzzle
    process_commands(); // then process input as commands 

    // clean memory
    for (int i = 0; i < rows; i++) {
        free(puzzle[i]);
        free(struck_out[i]);
    }
    free(puzzle);
    free(struck_out);
    return 0;
}
