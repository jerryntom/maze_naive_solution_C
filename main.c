#include <stdio.h>
#include <stdlib.h>
#include "tested_declarations.h"
#include "rdebug.h"

int load_maze(const char *filename, char ***labirynth);
int solve_maze(char **maze, int x, int y);
void free_maze(char **maze);

int load_maze(const char *filename, char ***labirynth) {
    if(filename == NULL || labirynth == NULL) return 1;

    int width = 0, height = 0;
    char c;

    FILE *fin = fopen(filename, "rt");
    if(fin == NULL) return 2;

    fseek(fin, 0, SEEK_END);
    if(ftell(fin) == 0) {
        fclose(fin);
        return 3;
    }
    else fseek(fin, 0, SEEK_SET);

    while(1) {
        c = (char)fgetc(fin);
        if(c == '\n') {
            height++;
            break;
        }
        width++;
    }

    int temp_width = 0;
    while(1) {
        c = (char)fgetc(fin);
        if(c == '\n') {
            if(temp_width < width || temp_width > width) {
                fclose(fin);
                return 3;
            }
            temp_width = 0;
            height++;
            continue;
        }
        else if(c == EOF) {
            height++;
            break;
        }
        temp_width++;
    }

    (*labirynth) = malloc((height + 1) * sizeof(char *));
    if((*labirynth) == NULL) {
        fclose(fin);
        return 4;
    }

    for(int i = 0; i < height; i++) {
        *(*labirynth + i) = calloc(width + 1, sizeof(char));
        if(*(*labirynth + i) == NULL) {
            for(int j = 0; j < i; j++) {
                free(*(*labirynth + j));
                *(*labirynth + j) = NULL;
            }
            free(*labirynth);
            *labirynth = NULL;
            fclose(fin);
            return 4;
        }
    }

    fseek(fin, 0, SEEK_SET);
    int row = 0, col = 0;

    while(1) {
        c = (char)fgetc(fin);
        if(c == EOF) break;
        else {
            if(c != '#' && c != ' ' && c != 'a' && c != 'b' && c != '\n') {
                free_maze(*labirynth);
                *labirynth = NULL;
                fclose(fin);
                return 3;
            }

            if(c == '\n') {
                row++;
                col = 0;
                if(row == height) break;
                else continue;
            }
            *(*(*labirynth + row) + col) = c;
            col++;
        }
    }

    *(*labirynth + height) = NULL;
    fclose(fin);

    return 0;
}

int is_corridor(char **maze, int x, int y) {
    if(*(*(maze + y) + x) == ' ') return 1;
    else return 0;
}

int is_path(char **maze, int x, int y) {
    if(*(*(maze + y) + x) == '*') return 1;
    else return 0;
}

int is_exit(char **maze, int x, int y) {
    if(*(*(maze + y) + x) == 'b') return 1;
    else return 0;
}

int is_blocked(char **maze, int x, int y) {
    if(*(*(maze + y) + x) == 'c') return 1;
    else return 0;
}

int solve_maze(char **maze, int x, int y) {
    if(maze == NULL || x < 0 || y < 0) return -1;

    int width = 0, height = 0, row, col;
    for(int i = 0; *(*(maze) + i) != '\0'; i++) {
        width = i;
    }
    width++;

    row = 0;

    int temp_width;

    while(*(maze + row) != NULL) {
        for(int i = 0; *(*(maze + row) + i) != '\0'; i++) {
            temp_width = i;
        }
        temp_width++;

        if(temp_width != width) return -1;

        row++;
    }

    if(x >= width) return -1;

    row = 0;
    while(*(maze + row) != NULL) {
        row++;
        height++;
    }

    if(y >= height) return -1;

    while(*(*(maze + y) + x) != 'b') {
        if (is_exit(maze, x + 1, y) || is_exit(maze, x - 1, y) || is_exit(maze, x, y + 1) ||
            is_exit(maze, x, y - 1)) {
            row = 0, col = 0;
            while(*(maze + row) != NULL) {
                if(*(*(maze + row) + col) == 'c') *(*(maze + row) + col) = ' ';
                col++;
                if(col == width) {
                    col = 0;
                    row++;
                }
            }
            return 1;
        }
        else if (is_corridor(maze, x, y - 1) && !is_blocked(maze, x, y - 1)) {
            y--;
            *(*(maze + y) + x) = '*';
        }
        else if (is_corridor(maze, x, y + 1) && !is_blocked(maze, x, y + 1)) {
            y++;
            *(*(maze + y) + x) = '*';
        }
        else if (is_corridor(maze, x + 1, y) && !is_blocked(maze, x + 1, y)) {
            x++;
            *(*(maze + y) + x) = '*';
        }
        else if (is_corridor(maze, x - 1, y) && !is_blocked(maze, x - 1, y)) {
            x--;
            *(*(maze + y) + x) = '*';
        }
        else if (is_corridor(maze, x, y - 1) && !is_blocked(maze, x, y - 1)) {
            y--;
            *(*(maze + y) + x) = '*';
        }
        else if (is_corridor(maze, x, y + 1) && !is_blocked(maze, x, y + 1)) {
            y++;
            *(*(maze + y) + x) = '*';
        }
        else {
            *(*(maze + y) + x) = 'c';
            if (is_path(maze, x - 1, y)) {
                x--;
                continue;
            }
            else if (is_path(maze, x + 1, y)) {
                x++;
                continue;
            }
            else if (is_path(maze, x, y - 1)) {
                y--;
                continue;
            }
            else if (is_path(maze, x, y + 1)) {
                y++;
                continue;
            }
            else return 0;
        };
    }

    return 0;
}

void free_maze(char **maze) {
    if(maze == NULL) return;

    int row = 0;

    while(*(maze + row) != NULL) {
        free(*(maze + row));
        *(maze + row) = NULL;
        row++;
    }

    free(maze);
    maze = NULL;
}

int main(void) {
    char **maze, *filename = calloc(31, sizeof(char)), c;

    if(filename == NULL) {
        printf("Failed to allocate memory");
        return 8;
    }

    printf("Please input filename:");
    scanf("%30[^\n]", filename);
    while((c = (char)getchar()) && c != EOF && c != '\n') {}

    int res = load_maze(filename, &maze);
    free(filename);
    filename = NULL;

    if(res == 2) {
        printf("Couldn't open file");
        return 4;
    }
    else if(res == 3) {
        printf("File corrupted");
        return 6;
    }
    else if(res == 4) {
        printf("Failed to allocate memory");
        return 8;
    }

    int row = 0, col = 0, a_exists = 0, b_exists = 0, a_counter = 0, b_counter = 0;

    while(*(maze + row) != NULL) {
        if(*(*(maze + row) + col) == 'a') {
            if(col == 0 || *(*(maze + row) + col + 1) == '\0') {
                printf("File corrupted");
                free_maze(maze);
                return 6;
            }
            a_counter++;
            a_exists = 1;
        }
        else if(*(*(maze + row) + col) == 'b') {
            if(col == 0 || *(*(maze + row) + col + 1) == '\0') {
                printf("File corrupted");
                free_maze(maze);
                return 6;
            }
            b_counter++;
            b_exists = 1;
        }
        col++;
        if(*(*(maze + row) + col) == '\0') {
            col = 0;
            row++;
        }
    }

    if(a_exists == 0 || b_exists == 0 || a_counter > 1 || b_counter > 1) {
        printf("File corrupted");
        free_maze(maze);
        return 6;
    }

    int x = 0, y = 0;
    row = 0, col = 0;

    while(*(maze + row) != NULL) {
        if(*(*(maze + row) + col) == 'a') {
            x = col;
            y = row;
            break;
        }
        col++;
        if(*(*(maze + row) + col) == '\0') {
            col = 0;
            row++;
        }
    }

    res = solve_maze(maze, x, y);

    if(res == 0) printf("Couldn't find path");
    else if(res == 1) {
        row = 0, col = 0;

        while(*(maze + row) != NULL) {
            printf("%c", *(*(maze + row) + col));
            col++;
            if(*(*(maze + row) + col) == '\0') {
                col = 0;
                row++;
                printf("\n");
            }
        }
    }

    free_maze(maze);
    return 0;
}

