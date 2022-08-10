#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <ncurses.h>
#include "game_timer.h"

#define BOARD_R 	5
#define BOARD_C 	20
#define TIMEOUT_MS	1

typedef struct s_piece{
    char **array;
    int width, row, col;
} t_piece;

t_piece duplicate_piece(const t_piece *piece){
	t_piece new_piece = *piece;
	char **copypiece = piece->array;
	new_piece.array = (char**)malloc(new_piece.width*sizeof(char*));
    int i, j;
    for(i = 0; i < new_piece.width; i++){
		new_piece.array[i] = (char*)malloc(new_piece.width*sizeof(char));
		for(j=0; j < new_piece.width; j++) {
			new_piece.array[i][j] = copypiece[i][j];
		}
    }
    return new_piece;
}

t_piece make_random_piece(){
	const t_piece StructsArray[7]= {
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
	t_piece piece = duplicate_piece(&(StructsArray[rand() % 7]));
	piece.col = rand()%(BOARD_C - piece.width + 1);
	piece.row = 0;
	return piece;
}

void free_piece(t_piece *piece){
    for(int i = 0; i < piece->width; i++){
		free(piece->array[i]);
    }
    free(piece->array);
}

bool	is_valid_pos_offset(const t_piece *piece, char (*board)[BOARD_R][BOARD_C], int offset_row, int offset_col)
{
	const bool	offset_pos_is_blank = !piece->array[offset_row][offset_col];
	if (offset_pos_is_blank)
		return true;
	const int	cur_row		= piece->row + offset_row;
	const int	cur_col		= piece->col + offset_col;
	const bool	overhang	= (cur_col < 0 || cur_col >= BOARD_C || cur_row < 0 || cur_row >= BOARD_R);
	const bool	overlap		= (*board)[cur_row][cur_col];
	return (!overhang && !overlap);
}

bool is_valid_piece(const t_piece *piece, char (*board)[BOARD_R][BOARD_C]){
	for(int i = 0; i < piece->width;i++) {
		for(int j = 0; j < piece->width ;j++){
			if (!is_valid_pos_offset(piece, board, i, j))
				return false;
		}
	}
	return true;
}

void rotate_piece(t_piece *piece){
	t_piece temp = duplicate_piece(piece);
	int width = piece->width;
	for(int i = 0; i < width ; i++){
		for(int j = 0, k = width-1; j < width ; j++, k--){
			piece->array[i][j] = temp.array[k][i];
		}
	}
	free_piece(&temp);
}

void	print_board(t_piece *current, const char (*board)[BOARD_R][BOARD_C]){
	char piece_of_board[BOARD_R][BOARD_C] = {0};
	if (current){
	for(int i = 0; i < current->width ;i++){
		for(int j = 0; j < current->width ; j++){
			if(current->array[i][j])
				piece_of_board[current->row+i][current->col+j] = current->array[i][j];
		}
	}
	}
	for(int i = 0; i < BOARD_R ;i++){
		for(int j = 0; j < BOARD_C ; j++){
			printw("%c ", piece_of_board[i][j] || (*board)[i][j] ? '#': '.');
		}
		printw("\n");
	}
}

void display_screen(t_piece *current, char (*board)[BOARD_R][BOARD_C], int score){
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
	print_board(current, board);
	/*for(int i = 0; i < BOARD_R ;i++){
		for(int j = 0; j < BOARD_C ; j++){
			printw("%c ", ((*board)[i][j] + Buffer[i][j])? '#': '.');
		}
		printw("\n");
	}*/
	printw("\nScore: %d\n", score);
}

void add_to_the_board(t_piece *piece, char (*board)[BOARD_R][BOARD_C])
{
	for(int i = 0; i < piece->width; i++){
		for(int j = 0; j < piece->width; j++){
			(*board)[piece->row + i][piece->col + j] |= piece->array[i][j];
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

void	move_key_left(t_piece *current, char (*board)[BOARD_R][BOARD_C], int *score)
{
	t_piece tmp_piece = duplicate_piece(current);
	tmp_piece.col--;
	if(is_valid_piece(&tmp_piece, board))
		current->col--;
	display_screen(current, board, *score);	
}

void	move_piece_right(t_piece *current, char (*board)[BOARD_R][BOARD_C], int *score)
{
	t_piece tmp_piece = duplicate_piece(current);
	tmp_piece.col++;
	if(is_valid_piece(&tmp_piece, board))
		current->col++;
	display_screen(current, board, *score);	
}

void	move_piece_up(t_piece *current, char (*board)[BOARD_R][BOARD_C], int *score)
{
	t_piece tmp_piece = duplicate_piece(current);
	rotate_piece(&tmp_piece);
	if(is_valid_piece(&tmp_piece, board))
		rotate_piece(current);
	display_screen(current, board, *score);	
}

void	move_piece_down(t_piece *current, t_game_timer *timer, char (*board)[BOARD_R][BOARD_C], int *score, bool *game_on)
{
	t_piece tmp_piece = duplicate_piece(current);
	tmp_piece.row++;
	if(is_valid_piece(&tmp_piece, board)){
		current->row++;
		free_piece(&tmp_piece);
		display_screen(current, board, *score);
		return;
	}
	free_piece(&tmp_piece);
	add_to_the_board(current, board);
	free_piece(current);
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
	t_piece	 new_piece = make_random_piece();
	*current = new_piece;
	if(!is_valid_piece(current, board)){
		*game_on = false;
	}
	display_screen(current, board, *score);
}

bool is_valid_key()
{
	const char	pressed_key = getch();
	return (pressed_key == key_left ||
			pressed_key == key_right||
			pressed_key == key_up	||
			pressed_key == key_down);
}

void recieve_key_and_do_operation(t_piece *current, t_game_timer *game_timer, char (*board)[BOARD_R][BOARD_C], int *score, bool *game_on)
{
	char	pressed_key;

	pressed_key = getch();
	switch(pressed_key){
		case key_left:
			move_key_left(current, board, score);
			break;
		case key_right:
			move_piece_right(current, board, score);
			break;
		case key_up:
			move_piece_up(current, board, score);
			break;
		case key_down:
			move_piece_down(current, game_timer, board, score, game_on);
			break;
	}
}

void print_result(int	score, const char	(*board)[BOARD_R][BOARD_C])
{
	print_board(NULL, board);
	printf("\nGame over!\n");
	printf("\nScore: %d\n", score);
}

void	init_game(t_game_timer *game_timer, t_piece *cur_piece, bool *game_on){
	srand(time(0));
	initscr();
	timeout(TIMEOUT_MS);
	init_game_timer(game_timer);
	*cur_piece = make_random_piece();
	*game_on = true;
}

void	finish_game_start(t_piece *cur_piece)
{
	free_piece(cur_piece);
	endwin();
}

void	game_start(int *score, char	(*board)[BOARD_R][BOARD_C])
{
	t_game_timer	game_timer;
	t_piece			cur_piece;
	bool			game_on;

	init_game(&game_timer, &cur_piece, &game_on);
	display_screen(&cur_piece, board, *score);
	record_time(&game_timer);
	while(game_on){
		recieve_key_and_do_operation(&cur_piece, &game_timer, board, score, &game_on);
		suseconds_t	erapsed_time = calc_elapsed_time_sinece_last_record(&game_timer);
		if (erapsed_time > game_timer.auto_down_interval) {
			move_piece_down(&cur_piece, &game_timer, board, score, &game_on);
			record_time(&game_timer);
		}
	}
	finish_game_start(&cur_piece);
}

int main() {
	int		score					= 0;
	char	board[BOARD_R][BOARD_C] = {0};
	game_start(&score, &board);
	print_result(score, &board);
	//system("leaks tetris");
    return 0;
}
