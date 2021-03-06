#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "mazegen.h"


void display_maze(int* maze, int rows, int cols);
int* create_maze(int rows, int cols, float horiSkew);
int* break_walls(int* maze, int holes, int rows, int cols);

int* initialize_maze(int rows, int cols);
void dfs_recursive(int* maze, int current, int rows, int cols, float horiSkew);

int check_square(int* maze, int current, int translation, int rows, int cols);
int check_neighbors(int* maze, int current, int rows, int cols);
int choose_dir(float horiSkew);

// horiSkew determines how much the maze goes left-right (0 == All up and down, 1 == All left and right)
int* create_maze(int rows, int cols, float horiSkew) {

	srand(time(NULL));

	// empty squares are 0s
	int *maze = initialize_maze(rows, cols);

	// Selects start position from left side of maze
	// First line selects how far down to start (has to start at an odd index)
	// Second line gets it to row and moves over one (so doesn't start in wall)
	int start = (rand() % (rows/2 - 1)) * 2 + 1;
	start *= cols; start++;

	// sets the start of the maze to an empty square
	maze[start] = 0;

	dfs_recursive(maze, start, rows, cols, horiSkew);
	
	return maze;
}

// loops to create path through maze
void dfs_recursive(int* maze, int current, int rows, int cols, float horiSkew) {

	// offsets for each direction
	int translations[4] = {-1, 1, cols, -cols};

	// checks for empty neighbor
	// if none found backtrack
	if (!check_neighbors(maze, current, rows, cols)) return;
	
	while (check_neighbors(maze, current, rows, cols)) {
		
		// chooses direction to go in
		int dir = choose_dir(horiSkew);
	
		int trans = translations[dir];

		// if the square in the chosen direction is full (unvisited), continue the loop
		// otherwise (square is empty/visited) choose new direction and try it again
		if (!check_square(maze, current, trans*2, rows, cols)) continue;
		// clears chosen square and intermediate wall
		maze[current+trans] = 0;
		maze[current+trans*2] = 0;

		// calls itself again
		dfs_recursive(maze, current+trans*2, rows, cols, horiSkew);
	}
}

// Checks surrounding squares (skipping one since those are walls)
int check_neighbors(int* maze, int current, int rows, int cols) {
	int room = 0;

	if (check_square(maze, current,  2, rows, cols)) room = 1; // checks right
	if (check_square(maze, current, -2, rows, cols)) room = 1; // checks left
	if (check_square(maze, current,  2*cols, rows, cols)) room = 1; // checks down
	if (check_square(maze, current, -2*cols, rows, cols)) room = 1; // checks up
	return room;
}

// ensures that square is in grid
int check_square(int* maze, int current, int translation, int rows, int cols) {
    return ((current % cols + translation % cols) > -1 && 
            (current % cols + translation % cols) < cols &&
            (current / cols + translation / cols) > -1 &&
            (current / cols + translation / cols) < rows &&
            maze[current+translation]);
}

int* initialize_maze(int rows, int cols) { 
	// allocate memory
	int *a = (int*)calloc(rows*cols, sizeof(int));

	// fill maze with 1s
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			a[cols*i+j] = 1;
		}
	}
	return a;
}

void display_maze(int *maze, int rows, int cols) {
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			// all chars are doubled to make more square-ish
			if (maze[cols*i+j]) {
				// colored block unicode
				printf("\u2588\u2588");
			} else {
				printf("  ");
			}
		}
		printf("\n");
	}
}

int choose_dir(float horiSkew) {

	float choice = (float)(rand() % 100);

	enum directions{LEFT, RIGHT, DOWN, UP};

	if (choice < horiSkew*100) {
		// randomly chose between left and right
		if (rand() % 2) return LEFT;
		return RIGHT;
	}  else {
		// randomly choose between up and down
		if (rand() % 2) return DOWN;
		return UP;
	}
}

int* break_walls(int* maze, int holes,  int rows, int cols) {

	srand(time(NULL));

	int created  = 0;

	while (created < holes) {
		int randRow = (rand() % (rows/2 - 1)) * 2 + 1;
		int randCol = (rand() % (cols/2 - 1)) * 2 + 1;
		
		// either clears right or clears down
		int goRight = rand() % 2;
		int square = randRow*cols + randCol;
		if (goRight && maze[square+1] == 1) {
			maze[square+1] = 0;
			created++;
		} else if (!goRight && maze[square+cols] == 1) {
			maze[square+cols] = 0;
			created++;
		}
	}
	return maze;
}


