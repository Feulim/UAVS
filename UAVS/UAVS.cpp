#include "Gen_file.c"
#include "Alg.c"


int testing_map_generation() {
    setlocale(LC_ALL, "");
    srand(time(NULL));
    int map_width, map_height, num_obstacles, aircraft_size;

    printf("MAP WIDTH: ");
    scanf_s("%d", &map_width);
    printf("MAP HEIGHT: ");
    scanf_s("%d", &map_height);
    printf("OBSTACLES COUNT: ");
    scanf_s("%d", &num_obstacles);
    printf("UAVS SIZE: ");
    scanf_s("%d", &aircraft_size);

    if (num_obstacles > MAX_OBSTACLES) {
        printf("MAX COUNT OF OBSTACLES = %d.\n", MAX_OBSTACLES);
        return 1;
    }

    Obstacle obstacles[MAX_OBSTACLES];
    Point PointOFInterest[2];
    int count = 0;

    generate_obstacles(obstacles, &count, map_width, map_height, num_obstacles, aircraft_size);
    //generate_start_and_finish_points(PointOFInterest, obstacles, &count, map_width, map_height, aircraft_size);
    save_map_to_file("map.txt", map_width, map_height, obstacles, PointOFInterest, count);

    printf("Map was saved as map.txt with %d obstacles.\n", count);
    return aircraft_size;
}

int main() {
    int size = testing_map_generation();

    printf("Open the file 'map.txt' and enter points A and B in place of the empty cells. Close the text editor for the program to function correctly. After completing these steps, press any key in the console.");
    char waiting = getchar();
    waiting = getchar();
    printf("%c", waiting);

    int rows, cols;
    char** map = read_map_file("map.txt", &rows, &cols);
    if (map == NULL) {
        printf("Failed to read map file\n");
        return -1;
    }

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%c", map[i][j]);
        }
        printf("\n");
    }

    Point a, b;
    find_points(map, rows, cols, &a, &b);

    if (a.x != -1 && a.y != -1) {
        printf("Point A found at: (%d, %d)\n", a.x, a.y);
    }
    else {
        printf("Point A not found\n");
    }

    if (b.x != -1 && b.y != -1) {
        printf("Point B found at: (%d, %d)\n", b.x, b.y);
    }
    else {
        printf("Point B not found\n");
    }

    int path_length;
    Point* path = find_path(map, a, b, rows, cols, 1, &path_length, size);

    if (path) {
        printf("Path found (length %d):\n", path_length);
        char** visual = visualize_path(map, path, path_length, a, b, rows, cols);
        for (int i = 0; i < rows; i++) {
            //printf("%s\n", visual[i]);
            free(visual[i]);
        }
        free(visual);
        free(path);

        printf("I'm too tired to output the result to the console, and by the way, I still need to free up some memory. Please check the file map_path.txt.");
    }
    else {
        printf("No path found\n");
    }
    return 0;
}