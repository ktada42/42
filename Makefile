tetris: tetris.c
	gcc tetris.c game_timer.c -lncurses -g -o tetris
clean:
	rm tetris