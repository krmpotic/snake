#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

#define CH_BODY '*'
#define CH_HEAD '@'
#define CH_FOOD 'x'
#define MAX_LEN 1000
#define START_LEN 50
#define MIN_ROW 20
#define MIN_COL 20

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

void init_snake(int row, int col)
{
	row = (row - START_LEN) / 2;
	col /= 2;
	for (S.ihead = 0; S.ihead < START_LEN; ++S.ihead) {
		S.x[S.ihead] = row + S.ihead;
		S.y[S.ihead] = col;
	}
	--S.ihead;
	S.len = START_LEN;
}

void draw_snake()
{
	int i;
	mvaddch(S.y[S.ihead], S.x[S.ihead], CH_HEAD);
	for (i = 1; i < S.len && i < MAX_LEN; ++i)
		mvaddch(S.y[(MAX_LEN + S.ihead - i) % MAX_LEN],
		        S.x[(MAX_LEN + S.ihead - i) % MAX_LEN],
		        CH_BODY);
}

void mv_snake(int y, int x)
{
	++S.ihead;
	S.y[S.ihead % MAX_LEN] = y;
	S.x[S.ihead % MAX_LEN] = x;
}

int main()
{
	int ch;
	int row, col;

	initscr();
	keypad(stdscr, TRUE);
	curs_set(0);

	getmaxyx(stdscr,row,col);
	if (row < MIN_ROW || col < MIN_COL) {
		endwin();
		fprintf(stderr, "Screen should be at least %d x %d\n", MIN_COL, MIN_ROW);
		exit(1);
	}
	init_snake(row, col);
	clear();
	draw_snake();
	refresh();

	while ((ch = getch()) != 'q') {
		int y = S.y[S.ihead];
		int x = S.x[S.ihead];

		switch(ch) {
		case 'W': case 'w': case KEY_UP:
			--y;
			break;
		case 'A': case 'a': case KEY_LEFT:
			--x;
			break;
		case 'S': case 's': case KEY_DOWN:
			++y;
			break;
		case 'D': case 'd': case KEY_RIGHT:
			++x;
			break;
		default:
			break;
		}

		mv_snake(y, x);
		clear();
		draw_snake();
		refresh();
	}

	endwin();
	return 0;
}


