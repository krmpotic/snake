snake: snake.c
	gcc -lncurses snake.c -o snake

clean:
	rm -f snake

.PHONY: clean
