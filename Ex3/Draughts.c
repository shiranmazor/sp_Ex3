#include "Draughts.h"


//globals:
struct Players
{
	char user_k;
	char user_m;
	char computer_k;
	char computer_m;
	char computer_direction;
	char user_direction;

};

int minimax_depth = 1;
int computer_color = BLACK;//by default the use played is white = 1
int objectsInMemory = 0;
Players game_players;

void *myMalloc(size_t size) {
	objectsInMemory++;
	void * ptr = malloc(size);
	assert(ptr != NULL);
	return ptr;
}

void myFree(void  *memory) {
	if (memory == NULL)
		return;
	objectsInMemory--;
	//printf("%p\n", memory);
	free(memory);
}

void *myCalloc(void  *memory, size_t size) {
	objectsInMemory++;
	void *ptr = calloc(memory, size);
	assert(ptr != NULL);
	return ptr;
}

void *myRealloc(void  *memory, size_t newSize) {
	if (memory == NULL)
		objectsInMemory++;
	
	void *ptr = realloc(memory, newSize);
	assert(ptr != NULL);
	return ptr;
}

#define malloc(x) myMalloc(x)
#define free(x) myFree(x)
#define calloc(x,y) myCalloc(x,y)
#define realloc(x,y) myRealloc(x,y)


char board[BOARD_SIZE][BOARD_SIZE];


struct PosNode
{
	Pos *pos;
	PosNode *next;
};

struct Pos
{
	//char x;
	int x;
	int y;
};

struct MoveNode
{
	Move *move;
	MoveNode *next;

};

struct Move{
	Pos *currPos;
	int eat;
	PosNode *dest;
};


void freeMove(Move *move)
{
	free(move->currPos);
	PosNode *node = move->dest;
	while (node != NULL)
	{
		PosNode *toFree = node;
		node = node->next;
		free(toFree->pos);
		free(toFree);
		
	}
	free(move);
}

void freeMoveWithoutDest(MoveNode *moveNode)
{
	if (moveNode->move)
		free(moveNode->move->currPos);
	free(moveNode->move);
	free(moveNode);
}

void freeMoves(MoveNode *moveNodeHead, Move* notDelete)
{
	while (moveNodeHead != NULL)
	{
		MoveNode* currNode = moveNodeHead;
		Move* move = moveNodeHead->move;
		if (move != notDelete)
			freeMove(move);
		
		moveNodeHead = moveNodeHead->next;
		free(currNode);
	}
}

void freeMoveNode(MoveNode *moveNode)
{
	if (moveNode != NULL)
		freeMove(moveNode->move);
	free(moveNode);
}

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


MoveNode * getMoves(char board[BOARD_SIZE][BOARD_SIZE], char userM, char userK, char direction)
{
	MoveNode *firstMoveNode = NULL;
	MoveNode *lastNode = NULL;

	int i, j;
	int maxEats = -1;
	for (i = 0; i < BOARD_SIZE; i++)
	{
		for (j = 0; j < BOARD_SIZE; j++)
		{
			if ((i + j) % 2 == 0)
			{
				Pos pos;
				pos.x = i;
				pos.y = j;

				MoveNode *movesList = NULL;
				if (board[i][j] == userM)
				{
					movesList = getManMoves(pos, userM, userK, board, direction, 0);
				}
				else if (board[i][j] == userK)
				{
					movesList = getKingMoves(pos, userM, userK, board, direction);
				}

				MoveNode *moveNode = movesList;
				while (moveNode)
				{
					MoveNode *toFree = NULL;
					if (moveNode->move->eat >= maxEats) //don't add if we can eat more in other move
					{
						maxEats = moveNode->move->eat;
						addMoveNodeToList(&firstMoveNode, &lastNode, moveNode);
					}
					else
					{
						toFree = moveNode;
					}
					
					moveNode = moveNode->next;
					free(toFree);
				}
			}
		}
	}
	
	if (maxEats!=-1)
		firstMoveNode = keepOnlyMaxEatNodes(firstMoveNode, maxEats);

	return firstMoveNode;
}

//returns false if pos is outside the board
int isValidPos(Pos *pos)
{
	if (pos->x >= BOARD_SIZE || pos->x < 0 || pos->y >= BOARD_SIZE || pos->y < 0)
		return 0;
	return 1;
}

Pos * getAdjPositions(Pos pos, Pos** adj)
{
	//down:
	adj[0]->x = pos.x - 1;
	adj[0]->y = pos.y - 1;

	adj[1]->x = pos.x + 1;
	adj[1]->y = pos.y - 1;

	//up:
	adj[2]->x = pos.x - 1;
	adj[2]->y = pos.y + 1;

	adj[3]->x = pos.x + 1;
	adj[3]->y = pos.y + 1;

	for (int i = 0; i < 4; i++)
	{
		if (!isValidPos(adj[i]))
		{
			free(adj[i]);
			adj[i] = NULL;;
		}
	}
	return adj;
}

MoveNode *keepOnlyMaxEatNodes(MoveNode *movesList, int maxEats)
{
	MoveNode *prev = movesList;
	MoveNode *moveNode = movesList;

	while (moveNode)
	{
		MoveNode *toFree = NULL;
		if (moveNode->move->eat < maxEats)
		{
			toFree = moveNode;
			if (moveNode == movesList) //firstElement
			{
				movesList = moveNode->next;
			}
			else
			{
				prev->next = moveNode->next; //remove this element as it has low number of eats
			}
		}
		prev = moveNode;
		moveNode = moveNode->next;
		freeMoveNode(toFree);
	}
	return movesList;
}


MoveNode *createMoveNode(Pos pos, Pos destPos, int eat)
{
	MoveNode *moveNode = malloc(sizeof(MoveNode));
	Move *move = malloc(sizeof(Move));
	move->currPos = malloc(sizeof(Pos));
	move->currPos->x = pos.x;
	move->currPos->y = pos.y;
	move->dest = malloc(sizeof(PosNode));
	move->dest->pos = malloc(sizeof(Pos));
	move->dest->pos->x = destPos.x;
	move->dest->pos->y = destPos.y;
	move->dest->next = NULL;
	move->eat = eat;
	moveNode->move = move;
	moveNode->next = NULL;

	return moveNode;
}


void addMoveNodeToList(MoveNode **movesList, MoveNode **last, MoveNode * moveNode)
{
	if (!*(movesList)) //empty list
	{
		*(movesList) = moveNode;
		*(last )= *(movesList);
	}
	else
	{
		(*(last))->next = moveNode;
		*(last) = (*(last))->next;
	}
}

/*
recives current position of the man, user tools identifier, board and moving direction. 
Use onlyEatMove=1 if you want to prefer eat move only - can be useful after first eat.

return linked list of possible moves for the specified man. Will return only moves with the maximum captures possible
caller should free all returned linked list.
*/
MoveNode *getManMoves(Pos pos, char userM, char userK, char board[BOARD_SIZE][BOARD_SIZE], char direction, int onlyEatMove)
{
	char curBoard[BOARD_SIZE][BOARD_SIZE];
	//copy board
	for (int i = 0; i<10; i++)
	{
		memcpy(&curBoard[i], &board[i], sizeof(board[0]));
	}

	Pos** adj = malloc(4*sizeof(Pos*));
	for (int a = 0; a < 4; a++)
	{
		adj[a] = malloc(sizeof(Pos));
	}

	getAdjPositions(pos, adj);
	MoveNode *movesList = NULL;
	MoveNode *last = NULL;
	int maxEats = 0;

	for (int i = 0; i < 4; i++)
	{
		if (adj[i] != NULL)
		{			
			char adjVal = curBoard[adj[i]->x][adj[i]->y];
			if (adjVal == userM || adjVal == userK) //can't eat, same team buddy!
				continue;

			if (adjVal == EMPTY)
			{
				if (onlyEatMove) //we were requested to provide eat move only
					continue;
				if (direction == 'U' && i < 2) //moving in the wrong direction
					continue;
				if ((direction == 'D') && i > 1) //moving in the wrong direction
					continue;

				MoveNode *moveNode = createMoveNode(pos, *(adj[i]), 0);

				addMoveNodeToList(&movesList, &last, moveNode);
			}
			else if (adjVal != userM && adjVal != userK) //eating?
			{
				MoveNode *nextMovesList;
				int xDiff = adj[i]->x - pos.x;
				int yDiff = adj[i]->y - pos.y;
				if (adj[i]->x + xDiff > BOARD_SIZE || adj[i]->x + xDiff<0 || adj[i]->y + yDiff > BOARD_SIZE || adj[i]->y + yDiff < 0) //next tool in this direction is outside the board
					continue;

				char nextToolOnTheSamePath = curBoard[adj[i]->x + xDiff][adj[i]->y + yDiff];
				if (nextToolOnTheSamePath != EMPTY) //can't eat - invalid move
					continue;

				//yay! we can eat at least one! maybe more?
				Pos destPos;
				destPos.x = adj[i]->x + xDiff;
				destPos.y = adj[i]->y + yDiff;
				
				if (maxEats < 1)
					maxEats = 1;

				curBoard[adj[i]->x][adj[i]->y] = EMPTY;

				if ((direction == 'U') && destPos.y == BOARD_SIZE - 1) //becoming a king!
					nextMovesList = NULL;
				else if ((direction == 'D') && destPos.y == 0) //becoming a king!
					nextMovesList = NULL;
				else //no king created
					nextMovesList = getManMoves(destPos, userM, userK, curBoard, 'b', 1); //"both" since we now can eat backword
				
				if (!nextMovesList)
				{
					MoveNode *moveNode  = createMoveNode(pos, destPos, 1);

					addMoveNodeToList(&movesList, &last, moveNode);
					//todo free something?
				}

				MoveNode *moveNodeNew = nextMovesList;
				while (moveNodeNew)
				{
					if (moveNodeNew->move->eat < maxEats) //we have better moves buddy, bye!
					{
						MoveNode *toFree = moveNodeNew;
						moveNodeNew = moveNodeNew->next;
						free(toFree);
						continue;
					}
					else
					{
						maxEats = 1 + moveNodeNew->move->eat;
					}
					MoveNode *moveNode = createMoveNode(pos, destPos, maxEats);
					MoveNode *toFree = moveNodeNew;
					moveNode->move->dest->next = moveNodeNew->move->dest;
					moveNodeNew = moveNodeNew->next;
					//todo - copy to king
					freeMoveWithoutDest(toFree);
					addMoveNodeToList(&movesList, &last, moveNode);
				}
			}
		}
	}
	
	if (adj)
	{
		for (int a = 0; a < 4; a++)
		{
			free(adj[a]);
		}
		free(adj);
	}

	movesList= keepOnlyMaxEatNodes(movesList, maxEats);	
	return movesList;
}

/*
recives current position of the king, user tools identifier, board and moving direction.

return linked list of possible moves for the specified king. Will return only moves with the maximum captures possible
caller should free all returned linked list.
*/
MoveNode *getKingMoves(Pos pos, char userM, char userK, char board[BOARD_SIZE][BOARD_SIZE], char direction)
{
	char curBoard[BOARD_SIZE][BOARD_SIZE];

	//copy board
	for (int i = 0; i<10; i++)
		memcpy(&curBoard[i], &board[i], sizeof(board[0]));

	Pos** adj = malloc(4 * sizeof(Pos*));
	for (int a = 0; a < 4; a++)
		adj[a] = malloc(sizeof(Pos));

	getAdjPositions(pos, adj);

	MoveNode *movesList = NULL;
	MoveNode *last = NULL;
	int maxEats = 0;

	for (int i = 0; i < 4; i++)
	{
		if (adj[i] == NULL)
			continue;

		char adjVal = curBoard[adj[i]->x][adj[i]->y];
		if (adjVal == userK || adjVal == userM) //blocked
			continue; 

		
		MoveNode *nextMovesList;

		int xDiff = adj[i]->x - pos.x;
		int yDiff = adj[i]->y - pos.y;
		Pos nextPosOnSameDirection;
		nextPosOnSameDirection.x = adj[i]->x + xDiff;
		nextPosOnSameDirection.y = adj[i]->y + yDiff;

		if (!isValidPos(&nextPosOnSameDirection)) //we reached end of board
			continue;

		char nextToolOnTheSamePath = curBoard[nextPosOnSameDirection.x][nextPosOnSameDirection.y];

		if (adjVal != EMPTY && adjVal != userK && adjVal != userM)
		{
			//opponent nearby!
			if (nextToolOnTheSamePath == EMPTY) //can eat!
			{
				//MoveNode *moveNode = createMoveNode(pos, nextPosOnSameDirection, 1);
				//addMoveNodeToList(&movesList, &last, moveNode);
			}
		}
		else if (adjVal == EMPTY)
		{

			while (nextToolOnTheSamePath == EMPTY)
			{
				MoveNode *moveNode = createMoveNode(pos, nextPosOnSameDirection, 0);
				
				addMoveNodeToList(&movesList, &last, moveNode);

				nextPosOnSameDirection.x += xDiff;
				nextPosOnSameDirection.y += yDiff;
				
				if (!isValidPos(&nextPosOnSameDirection))
					break;

				nextToolOnTheSamePath = curBoard[nextPosOnSameDirection.x][nextPosOnSameDirection.y];
			}
			
			if (!isValidPos(&nextPosOnSameDirection))
				continue;

			if (board[nextPosOnSameDirection.x][nextPosOnSameDirection.y] == userK || board[nextPosOnSameDirection.x][nextPosOnSameDirection.y] == userM) //can't move further, we reached the same color
				continue;

			//if you reached here maybe you have something to eat!!
			Pos mustBeEmptyInOrderToEat;
			mustBeEmptyInOrderToEat.x = nextPosOnSameDirection.x + xDiff;
			mustBeEmptyInOrderToEat.y = nextPosOnSameDirection.y + yDiff;

			if (!isValidPos(&mustBeEmptyInOrderToEat)) //blocked
				continue;

			if (curBoard[mustBeEmptyInOrderToEat.x][mustBeEmptyInOrderToEat.y] != EMPTY) //can't eat, two tools in a row
				continue;

			//now we can eat!@#!
			if (maxEats < 1)
				maxEats = 1;

			curBoard[nextPosOnSameDirection.x][nextPosOnSameDirection.y] = EMPTY;

			//maybe we can eat more!
			nextMovesList = getManMoves(mustBeEmptyInOrderToEat, userM, userK, curBoard, 'b', 1); //"both" since we now can eat backword
			
			if (!nextMovesList) //can't eat more
			{
				MoveNode *moveNode = createMoveNode(pos, mustBeEmptyInOrderToEat, 1);
				addMoveNodeToList(&movesList, &last, moveNode);
				continue;
			}

			//maybe we ate more!
			MoveNode *moveNodeNew = nextMovesList;
			while (moveNodeNew)
			{
				if (moveNodeNew->move->eat < maxEats)
				{
					MoveNode *toFree = moveNodeNew;
					moveNodeNew = moveNodeNew->next;
					free(toFree);
					continue;
				}
				else
				{
					maxEats = 1 + moveNodeNew->move->eat;
				}

				MoveNode *moveNode = createMoveNode(pos, mustBeEmptyInOrderToEat, maxEats);
				MoveNode *toFree = moveNodeNew;

				moveNode->move->dest->next = moveNodeNew->move->dest;
				moveNodeNew = moveNodeNew->next;

				freeMoveWithoutDest(toFree);

				addMoveNodeToList(&movesList, &last, moveNode);
			}
		}
	}

	if (adj)
	{
		for (int a = 0; a < 4; a++)
		{
			free(adj[a]);
		}
		free(adj);
	}

	movesList = keepOnlyMaxEatNodes(movesList, maxEats);
	return movesList;
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

void clear_board()
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
	char * toFree = pos_input;
	pos_input = replace(pos_input, '>', "");
	int arr_len = split(pos_input, ',', &arr);
	free(pos_input);
	free(toFree);
	pos->x = getIntValue(arr[0][0]);
	pos->y = atoi(arr[1])-1;
	freeArray(arr, arr_len);

	
	if (pos->x < 0 || pos->x > 9 || pos->y<0 || pos->y>9 || ((pos->x+pos->y) %2 !=0))
	{
		printf("%s", WRONG_POSITION);
		free(pos);
		return NULL; 
	}
	
	return pos;
}

void remove_disc(char* input)
{
	//input is <x,y>
	Pos *pos = formatPos(input);

	if (pos)
	{
		int x_int = pos->x;
		board[x_int][pos->y] = EMPTY;
	}
	free(pos);
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
	

	char** arr = NULL;
	int arrLen = split(command, ' ', &arr);

	move->currPos  = formatPos(arr[1]);
	if (!move->currPos)//position was invalid
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
	freeArray(arr, arrLen);
	freeArray(destArr, destArrLen);
	return move;
}

void set_disc(char* pos_input, char* color, char* type)
{
	Pos *pos = formatPos(pos_input);
	if (pos == NULL)
	{
		return;
	}
	else
	{
		int x_int = pos->x;
		if (strcmp(color, "black") == 0)
		{
			if (strcmp(type, "k") == 0)
				board[x_int][pos->y] = BLACK_K;
			else if (strcmp(type, "m") == 0)
				board[x_int][pos->y] = BLACK_M;
		}
		else if (strcmp(color, "white") == 0)
		{
			if (strcmp(type, "k") == 0)
				board[x_int][pos->y] = WHITE_K;
			else if (strcmp(type, "m") == 0)
				board[x_int][pos->y] = WHITE_M;
		}
	}
	free(pos);
}

void unitTests()
{

	//parse move:
	char cmd[] = "move <b,2> to <b,4>";
	trimwhitespace(cmd);
	Move *res = parseMoveCommand(cmd);
	assert(res->currPos->x == 1 && res->currPos->y == 1);
	assert(res->dest->pos->x == 1 && res->dest->pos->y == 3);
	freeMove(res);

	char cmd2[] = "move <j,10> to <b,4>";
	res = parseMoveCommand(cmd2);
	assert(res->currPos->x == 9 && res->currPos->y == 9);
	freeMove(res);
	
	char cmd3[] = "move <j,10> to <b,4><b,10><d,8>";
	res = parseMoveCommand(cmd3);
	assert(res->currPos->x == 9 && res->currPos->y == 9);
	assert(res->dest->pos->x == 1 && res->dest->pos->y == 3);
	assert(res->dest->next->pos->x == 1 && res->dest->next->pos->y == 9);
	assert(res->dest->next->next->pos->x == 3 && res->dest->next->next->pos->y == 7);
	freeMove(res);

	//getManMoves

	clear_board(board);

	//No eating:
	board[0][0] = WHITE_M;
	Pos pos;
	pos.x = 0;
	pos.y = 0;
	
	MoveNode *movesList = getManMoves(pos, WHITE_M, WHITE_K, board, 'U', 0);
 	assert(movesList->next == NULL); //only one possible move
	assert(movesList->move->currPos->x == pos.x);
	assert(movesList->move->currPos->y == pos.y);
	assert(movesList->move->dest->next == NULL);
	assert(movesList->move->dest->pos->x == 1);
	assert(movesList->move->dest->pos->y == 1);
	assert(movesList->move->eat == 0);
	freeMoves(movesList, NULL);
	
	//eating one:
	board[1][1] = BLACK_K;
	
	movesList = getManMoves(pos, WHITE_M, WHITE_K, board, 'U', 0);
	assert(movesList->next == NULL); //only one possible move
	assert(movesList->move->currPos->x == pos.x);
	assert(movesList->move->currPos->y == pos.y);
	assert(movesList->move->dest->next == NULL);
	assert(movesList->move->eat == 1);
	assert(movesList->move->dest->pos->x == 2);
	assert(movesList->move->dest->pos->y == 2);
	freeMoves(movesList, NULL);

	//eating two on the same path:
	board[1][1] = BLACK_M;
	board[3][3] = BLACK_M;
	movesList = getManMoves(pos, WHITE_M, WHITE_K, board, 'U', 0);
	assert(movesList->next == NULL); //only one possible move
	assert(movesList->move->currPos->x == pos.x);
	assert(movesList->move->currPos->y == pos.y);
	assert(movesList->move->eat == 2);
	assert(movesList->move->dest->pos->x == 2);
	assert(movesList->move->dest->pos->y == 2);
	assert(movesList->move->dest->next->pos->x == 4);
	assert(movesList->move->dest->next->pos->y == 4);
	assert(movesList->move->dest->next->next == NULL);
	freeMoves(movesList, NULL);
	//eating farward than backward:
	board[3][3] = EMPTY;
	board[3][1] = BLACK_M;

	movesList = getManMoves(pos, WHITE_M, WHITE_K, board, 'U', 0);
	assert(movesList->next == NULL); //only one possible move
	assert(movesList->move->currPos->x == pos.x);
	assert(movesList->move->currPos->y == pos.y);
	assert(movesList->move->eat == 2);
	assert(movesList->move->dest->pos->x == 2);
	assert(movesList->move->dest->pos->y == 2);
	assert(movesList->move->dest->next->pos->x == 4);
	assert(movesList->move->dest->next->pos->y == 0);
	assert(movesList->move->dest->next->next == NULL);
	freeMoves(movesList, NULL);

	pos.x = 2;
	pos.y = 2;
	clear_board(board);
	board[2][2] = WHITE_M;
	board[1][3] = BLACK_M;
	board[3][3] = BLACK_M;

	movesList = getManMoves(pos, WHITE_M, WHITE_K, board, 'U', 0);
	assert(movesList->next != NULL); //more than one possible move
	assert(movesList->next->next == NULL); //two possible moves
	assert(movesList->move->currPos->x == pos.x);
	assert(movesList->move->currPos->y == pos.y);
	assert(movesList->move->eat == 1);
	assert(movesList->next->move->eat == 1);
	assert(movesList->move->dest->pos->x == 0);
	assert(movesList->move->dest->pos->y == 4);
	assert(movesList->next->move->dest->pos->x == 4);
	assert(movesList->next->move->dest->pos->y == 4);
	
	assert(movesList->move->dest->next == NULL);
	freeMoves(movesList, NULL);

	//one eat with one option to not eat
	board[3][3] = WHITE_M;
	movesList = getManMoves(pos, WHITE_M, WHITE_K, board, 'U', 0);
	assert(movesList->next == NULL); //only one possible move
	assert(movesList->move->currPos->x == pos.x);
	assert(movesList->move->currPos->y == pos.y);
	assert(movesList->move->eat == 1);
	assert(movesList->move->dest->pos->x == 0);
	assert(movesList->move->dest->pos->y == 4);
	assert(movesList->move->dest->next == NULL);
	freeMoves(movesList, NULL);

	//todo - try to eat down
	pos.x = 1;
	pos.y = 3;
	clear_board(board);
	board[2][2] = WHITE_M;
	board[1][3] = BLACK_M;

	movesList = getManMoves(pos, BLACK_M, BLACK_K, board, 'D', 0);
	assert(movesList->next == NULL); //only one possible move
	assert(movesList->move->currPos->x == pos.x);
	assert(movesList->move->currPos->y == pos.y);
	assert(movesList->move->eat == 1);
	assert(movesList->move->dest->pos->x == 3);
	assert(movesList->move->dest->pos->y == 1);
	assert(movesList->move->dest->next == NULL);
	freeMoves(movesList, NULL);

	//After a man becomes a king, it cannot move or capture in the same turn:
	pos.x = 3;
	pos.y = 7;
	clear_board(board);
	board[3][7] = WHITE_M;
	board[4][8] = BLACK_M;
	board[6][8] = BLACK_M;
	movesList = getManMoves(pos, WHITE_M, WHITE_K, board, 'U', 0);
	assert(movesList->next == NULL); //only one possible move
	assert(movesList->move->currPos->x == pos.x);
	assert(movesList->move->currPos->y == pos.y);
	assert(movesList->move->eat == 1);
	assert(movesList->move->dest->pos->x == 5);
	assert(movesList->move->dest->pos->y == 9);
	assert(movesList->move->dest->next == NULL);
	freeMoves(movesList, NULL);

	//same but now with the black
	pos.x = 0;
	pos.y = 2;
	clear_board(board);
	board[0][2] = BLACK_M;
	board[1][1] = WHITE_M;
	board[3][1] = WHITE_M;
	movesList = getManMoves(pos, BLACK_M, BLACK_K, board, 'D', 0);
	assert(movesList->next == NULL); //only one possible move
	assert(movesList->move->currPos->x == pos.x);
	assert(movesList->move->currPos->y == pos.y);
	assert(movesList->move->eat == 1);
	assert(movesList->move->dest->pos->x ==2);
	assert(movesList->move->dest->pos->y == 0);
	assert(movesList->move->dest->next == NULL);
	freeMoves(movesList, NULL);

	//eatin backward:
	clear_board(board);
	board[1][1] = BLACK_M;
	board[0][2] = WHITE_M;
	//print_board(board);
	movesList = getManMoves(pos, WHITE_M, WHITE_K, board, 'U', 0);
	assert(movesList->next == NULL); //only one possible move
	assert(movesList->move->currPos->x == pos.x);
	assert(movesList->move->currPos->y == pos.y);
	assert(movesList->move->eat == 1);
	assert(movesList->move->dest->pos->x == 2);
	assert(movesList->move->dest->pos->y == 0);
	assert(movesList->move->dest->next == NULL);
	freeMoves(movesList, NULL);

	init_board(board);
	
	MoveNode * list = getMoves(board, WHITE_M, WHITE_K, 'U');
	assert(list != NULL);
	int movesCount = 0;
	MoveNode *node = list;
	int x = 1;
	while (node)
	{
		movesCount++;
		assert(node->move->eat == 0);
		assert(node->move->currPos->y == 3);
		assert(node->move->currPos->x == x);
		if (movesCount % 2 == 0)
			x += 2;
		node = node->next;
	}
	assert(movesCount == 9);
	freeMoves(list, NULL);

	clear_board(board);
	board[0][0] = WHITE_K;
	board[7][7] = BLACK_K;

	
	pos.y = pos.x = 0;

	movesList = getKingMoves(pos, WHITE_M, WHITE_K, board, 'U');
	assert(movesList != NULL);
	assert(movesList->next == NULL);
	assert(movesList->move->eat == 1);
	assert(movesList->move->dest->pos->x == 8);
	assert(movesList->move->dest->pos->y == 8);
	freeMoves(movesList, NULL);
}

void unitTestsSettingFuncs()
{
	clear_board();
	//first check all functions of setting state:
	//set_disc:
	char* pos = "<d,1>";
	char* color = "white";
	char* type = "k";
	set_disc(pos, color, type);
	assert(board[3][0] == EMPTY);

	clear_board();
	pos = "<b,8>";
	color = "black";
	type = "m";
	set_disc(pos, color, type);
	assert(board[1][7] == BLACK_M);

	clear_board();
	pos = "<i,3>";
	color = "stam";
	type = "m";
	set_disc(pos, color, type);
	assert(board[8][2] == EMPTY);

	clear_board();
	pos = "<g,6>";
	color = "white";
	type = "m";
	set_disc(pos, color, type);
	assert(board[6][5] == EMPTY);

	clear_board();
	pos = "<f,10>";
	color = "white";
	type = "M";
	set_disc(pos, color, type);
	assert(board[5][9] == EMPTY);

	clear_board();
	pos = "<d,4>";
	color = "white";
	type = "m";
	set_disc(pos, color, type);
	assert(board[3][3] == WHITE_M);
	pos = "<d,4>";
	color = "black";
	type = "k";
	set_disc(pos, color, type);
	assert(board[3][3] == BLACK_K);
	
	clear_board();
	//remove disc:
	set_disc("<e,5>", "white", "k");
	remove_disc("<e,5");
	assert(board[4][4] == EMPTY);


	clear_board();
}

void unitTestValidMoves()
{
	//init game players f:
	//computer = black player:
	computer_color = BLACK;
	game_players.computer_direction = 'D';
	game_players.computer_m = BLACK_M;
	game_players.computer_k = BLACK_K;
	game_players.user_k = WHITE_K;
	game_players.user_m = WHITE_M;
	game_players.user_direction = 'U';
	//int checkMoveIsValidMan( Move move, char direction)
	clear_board();
	set_disc("<f,2>", "white", "m");
	set_disc("<g,3>", "black", "k");
	set_disc("<g,5>", "black", "m");
	set_disc("<e,5>", "black", "m");
	char cmd[] = "move <f,2> to <h,4><f,6><d,4>";
	trimwhitespace(cmd);
	Move *res = parseMoveCommand(cmd);
	int output = checkMoveIsValidMan(*res, 'U');
	assert(output == 1);
	freeMove(res);
	clear_board();


	//computer = white player:
	computer_color = WHITE;
	game_players.user_direction = 'D';
	game_players.user_m = BLACK_M;
	game_players.user_k = BLACK_K;
	game_players.computer_k = WHITE_K;
	game_players.computer_m = WHITE_M;
	game_players.computer_direction = 'U';
	set_disc("<e,7>", "black", "m");
	set_disc("<d,6>", "white", "m");
	char cmd2[] = "move <e,7> to <c,5>";
	trimwhitespace(cmd2);
	res = parseMoveCommand(cmd2);
	output = checkMoveIsValidMan(*res, 'D');
	assert(output == 1);
	freeMove(res);

	clear_board();
	set_disc("<e,7>", "black", "m");
	char cmd3[] = "move <e,7> to <c,5>";
	trimwhitespace(cmd3);
	res = parseMoveCommand(cmd3);
	output = checkMoveIsValidMan(*res, 'D');
	assert(output == 0);
	freeMove(res);

	char cmd4[] = "move <g,3> to <c,5>";
	trimwhitespace(cmd4);
	res = parseMoveCommand(cmd4);
	output = checkMoveIsValidMan(*res, 'D');
	assert(output == 0);
	freeMove(res);
	clear_board();

	//int checkMoveIsValidKing(Move move, char direction)
	//computer = black player:
	computer_color = BLACK;
	game_players.computer_direction = 'D';
	game_players.computer_m = BLACK_M;
	game_players.computer_k = BLACK_K;
	game_players.user_k = WHITE_K;
	game_players.user_m = WHITE_M;
	game_players.user_direction = 'U';
	set_disc("<d,2>", "white", "k");
	char cmd5[] = "move <d,2> to <j,8>";
	trimwhitespace(cmd5);
	res = parseMoveCommand(cmd5);
	output = checkMoveIsValidKing(*res, 'U');
	assert(output == 1);
	freeMove(res);
	clear_board();

	set_disc("<d,2>", "white", "k");
	set_disc("<f,4>", "black", "m");
	char cmd6[] = "move <d,2> to <g,5>";
	trimwhitespace(cmd6);
	res = parseMoveCommand(cmd6);
	output = checkMoveIsValidKing(*res, 'U');
	assert(output == 1);
	freeMove(res);
	clear_board();

	set_disc("<d,2>", "white", "k");
	set_disc("<f,4>", "black", "m");
	set_disc("<g,5>", "black", "m");
	char cmd7[] = "move <d,2> to <h,6>";
	trimwhitespace(cmd7);
	res = parseMoveCommand(cmd7);
	output = checkMoveIsValidKing(*res, 'U');
	assert(output == 0);
	freeMove(res);


	//computer = white player:
	computer_color = WHITE;
	game_players.user_direction = 'D';
	game_players.user_m = BLACK_M;
	game_players.user_k = BLACK_K;
	game_players.computer_k = WHITE_K;
	game_players.computer_m = WHITE_M;
	game_players.computer_direction = 'U';
	set_disc("<b,8>", "black", "k");
	set_disc("<d,6>", "white", "m");
	set_disc("<g,3>", "white", "m");
	char cmd8[] = "move <b,8> to <e,5><h,2>";
	trimwhitespace(cmd8);
	res = parseMoveCommand(cmd8);
	output = checkMoveIsValidKing(*res, 'D');
	assert(output == 0);
	freeMove(res);
	clear_board();

	//isManBecomeKing:
	//computer = black player:
	computer_color = BLACK;
	game_players.computer_direction = 'D';
	game_players.computer_m = BLACK_M;
	game_players.computer_k = BLACK_K;
	game_players.user_k = WHITE_K;
	game_players.user_m = WHITE_M;
	game_players.user_direction = 'U';
	Pos* next = formatPos("<d,10>");
	int result = isManBecomeKing(next, 'U');
	assert(result == 1);
	free(next);

	next = formatPos("<c,3>");
	result = isManBecomeKing(next, 'U');
	assert(result == 0);
	free(next);

	//computer = white player:
	computer_color = WHITE;
	game_players.user_direction = 'D';
	game_players.user_m = BLACK_M;
	game_players.user_k = BLACK_K;
	game_players.computer_k = WHITE_K;
	game_players.computer_m = WHITE_M;
	game_players.computer_direction = 'U';
	next = formatPos("<e,1>");
	result = isManBecomeKing(next, 'D');
	assert(result == 1);
	free(next);

	//getOponnentPos
	clear_board();
	set_disc("<e,3>", "white", "m");
	set_disc("<f,4>", "black", "m");
	Pos* curr = formatPos("<e,3>");
	Pos* nextP = formatPos("<g,5>");
	Pos p = getOponnentPos(curr, nextP);
	free(curr);
	free(nextP);
	assert(p.x == 5 && p.y == 3);

	clear_board();
	set_disc("<c,9>", "black", "m");
	set_disc("<b,8>", "white", "m");
	curr = formatPos("<c,9>");
	nextP = formatPos("<a,7>");
	p = getOponnentPos(curr, nextP);
	free(curr);
	free(nextP);
	assert(p.x == 1 && p.y == 7);


}

void unitTestCheckStuckAndScore()
{
	//checkClosedMovesMan
	clear_board();
	set_disc("<f,2>", "white", "m");
	set_disc("<e,3>", "white", "k");
	int res1 = checkClosedMovesMan(5, 1, WHITE_M, BLACK_M, BLACK_K, 'U', 0);
	assert(res1 == 1);

	clear_board();
	set_disc("<f,2>", "white", "m");
	set_disc("<e,3>", "white", "k");
	set_disc("<g,3>", "black", "m");
	res1 = checkClosedMovesMan(5, 1, WHITE_M, BLACK_M, BLACK_K, 'U', 0);
	assert(res1 == 1);

	clear_board();
	set_disc("<f,2>", "white", "m");
	set_disc("<e,3>", "white", "k");
	set_disc("<g,3>", "white", "m");
	res1 = checkClosedMovesMan(5, 1, WHITE_M, BLACK_M, BLACK_K, 'U', 0);
	assert(res1 == 0);

	clear_board();
	set_disc("<j,8>", "black", "m");
	set_disc("<i,7>", "black", "m");
	set_disc("<i,9>", "white", "m");
	res1 = checkClosedMovesMan(9, 7, BLACK_M, WHITE_M, WHITE_K, 'D', 0);
	assert(res1 == 1);

	//checkClosedMovesKing:
	clear_board();
	set_disc("<f,10>", "black", "k");
	set_disc("<e,9>", "black", "m");
	set_disc("<d,8>", "black", "m");
	set_disc("<g,9>", "white", "m");
	set_disc("<h,8>", "white", "m");
	int res2 = checkClosedMovesKing(5, 9, BLACK_M,BLACK_K, WHITE_M,WHITE_K, 'D',1);
	assert(res2 == 0);

	clear_board();
	set_disc("<f,10>", "black", "k");
	set_disc("<e,9>", "black", "m");
	set_disc("<d,8>", "black", "m");
	set_disc("<g,9>", "white", "m");
	res2 = checkClosedMovesKing(5, 9, BLACK_M, BLACK_K, WHITE_M, WHITE_K, 'D', 1);
	assert(res2 == 1);

	//isPlayerStuck:
	clear_board();
	set_disc("<f,4>", "white", "m");
	set_disc("<e,3>", "black", "m");
	set_disc("<g,3>", "black", "m");
	set_disc("<e,5>", "black", "m");
	set_disc("<g,5>", "black", "m");
	set_disc("<h,2>", "black", "m");
	set_disc("<d,2>", "black", "m");
	set_disc("<h,6>", "black", "k");
	set_disc("<d,6>", "black", "k");
	int res3 = isPlayerStuck(WHITE_M, WHITE_K, BLACK_M, BLACK_K, 'U');
	assert(res3 == 0);

	clear_board();
	set_disc("<f,4>", "white", "m");
	set_disc("<e,3>", "white", "m");
	set_disc("<e,5>", "white", "m");
	set_disc("<g,5>", "white", "m");
	set_disc("<g,3>", "black", "m");
	res3 = isPlayerStuck(WHITE_M, WHITE_K, BLACK_M, BLACK_K, 'U');
	assert(res3 == 1);
	res3 = isPlayerStuck(BLACK_M, BLACK_K, WHITE_M, WHITE_K, 'D');
	assert(res3 == 1);

	set_disc("<f,10>", "white", "k");
	res3 = isPlayerStuck(WHITE_M,WHITE_K, BLACK_M,BLACK_K, 'U');
	assert(res3 == 1);

	clear_board();
	set_disc("<c,1>", "black", "k");
	res3 = isPlayerStuck(BLACK_M, BLACK_K, WHITE_M, WHITE_K, 'D');
	assert(res3 == 1);

	//score:
	clear_board();
	init_board(board);
	int s = score(board, WHITE);
	assert(s == 0);
	
	//still on init board:
	//computer = black player:
	computer_color = BLACK;
	game_players.computer_direction = 'D';
	game_players.computer_m = BLACK_M;
	game_players.computer_k = BLACK_K;
	game_players.user_k = WHITE_K;
	game_players.user_m = WHITE_M;
	game_players.user_direction = 'U';
	remove_disc("<c,7>");
	remove_disc("<e,9>");
	remove_disc("<j,10>");
	s = score(board, WHITE);
	assert(s == 3);

	clear_board();
	set_disc("<d,6>", "black", "k");
	set_disc("<b,2>", "white", "m");
	print_board(board);
	s = score(board, WHITE);
	assert(s == -2);

	clear_board();
	computer_color = WHITE;
	game_players.computer_direction = 'U';
	game_players.computer_k = WHITE_K;
	game_players.computer_m = WHITE_M;
	game_players.user_direction = 'U';
	game_players.user_k = BLACK_K;
	game_players.user_m = BLACK_M;

	set_disc("<d,6>", "black", "k");
	set_disc("<h,8>", "black", "k");
	set_disc("<b,2>", "white", "m");
	set_disc("<f,4>", "white", "m");
	set_disc("<e,3>", "white", "m");
	s = score(board, BLACK);
	assert(s == 3);


	clear_board();

}

void unitTestMinimaxAndMoves()
{
	//format move to string:
	char cmd[] = "move <b,2> to <d,4><f,6><h,4>";
	trimwhitespace(cmd);
	Move *move = parseMoveCommand(cmd);
	char* res = getStringFormatMove(*move);
	char cmdRes[] = "move <b,2> to <d,4><f,6><h,4>\n";
	assert(strcmp(res, cmdRes) == 0);
	freeMove(move);
}

int score(char board[BOARD_SIZE][BOARD_SIZE],int player_color)
{
	int score = 0;
	if (player_color == computer_color)
	{

		if (isPlayerStuck(game_players.computer_m, game_players.computer_k,
			game_players.user_m, game_players.user_k, game_players.computer_direction) == 0)//we are stuck loose
			score = -100;

		else if (isPlayerStuck(game_players.user_m, game_players.user_k, game_players.computer_m,
			game_players.computer_k, game_players.user_direction) == 0)//opponent stuck we win
			score == 100;
		else
		{
			//player is the computer
			int player_counter = 0;
			int opponent_counter = 0;
			int i, j;
			for (i = 0; i < BOARD_SIZE; i++)
			{

				for (j = 0; j < BOARD_SIZE; j++)
				{
					if ((i + j) % 2 == 0)
					{
						if (board[i][j] == game_players.computer_m)
							player_counter++;
						if (board[i][j] == game_players.computer_k)
							player_counter = player_counter + 3;
						if (board[i][j] == game_players.user_m)
							opponent_counter++;
						if (board[i][j] == game_players.user_k)
							opponent_counter = opponent_counter + 3;
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

	}
	else 
	{
		if (isPlayerStuck(game_players.user_m, game_players.user_k, game_players.computer_m,
			game_players.computer_k, game_players.user_direction) == 0)// we are stuck loose
			score == -100;
		else if (isPlayerStuck(game_players.computer_m, game_players.computer_k,
			game_players.user_m, game_players.user_k, game_players.computer_direction) == 0) // opponent stuck we win 
			score = 100;
		else
		{
			//player is the user
			int player_counter = 0;
			int opponent_counter = 0;
			int i, j;
			for (i = 0; i < BOARD_SIZE; i++)
			{
				for (j = 0; j < BOARD_SIZE; j++)
				{
					if ((i + j) % 2 == 0)
					{
						if (board[i][j] == game_players.user_m)
							player_counter++;
						if (board[i][j] == game_players.user_k)
							player_counter = player_counter + 3;
						if (board[i][j] == game_players.computer_m)
							opponent_counter++;
						if (board[i][j] == game_players.computer_k)
							opponent_counter = opponent_counter + 3;
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
	}
		
		
	return score;	

}

/*return 0 if player stuck else return 1*/
int isPlayerStuck(char player_man, char player_king, char opponent_man, char opponent_king, char direction)
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
					if (checkClosedMovesMan(i, j, player_man, opponent_man, opponent_king, direction, 0) ==1)
						hasMoves = 1;

				}
				else if (board[i][j] == player_king)
				{
					if (checkClosedMovesKing(i, j, player_man, player_king, opponent_man, opponent_king, direction, 1) == 1)
						hasMoves = 1;
				}
			}
		}
	}
	return hasMoves;

}

/*return 1 if man still have moves else return 0*/
int checkClosedMovesMan(int i, int j, char player, char opponentM, char opponentK, char direction, int king)
{
	int hasMove = 0;
	if ((direction == 'U') || king ==1)// j!=9 always because it's not a king
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
	else if (direction== 'D' || king ==1) // direction is down j !=0 always
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

/*return 1 if king still have moves else return 0*/
int checkClosedMovesKing(int i, int j, char playerM, char playerK, char opponentM, char opponentK, char direction, int king)
{
	int hasMoves = 0;
	if (i == 0 && j == 0)//left bottom
	{
		if (board[i + 1][j + 1] == EMPTY )
			hasMoves = 1;
		else if (board[i + 1][j + 1] == opponentK || board[i + 1][j + 1] == opponentM)
		{
			//check if can eat
			if (board[i + 2][j + 2] == EMPTY)
				hasMoves = 1;
		}

	}
	else if (i == 9 && j == 9)
	{
		if (board[i - 1][j - 1] == EMPTY)
			hasMoves = 1;
		else if (board[i -1][j - 1] == opponentK || board[i - 1][j - 1] == opponentM)
		{
			//check if can eat
			if (board[i - 2][j - 2] == EMPTY)
				hasMoves = 1;
		}
	}
	else if (j == 0)//only first line
	{
		if (board[i - 1][j + 1] == EMPTY || board[i + 1][j + 1] == EMPTY)
			hasMoves = 1;
		else if (board[i - 1][j + 1] == opponentK || board[i - 1][j + 1] == opponentM)
		{
			// check if can eat
			if (board[i - 2][j + 2] == EMPTY)
				hasMoves = 1;
		}
		else if (board[i + 1][j + 1] == opponentK || board[i + 1][j + 1] == opponentM)
		{
			// check if can eat
			if (board[i + 2][j + 2] == EMPTY)
				hasMoves = 1;
		}

	}
	else if (j == 9)//only last line
	{
		if (board[i - 1][j - 1] == EMPTY || board[i + 1][j - 1] == EMPTY)
			hasMoves = 1;
		else if (board[i - 1][j - 1] == opponentK || board[i - 1][j - 1] == opponentM)
		{
			//check if can't eat
			if (board[i - 2][j - 2] == EMPTY)
				hasMoves = 1;
		}
		else if (board[i + 1][j - 1] == opponentK || board[i + 1][j - 1] == opponentM)
		{
			//check if can't eat
			if (board[i + 2][j - 2] == EMPTY)
				hasMoves = 1;
		}
	}
	else if (i == 0)//only first colunm
	{
		if (board[i + 1][j - 1] == EMPTY || board[i + 1][j + 1] == EMPTY)
			hasMoves = 1;
		else if (board[i + 1][j - 1] == opponentK || board[i + 1][j - 1] == opponentM)
		{
			//check if can eat
			if (board[i + 2][j - 2] == EMPTY)
				hasMoves = 1;
			
		}
		else if (board[i + 1][j + 1] == opponentK || board[i + 1][j + 1] == opponentM)
		{
			if (board[i + 2][j + 2] == EMPTY)
				hasMoves = 1;
		}
	}
	else if (i == 9)//only last column
	{
		if (board[i - 1][j - 1] == EMPTY || board[i - 1][j + 1] == EMPTY)
			hasMoves = 1;
		else if (board[i - 1][j - 1] == opponentK || board[i - 1][j - 1] == opponentM)
		{
			//check if can eat
			if (board[i - 2][j - 2] == EMPTY)
				hasMoves = 1;
			
		}
		else if (board[i - 1][j + 1] == opponentK || board[i - 1][j + 1] == opponentM)
		{
			if (board[i - 2][j + 2] == EMPTY)
				hasMoves = 1;
		}
	}
	else// in the middle
	{
		if (board[i - 1][j - 1] == EMPTY || board[i + 1][j - 1] == EMPTY)
			hasMoves = 1;
		else if (board[i - 1][j - 1] == opponentK || board[i - 1][j - 1] == opponentM)
		{
			//check if can't eat
			if (board[i - 2][j - 2] == EMPTY)
				hasMoves = 1;
		}
		else if (board[i + 1][j - 1] == opponentK || board[i + 1][j - 1] == opponentM)
		{
			//check if can't eat
			if (board[i + 2][j - 2] == EMPTY)
				hasMoves = 1;
		}
		else if (board[i - 1][j + 1] == EMPTY || board[i + 1][j + 1] == EMPTY)
			hasMoves = 1;
		else if (board[i - 1][j + 1] == opponentK || board[i - 1][j + 1] == opponentM)
		{
			// check if can eat
			if (board[i - 2][j + 2] == EMPTY)
				hasMoves = 1;
		}
		else if (board[i + 1][j + 1] == opponentK || board[i + 1][j + 1] == opponentM)
		{
			// check if can eat
			if (board[i + 2][j + 2] == EMPTY)
				hasMoves = 1;
		}

		
	}

	return hasMoves;
}

void settingState()
{
	init_board(board);
	printf("%s", ENTER_SETTINGS);
	char *command = getString(stdin, 10);
	while (strcmp(command, "quit") != 0 && strcmp(command, "start") != 0)
	{ 
		reduceSpaces(command);
		executeSettingCmd(command);

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
		free(command);
		if (objectsInMemory > 0)
		{
			printf("You have a memory leak! There are %d objects that were allocated but never freed", objectsInMemory);
			scanf("%s");
		}

		exit(0);
	}
	free(command);
}

void executeSettingCmd(char* input)
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
			clear_board();
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
			if (computer_color == BLACK)
			{
				game_players.user_m = WHITE_M;
				game_players.user_k = WHITE_K;
				game_players.computer_m = BLACK_M;
				game_players.computer_k = BLACK_K;
				game_players.computer_direction = 'D';
				game_players.user_direction = 'U';
			}
			else
			{
				game_players.user_m = BLACK_M;
				game_players.user_k = BLACK_K;
				game_players.computer_k = WHITE_K;
				game_players.computer_m = WHITE_M;
				game_players.computer_direction = 'U';
				game_players.user_direction = 'D';
			}
		}
		else if (strcmp(arr[0], "rm") == 0)
		{
			remove_disc(arr[1]);
		}

	}
	else if (arr_len == 4 && strcmp(arr[0], "set") == 0)//set <x,y> a b
	{
		set_disc(arr[1], arr[2], arr[3]);
	}
	else
	{
		printf("%s", ILLEGAL_COMMAND);
	}


	freeArray(arr, arr_len);
}

void GameState()
{
	int resComputer = 0;
	int resUser = 0;
	int isComputerTurn;
	//first turn
	if (computer_color == WHITE)
	{
		resComputer = computerTurn();
		if (resComputer == 1)
			return;//computer won
		else
		{
			resUser = userTurn();
			isComputerTurn = 1;
		}
			
	}
	else //user starts
	{
		resUser = userTurn();
		if (resUser == 1)
			return;//user won
		else
		{
			resComputer = computerTurn();
			isComputerTurn = 0;
		}
			
	}
	while (resComputer != 1 && resUser != 1)//main loop of the game
	{
		if (isComputerTurn == 1)
			resComputer = computerTurn();
		else
			resUser = userTurn();
	}		

}

char* getStringFormatMove(Move move)
{
	char res[1024] = "move ";
	Pos* curr = move.currPos;
	char* curr_str = getStringFormatPos(curr);
	strcat(res, curr_str);
	strcat(res, " to ");
	PosNode* posList = move.dest;
	while (posList != NULL)
	{
		curr = posList->pos; 
		curr_str = getStringFormatPos(curr);
		strcat(res, curr_str);

		posList = posList->next;
	}
	strcat(res, "\n");
	strcat(res, "\0");
	return res;
}
char* getStringFormatPos(Pos* pos)
{
	char res[1024] = "<";
	char x_char = pos->x + 'a';
	char x_str1[2];
	char y_str1[2];
	x_str1[0] = x_char;
	x_str1[1] = '\0';
	strcat(res, x_str1);//res = <x
	strcat(res, ",");//res = <x,
	int y_num = pos->y + 1;
	char y_char = y_num + '0';
	y_str1[0] = y_char;
	y_str1[1] = '\0';
	strcat(res, y_str1);
	strcat(res, ">");
	strcat(res, "\0");
	return res;

}
/*if computerTurn or playerTurn return 1 - they won otherwise return 0*/
int computerTurn()
{
	//call minimax algorithm
	Move* computerMove = NULL;
	int scorRes = minimax(board, minimax_depth, 1, computerMove);
	//perforam chosen  move
	performMove(board, board, *computerMove, game_players.computer_direction);

	//Todo:print computerMove!!
	char* moveStr = getStringFormatMove(*computerMove);
	printf("%s%s", "Computer: ", moveStr);

	print_board(board);
	//if score is winning return 1 and print computer win!
	if (checkifPlayerWins(computer_color) == 1)
		return 1;
	//else:return  0 and this is the user turn
	return 0;
}
int userTurn()
{
	int player_color;
	if (computer_color == BLACK)
		player_color = WHITE;
	else
		player_color = BLACK;

	printf("%s", ENTER_YOUR_MOVE);
	char* command = getString(stdin, 10);
	if (strstr(command, "get_moves"))
	{
		//call get moves and print it:
		MoveNode* moves = getMoves(board, game_players.user_m, game_players.user_k, game_players.user_direction);
		while (moves != NULL)
		{
			char*  moveStr = getStringFormatMove(*moves->move);
			printf("%s", moveStr);
			moves = moves->next;
		}
	}
	else if (strstr(command, "move"))
	{
		Move *move = parseMoveCommand(command);
		if (move != NULL)
		{
			performUserMove(*move);

			if (checkifPlayerWins(player_color) == 1)
			{
				freeMove(move);
				free(command);
				return 1;			
			}						
		}
			
		freeMove(move);
	}
	else if (strstr(command, "quit"))
	{
		free(command);
		exit(0);
	}
	free(command);
}

int checkifPlayerWins(int player_color)
{
	int scoreNum = score(board, player_color);
	char* color = player_color == WHITE ? "WHITE" : "BLACK" ;
	if (scoreNum == 100)
	{
		printf("%s %s", color, "player wins!\n");
		return 1;
	}
		
	return 0;
}

int performUserMove(Move move)
{
	char user_m;
	char user_k;
	char computer_m;
	char computer_k;
	int king = 0;
	//check if disc is king:
	if (board[move.currPos->x][move.currPos->y] == game_players.user_k)
		king = 1;
	
	
	int x_int = move.currPos->x;
	if (board[x_int][move.currPos->y] != game_players.user_m && board[x_int][move.currPos->y] != game_players.user_k)
	{
		printf("%s", NO_DICS);
		return 0;
	}
	else if (king == 0)
	{
		if (checkMoveIsValidMan(move, game_players.user_direction) == 0)//check man move
		{
			//move is not valid
			printf("%s", ILLEGAL_MOVE);
			return 0;
		}
	}	
	else if (king == 1)
	{
		if (checkMoveIsValidKing(move, game_players.user_direction) == 0)//check king move
		{
			//move is not valid
			printf("%s", ILLEGAL_MOVE);
			return 0;
		}
	}
	int player_color = WHITE;
	if (computer_color == WHITE)
		player_color = BLACK;
		
	//perform moves - if we eat set empty at opponent
	if (king == 0)
	{
		performManMove(board, move, game_players.user_direction);
		print_board(board);
	}
		
	else
	{
		performKingMove(board, move, game_players.user_direction);
		print_board(board);	
	}
		

}

int checkMoveIsValidMan( Move move, char direction)
{
	//no need to check pos border because we already check it in parse move
	int player_color;
	if (direction == 'U')
		player_color = WHITE;
	else
		player_color = BLACK;

	int valid = 0;
	int eat = 0;
	Pos *currPos = move.currPos;
	Pos *nextPos = move.dest->pos;

	int curr_x_int = currPos->x;
	int next_int_x= nextPos->x;

	//check if we eat or not - we have more then one eat
	if (move.dest->next != NULL)
		eat = 1;
	if (abs(curr_x_int - next_int_x) == 1 && abs(currPos->y - nextPos->y) == 1)//single move no eat
	{
		if (direction == 'U')
		{
			if (next_int_x == curr_x_int + 1 && nextPos->y == currPos->y + 1 && board[next_int_x][nextPos->y] == EMPTY)
				valid = 1;
			if (next_int_x == curr_x_int - 1 && nextPos->y == currPos->y + 1 && board[next_int_x][nextPos->y] == EMPTY)
				valid = 1;
		}
		else//user us going down
		{
			if (next_int_x == curr_x_int - 1 && nextPos->y == currPos->y - 1 && board[next_int_x][nextPos->y] == EMPTY)
				valid = 1;
			if (next_int_x == curr_x_int + 1 && nextPos->y == currPos->y - 1 && board[next_int_x][nextPos->y] == EMPTY)
				valid = 1;
		}
		return valid;
	}

	else if (eat == 1 || (abs(curr_x_int - next_int_x) == 2 && abs(currPos->y - nextPos->y) == 2))
	{
		int eatValid = 1;
		PosNode *posList = move.dest;
		//need also to check if the next eat turn man to king and there is more pos in posList then stop and not valid
		
		while (eatValid == 1 && posList->next != NULL)
		{
			if (isManBecomeKing(nextPos, direction) == 1)//posList->next != NULL
			{
				eatValid = 0;
				break;
			}
			eatValid = checkOnePosEat(currPos, nextPos, player_color);

			currPos = nextPos;
			posList = posList->next;
			nextPos = posList->pos;
		}
		//check last move in the list or one eating
		if (eatValid == 1)
			eatValid = checkOnePosEat(currPos, nextPos, player_color);

		valid = eatValid;

		return valid;
	}
	return valid;
}

int isManBecomeKing(Pos* next, char direction)
{
	if (direction == 'U' && next->y == 9)
	{
		return 1;
	}
	else if (direction == 'D' && next->y == 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int checkMoveIsValidKing(Move move, char direction)
{
	int player_color;
	char player_m;
	char player_k;
	char oponnet_m;
	char oponnent_k;
	if (direction == 'U')
	{
		player_color = WHITE;
		player_m = WHITE_M;
		player_k = WHITE_K;
		oponnet_m = BLACK_M;
		oponnent_k = BLACK_K;
	}
		
	else
	{
		player_color = BLACK;
		player_m = BLACK_M;
		player_k = BLACK_K;
		oponnet_m = WHITE_M;
		oponnent_k = WHITE_K;
	}
		

	int valid = 0;
	int eat = 0;
	Pos *currPos = move.currPos;
	Pos *nextPos = move.dest->pos;
	//we already check bounderies in parse move command
	//check next pos is on diagonal of king pos
	if (abs(currPos->x - nextPos->x) != abs(currPos->y - nextPos->y))
	{
		return valid;
	}
	else
	{
		//we are on diagonal
		int empty = 1;
		int opponent_count = 0;
		//we have 4 cases
		//case 1 - up right
		if (currPos->x < nextPos->x && currPos->y < nextPos->y)
		{
			int i = currPos->x+1;
			int j = currPos->y+1;
			while (i <= nextPos->x && j <= nextPos->y)
			{
				if (board[i][j] == player_m || board[i][j] == player_k)
				{
					valid = 0;
					return valid;
				}
				else if (board[i][j] == oponnet_m || board[i][j] == oponnent_k)
				{
					opponent_count++;
				}

				i++;
				j++;
			}
		}
		else if (currPos->x < nextPos->x && currPos->y > nextPos->y)//down right
		{
			int i = currPos->x+1;
			int j = currPos->y-1;
			while (i <= nextPos->x && j >= nextPos->y)
			{
				if (board[i][j] == player_m || board[i][j] == player_k)
				{
					valid = 0;
					return valid;
				}
				else if (board[i][j] == oponnet_m || board[i][j] == oponnent_k)
				{
					opponent_count++;
				}
				i++;
				j--;
			}
		}
		else if (currPos->x > nextPos->x && currPos->y < nextPos->y)//up left
		{
			int i = currPos->x - 1;
			int j = currPos->y + 1;
			while (i >= nextPos->x && j <= nextPos->y)
			{
				if (board[i][j] == player_m || board[i][j] == player_k)
				{
					valid = 0;
					return valid;
				}
				else if (board[i][j] == oponnet_m || board[i][j] == oponnent_k)
				{
					opponent_count++;
				}
				i--;
				j++;
			}
		}
		else if (currPos->x > nextPos->x && currPos->y > nextPos->y)//down left
		{
			int i = currPos->x - 1;
			int j = currPos->y - 1;
			while (i >= nextPos->x && j >= nextPos->y)
			{
				if (board[i][j] == player_m || board[i][j] == player_k)
				{
					valid = 0;
					return valid;
				}
				else if (board[i][j] == oponnet_m || board[i][j] == oponnent_k)
				{
					opponent_count++;
				}
				i--;
				j--;
			}
		}
		if (opponent_count > 1)
		{
			valid = 0;
			return valid;
		}
		//if we reached here thee diagonals are empty/ we have only one opponent player we can eat
		PosNode *posList = move.dest;
		if (opponent_count == 0 && posList->next != NULL)//we try to perform multiple move without eating
		{
			valid = 0;
			return valid;
		}
		else if (posList->next == NULL)//no next, eating or not
		{
			valid = 1;
			return valid;
		}
		else if (opponent_count == 1)//multiple eating
		{
			int eatValid = 1;
			currPos = nextPos;//check multiple eating the same as man after first eating
			posList = posList->next;
			nextPos = posList->pos;
			while (eatValid == 1 && posList->next != NULL)
			{
				eatValid = checkOnePosEat(currPos, nextPos, player_color);

				currPos = nextPos;
				posList = posList->next;
				nextPos = posList->pos;
			}
			//check last move in the list
			if (eatValid == 1)
				eatValid = checkOnePosEat(currPos, nextPos, player_color);

			valid = eatValid;
		}
		
		return valid;
	}
}

/*in case of a valid eating, return the pos of the oponnent on the board*/
Pos getOponnentPos(Pos* curr, Pos* next)
{
	Pos oponnent_pos;
	if (next->x == curr->x + 2 && next->y == curr->y + 2 )
	{
		oponnent_pos.x = curr->x + 1;
		oponnent_pos.y = curr->y + 1;
	}
	else if (next->x == curr->x - 2 && next->y == curr->y + 2 )
	{
		oponnent_pos.x = curr->x - 1;
		oponnent_pos.y = curr->y + 1;
	}
	else if (next->x == curr->x + 2 && next->y == curr->y - 2 )
	{
		oponnent_pos.x = curr->x + 1;
		oponnent_pos.y = curr->y - 1;
	}
	else if (next->x == curr->x - 2 && next->y == curr->y - 2 )
	{
		oponnent_pos.x = curr->x - 1;
		oponnent_pos.y = curr->y - 1;
	}
	return oponnent_pos;
}

int checkOnePosEat(Pos* curr, Pos* next,int player_color)
{
	//check if one eat move is valid'
	int next_int_x = next->x;
	int curr_x_int = curr->x;
	int valid = 0;
	//eat forward:
	if (player_color == computer_color)
	{
		if (next_int_x == curr_x_int + 2 && next->y == curr->y + 2 && board[next_int_x][next->y - 1] == EMPTY)
		{
			if (board[curr_x_int + 1][curr->y+1] == game_players.user_m || board[curr_x_int + 1][curr->y+1] == game_players.user_k)
				valid = 1;
		}

		if (next_int_x == curr_x_int - 2 && next->y == curr->y + 2 && board[next_int_x][next->y - 1] == EMPTY)
		{
			if (board[curr_x_int - 1][curr->y+1] == game_players.user_m || board[curr_x_int - 1][curr->y+1] == game_players.user_k)
				valid = 1;
		}
		if (next_int_x == curr_x_int + 2 && next->y == curr->y - 2 && board[next_int_x][next->y - 1] == EMPTY)
		{
			if (board[curr_x_int + 1][curr->y-1] == game_players.user_m || board[curr_x_int + 1][curr->y-1] == game_players.user_k)
				valid = 1;
		}
		if (next_int_x == curr_x_int - 2 && next->y == curr->y - 2 && board[next_int_x][next->y - 1] == EMPTY)
		{
			if (board[curr_x_int - 1][curr->y-1] == game_players.user_m || board[curr_x_int - 1][curr->y-1] == game_players.user_k)
				valid = 1;
		}
	}
	else // player == user
	{
		if (next_int_x == curr_x_int + 2 && next->y == curr->y + 2 && board[next_int_x][next->y - 1] == EMPTY)
		{
			if (board[curr_x_int + 1][curr->y+1] == game_players.computer_m || board[curr_x_int + 1][curr->y+1] == game_players.computer_k)
				valid = 1;
		}
		if (next_int_x == curr_x_int - 2 && next->y == curr->y + 2 && board[next_int_x][next->y - 1] == EMPTY)
		{
			if (board[curr_x_int - 1][curr->y+1] == game_players.computer_m || board[curr_x_int - 1][curr->y+1] == game_players.computer_k)
				valid = 1;
		}
		if (next_int_x == curr_x_int + 2 && next->y == curr->y - 2 && board[next_int_x][next->y - 1] == EMPTY)
		{
			if (board[curr_x_int + 1][curr->y - 1] == game_players.computer_m || board[curr_x_int + 1][curr->y - 1] == game_players.computer_k)
				valid = 1;
		}
		if (next_int_x == curr_x_int - 2 && next->y == curr->y - 2 && board[next_int_x][next->y - 1] == EMPTY)
		{
			if (board[curr_x_int - 1][curr->y - 1] == game_players.computer_m || board[curr_x_int - 1][curr->y - 1] == game_players.computer_k)
				valid = 1;
		}
	}
	return valid;



}

//perform move for a man only, in case a man becomes a king - change player disc
void performManMove(char board[BOARD_SIZE][BOARD_SIZE],Move move, char direction)
{
	Pos *currPos = move.currPos;
	Pos *nextPos = move.dest->pos;
	PosNode *posList = move.dest;
	//if move is not eating - at man it will be that next
	if (abs(currPos->x - nextPos->x) == 1 && abs(currPos->y - nextPos->y) == 1)
	{
		if (isManBecomeKing(nextPos, direction))
		{
			board[currPos->x][currPos->y] = EMPTY;
			if (direction == 'U')
				board[nextPos->x][nextPos->y] = WHITE_K;
			else
				board[nextPos->x][nextPos->y] = BLACK_K;
		}
		else
		{
			char player = board[currPos->x][currPos->y];
			board[currPos->x][currPos->y] = EMPTY;
			board[nextPos->x][nextPos->y] = player;
		}
		
	}
	else//eating
	{
		while (posList->next != NULL)
		{
			char player = board[currPos->x][currPos->y];
			board[currPos->x][currPos->y] = EMPTY;
			board[nextPos->x][nextPos->y] = player;
			//clean the oponnent disc:
			Pos oponnent_pos = getOponnentPos(currPos, nextPos);
			board[oponnent_pos.x][oponnent_pos.y] = EMPTY;

			currPos = nextPos;
			posList = posList->next;
			nextPos = posList->pos;
		}

		//first or last eat move in list - only single \last eat can turn user to a king!
		//we check that the move is valid already
		if (isManBecomeKing(nextPos, direction))
		{
			board[currPos->x][currPos->y] = EMPTY;
			Pos oponnent_pos = getOponnentPos(currPos, nextPos);
			board[oponnent_pos.x][oponnent_pos.y] = EMPTY;
			if (direction == 'U')
				board[nextPos->x][nextPos->y] = WHITE_K;
			else
				board[nextPos->x][nextPos->y] = BLACK_K;
		}
		else//no king
		{
			char player = board[currPos->x][currPos->y];
			board[currPos->x][currPos->y] = EMPTY;
			board[nextPos->x][nextPos->y] = player;
			Pos oponnent_pos = getOponnentPos(currPos, nextPos);
			board[oponnent_pos.x][oponnent_pos.y] = EMPTY;
		}
		
	}

	
}

void performKingMove(char board[BOARD_SIZE][BOARD_SIZE], Move move, char direction)
{
	char player_color;
	char player_m;
	char player_k;
	char oponnet_m;
	char oponnent_k;
	if (direction == 'U')
	{
		player_color = WHITE;
		player_m = WHITE_M;
		player_k = WHITE_K;
		oponnet_m = BLACK_M;
		oponnent_k = BLACK_K;
	}
	else
	{
		player_color = BLACK;
		player_m = BLACK_M;
		player_k = BLACK_K;
		oponnet_m = WHITE_M;
		oponnent_k = WHITE_K;
	}

	Pos *currPos = move.currPos;
	Pos *nextPos = move.dest->pos;
	PosNode *posList = move.dest;
	//first move is the hardest, after the first eat the eating is like a man
	//case 1 - up right
	if (currPos->x < nextPos->x && currPos->y < nextPos->y)
	{
		int i = currPos->x + 1;
		int j = currPos->y + 1;
		while (i <= nextPos->x && j <= nextPos->y)
		{
			
			if (board[i][j] == oponnet_m || board[i][j] == oponnent_k)
			{
				board[i][j] = EMPTY;
			}

			i++;
			j++;
		}
		board[currPos->x][currPos->y] = EMPTY;
		board[nextPos->x][nextPos->y] = player_k;
	}
	else if (currPos->x < nextPos->x && currPos->y > nextPos->y)//down right
	{
		int i = currPos->x + 1;
		int j = currPos->y - 1;
		while (i <= nextPos->x && j >= nextPos->y)
		{
			if (board[i][j] == oponnet_m || board[i][j] == oponnent_k)
			{
				board[i][j] = EMPTY;
			}
			i++;
			j--;
		}
		board[currPos->x][currPos->y] = EMPTY;
		board[nextPos->x][nextPos->y] = player_k;
	}
	else if (currPos->x > nextPos->x && currPos->y < nextPos->y)//up left
	{
		int i = currPos->x - 1;
		int j = currPos->y + 1;
		while (i >= nextPos->x && j <= nextPos->y)
		{
			if (board[i][j] == oponnet_m || board[i][j] == oponnent_k)
			{
				board[i][j] = EMPTY;
			}
			i--;
			j++;
		}
		board[currPos->x][currPos->y] = EMPTY;
		board[nextPos->x][nextPos->y] = player_k;
	}
	else if (currPos->x > nextPos->x && currPos->y > nextPos->y)//down left
	{
		int i = currPos->x - 1;
		int j = currPos->y - 1;
		while (i >= nextPos->x && j >= nextPos->y)
		{
			if (board[i][j] == oponnet_m || board[i][j] == oponnent_k)
			{
				board[i][j] = EMPTY;
			}
			i--;
			j--;
		}
		board[currPos->x][currPos->y] = EMPTY;
		board[nextPos->x][nextPos->y] = player_k;
	}
	//done with first eating- now let's eat like a refgular man:
	if (posList->next != NULL)
	{
		currPos = nextPos;
		posList = posList->next;
		nextPos = posList->pos;
		while (posList->next != NULL)
		{
			char player = board[currPos->x][currPos->y];
			board[currPos->x][currPos->y] = EMPTY;
			board[nextPos->x][nextPos->y] = player;
			//clean the oponnent disc:
			Pos oponnent_pos = getOponnentPos(currPos, nextPos);
			board[oponnent_pos.x][oponnent_pos.y] = EMPTY;

			currPos = nextPos;
			posList = posList->next;
			nextPos = posList->pos;
		}
		//last eating:
		char player = board[currPos->x][currPos->y];
		board[currPos->x][currPos->y] = EMPTY;
		board[nextPos->x][nextPos->y] = player;

	}
	
}

/*the  board reamin the same and the new one will contain the configuration after performing the move
this function is for the minimax algo*/
void performMove(char board[BOARD_SIZE][BOARD_SIZE], char newBoard[BOARD_SIZE][BOARD_SIZE], Move move, char direction)
{
	int king = 0;
	char playerm;
	char playerk;
	int player_color = WHITE;
	if (direction == game_players.computer_direction)
	{
		playerm = game_players.computer_m;
		playerk = game_players.computer_k;
		player_color = computer_color;
	}
	else
	{
		playerm = game_players.user_m;
		playerk = game_players.user_k;
		if (computer_color == WHITE)
			player_color = BLACK;
	}
	//check if disc is king:
	if (board[move.currPos->x][move.currPos->y] == playerk)
		king = 1;
	//copy board to move board:
	for (int j =0; j <BOARD_SIZE ; j++)
	{
		for (int i = 0; i < BOARD_SIZE; i++)
		{
			newBoard[i][j] = board[i][j];
		}
	}
	//perform the move on the new board:
	if (king == 0)
		performManMove(newBoard, move, direction);
	else
		performKingMove(newBoard, move, direction);
}

//recursive function for return the scoring result of the best move
int minimax(char board[BOARD_SIZE][BOARD_SIZE],int depth, int isMaxplayer, Move* bestMove)
{
	char playerm;
	char playerk;
	char direction;
	
	int player_color = WHITE;
	//the computer is always max player
	if (isMaxplayer == 1)
	{
		player_color = computer_color;
		playerk = game_players.computer_k;
		playerm = game_players.computer_m;
		direction = game_players.computer_direction;
	}
	else
	{
		if (computer_color == WHITE)
			player_color = BLACK;
		playerk = game_players.user_k;
		playerm = game_players.user_m;
		direction = game_players.user_direction;
	}
	//get user and
	MoveNode* moves = getMoves(board, playerm, playerk, direction);
	//check if no moves or depth is 0
	if (moves == NULL || depth == 0)
	{
		int res = score(board, player_color);
		free(moves);
		return res;
	}
	else//############ let's generate minimax tree!
	{
		if (isMaxplayer == 1)//player is the computer
		{
			int bestValue = -100;

			while (moves != NULL)
			{
				int newRes = 0;
				char newBoard[BOARD_SIZE][BOARD_SIZE];
				performMove(board, newBoard, *(moves->move), direction);
				newRes = minimax(newBoard, depth - 1, 0, bestMove);
				

				//check if we are in first recursion
				if (depth == minimax_depth && bestValue < newRes)
				{
					bestValue = newRes;
					bestMove = moves->move;
					
				}
					
				moves = moves->next;
			}
			freeMoves(moves, bestMove);
			return bestValue;
		}
		else//player is the user:
		{
			int bestValue = 100;
			while (moves != NULL)
			{
				int newRes = 0;
				char newBoard[BOARD_SIZE][BOARD_SIZE];
				performMove(board, newBoard, *(moves->move), direction);
				newRes = minimax(newBoard, depth - 1, 1,bestMove);
				if (bestValue > newRes)
					bestValue = newRes;
				

				moves = moves->next;
			}
			freeMoves(moves, bestMove);
			return bestValue;

		}

	}
	freeMoves(moves, bestMove);
}

int main()
{
	unitTests();
	unitTestsSettingFuncs();
	unitTestValidMoves();
	unitTestCheckStuckAndScore();
	unitTestMinimaxAndMoves();

	printf("%s", WELCOME_TO_DRAUGHTS);
	settingState(board);

	//print_message(WRONG_MINIMAX_DEPTH);
	//perror_message("TEST");
	return 0;
	scanf("%s");
}