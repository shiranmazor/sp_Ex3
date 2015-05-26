#ifndef DRAUGHTS_
#define DRAUGHTS_

#include<stdio.h>


#define WHITE_M 'm'
#define WHITE_K 'k'
#define BLACK_M 'M'
#define BLACK_K 'K'
#define EMPTY ' '
#define WHITE  1
#define BLACK  0

#define BOARD_SIZE 10

typedef char** board_t;
#define WELCOME_TO_DRAUGHTS "Welcome to Draughts!\n"
#define ENTER_SETTINGS "Enter game settings:\n" 
#define WRONG_MINIMAX_DEPTH "Wrong value for minimax depth. The value should be between 1 to 6\n"
#define WRONG_POSITION "Invalid position on the board\n"
#define NO_DICS "The specified position does not contain your piece\n"
#define ILLEGAL_COMMAND "Illegal command, please try again\n"
#define ILLEGAL_MOVE "Illegal move\n"
#define WROND_BOARD_INITIALIZATION "Wrong board initialization\n"

#define ENTER_YOUR_MOVE "Enter your move:\n" 
#define perror_message(func_name) (fprintf(stderr, "Error: standard function %s has failed\n", func_name))
#define print_message(message) (printf("%s", message));


typedef struct Pos Pos;
typedef struct Move Move;
int getIntValue(char c);
int isPlayerStuck(char board[BOARD_SIZE][BOARD_SIZE], int color);
void settingState(char board[BOARD_SIZE][BOARD_SIZE]);
char* getString(FILE* fp, size_t size);
char *trimwhitespace(char *str);
int split(char *str, char c, char ***arr);
void executeSettingCmd(char board[BOARD_SIZE][BOARD_SIZE], char* input);
void freeArray(char** arrMul, int c);
void reduceSpaces(char *str);
char* replace(char *s, char ch, char *repl);
int formatPos(char* pos_input, char **arr);

void print_board(char board[BOARD_SIZE][BOARD_SIZE]);
void init_board(char board[BOARD_SIZE][BOARD_SIZE]);
void clear_board(char board[BOARD_SIZE][BOARD_SIZE]);
void set_minimax_depth(int depth);
void remove_disc(char board[BOARD_SIZE][BOARD_SIZE], char* input);
void set_disc(char board[BOARD_SIZE][BOARD_SIZE], char* pos_input, char* color, char* type);

#endif  
