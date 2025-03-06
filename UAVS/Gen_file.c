#include "MapGeneration.h"

#define RUNTIME_ERROR_MAX 1000


int do_intersect(Obstacle line1, Obstacle line2) {
    float d1, d2, d3, d4;

    float x1 = line1.x2 - line1.x1;
    float y1 = line1.y2 - line1.y1;
    float x2 = line2.x2 - line2.x1;
    float y2 = line2.y2 - line2.y1;

    d1 = cross_product(x2, y2, line1.x1 - line2.x1, line1.y1 - line2.y1);
    d2 = cross_product(x2, y2, line1.x2 - line2.x1, line1.y2 - line2.y1);
    d3 = cross_product(x1, y1, line2.x1 - line1.x1, line2.y1 - line1.y1);
    d4 = cross_product(x1, y1, line2.x2 - line1.x1, line2.y2 - line1.y1);

    if (d1 * d2 < 0 && d3 * d4 < 0) {
        return 1;
    }

    return 0;
}


double distanceToSegment(int px, int py, int x1, int y1, int x2, int y2) {
    double lineMag = distance(x1, y1, x2, y2);
    if (lineMag < 1e-10) return distance(px, py, x1, y1);

    double u = ((px - x1) * (x2 - x1) + (py - y1) * (y2 - y1)) / (lineMag * lineMag);
    u = fmax(0, fmin(1, u));

    int closestX = x1 + u * (x2 - x1);
    int closestY = y1 + u * (y2 - y1);

    return distance(px, py, closestX, closestY);
}


double minDistance(Obstacle a, Obstacle b) {

    double d1 = distanceToSegment(a.x1, a.y1, b.x1, b.y1, b.x2, b.y2);
    double d2 = distanceToSegment(a.x2, a.y2, b.x1, b.y1, b.x2, b.y2);
    double d3 = distanceToSegment(b.x1, b.y1, a.x1, a.y1, a.x2, a.y2);
    double d4 = distanceToSegment(b.x2, b.y2, a.x1, a.y1, a.x2, a.y2);

    return fmin(fmin(d1, d2), fmin(d3, d4));
}


int is_valid_obstacle(Obstacle* obstacles, int count, Obstacle new_obstacle, int map_width, int map_height, int aircraft_size) {
    for (int i = 0; i < count; i++) {
        Obstacle existing = obstacles[i];

        if (do_intersect(new_obstacle, existing)) {
            return 0;
        }

        if (minDistance(existing, new_obstacle) < aircraft_size + 1) {
            return 0;
        }
    }

    if (new_obstacle.x1 < 0 || new_obstacle.x2 < 0 || new_obstacle.y1 < 0 || new_obstacle.y2 < 0 ||
        new_obstacle.x1 >= map_width || new_obstacle.x2 >= map_width ||
        new_obstacle.y1 >= map_height || new_obstacle.y2 >= map_height) {
        return 0;
    }
    return 1;
}

int calculateSixtyPercentArea(int length, int width) {
    int area = length * width;
    int sixtyPercent = (area * 60) / 100;
    return sixtyPercent;
}

int minimum(int a, int b) {
    return (a < b) ? a : b;
}

void generate_obstacles(Obstacle* obstacles, int* count, int map_width, int map_height, int num_obstacles, int aircraft_size) {
    int max_area = calculateSixtyPercentArea(map_width, map_height);
    if (max_area < num_obstacles) { 
        printf("\nCRITICAL ERROR: The size of the map does not allow for %d obstacles. \n", num_obstacles);
        exit(-3); }
    int runtime_indicator = 0;
    while (*count < num_obstacles) {
        Obstacle new_obstacle;
        int type = rand() % 3;

        if (type == 0) {
            new_obstacle.x1 = rand() % (map_width - 1);
            new_obstacle.y1 = rand() % map_height;
            new_obstacle.x2 = new_obstacle.x1 + rand() % minimum((map_width - new_obstacle.x1), (new_obstacle.x1 + max_area - (num_obstacles - *count)));
            new_obstacle.y2 = new_obstacle.y1;
            max_area -= (new_obstacle.x2 - new_obstacle.x1);
        }
        else if (type == 1) {
            new_obstacle.x1 = rand() % map_width;
            new_obstacle.y1 = rand() % (map_height - 1);
            new_obstacle.x2 = new_obstacle.x1;
            new_obstacle.y2 = new_obstacle.y1 + rand() % minimum((map_height - new_obstacle.y1), (new_obstacle.y1 + max_area - (num_obstacles - *count)));
            max_area -= (new_obstacle.y2 - new_obstacle.y1);
        }
        else {
            new_obstacle.x1 = rand() % (map_width - 1);
            new_obstacle.y1 = rand() % (map_height - 1);
            new_obstacle.x2 = new_obstacle.x1 + rand() % minimum((map_width - new_obstacle.x1), (new_obstacle.x1 + max_area - (num_obstacles - *count)));
            new_obstacle.y2 = new_obstacle.y1 + (new_obstacle.x2 - new_obstacle.x1);
            max_area -= (new_obstacle.x2 - new_obstacle.x1);
        }

        if (is_valid_obstacle(obstacles, *count, new_obstacle, map_width, map_height, aircraft_size)) {
            obstacles[*count] = new_obstacle;
            (*count)++;
            runtime_indicator = 0;
        }
        else {
            if (runtime_indicator > RUNTIME_ERROR_MAX) {
                printf("RUNTIME ERROR: waiting too long for a program response");
                exit(-4);
            }
            runtime_indicator++;
        }
    }
}

Point set_point(Obstacle* obstacles, int* count, int map_width, int map_height, int aircraft_size) {
    Obstacle point_ch;
    point_ch.x1 = rand() % (map_width - 1);
    point_ch.y1 = rand() % map_height;
    point_ch.x2 = point_ch.x1;
    point_ch.y2 = point_ch.y1;
    if (is_valid_obstacle(obstacles, *count, point_ch, map_width, map_height, aircraft_size)) {
        Point point;
        point.x = point_ch.x1;
        point.y = point_ch.y1;
        return point;
    }
    else {
        return set_point(obstacles, count, map_width, map_height, aircraft_size);
    }
}

void generate_start_and_finish_points(Point* Points, Obstacle* obstacles, int* count, int map_width, int map_height, int aircraft_size) {
    Points[0] = set_point(obstacles, count, map_width, map_height, aircraft_size);
    Points[1] = set_point(obstacles, count, map_width, map_height, aircraft_size);
    while (Points[0].x == Points[1].x && Points[0].y == Points[1].y) {
        Points[1] = set_point(obstacles, count, map_width, map_height, aircraft_size);
    }
}

void save_map_to_file(const char* filename, int map_width, int map_height, Obstacle* obstacles, Point* points, int count) {
    FILE* file;
    errno_t error = fopen_s(&file, filename, "w");
    if (error != 0) {
        perror("FILE OPEN ERROR");
        return;
    }

    int full_map_height = map_height + 2;
    int full_map_width = map_width + 2;
    char* map = (char*)malloc(full_map_height * full_map_width * sizeof(char));
    if (map == NULL) {
        printf("MALLOC ERROR.\n");
        fclose(file);
        return;
    }

    for (int y = 0; y < full_map_height; y++) {
        for (int x = 0; x < full_map_width; x++) {
            map[y * full_map_width + x] = NULL_CHAR;
        }
    }

    if (points[0].x >= 0 && points[0].x < map_width && points[0].y >= 0 && points[0].y < map_height) {
        map[(points[0].y + 1) * full_map_width + (points[0].x + 1)] = START_POINT_CHAR;
    }
    if (points[1].x >= 0 && points[1].x < map_width && points[1].y >= 0 && points[1].y < map_height) {
        map[(points[1].y + 1) * full_map_width + (points[1].x + 1)] = FINISH_POINT_CHAR;
    }

    for (int x = 0; x < full_map_width; x++) {
        map[x] = '-';
        map[(full_map_height - 1) * full_map_width + x] = '-';
    }
    for (int y = 1; y < full_map_height - 1; y++) {
        map[y * full_map_width] = '|';
        map[y * full_map_width + (full_map_width - 1)] = '|';
    }
    // corners
    map[0] = '+';
    map[full_map_width - 1] = '+';
    map[(full_map_height) * full_map_width - 1] = '+';
    map[(full_map_height - 1) * full_map_width] = '+';

    for (int i = 0; i < count; i++) {
        Obstacle obs = obstacles[i];

        if (obs.y1 == obs.y2) {
            for (int x = obs.x1; x <= obs.x2; x++) {
                if (x >= 0 && x < map_width && obs.y1 >= 0 && obs.y1 < map_height) {
                    map[(obs.y1 + 1) * full_map_width + (x + 1)] = OBSTACLE_CHAR;
                }
            }
        }
        else if (obs.x1 == obs.x2) {
            for (int y = obs.y1; y <= obs.y2; y++) {
                if (y >= 0 && y < map_height && obs.x1 >= 0 && obs.x1 < map_width) {
                    map[(y + 1) * full_map_width + (obs.x1 + 1)] = OBSTACLE_CHAR;
                }
            }
        }
        else {
            int dx = obs.x2 - obs.x1;
            int dy = obs.y2 - obs.y1;
            int steps = abs(dx > dy ? dx : dy);
            float x_inc = dx / (float)steps;
            float y_inc = dy / (float)steps;
            float x = obs.x1 + 1;
            float y = obs.y1 + 1;

            for (int j = 0; j <= steps; j++) {
                if ((int)x >= 0 && (int)x < map_width && (int)y >= 0 && (int)y < map_height) {
                    map[(int)y * full_map_width + (int)x] = OBSTACLE_CHAR;
                }
                x += x_inc;
                y += y_inc;
            }
        }
    }

    for (int y = 0; y < full_map_height; y++) {
        for (int x = 0; x < full_map_width; x++) {
            fprintf(file, "%c", map[y * full_map_width + x]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
    free(map);
}
