#include "Draughts.h"
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int objectsInMemory = 0;
void *myMalloc(size_t size) {
	//printf("allocated memory!\n");
	objectsInMemory++;
	return malloc(size);
}

void myFree(void  *memory) {
	if (memory == NULL)
		return;
	objectsInMemory--;
	//printf("freed memory!\n");
	//printf("%p\n", memory);
	free(memory);
}

void *myCalloc(void  *memory, size_t size) {
	objectsInMemory++;
	//printf("calloced memory!\n");
	return calloc(memory, size);
}

void *myRealloc(void  *memory, size_t newSize) {
	if (memory == NULL)
		objectsInMemory++;
	//printf("relloced memory!\n");
	return realloc(memory, newSize);
}

#define malloc(x) myMalloc(x)
#define free(x) myFree(x)
#define calloc(x,y) myCalloc(x,y)
#define realloc(x,y) myRealloc(x,y)

//globals:
int minimax_depth = 1;
int computer_color = BLACK;//by default the use played is white = 1

struct PosNode{
	Pos *pos;
	PosNode *next;
};

struct Pos{
	char x;
	int y;
};

struct Move{
	Pos *currPos;
	int eat;
	PosNode *dest;
};

int getIntValue(char c)
{
	return c - 'a';
}

char *str_replace(char *orig, char *rep, char *with) {
	char *result; // the return string
	char *ins;    // the next insert point
	char *tmp;    // varies
	int len_rep;  // length of rep
	int len_with; // length of with
	int len_front; // distance between rep and end of last rep
	int count;    // number of replacements

	if (!orig)
		return NULL;
	if (!rep)
		rep = "";
	len_rep = strlen(rep);
	if (!with)
		with = "";
	len_with = strlen(with);

	ins = orig;
	for (count = 0; (tmp = strstr(ins, rep)); ++count) {
		ins = tmp + len_rep;
	}

	tmp = result = malloc(strlen(orig) + (len_with - len_rep) * count + 1);

	if (!result)
		return NULL;

	while (count--) {
		ins = strstr(orig, rep);
		len_front = ins - orig;
		tmp = strncpy(tmp, orig, len_front) + len_front;
		tmp = strcpy(tmp, with) + len_with;
		orig += len_front + len_rep;
	}
	strcpy(tmp, orig);
	return result;
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

//removes any leading/trailing whitespaces
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

/*
recieves <x,y> string and return Pos object
*/
Pos * formatPos(char* pos_input)
{
	char **arr = NULL;
	Pos* pos = malloc(sizeof(Pos));
	pos_input = replace(pos_input, '<', "");
	pos_input = replace(pos_input, '>', "");
	int arr_len = split(pos_input, ',', &arr);
	pos->x = arr[0][0];
	pos->y = atoi(arr[1]);
	freeArray(arr, arr_len);

	
	if (pos->x < 97 || pos->x > 106 || pos->y<1 || pos->y>10 || ((pos->x+pos->y) %2 !=0))
	{
		printf("%s", WRONG_POSITION);
		return NULL; //todo - is this OK? should be handled by caller?
	}
	
	return pos;
}

void remove_disc(char board[BOARD_SIZE][BOARD_SIZE], char* input)
{
	//input is <x,y>
	Pos *pos = formatPos(input);

	if (pos)
	{
		int x_int = getIntValue(pos->x);
		board[x_int][pos->y-1] = EMPTY;
	}
	//freeArray(arr, arr_len);
}

/*
This function assumes the command is a valid move command, parses it and return a move object
move <x,y> to <i,j>[<k,l>…]
*/
Move * parseMoveCommand(char *command)
{
	trimwhitespace(command);

	//remove double spaces
	char *toFree = NULL;
	int cnt = 0;
	while (strstr(command, "  "))
	{
		cnt++;
		toFree = command;
		command = str_replace(command, "  ", " ");
		if (command != NULL && cnt>1)
			free(toFree);
	}

	Move *move = (Move*) malloc(sizeof(Move));
	//TODO trim and remove double spcaes

	char** arr = NULL;
	int arrLen = split(command, ' ', &arr);

	move->currPos  = formatPos(arr[1]);
	if (move->currPos)//position was invalid
		return NULL;
	
	char **destArr = NULL;
	int destArrLen = split(arr[3], '>',&destArr);

	PosNode *lastPos = NULL;
	for (int i = 0; i < destArrLen-1; i++)
	{
		PosNode *posToAdd = malloc(sizeof(PosNode));
		posToAdd->pos = formatPos(destArr[i]);
		if (posToAdd->pos == NULL)
			return NULL;
		posToAdd->next = NULL;

		if (i == 0)
		{
			move->dest = posToAdd;
			lastPos = move->dest;
		}
		else
		{
			lastPos->next = posToAdd;
			lastPos = lastPos->next;
		}
	}

	return move;
}

void set_disc(char board[BOARD_SIZE][BOARD_SIZE],  char* pos_input, char* color, char* type)
{
	//char **arr = NULL;
	Pos *pos = formatPos(pos_input);
	//char x = arr[0][0];
	//int y = atoi(arr[1]);
	if (pos == NULL)
	{
		printf("%s", WRONG_POSITION);
	}
	else
	{
		int x_int = getIntValue(pos->x);
		if (strcmp(color, "black") == 0)
		{
			if (strcmp(type, "k") == 0)
				board[x_int][pos->y - 1] = BLACK_K;
			else if (strcmp(type, "m") == 0)
				board[x_int][pos->y - 1] = BLACK_M;
		}
		else if (strcmp(color, "white") == 0)
		{
			if (strcmp(type, "k") == 0)
				board[x_int][pos->y - 1] = WHITE_K;
			else if (strcmp(type, "m") == 0)
				board[x_int][pos->y - 1] = WHITE_M;
		}
	}
}

void unitTests()
{
	char cmd[] = "move <b,3> to <a,2>";
	trimwhitespace(cmd);
	Move *res = parseMoveCommand(cmd);
	assert(res->currPos->x == 'b' && res->currPos->y == 3);
	assert(res->dest->pos->x == 'a' && res->dest->pos->y == 2);
	free(res);

	char cmd2[] = "move <j,10> to <a,3>";
	res = parseMoveCommand(cmd2);
	assert(res->currPos->x == 'j' && res->currPos->y == 10);
	free(res);
	
	char cmd3[] = "move <j,10> to <a,3><b,10><c,8>";
	res = parseMoveCommand(cmd3);
	assert(res->currPos->x == 'j' && res->currPos->y == 10);
	assert(res->dest->pos->x == 'a' && res->dest->pos->y == 3);
	assert(res->dest->next->pos->x == 'b' && res->dest->next->pos->y == 10);
	assert(res->dest->next->next->pos->x == 'c' && res->dest->next->next->pos->y == 8);
	free(res);

}


int score(char board[BOARD_SIZE][BOARD_SIZE], int player_color)
{
	int score = 0;
	char player_man;
	char player_king;
	char opponent_man;
	char opponent_king;
	char* direction;
	char* opponent_direction;

	if (player_color == WHITE)
	{
		player_man = WHITE_M;
		player_king = WHITE_K;
		opponent_man = BLACK_M;
		opponent_king = BLACK_K;
		direction = "up";
		opponent_direction = "down";

	}
	else
	{
		player_man = BLACK_M;
		player_king = BLACK_K;
		opponent_man = WHITE_M;
		opponent_king = WHITE_K;
		direction = "down";
		opponent_direction = "up";
	}
	//check if the player or the oponent stuck
	if (isPlayerStuck(board, player_man, player_king, opponent_man, opponent_king, direction) == 0)//we are stuck loose
		score = -100;
	else if (isPlayerStuck(board, opponent_man, opponent_king, player_man, player_king, opponent_direction) == 0)//opponent stuck we win
		score == 100;
	else
	{
		int player_counter = 0;
		int opponent_counter = 0;
		int i, j;
		for (i = 0; i < BOARD_SIZE; i++)
		{
			for (j = 0; j < BOARD_SIZE; j++)
			{
				if ((i + j) % 2 == 0)
				{
					if (board[i][j] == player_man)
						player_counter++;
					if (board[i][j] == player_king)
						player_counter = player_counter + 3;
					if (board[i][j] == opponent_man)
						opponent_counter++;
					if (board[i][j] == opponent_king)
						opponent_counter = player_counter + 3;
				}
			}
		}
		//end loops
		if (player_counter == 0)
			score = -100;
		else if (opponent_counter == 0)
			score = 100;
		else
			score = player_counter - opponent_counter;
	}
	return score;	

}
int isPlayerStuck(char board[BOARD_SIZE][BOARD_SIZE], char player_man, char player_king, char opponent_man
, char opponent_king, char* direction)
{

	int hasMoves = 0;//will change to 1 if we find one single move

	//scan all diagonals, find a white player and check all his close moves
	int i, j;
	for (i = 0; i < BOARD_SIZE; i++)
	{
		for (j = 0; j < BOARD_SIZE; j++)
		{
			if ((i + j) % 2 == 0)
			{
				if (board[i][j] == player_man)
				{
					if (checkClosedMovesMan(board, i, j, player_man, opponent_man, opponent_king, direction, 0) ==1)
						hasMoves = 1;

				}
				else if (board[i][j] == player_king)
				{
					if (checkClosedMovesMan(board, i, j, player_man, opponent_man, opponent_king, direction, 1) == 1)
						hasMoves = 1;
				}
			}
		}
	}
	return hasMoves;

}

int checkClosedMovesMan(char board[BOARD_SIZE][BOARD_SIZE], int i, int j, char player, 
	char opponentM, char opponentK, char* direction, int king)
{
	int hasMove = 0;
	if ((strcmp(direction, "up") == 0) || king ==1)// j!=9 always because it's not a king
	{
		if (i == 0 && j == 0)//left bottom
		{
			if (board[i + 1][j + 1] == EMPTY)//move forward
				hasMove = 1;
			else if ((board[i + 1][j + 1] == opponentM || board[i + 1][j + 1] == opponentK) && board[i + 2][j + 2] == EMPTY)//eeating forward
				hasMove = 1;
		}
		else if (i == 0)
		{
			if (board[i + 1][j + 1] == EMPTY)//move forward
				hasMove = 1;
			else if ((board[i + 1][j + 1] == opponentM || board[i + 1][j + 1] == opponentK) && board[i + 2][j + 2] == EMPTY) //eating forward
				hasMove = 1;
			if ((board[i + 1][j - 1] == opponentM || board[i + 1][j - 1] == opponentK) && board[i + 2][j - 2] == EMPTY && j != 8)//eating backward
				hasMove = 1;
		}
		else if (i == 9)
		{
			if (board[i - 1][j + 1] == EMPTY)//move forward
				hasMove = 1;
			else if ((board[i - 1][j + 1] == opponentM || board[i - 1][j + 1] == opponentK) && board[i - 2][j + 2] == EMPTY && j != 1) // eating forward
				hasMove = 1;
			if ((board[i - 1][j - 1] == opponentM || board[i - 1][j - 1] == opponentM) && board[i - 2][j - 2] == EMPTY && j != 8)//eating backward
				hasMove = 1;
		}
		else 
		{
			if (board[i - 1][j + 1] == EMPTY || board[i + 1][j + 1] == EMPTY)//move forward
				hasMove = 1;
			if ((board[i - 1][j + 1] == opponentM || board[i - 1][j + 1] == opponentK) && board[i - 2][j + 2] == EMPTY && i != 1)//eat forward
				hasMove = 1;
			if ((board[i + 1][j + 1] == opponentM || board[i + 1][j + 1] == opponentK) && board[i + 2][j + 2] == EMPTY && i != 8)//eat forward
				hasMove = 1;
			if ((board[i - 1][j - 1] == opponentM || board[i - 1][j - 1] == opponentK) && board[i - 2][j - 2] == EMPTY && i != 1)//eat backward
				hasMove = 1;
			if ((board[i + 1][j - 1] == opponentM || board[i + 1][j - 1] == opponentK) && board[i + 2][j - 2] == EMPTY && i != 8)//eat backward
				hasMove = 1;

		}
	}
	else if (strcmp(direction,"down") == 0 || king ==1) // direction is down j !=0 always
	{
		if (i == 9 && j == 9)//right upper
		{
			if (board[i - 1][j - 1] == EMPTY)
				hasMove = 1;
			else if ((board[i - 1][j - 1] == opponentM || board[i - 1][j - 1] == opponentK) && board[i - 2][j - 2] == EMPTY)
				hasMove = 1;
		}	
		else if (i == 0)//first column , j!=0 beacuse it's a man not a king
		{
			if (board[i + 1][j - 1] == EMPTY)//move forward
				hasMove = 1;
			else if ((board[i + 1][j - 1] == opponentM || board[i + 1][j - 1] == opponentK) && board[i + 2][j - 2] == EMPTY) //eating forward
				hasMove = 1;
			if ((board[i + 1][j + 1] == opponentM || board[i + 1][j + 1] == opponentK ) && board[i + 2][j + 2] == EMPTY && j != 8)//eating backward
				hasMove = 1;
		}
		else if (i == 9)//last colum
		{
			if (board[i - 1][j - 1] == EMPTY)//move forward
				hasMove = 1;
			else if ((board[i - 1][j - 1] == opponentM || board[i - 1][j - 1] == opponentK) && board[i - 2][j - 2] == EMPTY && j != 1) // eating forward
				hasMove = 1;
			if ((board[i - 1][j + 1] == opponentM || board[i - 1][j + 1] == opponentK) && board[i - 2][j + 2] == EMPTY && j != 8)//eating backward
				hasMove = 1;
		}
		else 
		{
			if (board[i - 1][j - 1] == EMPTY || board[i + 1][j - 1] == EMPTY)//move forward
				hasMove = 1;
			if ((board[i - 1][j - 1] == opponentM || board[i - 1][j - 1] == opponentK) && board[i - 2][j - 2] == EMPTY && j != 1)//eating forward
				hasMove = 1;
			if ((board[i + 1][j - 1] == opponentM || board[i + 1][j - 1] == opponentK) && board[i + 2][j - 2] == EMPTY && j != 1)//eating forward
				hasMove = 1;
			if ((board[i - 1][j + 1] == opponentM || board[i - 1][j + 1] == opponentK) && board[i - 2][j + 2] == EMPTY && j != 8)//eating backward
				hasMove = 1;
			if ((board[i + 1][j + 1] == opponentM || board[i + 1][j + 1] == opponentK) && board[i + 2][j + 2] == EMPTY && j != 8)//eating backward
				hasMove = 1;
		}
		
	}
	return hasMove;

}

void settingState(char board[BOARD_SIZE][BOARD_SIZE])
{
	init_board(board);
	printf("%s", ENTER_SETTINGS);
	char *command = getString(stdin, 10);
	while (strcmp(command, "quit") != 0 && strcmp(command, "start") != 0)
	{
		reduceSpaces(command);
		executeSettingCmd(board, command);

		command = getString(stdin, 10);
	}
	if (strcmp(command, "start") == 0)
	{
		//call game state on the board
		GameState(board);
	}
	else if (strcmp(command, "quit") == 0)
	{
		//TODO:clean all memory
		if (objectsInMemory > 0)
			printf("You have a memory leak! There are %d objects that were allocated but never freed", objectsInMemory);

		exit(0);
	}

}

void executeSettingCmd(char board[BOARD_SIZE][BOARD_SIZE], char* input)
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
		if (strcmp(arr[0], "minimax_depth") == 0)
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
	else if (arr_len == 4 && strcmp(arr[0], "set") == 0)//set <x,y> a b
	{
		set_disc(board, arr[1], arr[2], arr[3]);
	}
	else
	{
		printf("%s", ILLEGAL_COMMAND);
	}


	freeArray(arr, arr_len);
}

void GameState(char board[BOARD_SIZE][BOARD_SIZE])
{
	int resComputer = 0;
	int resUser = 0;
	int isComputerTurn;
	//first turn
	if (computer_color == WHITE)
	{
		resComputer = computerTurn(board);
		if (resComputer == 1)
			return;//computer won
		else
		{
			resUser = userTurn(board);
			isComputerTurn = 1;
		}
			
	}
	else //user starts
	{
		resUser = userTurn(board);
		if (resUser == 1)
			return;//user won
		else
		{
			resComputer = computerTurn(board);
			isComputerTurn = 0;
		}
			
	}
	while (resComputer != 1 && resUser != 1)//main loop of the game
	{
		if (isComputerTurn == 1)
			resComputer = computerTurn(board);
		else
			resUser = userTurn(board);
	}
		

}

/*if computerTurn or playerTurn return 1 - they won otherwise return 0*/
int computerTurn(char board[BOARD_SIZE][BOARD_SIZE])
{
	//Todo:call minimax algorithm
	//perforam chosen  move
	//if score is winning return 1 and print computer win!
	return 0;
}
int userTurn(char board[BOARD_SIZE][BOARD_SIZE])
{

	printf("%s", ENTER_YOUR_MOVE);
	char* command = getString(stdin, 10);
	if (strstr(command, "move"))
	{
		Move *move = parseMoveCommand(command);
		if (move != NULL)
			performUserMove(board, *move);
	}
}

int performUserMove(char board[BOARD_SIZE][BOARD_SIZE], Move move)
{
	char user_m;
	char user_k;
	char computer_m;
	char computer_k;
	if (computer_color == BLACK)
	{
		user_m = WHITE_M;
		user_k = WHITE_K;
		computer_m = BLACK_M;
		computer_k = BLACK_K;
	}
	else
	{
		user_m = BLACK_M;
		user_k = BLACK_K;
		computer_k = WHITE_K;
		computer_m = WHITE_M;
	}
	int x_int = getIntValue(move.currPos->x);
	if (board[x_int][move.currPos->y - 1] != user_m || board[x_int][move.currPos->y - 1] != user_k)
	{
		printf("%s", NO_DICS);
		return 0;
	}
	else if (checkMoveIsValid(board, move) == 0)
	{
		//move is not valid
		printf("%s", ILLEGAL_MOVE);
		return 0;
	}
	//perform moves - if we eat set empty at opponent
	//only one pos

}

int checkMoveIsValid(char board[BOARD_SIZE][BOARD_SIZE], Move move)
{

}