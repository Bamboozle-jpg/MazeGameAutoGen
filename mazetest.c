#include <stdio.h>
#include "mazegen.h"

int main() {

	int rows = 31;
	int cols = 41;
	float horiSkew = 0.1;

	int* maze = create_maze(rows, cols, horiSkew);
	// display_maze(maze, rows, cols);

	maze = break_walls(maze, 50, rows, cols);
	display_maze(maze, rows, cols);
	return 0;
}
