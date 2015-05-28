#include "Draughts.h"
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>


//globals:
int minimax_depth = 1;
int computer_color = BLACK;//by default the use played is white = 1

struct Pos{
	char x;
	int y;
};

struct Move{
	Pos currPos;
	int eat;
	Move *next;
};

int getIntValue(char c)
{
	return c - 'a';
}

/*
reading string input from user and relloc the char pointer
size - initial size of the pointer
*/
char* getString(FILE* fp, size_t size)
{
	char *str;
	int ch;
	size_t len = 0;
	str = realloc(NULL, sizeof(char)*size);
	if (!str)
		return str;
	while (EOF != (ch = fgetc(fp)) && ch != '\n')
	{
		str[len++] = ch;
		if (len == size)
		{
			str = realloc(str, sizeof(char)*(size += 16));
			if (!str)
				return str;
		}
	}
	str[len++] = '\0';
	return str;
}


int main()
{
	char board[BOARD_SIZE][BOARD_SIZE];
	
	printf("%s", WELCOME_TO_DRAUGHTS);
	settingState(board);

	//print_message(WRONG_MINIMAX_DEPTH);
	//perror_message("TEST");
	return 0;
}

void settingState(char board[BOARD_SIZE][BOARD_SIZE])
{
	init_board(board);
	printf("%s", ENTER_SETTINGS);
	char *command = getString(stdin, 10);
	while (strcmp(command, "quit") != 0 || strcmp(command, "start") != 0)
	{
		reduceSpaces(command);
		executeSettingCmd(board, command);

		command = getString(stdin, 10);
	}
	if (strcmp(command, "start") == 0)
	{
		//call game state on the board
	}
	else if(strcmp(command, "quit") == 0)
	{
		//TODO:clean all memory
		exit(0);
	}

}

void executeSettingCmd(char board[BOARD_SIZE][BOARD_SIZE],char* input)
{
	//trim all spaces from start and end:
	input = trimwhitespace(input);
	char **arr = NULL;
	int arr_len = split(input, ' ', &arr);
	if (arr_len == 1)//print,clear
	{
		if (strcmp(arr[0], "clear") == 0)
		{
			//call clear
			clear_board(board);
		}
		else if (strcmp(arr[0], "print") == 0)
		{
			print_board(board);
		}
	}
	else if (arr_len == 2)//rm <x,y>, user_color x, minimax_depth x
	{
		if (strcmp(arr[0], "minimax_depth") ==0)
		{
			set_minimax_depth(atoi(arr[1]));
		}
		else if (strcmp(arr[0], "user_color") == 0)
		{
			if (strcmp(arr[1], "black") == 0)
				computer_color = WHITE;
			else if (strcmp(arr[1], "white") == 0)
				computer_color = BLACK;
		}
		else if (strcmp(arr[0], "rm") == 0)
		{
			remove_disc(board, arr[1]);
		}

	}
	else if (arr_len == 4 && strcmp(arr[0],"set") == 0)//set <x,y> a b
	{ 
		set_disc(board, arr[1], arr[2], arr[3]);
	}
	else
	{
		printf("%s", ILLEGAL_COMMAND);
	}
		

	freeArray(arr, arr_len);
}

char* replace(char *s, char ch, char *repl) 
{
	int count = 0;
	const char *t;
	for (t = s; *t; t++)
		count += (*t == ch);

	size_t rlen = strlen(repl);
	char *res = malloc(strlen(s) + (rlen - 1)*count + 1);
	if (res == NULL)
	{
		perror("memory allocation in replace has failed!");
		assert(res != NULL);
	}

	char *ptr = res;
	for (t = s; *t; t++) {
		if (*t == ch) {
			memcpy(ptr, repl, rlen);
			ptr += rlen;
		}
		else {
			*ptr++ = *t;
		}
	}
	*ptr = 0;
	return res;
}


char *trimwhitespace(char *str)
{
	char *end;

	// Trim leading space
	while (isspace(*str)) str++;

	if (*str == 0)  // All spaces?
		return str;

	// Trim trailing space
	end = str + strlen(str) - 1;
	while (end > str && isspace(*end)) end--;

	// Write new null terminator
	*(end + 1) = 0;

	return str;
}

int split(char *str, char c, char ***arr)
{
	int count = 1;
	int token_len = 1;
	int i = 0;
	char *p; char *t;
	p = str;
	//1 - counting delimiter\splitted output string
	while (*p != '\0')
	{
		if (*p == c)
			count++;
		p++;
	}
	int num = sizeof(char*)*count;
	*arr = (char**)malloc(num);
	if (*arr == NULL)
	{
		perror_message("split");
		//TODO:free all alocated memory of the program
		exit(0);
	}
	p = str;
	//2 - allocating arr space
	while (*p != '\0')
	{
		if (*p == c)
		{
			int num2 = sizeof(char)*token_len;
			(*arr)[i] = (char*)malloc(num2);
			if ((*arr)[i] == NULL)
			{
				perror_message("split");
				//TODO:free all alocated memory of the program
				exit(0);
			}

			token_len = 0;
			i++;
		}
		p++;
		token_len++;
	}

	int num3 = sizeof(char)*token_len;
	(*arr)[i] = (char*)malloc(num3);
	if ((*arr)[i] == NULL)
	{
		perror_message("split");
		//TODO:free all alocated memory of the program
		exit(0);
	}

	i = 0;
	p = str;
	t = ((*arr)[i]);
	//3 - splitting the pointer by delimiter
	while (*p != '\0')
	{
		if (*p != c && *p != '\0')
		{
			*t = *p;
			t++;
		}
		else
		{
			*t = '\0';
			i++;
			t = ((*arr)[i]);
		}
		p++;
	}

	*t = '\0';
	i++;
	return count;

}

void freeArray(char** arrMul, int c)
{
	if (arrMul == NULL)
		return;

	for (int i = 0; i < c; ++i) {
		if (arrMul[i] != NULL)
			free(arrMul[i]);
	}

	free(arrMul);
}

void reduceSpaces(char *str)
{
	char *dest = str;  

	/* While we're not at the end of the string, loop... */
	while (*str != '\0')
	{
		/* Loop while the current character is a space, AND the next
		* character is a space
		*/
		while (*str == ' ' && *(str + 1) == ' ')
			str++; 
		*dest++ = *str++;
	}

	*dest = '\0';
}

void print_line(){
	int i;
	printf("  |");
	for (i = 1; i < BOARD_SIZE * 4; i++){
		printf("-");
	}
	printf("|\n");
}

void print_board(char board[BOARD_SIZE][BOARD_SIZE])
{
	int i, j;
	print_line();
	for (j = BOARD_SIZE - 1; j >= 0; j--)
	{
		printf((j < 9 ? " %d" : "%d"), j + 1);
		for (i = 0; i < BOARD_SIZE; i++){
			printf("| %c ", board[i][j]);
		}
		printf("|\n");
		print_line();
	}
	printf("   ");
	for (j = 0; j < BOARD_SIZE; j++){
		printf(" %c  ", (char)('a' + j));
	}
	printf("\n");
}

void clear_board(char board[BOARD_SIZE][BOARD_SIZE])
{
	int i, j;
	for (i = 0; i < BOARD_SIZE; i++)
	{
		for (j = 0; j < BOARD_SIZE; j++)
		{
			if (board[i][j] != EMPTY)
				board[i][j] = EMPTY;
		}
	}
}

void init_board(char board[BOARD_SIZE][BOARD_SIZE]){
	int i, j;
	for (i = 0; i < BOARD_SIZE; i++){
		for (j = 0; j < BOARD_SIZE; j++){
			if ((i + j) % 2 == 0){
				if (j <= 3){
					board[i][j] = WHITE_M;
				}
				else if (j >= 6){
					board[i][j] = BLACK_M;
				}
				else{
					board[i][j] = EMPTY;
				}
			}
			else{
				board[i][j] = EMPTY;
			}
		}
	}
}

void set_minimax_depth(int depth)
{
	if (depth < 1 || depth > 6)
		printf("%s", WRONG_MINIMAX_DEPTH);
	else
		minimax_depth = depth;
}


int formatPos(char* pos_input, char ***arr)
{
	pos_input = replace(pos_input, '<', "");
	pos_input = replace(pos_input, '>', "");
	int arr_len = split(pos_input, ',', arr);
	return arr_len;
}
void remove_disc(char board[BOARD_SIZE][BOARD_SIZE], char* input)
{
	//input is <x,y>
	char **arr = NULL;
	int arr_len = formatPos(input, &arr);
	char x = arr[0][0];
	int y = atoi(arr[1]);
	if (x < 97 || x > 106 || y<1 || y>10 || y % 2 == 0)
	{
		printf("%s", WRONG_POSITION);
	}
	else
	{
		int x_int = getIntValue(x);
		board[x_int][y-1] = EMPTY;
	}
	freeArray(arr, arr_len);
}

void set_disc(char board[BOARD_SIZE][BOARD_SIZE],  char* pos_input, char* color, char* type)
{
	char **arr = NULL;
	int arr_len = formatPos(pos_input, &arr);
	char x = arr[0][0];
	int y = atoi(arr[1]);
	if (x < 97 || x > 106 || y<1 || y>10 || y % 2 == 0)
	{
		printf("%s", WRONG_POSITION);
	}
	else
	{
		int x_int = getIntValue(x);
		if (strcmp(color, "black") == 0)
		{
			if (strcmp(type, "k") == 0)
				board[x_int][y - 1] = BLACK_K;
			else if (strcmp(type, "m") == 0)
				board[x_int][y - 1] = BLACK_M;
		}
		else if (strcmp(color, "white") == 0)
		{
			if (strcmp(type, "k") == 0)
				board[x_int][y - 1] = WHITE_K;
			else if (strcmp(type, "m") == 0)
				board[x_int][y - 1] = WHITE_M;
		}
	}
}

int score(char board[BOARD_SIZE][BOARD_SIZE], int player_color)
{
	if (player_color == WHITE)
	{

	}

}
