#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

void GetInfo();
void print_maze_line(int i);
int min(int i, int j);
int max(int i, int j);

int width, height;
int** cell;
int num = 1;
int* break_wall;
FILE* file;

int main() {
	int i, j, m, n, previous_value;
	int vertical_flag = 0;
	file = fopen("maze.maz", "wt");
	GetInfo();
	if (width < 0 || height < 0) return 0;
	
	cell = (int**)malloc(sizeof(int*) *height);
	for (i = 0; i < height; i++) {
		cell[i] = (int*)malloc(sizeof(int) * width);
		for (j = 0; j < width; j++)
			cell[i][j] = 0;
	}
	break_wall = (int*)malloc(sizeof(int) * (width - 1));
	for (i = 0; i < width - 1; i++) {
		break_wall[i] = 0;
	}

	srand(time(NULL));
	// first line
	for (i = 0; i < width; i++) {
		cell[0][i] = num++;
	}
	
	// loop for every lines
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) { // going to right
			if (j < width - 1) {
				if (cell[i][j] != cell[i][j + 1]) {
					if (rand() % 2) {
						previous_value = max(cell[i][j], cell[i][j+1]);
						cell[i][j] = min(cell[i][j], cell[i][j + 1]);
						cell[i][j + 1] = cell[i][j];
						break_wall[j] = 1;
						for (m=0; m<=i; m++){
							for (n=0; n<width; n++){
								if (cell[m][n] == previous_value) cell[m][n] = cell[i][j];
							}
						}
					}
					else if (!cell[i][j]) cell[i][j] = num++;
				}
			}
			else if (!cell[i][j]) cell[i][j] = num++;
		}
		if (i < height - 1) {// going down
			j = 0;
			while (j < width) {
				do {
					if (rand() % 2) {
						cell[i + 1][j] = cell[i][j];
						vertical_flag = 1;
					}
					else cell[i + 1][j] = num++;
					j++;
				} while (cell[i][j-1] == cell[i][j]);

				if (!vertical_flag) {
					do {
						j--;
					} while (j > 0 && cell[i][j] == cell[i][j - 1]);
				}
				else vertical_flag = 0;
			}
		}
		else { // for the final line
			for (j = 0; j < width - 1; j++) {
				if (cell[i][j] != cell[i][j + 1]) {
					previous_value = max(cell[i][j], cell[i][j+1]);
					cell[i][j] = min(cell[i][j], cell[i][j + 1]);
					cell[i][j + 1] = cell[i][j];
					break_wall[j] = 1;
					for (m=0; m<=i; m++){
						if (cell[m][n] == previous_value) cell[m][n] == cell[i][j];
					}
				}
			}
		}
		
		print_maze_line(i);
		for (j = 0; j < width - 1; j++)
			break_wall[j] = 0;
	}
	
	// end
	for (i = 0; i < height; i++)
		free(cell[i]);
	free(cell);
	free(break_wall);
	fclose(file);
}

void GetInfo() {
	printf("Width of the maze: ");
	scanf("%d", &width);
	printf("Height of the maze: ");
	scanf("%d", &height);
}

void print_maze_line(int i) {
	int j;
	if (i == 0) { // the very top
		for (j = 0; j < width; j++) {
			printf("+-");
			fprintf(file, "+-");
		}
		printf("+\n");
		fprintf(file, "+\n");
	}
	for (j = 0; j < width; j++) { // the room and the next room
		if (j == 0){ printf("|"); fprintf(file, "|");}
		else if (!break_wall[j-1]) {printf("|"); fprintf(file, "|");}
		else {printf(" "); fprintf(file, " ");}
		printf(" "); fprintf(file, " ");
	}
	printf("|\n"); fprintf(file, "|\n");
	if (i == height - 1) {
		for (j = 0; j < width; j++) {
			printf("+-"); fprintf(file, "+-");
		}
		printf("+\n"); fprintf(file, "+\n");
	}
	else { // the room and the bottom room
		for (j = 0; j < width; j++) {
			printf("+"); fprintf(file, "+");
			if (cell[i][j] != cell[i + 1][j]){ printf("-"); fprintf(file, "-");}
			else {printf(" "); fprintf(file, " ");}
		}
		printf("+\n"); fprintf(file, "+\n");
	}
}

int min(int i, int j){
	if (i>=j) return j;
	else return i;
}

int max(int i, int j){
	if (i>=j) return i;
	else return j;
}
