#ifndef GAME_TIMER_H
# define GAME_TIMER_H

#include <time.h>
#include <sys/time.h>
#include <ncurses.h>

# define INITIAL_FALL_INTERVAL			400000
# define INITIAL_DECREASE_FALL_INTERVAL	1000

typedef struct s_game_timer{
    suseconds_t		auto_down_interval;
    int				decrease_fall_interval;
	struct timeval	last_recorded_time;
} t_game_timer;

void		init_game_timer(t_game_timer *timer);
void		record_time(t_game_timer *timer);
suseconds_t	calc_elapsed_time_sinece_last_record(t_game_timer *timer);
bool		has_to_update_by_time_elapsed(t_game_timer *timer);

//cur_shape
//board
//gameon
//score

# endif