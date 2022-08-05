#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <ncurses.h>
#include "game_timer.h"

#define BOARD_R 	20
#define BOARD_C 	15
#define TIMEOUT_MS	1

typedef struct s_shape{
    char **array;
    int width, row, col;
} t_shape;

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

t_shape make_random_shape(){
	const t_shape StructsArray[7]= {
		{(char *[]){
			(char []){0,1,1},
			(char []){1,1,0}, 
			(char []){0,0,0}
		}, 3},
		{(char *[]){
			(char []){1,1,0},
			(char []){0,1,1}, 
			(char []){0,0,0}
		}, 3},
		{(char *[]){
			(char []){0,1,0},
			(char []){1,1,1}, 
			(char []){0,0,0}
		}, 3},
		{(char *[]){
			(char []){0,0,1},
			(char []){1,1,1}, 
			(char []){0,0,0}
		}, 3},
		{(char *[]){
			(char []){1,0,0},
			(char []){1,1,1}, 
			(char []){0,0,0}
		}, 3},
		{(char *[]){
			(char []){1,1},
			(char []){1,1}
		}, 2},
		{(char *[]){
			(char []){0,0,0,0}, 
			(char []){1,1,1,1}, 
			(char []){0,0,0,0}, 
			(char []){0,0,0,0}
		}, 4}
	};
	t_shape shape = duplicate_shape(&(StructsArray[rand() % 7]));
	shape.col = rand()%(BOARD_C - shape.width + 1);
	shape.row = 0;
	return shape;
}

void free_shape(t_shape *shape){
    for(int i = 0; i < shape->width; i++){
		free(shape->array[i]);
    }
    free(shape->array);
}

bool	is_valid_pos_offset(const t_shape *shape, char (*board)[BOARD_R][BOARD_C], int offset_row, int offset_col)
{
	const bool	offset_pos_is_blank = !shape->array[offset_row][offset_col];
	if (offset_pos_is_blank)
		return true;
	const int	cur_row		= shape->row + offset_row;
	const int	cur_col		= shape->col + offset_col;
	const bool	overhang	= (cur_col < 0 || cur_col >= BOARD_C || cur_row < 0 || cur_row >= BOARD_R);
	const bool	overlap		= (*board)[cur_row][cur_col];
	return (!overhang && !overlap);
}

bool is_valid_pos(const t_shape *shape, char (*board)[BOARD_R][BOARD_C]){
	for(int i = 0; i < shape->width;i++) {
		for(int j = 0; j < shape->width ;j++){
			if (!is_valid_pos_offset(shape, board, i, j))
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

void display_screen(t_shape *current, char (*board)[BOARD_R][BOARD_C], int score){
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
			printw("%c ", ((*board)[i][j] + Buffer[i][j])? '#': '.');
		}
		printw("\n");
	}
	printw("\nScore: %d\n", score);
}

void add_to_the_board(t_shape *shape, char (*board)[BOARD_R][BOARD_C])
{
	for(int i = 0; i < shape->width; i++){
		for(int j = 0; j < shape->width; j++){
			(*board)[shape->row + i][shape->col + j] |= shape->array[i][j];
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

void	press_key_left(t_shape *current, char (*board)[BOARD_R][BOARD_C], int *score)
{
	t_shape tmp_shape = duplicate_shape(current);
	tmp_shape.col--;
	if(is_valid_pos(&tmp_shape, board))
		current->col--;
	display_screen(current, board, *score);	
}

void	press_key_right(t_shape *current, char (*board)[BOARD_R][BOARD_C], int *score)
{
	t_shape tmp_shape = duplicate_shape(current);
	tmp_shape.col++;
	if(is_valid_pos(&tmp_shape, board))
		current->col++;
	display_screen(current, board, *score);	
}

void	press_key_up(t_shape *current, char (*board)[BOARD_R][BOARD_C], int *score)
{
	t_shape tmp_shape = duplicate_shape(current);
	rotate_shape(&tmp_shape);
	if(is_valid_pos(&tmp_shape, board))
		rotate_shape(current);
	display_screen(current, board, *score);	
}

void	press_key_down(t_shape *current, t_game_timer *timer, char (*board)[BOARD_R][BOARD_C], int *score, bool *game_on)
{
	t_shape tmp_shape = duplicate_shape(current);
	tmp_shape.row++;
	if(is_valid_pos(&tmp_shape, board)){
		current->row++;
		free_shape(&tmp_shape);
		display_screen(current, board, *score);
		return;
	}
	free_shape(&tmp_shape);
	add_to_the_board(current, board);
	free_shape(current);
	int sum, count=0;
	for(int i=0;i<BOARD_R;i++){
		sum = 0;
		for(int j=0;j< BOARD_C;j++) {
			sum+=(*board)[i][j];
		}
		if(sum==BOARD_C){
			count++;
			int l, k;
			for(k = i;k >=1;k--)
				for(l=0;l<BOARD_C;l++)
					(*board)[k][l]=(*board)[k-1][l];
			for(l=0;l<BOARD_C;l++)
				(*board)[0][l]=0;
			timer->auto_down_interval-=timer->decrease_fall_interval--;
		}
	}
	*score += 100*count;
	t_shape	 new_shape = make_random_shape();
	*current = new_shape;
	if(!is_valid_pos(current, board)){
		*game_on = false;
	}
	display_screen(current, board, *score);
}

void recieve_pressed_key(t_shape *current, t_game_timer *game_timer, char (*board)[BOARD_R][BOARD_C], int *score, bool *game_on)
{
	const char	pressed_key = getch();
	if (pressed_key == ERR)
		return ;
	switch(pressed_key){
		case key_left:
			press_key_left(current, board, score);
			break;
		case key_right:
			press_key_right(current, board, score);
			break;
		case key_up:
			press_key_up(current, board, score);
			break;
		case key_down:
			press_key_down(current, game_timer, board, score, game_on);
			break;
	}
}

void print_result(int	score, const char	(*board)[BOARD_R][BOARD_C])
{
	for(int i = 0; i < BOARD_R ;i++){
		for(int j = 0; j < BOARD_C ; j++){
			printf("%c ", (*board)[i][j] ? '#': '.');
		}
		printf("\n");
	}
	printf("\nGame over!\n");
	printf("\nScore: %d\n", score);
}

void	init_game(t_game_timer *game_timer){
	srand(time(0));
	initscr();
	timeout(TIMEOUT_MS);
	init_game_timer(game_timer);
}

void	game_start(int *score, char	(*board)[BOARD_R][BOARD_C])
{
	t_game_timer	game_timer;
	init_game(&game_timer);
	record_time(&game_timer);
	t_shape cur_shape = make_random_shape();
	if(!is_valid_pos(&cur_shape, board)){
		free_shape(&cur_shape);
		return;
	}
	display_screen(&cur_shape, board, *score);
	bool	game_on = true;
	while(game_on){
		recieve_pressed_key(&cur_shape, &game_timer, board, score, &game_on);
		suseconds_t	erapsed_time = calc_elapsed_time_sinece_last_record(&game_timer);
		if (erapsed_time > game_timer.auto_down_interval) {
			press_key_down(&cur_shape, &game_timer, board, score, &game_on);
			record_time(&game_timer);
		}
	}
	free_shape(&cur_shape);
	endwin();
}

int main() {
	int		score					= 0;
	char	board[BOARD_R][BOARD_C] = {0};
	game_start(&score, &board);
	print_result(score, &board);
	system("leaks tetris");
    return 0;
}
