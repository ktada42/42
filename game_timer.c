#include "game_timer.h"

void	init_game_timer(t_game_timer *timer){
	timer->auto_down_interval 			= INITIAL_FALL_INTERVAL;
	timer->decrease_fall_interval	= INITIAL_DECREASE_FALL_INTERVAL;
}

void	record_time(t_game_timer *timer){
	gettimeofday(&(timer->last_recorded_time), NULL);
}

suseconds_t	calc_elapsed_time_sinece_last_record(t_game_timer *timer)
{
	struct timeval	now;
	gettimeofday(&now, NULL);
	const suseconds_t elapsed = ((suseconds_t)(now.tv_sec*1000000 + now.tv_usec) -((suseconds_t)timer->last_recorded_time.tv_sec*1000000 + timer->last_recorded_time.tv_usec));
	return elapsed;
}

bool	has_to_update_by_time_elapsed(t_game_timer *timer){
	return (calc_elapsed_time_sinece_last_record(timer) > timer->auto_down_interval);
}
