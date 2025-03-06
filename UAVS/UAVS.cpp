#include "Gen_file.c"

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
    return 0;
}

int main() {
    testing_map_generation();
}