#include "Gen_file.c"
#include "Alg.c"
#include "Visual.c"


int map_generation() {
    setlocale(LC_ALL, "");
    srand(time(NULL));
    int map_width, map_height, num_obstacles, aircraft_size;

    print_with_delay("Enter the size parameters of the map\n\n", 10, 15);
    input_entry("MAP WIDTH", 14, &map_width, "int");
    input_entry("MAP HEIGHT", 14, &map_height, "int");

    clear_scr();
    print_with_delay("Enter the number of obstacles on the map\n\n", 10, 15);
    input_entry("OBSTACLES COUNT", 16, &num_obstacles, "int");

    clear_scr();
    print_with_delay("Enter the size of the drone(it has a square shape)\n\n", 10, 15);
    input_entry("DRONE SIZE", 14, &aircraft_size, "int");

    clear_scr();


    Obstacle* obstacles = (Obstacle*)malloc(num_obstacles * sizeof(Obstacle));
    if (obstacles == NULL) {
        perror("Failed to allocate");
        return 1;
    }
    Point PointOFInterest[2];
    int count = 0;

    printf("LOGS OF MAP GENERATION");
    generate_obstacles(obstacles, &count, map_width, map_height, num_obstacles, aircraft_size);
    //generate_start_and_finish_points(PointOFInterest, obstacles, &count, map_width, map_height, aircraft_size);
    save_map_to_file("map.txt", map_width, map_height, obstacles, PointOFInterest, count);

    free(obstacles);
    printf("\nMap was saved as map.txt with %d obstacles.\n\n", count);
    return aircraft_size;
}

int main() {
    int size = map_generation();

    print_with_delay("Open the file 'map.txt' and enter points A and B in place of the empty cells.\nClose the text editor for the program to function correctly.\nAfter completing these steps, press Enter in the console.", 10, 20);
    open_file("map.txt");
    char waiting = getchar();
    waiting = getchar();
    printf("%c", waiting);

    clear_scr();
    int rows, cols;
    char** map = read_map_file("map.txt", &rows, &cols);
    if (map == NULL) {
        perror("Failed to read map file\n");
        return -1;
    }

    Point a, b;
    find_points(map, rows, cols, &a, &b);

    if (a.x != -1 && a.y != -1) {
        printf("Point A found at: [%d, %d]\n", a.x, a.y);
    }
    else {
        printf("Point A not found\n");
    }

    if (b.x != -1 && b.y != -1) {
        printf("Point B found at: [%d, %d]\n", b.x, b.y);
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
            printf("%s\n", visual[i]);
            free(visual[i]);
        }
        free(visual);
        free(path);
        open_file("map_path.txt");

    }
    else {
        printf("No path found\n");
    }
    return 0;
}