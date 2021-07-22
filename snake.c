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

#define IS_HEAD(_y,_x) ((_y) == S.y[S.ihead] && (_x) == S.x[S.ihead])
#define TAIL(r) ((MAX_LEN + S.ihead + -1 * (r)) % MAX_LEN)

#define IS_OPPOSITE(d1,d2) (-(d1)==(d2))
enum dir { LEFT = -2, DOWN = -1, UP = 1, RIGHT = 2 };

int row, col;

struct food { int y; int x; } F;
struct snake {
	int len;
	int ihead;
	enum dir dir;
	int x[MAX_LEN];
	int y[MAX_LEN];
} S;

int is_inside_snake(int y, int x);
void init_snake();
void mv_snake(enum dir dir);
int check_crash();
void draw_snake();
void rand_food();
int check_food();
void draw_food();
int getch_last();

int
is_inside_snake(int y, int x)
{
	for (int i = 1; i < S.len; i++)
		if (x == S.x[TAIL(i)] && y == S.y[TAIL(i)])
			return 1;
	return 0;
}

void
init_snake()
{
	int i;
	int x = (col - START_LEN) / 2;
	int y = row / 2;
	for (i = 0; i < START_LEN; i++) {
		S.x[i] = x + i;
		S.y[i] = y;
	}
	S.ihead = i-1;
	S.len = START_LEN;
	S.dir = RIGHT;
}

void
mv_snake(enum dir dir)
{
	int y = S.y[S.ihead];
	int x = S.x[S.ihead];

	if (!IS_OPPOSITE(dir,S.dir)) /* prevent movement inside oneself */
		S.dir = dir;

	switch(S.dir) {
	case UP   : y--; break;
	case LEFT : x--; break;
	case DOWN : y++; break;
	case RIGHT: x++; break;
	}

	S.ihead++;
	S.ihead = TAIL(0);
	S.y[S.ihead] = y;
	S.x[S.ihead] = x;
}

int
check_crash()
{
	if (S.x[S.ihead] < 0 || S.x[S.ihead] >= col ||
	    S.y[S.ihead] < 0 || S.y[S.ihead] >= row)
		return 1;
	return is_inside_snake(S.y[S.ihead], S.x[S.ihead]);
}

void
draw_snake()
{
	for (int i = 0; i < S.len; i++)
		mvaddch(S.y[TAIL(i)], S.x[TAIL(i)], (i==0)?CH_HEAD:CH_BODY);
}

void
rand_food()
{
	F.x = rand() % col;
	F.y = rand() % row;

	do {
		F.x = ++F.x % col;
		if (F.x == 0)
			F.y == ++F.y % row;
	} while (is_inside_snake(F.y,F.x) || IS_HEAD(F.y, F.x));
}

int
check_food()
{
	return IS_HEAD(F.y,F.x);
}

void
draw_food()
{
	mvaddch(F.y, F.x, CH_FOOD);
}

int
getch_last()
{
	int b, c;
	b = ERR;
	while ((c = getch()) != ERR)
		b = c;
	return b;
}

int
main()
{
	int ch;
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

	init_snake();
	rand_food();

	while ((ch = getch_last()) != 'q') {
		enum dir dir = S.dir;
		switch(ch) {
		case 'w': case KEY_UP   : dir = UP   ; break;
		case 'a': case KEY_LEFT : dir = LEFT ; break;
		case 's': case KEY_DOWN : dir = DOWN ; break;
		case 'd': case KEY_RIGHT: dir = RIGHT; break;
		}
		mv_snake(dir);

		if (check_crash())
			break;
		if (check_food()) {
			if (++S.len == MAX_LEN)
				break;
			rand_food();
		}

		erase();
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
