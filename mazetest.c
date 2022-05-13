#include <stdio.h>
#include "mazegen.h"

int main() {

	int rows = 15;
	int cols = 21;

	int* maze = create_maze(rows, cols);
	display_maze(maze, rows, cols);

	maze = break_walls(maze, 10, rows, cols);
	display_maze(maze, rows, cols);
	return 0;
}
