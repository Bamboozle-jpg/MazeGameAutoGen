//Lol get ncurses
//COMPILE THIS PROGRAM USING gcc -Wall MyGame2.c mazegen.c -o MyGame2.out -lncurses
//DA DOCUMENTATION : https://linux.die.net/man/3/keypad  ,  https://linux.die.net/man/3/mvwprintw , https://www.mkssoftware.com/docs/man3/curs_clear.3.asp ,
//Most of these are just for me, the important one is ncurses
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <ncurses.h>
#include "mazegen.h"

//Sets Width of Window
#define WIDTH 75
//Sets Height of Window
#define HEIGHT 25

#define BOARDX 19
#define BOARDY 19

#define OFFSETX 8
#define OFFSETY 2
// SCOOL and sCool are sword cooldown
#define SCOOL 5
// #define GAMESPEED 200
#define ENEMCOOL 5

//enemy1/2,X/Y are coords of enemys, sCool, cool1, and cool2 are sword and enemy respawn cooldowns, choice is for menu, end is game end, shot1/2, X/Y are sword sprite locations, move1/2 are enemy AI
int enemy1X, enemy1Y, playerX, playerY, move1, move2, move3, enemy2X, enemy2Y, enemy3X, enemy3Y, shot1X, shot1Y, shot2X, shot2Y, score, sCool, cool1, cool2, cool3, end, choice;
char c;
//Setup for later when it centers the window in the screen
int startx = 0;
int starty = 0;
int gamespeed = 200;
float skew = 0.50;
int highlight = 1;

typedef struct enemySpawn {
	int x;
	int y;
} EnemySpawn;

//Array for menu
char *choices[] = {
	"Play Game      ",
	"Change speed   ",
	"Map Orient     ",
	"Exit           "
};
//N choices is an integer representing the number of choices the player has
int n_choices = sizeof(choices) / sizeof(char *);
//Sets up the function gcc -Wall MyGame2.c mazegen.c -o MyGame2 -lncursesthat prints it
void printGameState(WINDOW *menu_win, int highlight, int q, int board[BOARDY][BOARDX], int choose, EnemySpawn spawner1, EnemySpawn spawner2, EnemySpawn spawner3);
//Sleep function, I think this should work
int msleep(long msec);
//Move enemies
int enemyMove(int board[BOARDY][BOARDX], int posX, int posY, int dirMoved);
// Creates a spawner for the enemy (1 = left, 0 = right, 2 = bottom right)
EnemySpawn generateSpawner(int* maze, int dir, int rows, int cols);
//Make sure there's always an enemy
void createEnemy(int enemy, EnemySpawn spawner);
//Print the menu
void print_menu(WINDOW *menu_win, int highlight);
//Print the speed menu
void print_menu_speed(WINDOW *menu_win);

void print_menu_map(WINDOW *menu_win, int skewInd);

int main() {

  //Creates the Window
  WINDOW *menu_win;
  // Sets up Var for which thing to highlight?
	int highlight = 1;
	int choice = 0;
	int c;

	//GAME STUFF
	//Sets up game board
	int rows = BOARDY;
	int cols = BOARDX;

	int* mazeTemp = create_maze(rows, cols, skew);
	mazeTemp = break_walls(mazeTemp, 10, rows, cols);
	int maze[rows][cols];
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			maze[i][j] = mazeTemp[i*cols + j];
		}
	}

	EnemySpawn spawner1 = generateSpawner(*maze, 1, rows, cols);
	EnemySpawn spawner2 = generateSpawner(*maze, 0, rows, cols);
	EnemySpawn spawner3 = generateSpawner(*maze, 2, rows, cols);

	sCool = 0;

	//Setup and stuff
	playerX = 1;
	playerY = 1;
  	//Sets it up to print with ncurses
	initscr();
  	//Clears the screen.... for some reason (removing this does nothing)
	clear();
  	//Makes it so typed characters don't show up on the screen
	noecho();
	//Makes it so any character typed is immediately inputted and read
	//THIS IS THE IMPORTANT ONE
	halfdelay(1);	/* Line buffering disabled. pass on everything */
  	//Centers it in the screen
	startx = 20;
	starty = 4;
  	//Sets up the window with a height, width, and centers it
	menu_win = newwin(HEIGHT, WIDTH, starty, startx);
  	//Makes it so any key presses are gotten and read through the window
	keypad(menu_win, TRUE);
  	//prints out the line at the coordinates (static location.... I think) (use wprintw for something that can be scrolled I think)
	mvprintw(0, 0, "Arrow Keys up and down to navigate, enter to select. While playing, arrow keys to move, WASD to swing sword");
  	//Takes everything that's been told to be printed and puts it on the screen
	refresh();
  	//Set up for future proof
  	int q = 1;
	choice = 5;

	//menu
	while(choice == 5) {
		wclear(menu_win);
		refresh();
		choice = 5;
		while(1)
		{	c = wgetch(menu_win);
			switch(c)
			{	case KEY_UP:
					if(highlight == 1)
						highlight = n_choices;
					else
						--highlight;
					break;
				case KEY_DOWN:
					if(highlight == n_choices)
						highlight = 1;
					else
						++highlight;
					break;
				case 10:
					choice = highlight;
					break;
				default:
					refresh();
					break;
			}
			print_menu(menu_win, highlight);
			if(choice == 1 || choice == 2 || choice == 3)	{/* User did a choice come out of the infinite loop */
				break;
			}
		}

		//Prepares game board for if they want to play/play again
		if(choice == 1) {

			//Re-randomizes maze
			int* mazeTemp = create_maze(rows, cols, skew);
			mazeTemp = break_walls(mazeTemp, 20, rows, cols);
			for (int i = 0; i < BOARDY; i++) {
				for (int j = 0; j < BOARDX; j++) {
					maze[i][j] = mazeTemp[i*BOARDY + j];
				}
			}
			//Setup for game
			score = 0;
			//Sets enemy1 to start offscreen
			enemy1X = -1;
			enemy1Y = -1;
			//Decreases cooldowns
			cool1 = -1;
			cool2 = -1;
			cool3 = -1;
			//Creates the enemies
			createEnemy(1, spawner1);
			createEnemy(2, spawner2);
			createEnemy(3, spawner3);
			//Sets the direction of the enemies
			move1 = 3;
			move2 = 3;
			//Sets the starting point for the player
			playerX = 1;
			playerY = 1;
		}

		if (choice == 2) {
			int close = 0;
			while (close == 0) {
				print_menu_speed(menu_win);
				c = wgetch(menu_win);
				switch(c) {
					case KEY_UP:
						gamespeed += 10;
						break;
					case KEY_DOWN:
						gamespeed -= 10;
						break;
					case 10:
						close = 1;
						break;
				}
				if (gamespeed < 10) {
					gamespeed = 10;
				}
				if (gamespeed > 500) {
					gamespeed = 500;
				}
			}
			gamespeed = 510 - gamespeed;
			choice = 4;
		}

		if (choice == 3) {
			int close = 0;
			int choiceMap = 3;
			while (close == 0) {
				print_menu_map(menu_win, choiceMap);
				c = wgetch(menu_win);
				switch(c) {
					case KEY_UP:
					case KEY_DOWN:
						choiceMap += 1;
						break;
					case KEY_LEFT:
					case KEY_RIGHT:
						choiceMap -= 1;
						break;
					case 10:
						close = 1;
						break;
				}
				if (choiceMap < 0) {
					choiceMap = 0;
				}
				if (choiceMap > 6) {
					choiceMap = 6;
				}
			}
			choice = 5;
			float skews[7] = {
				0.9,
				0.75,
				0.6,
				0.5,
				0.4,
				0.25,
				0.1
			};
			skew = skews[choiceMap];
		}

		if (choice == 4) {
			clrtoeol();
			//takes everything from printws and puts it on the screen
			refresh();
			//turns off the window
			endwin();
			return 0;
		}

		//main game loop
	  	while(choice == 1) {
			if (sCool > 0) {
				sCool--;
			}

			if (cool1 == 0) {
				createEnemy(1, spawner1);
				move1 = 3;
				cool1--;
			} else {
				cool1--;
			}

			if (cool2 == 0) {
				createEnemy(2, spawner2);
				move2 = 3;
				cool2--;
			} else {
				cool2--;
			}

			if (cool3 == 0) {
				createEnemy(3, spawner3);
				move3 = 3;
				cool3--;
			} else {
				cool3--;
			}

			c = wgetch(menu_win);
			shot1X = -4;
			shot1Y = -4;
			shot2X = -4;
			shot2Y = -4;
			switch(c) {
				//moves the highlight depending on what's pressed
				case KEY_UP:
					if(maze[playerY-1][playerX] == 0) {
						playerY--;
					}
					break;
				case KEY_DOWN:
					if(maze[playerY+1][playerX] == 0) {
						playerY++;
					}
					break;
				case KEY_RIGHT:
					if(maze[playerY][playerX+1] == 0) {
						playerX++;
					}
					break;
				case KEY_LEFT:
					if(maze[playerY][playerX-1] == 0) {
						playerX--;
					}
					break;
				case 'd':
					if (sCool == 0) {
						sCool = SCOOL;
						if(maze[playerY][playerX+1] == 0) {
							shot1Y = playerY;
							shot1X = playerX+1;
							if(maze[playerY][playerX+2] == 0) {
								shot2Y = playerY;
								shot2X = playerX+2;
							}
						}
					}
					break;
				case 'a':
					if (sCool == 0) {
						sCool = SCOOL;
						if(maze[playerY][playerX-1] == 0) {
							shot1Y = playerY;
							shot1X = playerX-1;
							if(maze[playerY][playerX-2] == 0) {
								shot2Y = playerY;
								shot2X = playerX-2;
							}
						}
					}
					break;
				case 'w':
					if (sCool == 0) {
						sCool = SCOOL;
						if(maze[playerY-1][playerX] == 0) {
							shot1Y = playerY-1;
							shot1X = playerX;
							if(maze[playerY-2][playerX] == 0) {
								shot2Y = playerY-2;
								shot2X = playerX;
							}
						}
					}
					break;
				case 's':
					if (sCool == 0) {
						sCool = SCOOL;
						if(maze[playerY+1][playerX] == 0) {
							shot1Y = playerY+1;
							shot1X = playerX;
							if(maze[playerY+2][playerX] == 0) {
								shot2Y = playerY+2;
								shot2X = playerX;
							}
						}
					}
					break;
				default:
					break;
			}

			if ((shot1X == enemy1X && shot1Y == enemy1Y) || (shot2X == enemy1X && shot2Y == enemy1Y)) {
				score++;
				enemy1X = -100;
				enemy1Y = -100;
				cool1 = ENEMCOOL;
			}

			if ((shot1X == enemy2X && shot1Y == enemy2Y) || (shot2X == enemy2X && shot2Y == enemy2Y)) {
				score++;
				enemy2X = -100;
				enemy2Y = -100;
				cool2 = ENEMCOOL;
			}

			if ((shot1X == enemy3X && shot1Y == enemy3Y) || (shot2X == enemy3X && shot2Y == enemy3Y)) {
				score++;
				enemy3X = -100;
				enemy3Y = -100;
				cool3 = ENEMCOOL;
			}

			if ( (playerX == enemy1X && playerY == enemy1Y) || (playerX == enemy2X && playerY == enemy2Y) || (playerX == enemy3X && playerY == enemy3Y) ) {
				choice = 5;
				printGameState(menu_win, highlight, q, maze, choice, spawner1, spawner2, spawner3);
	    		msleep(gamespeed);
			}

			if (enemy1X != -1) {
				move1 = enemyMove(maze, enemy1X, enemy1Y, move1);
				switch (move1) {
					case 0:
						enemy1Y--;
						break;
					case 1:
						enemy1X--;
						break;
					case 2:
						enemy1Y++;
						break;
					case 3:
						enemy1X++;
						break;
					default:
						break;
				}
			}

			if (enemy2X != -1) {
				move2 = enemyMove(maze, enemy2X, enemy2Y, move2);
				switch (move2) {
					case 0:
						enemy2Y--;
						break;
					case 1:
						enemy2X--;
						break;
					case 2:
						enemy2Y++;
						break;
					case 3:
						enemy2X++;
						break;
					default:
						break;
				}
			}

			if (enemy3X != -1) {
				move3 = enemyMove(maze, enemy3X, enemy3Y, move3);
				switch (move3) {
					case 0:
						enemy3Y--;
						break;
					case 1:
						enemy3X--;
						break;
					case 2:
						enemy3Y++;
						break;
					case 3:
						enemy3X++;
						break;
					default:
						break;
				}
			}

			if ((shot1X == enemy1X && shot1Y == enemy1Y) || (shot2X == enemy1X && shot2Y == enemy1Y)) {
				score++;
				enemy1X = -100;
				enemy1Y = -100;
				cool1 = ENEMCOOL;
			}

			if ((shot1X == enemy2X && shot1Y == enemy2Y) || (shot2X == enemy2X && shot2Y == enemy2Y)) {
				score++;
				enemy2X = -100;
				enemy2Y = -100;
				cool2 = ENEMCOOL;
			}

			if ((shot1X == enemy3X && shot1Y == enemy3Y) || (shot2X == enemy3X && shot2Y == enemy3Y)) {
				score++;
				enemy3X = -100;
				enemy3Y = -100;
				cool3 = ENEMCOOL;
			}

			if ( (playerX == enemy1X && playerY == enemy1Y) || (playerX == enemy2X && playerY == enemy2Y) || (playerX == enemy3X && playerY == enemy3Y)) {
				choice = 5;
			}

			printGameState(menu_win, highlight, q, maze, choice, spawner1, spawner2, spawner3);
	    	msleep(gamespeed);
	  }
	}

	//out of loop
	//clear to end of line (lets see what happens when I get rid of it :flushed:, ABSOLUTELY NOTHING, I DON'T KNOW WHAT THIS LINE IS DOING HERE, but I'll keep it)
	clrtoeol();
	//takes everything from printws and puts it on the screen
	refresh();
	//turns off the window
	endwin();
	//aaaaaaand it's done
	return 0;
}


//Takes in a window (I'd call it a structure), which thing's highlighted, and q which keeps going up to prove that it will keep going when a key isn't pressed
void printGameState(WINDOW *menu_win, int highlight, int q, int board[BOARDY][BOARDX], int choose, EnemySpawn spawner1, EnemySpawn spawner2, EnemySpawn spawner3) {
	//setup
	//where to print it
	//
	box(menu_win, 0, 0);
	//Yooooo I think this inverts the colors
	// wattron(menu_win, A_REVERSE);
	//i is vertical, j is horizontal
	if (choose == 1) {
		for(int i = 0; i < BOARDY; i++) {
			//prints a row
			for (int j = 0; j < BOARDX; j++) {
				if (i == spawner1.y && j == spawner1.x) {
				    if((i == enemy1Y && j == enemy1X) || (i == enemy2Y && j == enemy2X) || (i == enemy3Y && j == enemy3X)) {
				    	mvwprintw(menu_win, i+OFFSETY, 3*j+OFFSETX, "%s", "[M ");
			        } else if (i == playerY && j == playerX) {
			        	mvwprintw(menu_win, i+OFFSETY, 3*j+OFFSETX, "%s", "[G ");
			        } else {
			        	mvwprintw(menu_win, i+OFFSETY, 3*j+OFFSETX, "%s", "[  ");
			        }
	      		} else if (i == spawner2.y && j == spawner2.x) {
				    if((i == enemy1Y && j == enemy1X) || (i == enemy2Y && j == enemy2X) || (i == enemy3Y && j == enemy3X)) {
				    	mvwprintw(menu_win, i+OFFSETY, 3*j+OFFSETX, "%s", " M]");
			        } else if (i == playerY && j == playerX) {
			        	mvwprintw(menu_win, i+OFFSETY, 3*j+OFFSETX, "%s", " G]");
			        } else {
			        	mvwprintw(menu_win, i+OFFSETY, 3*j+OFFSETX, "%s", "  ]");
			        }
				} else if (i == spawner3.y && j == spawner3.x) {
				    if((i == enemy1Y && j == enemy1X) || (i == enemy2Y && j == enemy2X) || (i == enemy3Y && j == enemy3X)) {
				    	mvwprintw(menu_win, i+OFFSETY, 3*j+OFFSETX, "%s", " M]");
			        } else if (i == playerY && j == playerX) {
			        	mvwprintw(menu_win, i+OFFSETY, 3*j+OFFSETX, "%s", " G]");
			        } else {
			        	mvwprintw(menu_win, i+OFFSETY, 3*j+OFFSETX, "%s", "  ]");
			        }
	      		} else if (playerY == i && playerX == j) {
				    //prints you (the gamer)
				    mvwprintw(menu_win, i+OFFSETY, 3*j+OFFSETX, " G ");
				} else if (shot1Y == i && shot1X == j) {
				    //prints beginning of sword
					if (shot2X > playerX) {
				        mvwprintw(menu_win, i+OFFSETY, 3*j+OFFSETX, "%s", "=[=");
					} else if (shot2X < playerX) {
				        mvwprintw(menu_win, i+OFFSETY, 3*j+OFFSETX, "%s", "=]=");
					} else {
						mvwprintw(menu_win, i+OFFSETY, 3*j+OFFSETX, "%s", "| |");
					}
				} else if (shot2Y == i && shot2X == j) {
					//prints the end of sword
				    if (shot2X > playerX) {
				        mvwprintw(menu_win, i+OFFSETY, 3*j+OFFSETX, "%s", "==>");
					} else if (shot2X < playerX) {
				        mvwprintw(menu_win, i+OFFSETY, 3*j+OFFSETX, "%s", "<==");
					} else if (shot2Y > playerY) {
						mvwprintw(menu_win, i+OFFSETY, 3*j+OFFSETX, "%s", " | ");
					} else {
						mvwprintw(menu_win, i+OFFSETY, 3*j+OFFSETX, "%s", " | ");
					}
				} else if(enemy1Y == i && enemy1X == j) {
				    //prints monster
				    mvwprintw(menu_win, i+OFFSETY, 3*j+OFFSETX, "%s", " M ");
				} else if(enemy2Y == i && enemy2X == j) {
					//prints monster
				    mvwprintw(menu_win, i+OFFSETY, 3*j+OFFSETX, "%s", " M ");
				} else if(enemy3Y == i && enemy3X == j) {
					//prints monster
				    mvwprintw(menu_win, i+OFFSETY, 3*j+OFFSETX, "%s", " M ");
				} else if (board[i][j] == 1) {
				    //Prints a wall if the corresponding number is 1
				    mvwprintw(menu_win, i+OFFSETY, 3*j+OFFSETX, "%s", "=#=");
				} else {
				    //Prints an empty space if there's a corresponding 0
				    mvwprintw(menu_win, i+OFFSETY, 3*j+OFFSETX, "%s", "   ");
				}
			}
			if (sCool == 0) {
				mvwprintw(menu_win, OFFSETY+BOARDY+1, OFFSETX, "%s", "Sword Swing Ready!            ");
			} else {
				mvwprintw(menu_win, OFFSETY+BOARDY+1, OFFSETX, "Sword Almost ready, Cooldown %d", sCool);
			}

			mvwprintw(menu_win, OFFSETY+BOARDY, OFFSETX, "Score : %d ", score);
		}

	} else {

		wclear(menu_win);
		for (int i = 0; i < 7; i++) {
			mvwprintw(menu_win, OFFSETY+i, OFFSETX, "                                             ");
		}

		mvwprintw(menu_win, OFFSETY+7, OFFSETX, " CONGRATULATIONS! YOU GOT A SCORE OF : %d!!!   ", score);
		mvwprintw(menu_win, OFFSETY+8, OFFSETX-2, "           Press enter to continue             ");

		for (int i = 9; i < 17; i++) {
			mvwprintw(menu_win, OFFSETY+i, OFFSETX, "                                             ");
		}

		int closed = 0;
		while(closed == 0) {
			c = wgetch(menu_win);
			switch(c) {
				// ENTER
				case 10:
					closed = 1;
					break;
			}
		}

	}
	// wattroff(menu_win, A_REVERSE);
	//refreshes the window, I think this does the same as refresh
	wrefresh(menu_win);
	clear();
}


int msleep(long msec) {
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

//Given a location and sends out a new location of the enemy
//Returns 0 for up, 1 for left, 2 for down, 3 for right
int enemyMove(int board[BOARDY][BOARDX], int posX, int posY, int dirMoved) {
	//Given an initial movement that it just took, chooses which way to turn and what that means

	enum directions{UP,LEFT,DOWN,RIGHT};

	// chance of turning in each direction
	int turnChance = 25;
	// Clock of prime number thing
	//   2
	// 3 X 7
	//   5
	// Multiplies each number where empty space

	int clock = 1;
	// up
	if (board[posY-1][posX] == 0) clock *= 2;
	// left
	if (board[posY][posX-1] == 0) clock *= 3;
	// down
	if (board[posY+1][posX] == 0) clock *= 5;
	// right
	if (board[posY][posX+1] == 0) clock *= 7;

	switch (dirMoved) {

		case UP:
			// empty in front
			if (clock % 2 == 0) {
				int num = rand() % 100;
				// chance to move left
				if (clock % 3 == 0 && num < turnChance) return LEFT;
				// chance to move right
				if (clock % 7 == 0 && num > (turnChance-1) && num < 2*turnChance) return RIGHT;
				// otherwise continue forward
				return UP;
			} else {
				// both right and left empty
				if (clock % 21 == 0) {
					int num = rand() % 2;
					if (num == 0) return LEFT;
					return RIGHT;
				}
				if (clock % 3 == 0) return LEFT;
				if (clock % 7 == 0) return RIGHT;
				return DOWN;
			}

		case LEFT:
			// empty in front
			if (clock % 3 == 0) {
				int num = rand() % 100;
				// chance to move up
				if (clock % 2 == 0 && num < turnChance) return UP;
				// chance to move down
				if (clock % 5 == 0 && num > (turnChance-1) && num < 2*turnChance) return DOWN;
				// otherwise continue left
				return LEFT;
			} else {
				// both up and down empty
				if (clock % 10 == 0) {
					int num = rand() % 2;
					if (num == 0) return UP;
					return DOWN;
				}
				if (clock % 2 == 0) return UP;
				if (clock % 5 == 0) return DOWN;
				return RIGHT;
			}

		case DOWN:
			// empty in front
			if (clock % 5 == 0) {
				int num = rand() % 100;
				// chance to move left
				if (clock % 3 == 0 && num < turnChance) return LEFT;
				// chance to move right
				if (clock % 7 == 0 && num > (turnChance-1) && num < 2*turnChance) return RIGHT;
				// otherwise continue forward
				return DOWN;
			} else {
				// both right and left empty
				if (clock % 21 == 0) {
					int num = rand() % 2;
					if (num == 0) return LEFT;
					return RIGHT;
				}
				if (clock % 3 == 0) return LEFT;
				if (clock % 7 == 0) return RIGHT;
				return UP;
			}

		case RIGHT:
			// empty in front
			if (clock % 7 == 0) {
				int num = rand() % 100;
				// chance to move up
				if (clock % 2 == 0 && num < turnChance) return UP;
				// chance to move down
				if (clock % 5 == 0 && num > (turnChance-1) && num < 2*turnChance) return DOWN;
				// otherwise continue left
				return RIGHT;
			} else {
				// both up and down empty
				if (clock % 10 == 0) {
					int num = rand() % 2;
					if (num == 0) return UP;
					return DOWN;
				}
				if (clock % 2 == 0) return UP;
				if (clock % 5 == 0) return DOWN;
				return LEFT;
			}
		
	}

	gamespeed = 200;
	//Just in case something fails, default case for if nothing happens.
	printf("Uh oh, there was an error");
	return 0;
}

// Creates spawner for enemies
// Left signifies if the spawner is on left or right side (1 for left, 0 for right)
EnemySpawn generateSpawner(int* maze, int left, int rows, int cols) {

	EnemySpawn spawner;

	if (left == 1) {
		spawner.x = 1;
		spawner.y = rows-2;
		return spawner;
	} else if (left == 0) {
		spawner.x = cols-2;
		spawner.y = 1;
		return spawner;
	} else {
		spawner.x = cols-2;
		spawner.y = rows-2;
		return spawner;
	}
}

void createEnemy(int enemy, EnemySpawn spawner) {

	if (enemy == 1) {
		enemy1X = spawner.x;
		enemy1Y = spawner.y;
	} else if (enemy == 2) {
		enemy2X = spawner.x;
		enemy2Y = spawner.y;
	} else if (enemy == 3) {
		enemy3X = spawner.x;
		enemy3Y = spawner.y;
	}
}

void print_menu(WINDOW *menu_win, int highlight) {
	int x, y, i;

	x = 20;
	y = OFFSETY + 6;
	box(menu_win, 0, 0);

	for (int i = 1; i < 10; i++) {
		mvwprintw(menu_win, i, 5, "                                                      ");
	}

	mvwprintw(menu_win, 10, 29, "          ");
	mvwprintw(menu_win, 10, OFFSETX+2, "     ");
	mvwprintw(menu_win, 11, 32, "   ");

	for(i = 0; i < n_choices; ++i) {	
		if(highlight == i + 1) { /* High light the present choice */
			wattron(menu_win, A_REVERSE);
			mvwprintw(menu_win, y, x, "%s", choices[i]);
			wattroff(menu_win, A_REVERSE);
		} else {
			mvwprintw(menu_win, y, x, "%s", choices[i]);
		}
		
		++y;
	}
	wrefresh(menu_win);
}

void print_menu_speed(WINDOW *menu_win) {
	wclear(menu_win);
	box(menu_win, 0, 0);
	mvwprintw(menu_win, 7, 5, "        Change the Game speed using the up and down arrows.", gamespeed);
	mvwprintw(menu_win, 8, 5, "        Default is 200. Higher is faster, lower is lower.");
	mvwprintw(menu_win, 9, 20, "              A");
	mvwprintw(menu_win, 10, 20, "GAME SPEED : %d ", gamespeed);
	mvwprintw(menu_win, 11, 20, "              V");
	mvwprintw(menu_win, 12, 20, "                ");
	wrefresh(menu_win);
}

void print_menu_map(WINDOW *menu_win, int skewInd) {
	wclear(menu_win);

	box(menu_win, 0, 0);
	mvwprintw(menu_win, 2, 5, "          Change the Map orientation. (Ues arrow keys)      ", gamespeed);
	mvwprintw(menu_win, 3, 4, "Make the hallways more likely to be up and down, or left and right.");
	mvwprintw(menu_win, 12 - skewInd, 38, "A");
	for (int i = 0; i < skewInd; i++) {
		mvwprintw(menu_win, 12 - i, 38, "|");
	}
	switch (skewInd) {
		case 6 : 
			mvwprintw(menu_win, 13, 37, "<+>");
			break;
		case 5 : 
			mvwprintw(menu_win, 13, 35, "<--+-->");
			break;
		case 4 : 
			mvwprintw(menu_win, 13, 33, "<----+---->");
			break;
		case 3 : 
			mvwprintw(menu_win, 13, 31, "<------+------>");
			break;
		case 2 : 
			mvwprintw(menu_win, 13, 29, "<--------+-------->");
			break;
		case 1 : 
			mvwprintw(menu_win, 13, 27, "<----------+---------->");
			break;
		case 0 : 
			mvwprintw(menu_win, 13, 25, "<------------+------------>");
			break;
	}
	for (int i = 0; i < skewInd; i++) {
		mvwprintw(menu_win, 14 + i, 38, "|");
	}
	mvwprintw(menu_win, 14 + skewInd, 38, "V");
	wrefresh(menu_win);
}
