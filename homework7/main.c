#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

int directions[4][2] = { //for computations of moves for kings
        {1, 1},   // bottom-right
        {-1, 1},  // bottom-left
        {1, -1},  // top-right
        {-1, -1}  // top-left
};

struct Piece{
    char letter; //is stored as it was inputted, but its value is bigger by 97 than address of element in board array, because 'a' is 0
    int number; //the same as letter, but it is bigger by 1, so 'a1' is first element representing board[0][0]
    bool is_king;
};

int opposite_direction(int diagonal){
    switch(diagonal){
        case 0: return 3;
        case 1: return 2;
        case 2: return 1;
        case 3: return 0;
        default: return -1;
    }
}

typedef struct Move{
    char end_l;
    int end_n;
}Move;

bool check_coord(int board_size, int number, char letter){ //check if the piece is positioned on the board
    return (number <= board_size && letter < (board_size + 97) && number >= 1 && letter >= 97);
}

void print_moves(struct Move** moves,const int num_moves, struct Piece piece){ //to print moves where black piece(s) is (are) captured
    printf("%c%d ", piece.letter, piece.number);
    for(int i = 0; i < num_moves; i++)
        printf("-> %c%d ", moves[i]->end_l, moves[i]->end_n);

    printf("+%d\n", num_moves);
}

struct Move check_daigonal(int size, char** board, int row, int col, int row_inc, int col_inc){ //checks diagonal for possibility of capturing black piece
    struct Move res = {(char)size, size}; //if we have not found any black piece
    int current_row = row + row_inc, current_col = col + col_inc;
    while(check_coord(size, current_col, (char)(current_row))){ //check every cell on diagonal until we get to the edge of the board
        if(board[current_row-97][current_col-1] == 'b' && check_coord(size, current_col+col_inc, (char)(current_row+row_inc))) {// or we get the black piece and there is the empty cell after it
            if (board[current_row + row_inc - 97][current_col + col_inc - 1] == '\0') {
                res.end_l = (char) (current_row + row_inc);
                res.end_n = current_col + col_inc;
                break;
            }
        }
        else if(board[current_row-97][current_col-1] == 'w' || board[current_row-97][current_col-1] == 'W') //or if diagonal is blocked by white piece
            break;
        current_row += row_inc;
        current_col += col_inc;
    }
    return res;
}

void captured_piece(int size, char** board, int num_moves, struct Piece piece, int diagonal, struct Move** moves, int index, int* total_m); //function for code optimization

void king_moves(struct Move** moves, int size, char** board, int num_moves, struct Piece piece, int diagonal, int* total_m){ //recursive function for kings
    print_moves(moves, num_moves, piece);
    (*total_m)++;
    for (int i = 0; i < 4; i++) {
        int dx = directions[i][0];
        int dy = directions[i][1];
        if(i == diagonal){ //we are moving on the same diagonal
            for (int j = 1 ;check_coord(size, moves[num_moves-1]->end_n + j * dy, (char) (moves[num_moves-1]->end_l + j * dx)); j++) { //check cell if it is in bounds
                if (board[moves[num_moves-1]->end_l + j * dx - 97][moves[num_moves-1]->end_n + j * dy - 1] == 'w' || //if we got white piece - there are no possibility of moving further on diagonal
                    board[moves[num_moves-1]->end_l + j * dx - 97][moves[num_moves-1]->end_n + j * dy - 1] == 'W')
                    break;
                else if (board[moves[num_moves-1]->end_l + j * dx - 97][moves[num_moves-1]->end_n + j * dy - 1] == '\0') { //we got empty cell
                    int tmp_n = moves[num_moves-1]->end_n; //we just explore every empty cell on having black pieces at their perpendicular diagonals
                    char tmp_l = moves[num_moves-1]->end_l;
                    moves[num_moves-1]->end_n = tmp_n + j * dy;
                    moves[num_moves-1]->end_l = (char)(tmp_l + j * dx);
                    king_moves(moves, size, board, num_moves, piece, i, total_m); //change to check_diagonal
                    //discard changes for next iterations
                    moves[num_moves-1]->end_l = tmp_l;
                    moves[num_moves-1]->end_n = tmp_n;
                }
                else { //we got black piece
                    if (check_coord(size, moves[num_moves-1]->end_n + (j + 1) * dy, (char) (moves[num_moves-1]->end_l + (j + 1) * dx))) //check if cell after it exists
                        if (board[moves[num_moves-1]->end_l + (j + 1) * dx - 97][moves[num_moves-1]->end_n + (j + 1) * dy - 1] == '\0') //check if cell after it is empty
                            captured_piece(size, board, num_moves, piece, i, moves, j, total_m);
                    break; //go to next diagonal
                }
            }
        }
        else if(i != opposite_direction(diagonal) && i != diagonal){ //the diagonal is perpendicular to the previous direction
            // check diagonal in the current direction to have black piece which could be captured
            struct Move tmp = check_daigonal(size, board, moves[num_moves-1]->end_l, moves[num_moves-1]->end_n, dx, dy);
            if ((tmp).end_l != size) {  // if black piece was found
                moves[num_moves] = &tmp;
                board[moves[num_moves]->end_l - dx - 97][moves[num_moves]->end_n - dy - 1] = '\0'; // clear the cell with captured piece
                king_moves(moves, size, board, num_moves+1, piece, i,total_m); //call recursion
                //discard changes for further iterations
                board[moves[num_moves]->end_l - dx - 97][moves[num_moves-1]->end_n - dy - 1] = 'b';
            }
        }
    }
}

void captured_piece(int size, char** board, int num_moves, struct Piece piece, int diagonal, struct Move** moves, int index, int* total_m){
    int dx = directions[diagonal][0];
    int dy = directions[diagonal][1];

    board[moves[num_moves-1]->end_l + index * dx - 97][moves[num_moves-1]->end_n + index * dy - 1] = '\0'; // clear the cell with captured piece
    moves[num_moves] = &(struct Move) {(char)(moves[num_moves-1]->end_l + (index + 1) * dx), (int)(moves[num_moves-1]->end_n + (index + 1) * dy)}; //add new move to the array
    king_moves(moves, size, board, num_moves+1, piece, diagonal,total_m); //call recursion
    //discard changes for further iterations
    board[moves[num_moves-1]->end_l + index * dx - 97][moves[num_moves-1]->end_n + index * dy - 1] = 'b';
}

void ordin_moves(struct Piece piece, struct Move** moves, int num_moves, int size, char** board, int* total_m){//recursive function for ordinary pieces
    print_moves(moves, num_moves, piece);
    (*total_m)++;
    if(moves[num_moves-1]->end_n < (size-2)) //check for edge case (if we already at the bottom of the board)
        for (int i = 0; i < 2; i++) { //go for every direction
            int dx = directions[i][0];
            int dy = directions[i][1];

            if (check_coord(size, moves[num_moves - 1]->end_n + dy, (char) (moves[num_moves - 1]->end_l + dx))) { //check for edge case
                 if (board[moves[num_moves - 1]->end_l + dx - 97][moves[num_moves - 1]->end_n + dy - 1] == 'b')  // if cell contains black piece, we check whether there are possibility to capture it (there is empty cell after it)
                    if (check_coord(size, moves[num_moves - 1]->end_n + 2 * dy, (char) (moves[num_moves - 1]->end_l + 2 * dx)))
                        if (board[moves[num_moves - 1]->end_l - 97 + 2 * dx][moves[num_moves - 1]->end_n + 2 * dy - 1] == '\0') { //if yes, write it to moves and call the recursion
                            moves[num_moves] = &(struct Move){(char)(moves[num_moves - 1]->end_l + 2 * dx), (int)(moves[num_moves - 1]->end_n + 2 * dy)};
                            ordin_moves(piece, moves, num_moves+1, size, board, total_m); // recursion function
                        }
            }
        }
}

void compute_moves(int size, char** board, struct Piece piece, int *total_moves) {
    if (!piece.is_king) { //if ordinary piece
        if (piece.number != (size - 1)) { //check move lower
            for (int i = 0; i < 2; i++) {
                int dx = directions[i][0];
                int dy = directions[i][1];

                if (check_coord(size, piece.number + dy, (char) (piece.letter + dx))) {
                    if (board[piece.letter - 97 + dx][piece.number - 1 + dy] == '\0'){  // if neigbouring cell is empty, just print the simple move
                        printf("%c%d -> %c%d\n", piece.letter, piece.number, (char) (piece.letter + dx), piece.number + dy);
                        (*total_moves)++;
                    }
                    else if (board[piece.letter - 97 + dx][piece.number - 1 + dy] == 'b')  // if it contains black piece, we check whether there are possibility to capture it (there is empty cell after it)
                        if (check_coord(size, (piece.number + 2 * dy), (char)(piece.letter + 2 * dx))){
                            if (board[piece.letter - 97 + (2 * dx)][piece.number + (2 * dy) - 1] == '\0') {
                                struct Move *moves = (struct Move *) malloc(size*size * sizeof(struct Move)); //we make array of moves with overestimated size and store this move to it (I tried to allocate at the beginning only memory for 1 move, and then at recursion to reallocate it if we will get more moves, but it turned out to be that when we use realloc, the address of memory changes and that causes a segmentation fault when we go out of recursion. So I used this approach)
                                moves[0] = (struct Move) {(char) (piece.letter + 2 * dx),(int) (piece.number + 2 * dy)};
                                ordin_moves(piece, &moves, 1, size, board, total_moves); // recursion function
                                free(moves);
                            }
                       }
                }
            }
        }
    } else { //if king
        for (int i = 0; i < 4; i++) {
            int dx = directions[i][0];
            int dy = directions[i][1];
            int j = 1;
            while (check_coord(size, piece.number + j * dy, (char) (piece.letter + j * dx))) { //iterate through evey cell in diagonal
                if (board[piece.letter + j * dx - 97][piece.number + j * dy - 1] == 'w' ||
                    board[piece.letter + j * dx - 97][piece.number + j * dy - 1] == 'W')
                    break;
                else if (board[piece.letter + j * dx - 97][piece.number + j * dy - 1] == '\0') {
                    printf("%c%d -> %c%d\n", piece.letter, piece.number, (piece.letter + j * dx), (piece.number + j * dy));
                    (*total_moves)++;
                    j++;
                } else { //we got black piece
                    if (check_coord(size, piece.number + (j + 1) * dy, (char) (piece.letter + (j + 1) * dx)))
                        if (board[piece.letter + (j + 1) * dx - 97][piece.number + (j + 1) * dy - 1] == '\0') { //and cell after it is empty
                            struct Move *moves = (struct Move *)malloc(size*size * sizeof(struct Move)); //allocate array of moves with overestimated size
                            board[piece.letter + j * dx - 97][piece.number + j * dy - 1] = '\0';
                            moves[0] = (struct Move) {(char) (piece.letter + 2 * dx),(int) (piece.number + 2 * dy)};
                            king_moves( &moves, size, board, 1, piece, i, total_moves); //recursion for king piece
                            board[piece.letter + j * dx - 97][piece.number + j * dy - 1] = 'b';
                            free(moves);
                        }

                    break;
                }
            }
        }

    }
}

bool is_on_black(struct Piece piece){ //check whether the piece is on black cell
    return (piece.number % 2) == 1 && ((piece.letter-96)%2) == 1;
}

int main(void) {
    int size;
    char team;
    int white_count = 0;
    struct Piece* white_t = (struct Piece*)malloc(10 * sizeof(struct Piece));

    printf("Board size:\n");

    if(scanf(" %d", &size) != 1){
        printf("Invalid input.\n");
        free(white_t);
        return 0;
    }
    if(size < 3 || size > 26){
        printf("Invalid input.\n");
        free(white_t);
        return 0;
    }

    char** board = (char**)malloc(size*sizeof(char *));
    for (int i = 0; i < size; i++)
        board[i] = (char *)calloc(size, sizeof(char)); //empty board cells would be '\0' or binary zero


    printf("Pieces:\n");
    while(scanf( " %c", &team) == 1 ){
        if(team == 'W' || team == 'w' || team == 'B' || team == 'b'){
            struct Piece piece;
            if(scanf(" %c%d", &piece.letter, &piece.number) != 2){
                printf("Invalid input.\n");
                free(white_t);
                return 0;
            }
            piece.is_king = (team == 'W' || team == 'B') != 0;
            if(!check_coord(size, piece.number, piece.letter) && !is_on_black(piece)){
                printf("Invalid input.\n");
                free(white_t);
                return 0;
            }
            if(board[piece.letter - 97][piece.number-1] != '\0'){
                    printf("Invalid input.\n");
                    free(white_t);
                    return 0;
            }

            if(team == 'W' || team == 'w'){
                white_count++;
                if(white_count > 10) white_t = (struct Piece *)realloc(white_t, white_count* sizeof(struct Piece));
                white_t[white_count-1] = piece;
                board[piece.letter - 97][piece.number-1] = (piece.is_king) ? 'W' : 'w';
            }
            else
                board[piece.letter - 97][piece.number-1] = 'b'; //there is no difference in black piece or black king for us
        }
        else{
            printf("Invalid input.\n");
            free(white_t);
            return 0;
        }
    }
    int total_m = 0; //variable of total moves (we increment it when we print the move)
    for(int i = 0; i < white_count; i++)
        compute_moves(size, board, white_t[i], &total_m);

    printf("Total moves: %d\n", total_m);
    //free the allocated memory
    free(white_t);
 /*   for(int i = 0; i < size; i++) {
        free(board[i]);
    }
    */
    free(board);
    return 0;
}