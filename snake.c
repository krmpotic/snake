#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

#define CH_BODY '*'
#define CH_HEAD '@'
#define CH_FOOD 'x'
#define MAX_LEN 1000
#define START_LEN 10
#define MIN_ROW 20
#define MIN_COL 20
#define rel_ind(r) ((MAX_LEN + S.ihead + (r)) % MAX_LEN)

enum dir { UP, DOWN, RIGHT, LEFT };

struct point {
	int y;
	int x;
};

struct snake {
	int len;
	int ihead;
	int x[MAX_LEN];
	int y[MAX_LEN];
} S;

WINDOW *sandbox;

void init_snake(int row, int col)
{
	col = (col - START_LEN) / 2;
	row /= 2;
	for (S.ihead = 0; S.ihead < START_LEN; ++S.ihead) {
		S.x[S.ihead] = col + S.ihead;
		S.y[S.ihead] = row;
	}
	--S.ihead;
	S.len = START_LEN;
}

void draw_snake()
{
	int i;
	mvwaddch(sandbox, S.y[S.ihead], S.x[S.ihead], CH_HEAD);
	for (i = 1; i < S.len && i < MAX_LEN; ++i)
		mvwaddch(sandbox,
		        S.y[rel_ind(-i)],
		        S.x[rel_ind(-i)],
		        CH_BODY);
}

void mv_snake(enum dir dir)
{
	int y = S.y[S.ihead];
	int x = S.x[S.ihead];

	/* prevent movement inside oneself */
	if (S.len > 1) {
		enum dir prev;
		if      (S.x[S.ihead] - S.x[rel_ind(-1)] == 1)
			prev = RIGHT;
		else if (S.x[S.ihead] - S.x[rel_ind(-1)] == -1)
			prev = LEFT;
		else if (S.y[S.ihead] - S.y[rel_ind(-1)] == 1)
			prev = DOWN;
		else
			prev = UP;

		if      (dir == LEFT  && prev == RIGHT)
			dir = RIGHT;
		else if (dir == RIGHT && prev == LEFT)
			dir = LEFT;
		else if (dir == UP    && prev == DOWN)
			dir = DOWN;
		else if (dir == DOWN  && prev == UP)
			dir = UP;
	}

	switch(dir) {
	case UP   : --y; break;
	case LEFT : --x; break;
	case DOWN : ++y; break;
	case RIGHT: ++x; break;
	}

	++S.ihead;
	S.ihead = rel_ind(0);
	S.y[S.ihead] = y;
	S.x[S.ihead] = x;
}

int check_crash(int height, int width)
{
	int i;
	if (S.x[S.ihead] < 1 || S.x[S.ihead] >= width - 1)
		return 1;
	if (S.y[S.ihead] < 1 || S.y[S.ihead] >= height - 1)
		return 1;

	for (i = 1; i < S.len; ++i)
		if (S.x[rel_ind(0)] == S.x[rel_ind(-i)] &&
		    S.y[rel_ind(0)] == S.y[rel_ind(-i)]) {
			return 1;
		}
	return 0;
}

int main()
{
	int ch;
	enum dir dir;
	int row, col;

	initscr();
	cbreak();
	keypad(stdscr, TRUE);
	curs_set(0);

	getmaxyx(stdscr,row,col);
	if (row < MIN_ROW || col < MIN_COL) {
		endwin();
		fprintf(stderr, "Screen should be at least %d x %d\n", MIN_COL, MIN_ROW);
		exit(1);
	}

	init_snake(row, col);

	refresh();
	sandbox = newwin(row-2, col-2, 1, 1);
	draw_snake();
	box(sandbox, 0, 0);
	wrefresh(sandbox);

	while ((ch = getch()) != 'q') {
		switch(ch) {
		case 'W': case 'w': case KEY_UP   : dir = UP   ; break;
		case 'A': case 'a': case KEY_LEFT : dir = LEFT ; break;
		case 'S': case 's': case KEY_DOWN : dir = DOWN ; break;
		case 'D': case 'd': case KEY_RIGHT: dir = RIGHT; break;
		}

		mv_snake(dir);
		if (check_crash(row-2, col-2))
			break;
		clear();
		refresh();
		wclear(sandbox);
		draw_snake();
        	box(sandbox, 0, 0);
		wrefresh(sandbox);
	}

	delwin(sandbox);
	endwin();
	return 0;
}


