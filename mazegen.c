#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "mazegen.h"

int* initialize_maze(int rows, int cols);
void display_maze(int* maze, int rows, int cols);

int* create_maze(int rows, int cols);
void dfs_recursive(int* maze, int current, int rows, int cols);

int check_square(int* maze, int current, int translation, int rows, int cols);
int check_neighbors(int* maze, int current, int rows, int cols);
int choose_dir(float weights[4]);


int* create_maze(int rows, int cols) {

	// empty squares are 0s
	int *maze = initialize_maze(rows, cols);


	int start = (rand() % (rows/2 - 1)) * 2 + 1;
	start *= cols; start++;


	maze[start] = 0;

	dfs_recursive(maze, start, rows, cols);
	
	return maze;
}

void dfs_recursive(int* maze, int current, int rows, int cols) {

	int translations[4] = {-1, 1, cols, -cols};

	// checks for empty neighbor
	// if none found backtrack
	if (!check_neighbors(maze, current, rows, cols)) return;
	
	while (check_neighbors(maze, current, rows, cols)) {
		// relative weights for each direction, adds up to 1
		float weights[4] = {0.25, 0.25, 0.25, 0.25};
		int dir = choose_dir(weights);
	
		int trans = translations[dir];

		if (!check_square(maze, current, trans*2, rows, cols)) continue;
		maze[current+trans] = 0;
		maze[current+trans*2] = 0;

		dfs_recursive(maze, current+trans*2, rows, cols);
	}
}


int check_neighbors(int* maze, int current, int rows, int cols) {
	int room = 0;

	if (check_square(maze, current,  2, rows, cols)) room = 1; // checks right
	if (check_square(maze, current, -2, rows, cols)) room = 1; // checks left
	if (check_square(maze, current,  2*cols, rows, cols)) room = 1; // checks down
	if (check_square(maze, current, -2*cols, rows, cols)) room = 1; // checks up
	return room;
}

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
			if (maze[cols*i+j]) {
				printf("\u2588\u2588");
			} else {
				printf("  ");
			}
		}
		printf("\n");
	}
}

int choose_dir(float weights[4]) {

	float choice = (float)(rand() % 100);

	if (choice < weights[0]*100) return 0;
	if (choice < (weights[0]+weights[1])*100) return 1;
	if (choice < (weights[0]+weights[1]+weights[2])*100) return 2;
	if (choice < (weights[0]+weights[1]+weights[2]+weights[3])*100) return 3;
	return 0;
}
