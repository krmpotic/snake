#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CH_BODY '*'
#define CH_HEAD '@'
#define CH_FOOD 'x'
#define MAX_LEN 1000
#define START_LEN 10
#define MIN_ROW 20
#define MIN_COL 20
#define SLEEP_NS 5e7
#define TAIL(r) ((MAX_LEN + S.ihead + -1 * (r)) % MAX_LEN)

enum dir { UP, RIGHT, DOWN, LEFT };
#define IS_OPPOSITE(d1,d2) ((4+(d1)-(d2))%4==2)


struct food {
	int y;
	int x;
} F;

struct snake {
	int len;
	int ihead;
	enum dir dir;
	int x[MAX_LEN];
	int y[MAX_LEN];
} S;

void init_snake(int row, int col);
void draw_snake(void);
void rand_food(int height, int width);
void draw_food(void);
void mv_snake(enum dir dir);
int check_crash(int height, int width);

void
init_snake(int row, int col)
{
	col = (col - START_LEN) / 2;
	row /= 2;
	for (S.ihead = 0; S.ihead < START_LEN; ++S.ihead) {
		S.x[S.ihead] = col + S.ihead;
		S.y[S.ihead] = row;
	}
	--S.ihead;
	S.len = START_LEN;
	S.dir = RIGHT;
}

void
draw_snake(void)
{
	int i;
	mvaddch(S.y[S.ihead], S.x[S.ihead], CH_HEAD);
	for (i = 1; i < S.len && i < MAX_LEN; ++i)
		mvaddch(S.y[TAIL(i)], S.x[TAIL(i)], CH_BODY);
}

void
rand_food(int height, int width)
{
	int f, i;
	F.x = rand() % width;
	F.y = rand() % height;

	do {
		f = 0;
		F.x = ++F.x % width;
		F.y = ++F.y % height;
		for (i = 0; i < S.len; ++i)
			if (S.x[TAIL(i)] == F.x && S.y[TAIL(i)] == F.y)
				f = 1;
	} while (f);
}

void
draw_food(void)
{
	mvaddch(F.y, F.x, CH_FOOD);
}

void
mv_snake(enum dir dir)
{
	int y = S.y[S.ihead];
	int x = S.x[S.ihead];

	/* prevent movement inside oneself */
	if (!IS_OPPOSITE(dir,S.dir))
		S.dir = dir;

	switch(S.dir) {
	case UP   : --y; break;
	case LEFT : --x; break;
	case DOWN : ++y; break;
	case RIGHT: ++x; break;
	}

	++S.ihead;
	S.ihead = TAIL(0);
	S.y[S.ihead] = y;
	S.x[S.ihead] = x;
}

int
check_crash(int height, int width)
{
	int i;
	if (S.x[S.ihead] < 0 || S.x[S.ihead] >= width)
		return 1;
	if (S.y[S.ihead] < 0 || S.y[S.ihead] >= height)
		return 1;

	for (i = 1; i < S.len; ++i)
		if (S.x[TAIL(0)] == S.x[TAIL(i)] &&
		    S.y[TAIL(0)] == S.y[TAIL(i)]) {
			return 1;
		}
	return 0;
}

int main()
{
	int ch;
	int row, col;
	struct timespec slp = { .tv_sec = 0, .tv_nsec = SLEEP_NS };
	srand((unsigned) time(NULL));

	initscr();
	timeout(0);
	noecho();
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
	rand_food(row, col);

	while ((ch = getch()) != 'q') {
		enum dir dir = S.dir;

		switch(ch) {
		case 'W': case 'w': case KEY_UP   : dir = UP   ; break;
		case 'A': case 'a': case KEY_LEFT : dir = LEFT ; break;
		case 'S': case 's': case KEY_DOWN : dir = DOWN ; break;
		case 'D': case 'd': case KEY_RIGHT: dir = RIGHT; break;
		}
		mv_snake(dir);

		if (check_crash(row, col))
			break;
		erase();
		if (S.x[S.ihead] == F.x && S.y[S.ihead] == F.y) {
			++S.len;
			if (S.len == MAX_LEN)
				break;
			rand_food(row, col);
		}
		draw_snake();
		draw_food();
		refresh();

		nanosleep(&slp, NULL);
	}

	endwin();

	if (S.len == MAX_LEN)
		printf("You won!\n");
	return 0;
}
