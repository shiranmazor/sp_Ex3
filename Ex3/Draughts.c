#include "Draughts.h"
#include <string.h>


//globals:
int maxDepth;
int computerColor = 0;//by default the use played is white = 1

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
	return c - 96;
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
	
	printf('%s', WELCOME_TO_DRAUGHTS);
	settingState(board);

	//print_message(WRONG_MINIMAX_DEPTH);
	//perror_message("TEST");
	return 0;
}

void settingState(char board[BOARD_SIZE][BOARD_SIZE])
{
	init_board(board);
	printf('%s', ENTER_SETTINGS);
	char *command = getString(stdin, 10);
	while (strcmp(command, "quit") != 0 || strcmp(command, "start") != 0)
	{

		command = getString(stdin, 10);
	}
	if (strcmp(command, "start") == 0)
	{
		//call game state on the board
	}
	if(strcmp(command, "quit") == 0)
	{
		//TODO:clean all memory
		exit(0);
	}

}

void executeSettingCmd(char board[BOARD_SIZE][BOARD_SIZE],char* input)
{
	//trim all spaces from start and end:
	input = trimwhitespace(input);
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
		perror("memory allocation while spiliting has failed!");
		assert(arr != NULL);
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
				perror("memory allocation while spiliting has failed!");
				assert((*arr)[i] != NULL);
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
		perror("memory allocation while spiliting has failed!");
		assert((*arr)[i] != NULL);
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

