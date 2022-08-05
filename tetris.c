#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <ncurses.h>

#define BOARD_R 20
#define BOARD_C 15

char board[BOARD_R][BOARD_C] = {0};
int final = 0;
bool game_on = true;
suseconds_t timer = 400000;
int decrease = 1000;

typedef struct s_shape{
    char **array;
    int width, row, col;
} t_shape;
t_shape current;

const t_shape StructsArray[7]= {
	{(char *[]){(char []){0,1,1},(char []){1,1,0}, (char []){0,0,0}}, 3},
	{(char *[]){(char []){1,1,0},(char []){0,1,1}, (char []){0,0,0}}, 3},
	{(char *[]){(char []){0,1,0},(char []){1,1,1}, (char []){0,0,0}}, 3},
	{(char *[]){(char []){0,0,1},(char []){1,1,1}, (char []){0,0,0}}, 3},
	{(char *[]){(char []){1,0,0},(char []){1,1,1}, (char []){0,0,0}}, 3},
	{(char *[]){(char []){1,1},(char []){1,1}}, 2},
	{(char *[]){(char []){0,0,0,0}, (char []){1,1,1,1}, (char []){0,0,0,0}, (char []){0,0,0,0}}, 4}
};

t_shape duplicate_shape(const t_shape *shape){
	t_shape new_shape = *shape;
	char **copyshape = shape->array;
	new_shape.array = (char**)malloc(new_shape.width*sizeof(char*));
    int i, j;
    for(i = 0; i < new_shape.width; i++){
		new_shape.array[i] = (char*)malloc(new_shape.width*sizeof(char));
		for(j=0; j < new_shape.width; j++) {
			new_shape.array[i][j] = copyshape[i][j];
		}
    }
    return new_shape;
}

void free_shape(t_shape shape){
    for(int i = 0; i < shape.width; i++){
		free(shape.array[i]);
    }
    free(shape.array);
}

bool	within_board_offset(const t_shape *shape, int offset_row, int offset_col)
{
	if (!shape->array[offset_row][offset_col])
		return true;
	int	cur_row = shape->row + offset_row;
	int	cur_col = shape->col + offset_col;
	if((cur_col < 0 || cur_col >= BOARD_C || cur_row >= BOARD_R)){
		return false;				
	}
	if(board[cur_row][cur_col])
		return false;
	return true;
}

bool within_board(const t_shape *shape){
	char **array = shape->array;
	for(int i = 0; i < shape->width;i++) {
		for(int j = 0; j < shape->width ;j++){
			if (!within_board_offset(shape, i, j))
				return (false);
		}
	}
	return true;
}

void rotate_shape(t_shape *shape){
	t_shape temp = duplicate_shape(shape);
	int width = shape->width;
	for(int i = 0; i < width ; i++){
		for(int j = 0, k = width-1; j < width ; j++, k--){
			shape->array[i][j] = temp.array[k][i];
		}
	}
	free_shape(temp);
}

void display_screen(){
	char Buffer[BOARD_R][BOARD_C] = {0};
	for(int i = 0; i < current.width ;i++){
		for(int j = 0; j < current.width ; j++){
			if(current.array[i][j])
				Buffer[current.row+i][current.col+j] = current.array[i][j];
		}
	}
	clear();
	for(int i=0; i<BOARD_C-9; i++)
		printw(" ");
	printw("42 Tetris\n");
	for(int i = 0; i < BOARD_R ;i++){
		for(int j = 0; j < BOARD_C ; j++){
			printw("%c ", (board[i][j] + Buffer[i][j])? '#': '.');
		}
		printw("\n");
	}
	printw("\nScore: %d\n", final);
}

struct timeval before_now, now;
int hasToUpdate(){
	return ((suseconds_t)(now.tv_sec*1000000 + now.tv_usec) -((suseconds_t)before_now.tv_sec*1000000 + before_now.tv_usec)) > timer;
}

void set_timeout(void) {
	timeout(1);
}

void add_to_the_board(t_shape *shape)
{
	for(int i = 0; i < shape->width; i++){
		for(int j = 0; j < shape->width; j++){
			board[shape->row + i][shape->col + j] |= shape->array[i][j];
		}
	}
}

enum e_input_keys
{
	key_up = 'w',
	key_down = 's',
	key_left = 'a',
	key_right = 'd'
};



void	press_key_left(t_shape *current)
{
	t_shape tmp_shape = duplicate_shape(current);
	tmp_shape.col--;
	if(within_board(&tmp_shape))
		current->col--;
}
void	press_key_right(t_shape *current)
{
	t_shape tmp_shape = duplicate_shape(current);
	tmp_shape.col++;
	if(within_board(&tmp_shape))
		current->col++;
}

void	press_key_up(t_shape *current)
{
	t_shape tmp_shape = duplicate_shape(current);
	rotate_shape(&tmp_shape);
	if(within_board(&tmp_shape))
		rotate_shape(current);
}

void	press_key_down(t_shape *current)
{
	t_shape tmp_shape = duplicate_shape(current);
	tmp_shape.row++;
	if(within_board(&tmp_shape)){
		current->row++;
		return;
	}
	add_to_the_board(current);
	int n, m, sum, count=0;
	for(n=0;n<BOARD_R;n++){
		sum = 0;
		for(m=0;m< BOARD_C;m++) {
			sum+=board[n][m];
		}
		if(sum==BOARD_C){
			count++;
			int l, k;
			for(k = n;k >=1;k--)
				for(l=0;l<BOARD_C;l++)
					board[k][l]=board[k-1][l];
			for(l=0;l<BOARD_C;l++)
				board[0][l]=0;
			timer-=decrease--;
		}
	}
	final += 100*count;
	t_shape	 new_shape = duplicate_shape(&(StructsArray[rand()%7]));
	new_shape.col = rand()%(BOARD_C-new_shape.width+1);
	new_shape.row = 0;
	free_shape(*current);
	*current = new_shape;
	if(!within_board(current)){
		game_on = false;
	}
}

void recieve_pressed_key(t_shape *current)
{
	const char	pressed_key = getch();
	if (pressed_key == ERR)
		return ;
	switch(pressed_key){
		case key_left:
			press_key_left(current);
			break;
		case key_right:
			press_key_right(current);
			break;
		case key_up:
			press_key_up(current);
			break;
		case key_down:
			press_key_down(current);
			break;
	}
}


int main() {
    srand(time(0));
    final = 0;
    initscr();
	gettimeofday(&before_now, NULL);
	set_timeout();
	t_shape new_shape = duplicate_shape(&(StructsArray[rand()%7]));
    new_shape.col = rand()%(BOARD_C-new_shape.width+1);
    new_shape.row = 0;
	current = new_shape;
	if(!within_board(&current)){
		game_on = false;
	}
    display_screen();
	while(game_on){
		recieve_pressed_key(&current);
		display_screen();
		gettimeofday(&now, NULL);
		if (hasToUpdate()) {
			press_key_down(&current);
			display_screen();
			gettimeofday(&before_now, NULL);
		}
	}
	free_shape(current);
	endwin();
	int i, j;
	for(i = 0; i < BOARD_R ;i++){
		for(j = 0; j < BOARD_C ; j++){
			printf("%c ", board[i][j] ? '#': '.');
		}
		printf("\n");
	}
	printf("\nGame over!\n");
	printf("\nScore: %d\n", final);
    return 0;
}
