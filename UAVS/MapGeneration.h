#pragma once
#include "typinclude.h"

#define MAX_OBSTACLES 100
#define NULL_CHAR '~'
#define START_POINT_CHAR 'S'
#define FINISH_POINT_CHAR 'F'
#define OBSTACLE_CHAR '@'

typedef struct obstacle {
    int x1, y1, x2, y2;
} Obstacle;

typedef struct point {
    int x, y;
} Point;

float cross_product(float x1, float y1, float x2, float y2) {
    return x1 * y2 - y1 * x2;
}

double distance(int x1, int y1, int x2, int y2) {
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}


int do_intersect(Obstacle line1, Obstacle line2);
int is_valid_obstacle(Obstacle* obstacles, int count, Obstacle new_obstacle, int map_width, int map_height, int aircraft_size);
void generate_obstacles(Obstacle* obstacles, int* count, int map_width, int map_height, int num_obstacles, int aircraft_size);
Point set_point(Obstacle* obstacles, int* count, int map_width, int map_height, int aircraft_size);
void generate_start_and_finish_points(Point* Points, Obstacle* obstacles, int* count, int map_width, int map_height, int aircraft_size);
void save_map_to_file(const char* filename, int map_width, int map_height, Obstacle* obstacles, Point* points, int count);
