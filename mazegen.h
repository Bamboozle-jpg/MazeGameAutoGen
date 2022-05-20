#ifndef MAZEGEN_H_
#define MAZEGEN_H_

int* create_maze(int rows, int cols, float horiSkew);
void display_maze(int* maze, int rows, int cols);
int* break_walls(int* maze, int holes, int rows, int cols);

#endif
