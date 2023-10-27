#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ROWS 20
#define COLS 40

// Function to initialize the grid with random cell states
void initializeGrid(int grid[ROWS][COLS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            grid[i][j] = rand() % 2;
        }
    }
}

// Function to print the grid
void printGrid(int grid[ROWS][COLS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (grid[i][j] == 0) {
                printf(" "); // Dead cell
            } else {
                printf("*"); // Alive cell
            }
        }
        printf("\n");
    }
}

// Function to count the number of live neighbors for a given cell
int countNeighbors(int grid[ROWS][COLS], int x, int y) {
    int count = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            int row = (x + i + ROWS) % ROWS;
            int col = (y + j + COLS) % COLS;
            count += grid[row][col];
        }
    }
    count -= grid[x][y]; // Exclude the cell itself
    return count;
}

// Function to update the grid according to the Game of Life rules
void updateGrid(int grid[ROWS][COLS]) {
    int newGrid[ROWS][COLS];
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            int neighbors = countNeighbors(grid, i, j);
            if (grid[i][j] == 1) {
                // Cell is alive
                if (neighbors < 2 || neighbors > 3) {
                    newGrid[i][j] = 0; // Cell dies
                } else {
                    newGrid[i][j] = 1; // Cell survives
                }
            } else {
                // Cell is dead
                if (neighbors == 3) {
                    newGrid[i][j] = 1; // Cell becomes alive
                } else {
                    newGrid[i][j] = 0; // Cell remains dead
                }
            }
        }
    }
    // Copy the new grid to the original grid
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            grid[i][j] = newGrid[i][j];
        }
    }
}

int main() {
    int grid[ROWS][COLS];
    
    // Seed the random number generator
    srand(time(NULL));
    
    // Initialize the grid
    initializeGrid(grid);
    
    // Main simulation loop
    while (1) {
        system("clear"); // Clear the terminal (Unix/Linux) or use "cls" on Windows
        printGrid(grid);
        updateGrid(grid);
        usleep(100000); // Sleep for 100ms
    }
    
    return 0;
}
