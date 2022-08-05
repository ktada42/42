#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <ncurses.h>
#include "game_timer.h"

#define BOARD_R 20
#define BOARD_C 15

char board[BOARD_R][BOARD_C] = {0};
int score = 0;
//suseconds_t timer = 400000;
//int decrease = 1000;

typedef struct s_shape{
    char **array;
    int width, row, col;
} t_shape;

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

void free_shape(t_shape *shape){
    for(int i = 0; i < shape->width; i++){
		free(shape->array[i]);
    }
    free(shape->array);
}

bool	is_overlap_offset(const t_shape *shape, int offset_row, int offset_col)
{
	const bool	shape_offset_blank = !shape->array[offset_row][offset_col];
	if (shape_offset_blank)
		return false;
	const int	cur_row		= shape->row + offset_row;
	const int	cur_col		= shape->col + offset_col;
	const bool	overhang	= (cur_col < 0 || cur_col >= BOARD_C || cur_row < 0 || cur_row >= BOARD_R);
	const bool	overlap		= board[cur_row][cur_col];
	return (overhang || overlap);
}

bool is_within_board(const t_shape *shape){
	for(int i = 0; i < shape->width;i++) {
		for(int j = 0; j < shape->width ;j++){
			if (is_overlap_offset(shape, i, j))
				return false;
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
	free_shape(&temp);
}

void display_screen(t_shape *current){
	char Buffer[BOARD_R][BOARD_C] = {0};
	for(int i = 0; i < current->width ;i++){
		for(int j = 0; j < current->width ; j++){
			if(current->array[i][j])
				Buffer[current->row+i][current->col+j] = current->array[i][j];
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
	printw("\nScore: %d\n", score);
}

//struct timeval before_now, now;
/*int hasToUpdate(){
	return ((suseconds_t)(now.tv_sec*1000000 + now.tv_usec) -((suseconds_t)before_now.tv_sec*1000000 + before_now.tv_usec)) > timer;
}*/

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
	if(is_within_board(&tmp_shape))
		current->col--;
}
void	press_key_right(t_shape *current)
{
	t_shape tmp_shape = duplicate_shape(current);
	tmp_shape.col++;
	if(is_within_board(&tmp_shape))
		current->col++;
}

void	press_key_up(t_shape *current)
{
	t_shape tmp_shape = duplicate_shape(current);
	rotate_shape(&tmp_shape);
	if(is_within_board(&tmp_shape))
		rotate_shape(current);
}

void	press_key_down(t_shape *current, t_game_timer *timer, bool *game_on)
{
	t_shape tmp_shape = duplicate_shape(current);
	tmp_shape.row++;
	if(is_within_board(&tmp_shape)){
		current->row++;
		return;
	}
	add_to_the_board(current);
	int sum, count=0;
	for(int i=0;i<BOARD_R;i++){
		sum = 0;
		for(int j=0;j< BOARD_C;j++) {
			sum+=board[i][j];
		}
		if(sum==BOARD_C){
			count++;
			int l, k;
			for(k = i;k >=1;k--)
				for(l=0;l<BOARD_C;l++)
					board[k][l]=board[k-1][l];
			for(l=0;l<BOARD_C;l++)
				board[0][l]=0;
			timer->auto_down_interval-=timer->decrease_fall_interval--;
		}
	}
	score += 100*count;
	t_shape	 new_shape = duplicate_shape(&(StructsArray[rand()%7]));
	new_shape.col = rand()%(BOARD_C-new_shape.width+1);
	new_shape.row = 0;
	free_shape(current);
	*current = new_shape;
	if(!is_within_board(current)){
		*game_on = false;
	}
}

void recieve_pressed_key(t_shape *current, t_game_timer *game_timer, bool *game_on)
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
			press_key_down(current, game_timer, game_on);
			break;
	}
}

void gameover(t_shape *cur_shape)
{
	free_shape(cur_shape);
	endwin();
	int i, j;
	for(i = 0; i < BOARD_R ;i++){
		for(j = 0; j < BOARD_C ; j++){
			printf("%c ", board[i][j] ? '#': '.');
		}
		printf("\n");
	}
	printf("\nGame over!\n");
	printf("\nScore: %d\n", score);
}

int main() {
    srand(time(0));
    score = 0;
    initscr();
	t_game_timer	game_timer;
	bool 			game_on = true;
	init_game_timer(&game_timer);
	record_time(&game_timer);
	set_timeout();
	t_shape cur_shape = duplicate_shape(&(StructsArray[rand()%7]));
    cur_shape.col = rand()%(BOARD_C-cur_shape.width+1);
    cur_shape.row = 0;
	if(!is_within_board(&cur_shape)){
		game_on = false;
	}
    display_screen(&cur_shape);
	while(game_on){
		recieve_pressed_key(&cur_shape, &game_timer, &game_on);
		display_screen(&cur_shape);
		suseconds_t	erapsed_time = calc_elapsed_time_sinece_last_record(&game_timer);
		if (erapsed_time > game_timer.auto_down_interval) {
			press_key_down(&cur_shape, &game_timer, &game_on);
			display_screen(&cur_shape);
			record_time(&game_timer);
		}
	}
	gameover(&cur_shape);
    return 0;
}
