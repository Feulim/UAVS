#include "typinclude.h"
#include "MapGeneration.h"
#include "Var_str.c"
#include <stdlib.h>
#include <stdio.h>

#define MIN_ROWS 3
#define MIN_COLS 3

char** create_matrix(int rows, int cols) {
    char** matrix = (char**)malloc(rows * sizeof(char*));
    if (matrix == NULL) {
        return NULL;
    }

    for (int i = 0; i < rows; i++) {
        matrix[i] = (char*)malloc(cols * sizeof(char));
        if (matrix[i] == NULL) {
            for (int j = 0; j < i; j++) {
                free(matrix[j]);
            }
            free(matrix);
            return NULL;
        }
    }

    return matrix;
}

void free_matrix(char** matrix, int rows) {
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

void find_points(char** matrix, int rows, int cols, Point* a, Point* b) {
    a->x = a->y = -1;
    b->x = b->y = -1;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (matrix[i][j] == 'A') {
                a->x = j;
                a->y = i;
            }
            else if (matrix[i][j] == 'B') {
                b->x = j;
                b->y = i;
            }
        }
    }
}

char** read_map_file(const char* filename, int* rows, int* cols) {
    FILE* file = NULL;
    errno_t err = fopen_s(&file, filename, "r");
    if (err != 0 || file == NULL) {
        perror("Failed to open file");
        return NULL;
    }

    char** matrix = create_matrix(MIN_ROWS, MIN_COLS);
    if (matrix == NULL) {
        fclose(file);
        return NULL;
    }

    *rows = 0;
    *cols = 0;
    char buffer[1024];
    int max_cols = 0;

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        int current_cols = (int)strlen(buffer);
        if (current_cols > 0 && buffer[current_cols - 1] == '\n') {
            buffer[current_cols - 1] = '\0';
            current_cols--;
        }

        if (current_cols > max_cols) {
            max_cols = current_cols;
        }
        (*rows)++;
    }

    if (*rows == 0) {
        *rows = MIN_ROWS;
        *cols = MIN_COLS;
        for (int i = 0; i < *rows; i++) {
            for (int j = 0; j < *cols; j++) {
                matrix[i][j] = ' ';
            }
        }
        fclose(file);
        return matrix;
    }

    *cols = max_cols > MIN_COLS ? max_cols : MIN_COLS;
    *rows = *rows > MIN_ROWS ? *rows : MIN_ROWS;

    char** new_matrix = (char**)realloc(matrix, *rows * sizeof(char*));
    if (new_matrix == NULL) {
        free_matrix(matrix, MIN_ROWS);
        fclose(file);
        return NULL;
    }
    matrix = new_matrix;

    for (int i = 0; i < *rows; i++) {
        if (i < MIN_ROWS) {
            char* new_row = (char*)realloc(matrix[i], *cols * sizeof(char));
            if (new_row == NULL) {
                free_matrix(matrix, *rows);
                fclose(file);
                return NULL;
            }
            matrix[i] = new_row;
        }
        else {
            matrix[i] = (char*)malloc(*cols * sizeof(char));
            if (matrix[i] == NULL) {
                free_matrix(matrix, *rows);
                fclose(file);
                return NULL;
            }
        }
    }

    rewind(file);

    for (int i = 0; i < *rows; i++) {
        if (fgets(buffer, sizeof(buffer), file) == NULL) {
            memset(matrix[i], ' ', *cols);
            continue;
        }

        int current_cols = (int)strlen(buffer);
        if (current_cols > 0 && buffer[current_cols - 1] == '\n') {
            buffer[current_cols - 1] = '\0';
            current_cols--;
        }

        int j;
        for (j = 0; j < current_cols && j < *cols; j++) {
            matrix[i][j] = buffer[j];
        }
        for (; j < *cols; j++) {
            matrix[i][j] = ' ';
        }
    }

    fclose(file);
    return matrix;
}

void enqueue(Queue* q, Point p) {
    node_t* newNode = (node_t*)malloc(sizeof(node_t));
    if (!newNode) {
        perror("Failed to allocate memory for queue node");
        exit(EXIT_FAILURE);
    }
    newNode->data = p;
    newNode->next = NULL;
    if (q->rear == NULL) {
        q->front = q->rear = newNode;
    }
    else {
        q->rear->next = newNode;
        q->rear = newNode;
    }
}

Point dequeue(Queue* q) {
    if (q->front == NULL) {
        fprintf(stderr, "Dequeue from empty queue\n");
        exit(EXIT_FAILURE);
    }
    node_t* temp = q->front;
    Point p = temp->data;
    q->front = q->front->next;
    if (q->front == NULL) {
        q->rear = NULL;
    }
    free(temp);
    return p;
}

int is_queue_empty(Queue* q) {
    return q->front == NULL;
}

int is_passable(char** map_data, int x, int y, int rows, int cols, Point** directions, int size) {
    if (x < 0 || y < 0 || x + size > cols || y + size > rows) {
        return 0;
    }

    for (int dy = 0; dy < size; dy++) {
        for (int dx = 0; dx < size; dx++) {
            int check_x = x + dx;
            int check_y = y + dy;

            if (map_data[check_y][check_x] == OBSTACLE_CHAR ||
                map_data[check_y][check_x] == '+' ||
                map_data[check_y][check_x] == '-' ||
                map_data[check_y][check_x] == '|') {
                return 0;
            }
        }
    }
    if (directions[y][x].x != -1 || directions[y][x].y != -1)
        return 0;
    return 1;
}

void get_neighbors(Point current, Point* neighbors, int* count, int allow_diagonal) {
    int base[4][2] = { {1,0}, {0,1}, {-1,0}, {0,-1} };
    int diag[4][2] = { {1,1}, {1,-1}, {-1,1}, {-1,-1} };
    *count = 0;
    for (int i = 0; i < 4; i++) {
        neighbors[*count].x = current.x + base[i][0];
        neighbors[*count].y = current.y + base[i][1];
        (*count)++;
    }
    if (allow_diagonal) {
        for (int i = 0; i < 4; i++) {
            neighbors[*count].x = current.x + diag[i][0];
            neighbors[*count].y = current.y + diag[i][1];
            (*count)++;
        }
    }
}

Point* reconstruct_path(Point start, Point end, Point** directions, int* path_length) {
    int length = 0;
    Point current = end;
    while (current.x != start.x || current.y != start.y) {
        if (directions[current.y][current.x].x == -1 && directions[current.y][current.x].y == -1) {
            *path_length = 0;
            return NULL;
        }
        Point delta = directions[current.y][current.x];
        current.x -= delta.x;
        current.y -= delta.y;
        length++;
    }
    length++;
    Point* path = (Point*)malloc(length * sizeof(Point));
    if (!path) {
        perror("Failed to allocate path");
        exit(EXIT_FAILURE);
    }
    current = end;
    int index = length - 1;
    while (index >= 0) {
        path[index] = current;
        if (index == 0) break;
        Point delta = directions[current.y][current.x];
        current.x -= delta.x;
        current.y -= delta.y;
        index--;
    }
    *path_length = length;
    return path;
}

Point* find_path(char** map_data, Point start, Point end, int rows, int cols, int allow_diagonal, int* path_length, int size) {
    Point** directions = (Point**)malloc(rows * sizeof(Point*));
    if (!directions) {
        perror("Failed to allocate directions");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < rows; i++) {
        directions[i] = (Point*)malloc(cols * sizeof(Point));
        if (!directions[i]) {
            perror("Failed to allocate directions row");
            for (int j = 0; j < i; j++) free(directions[j]);
            free(directions);
            exit(EXIT_FAILURE);
        }
        for (int j = 0; j < cols; j++) {
            directions[i][j].x = -1;
            directions[i][j].y = -1;
        }
    }
    directions[start.y][start.x].x = 0;
    directions[start.y][start.x].y = 0;

    Queue q = { NULL, NULL };
    enqueue(&q, start);
    int found = 0;

    while (!is_queue_empty(&q)) {
        Point current = dequeue(&q);
        if (current.x == end.x && current.y == end.y) {
            found = 1;
            break;
        }
        Point neighbors[8];
        int count;
        get_neighbors(current, neighbors, &count, allow_diagonal);
        for (int i = 0; i < count; i++) {
            Point neighbor = neighbors[i];
            if (is_passable(map_data, neighbor.x, neighbor.y, rows, cols, directions, size)) {
                directions[neighbor.y][neighbor.x].x = neighbor.x - current.x;
                directions[neighbor.y][neighbor.x].y = neighbor.y - current.y;
                enqueue(&q, neighbor);
            }
        }
    }

    Point* path = NULL;
    *path_length = 0;
    if (found)
        path = reconstruct_path(start, end, directions, path_length);

    for (int i = 0; i < rows; i++)
        free(directions[i]);
    free(directions);

    return path;
}


/*
kill yourself now!
*/
char** visualize_path(char** map_data, Point* path, int path_length, Point start, Point end, int rows, int cols) {
    char** visual = (char**)malloc(rows * sizeof(char*));
    if (!visual) {
        perror("Failed to allocate visual");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < rows; i++) {
        visual[i] = (char*)malloc((cols + 1) * sizeof(char));
        if (!visual[i]) {
            perror("Failed to allocate visual row");
            for (int j = 0; j < i; j++) free(visual[j]);
            free(visual);
            exit(EXIT_FAILURE);
        }

        for (int j = 0; j < cols; j++) {
            visual[i][j] = map_data[i][j];
        }
        visual[i][cols] = '\0';
    }

    //visual[start.y][start.x] = 'A'; maybe need \ o_O /
    //visual[end.y][end.x] = 'B';

    typedef struct { int dx, dy; char arrow; } ArrowMap;
    ArrowMap arrows[] = {
        {1,0,'>'}, {-1,0,'<'}, {0,1,'v'}, {0,-1,'^'},
        {1,1,'D'}, {1,-1,'U'}, {-1,1,'L'}, {-1,-1,'R'}
    };
    const int arrowCount = sizeof(arrows) / sizeof(arrows[0]);

    for (int i = 0; i < path_length - 1; i++) {
        Point current = path[i];
        if (current.x == start.x && current.y == start.y) continue;

        Point next = path[i + 1];
        int dx = next.x - current.x;
        int dy = next.y - current.y;

        char arrow = '?';
        for (int j = 0; j < arrowCount; j++) {
            if (arrows[j].dx == dx && arrows[j].dy == dy) {
                arrow = arrows[j].arrow;
                break;
            }
        }
        visual[current.y][current.x] = arrow;
    }

    FILE* file = NULL;
    if (fopen_s(&file, "map_path.txt", "w") != 0 || !file) {
        perror("Failed to save map");
    }
    else {
        for (int i = 0; i < rows; i++) {
            fwrite(visual[i], sizeof(char), cols, file);
            if (i < rows - 1) {
                fputc('\n', file);
            }
        }
        fclose(file);
    }

    return visual;
}
