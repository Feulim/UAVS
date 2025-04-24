#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <ctype.h> 
#include <windows.h>


void clear_scr() {
#ifdef _WIN32
    system("cls");  // Windows
#else
    system("clear");  // Linux/MacOS
#endif
}

void delay(int milliseconds) {
#ifdef _WIN32
    Sleep(milliseconds);
#else
    usleep(milliseconds * 1000);
#endif
}

void open_file(const char* file_path) {
    // Windows ONLY
    #ifdef _WIN32
    ShellExecuteA(NULL, "open", file_path, NULL, NULL, SW_SHOW);
    #endif
}

void move_cursor(int x, int y) {
    printf("\033[%d;%dH", y, x);
}

void move_up_one_line() {
    printf("\033[1A");
    printf("\r");
}

void move_forward(int chars) {
    printf("\033[%dC", chars);
}

void move_backward(int chars) {
    printf("\033[%dD", chars);
}

void input_entry(const char* title, int field_size, void* variable, const char* type) {
    printf("+%s:", title);
    for (int i = 0; i < field_size - strlen(title) - 1; i++) {
        printf("-");
    }
    printf("+\n");

    printf("|");
    for (int i = 0; i < field_size; i++) {
        printf(" ");
    }
    printf("|\n");

    printf("+");
    for (int i = 0; i < field_size; i++) {
        printf("-");
    }
    printf("+");

    move_up_one_line();
    move_forward(1);

    if (strcmp(type, "int") == 0) {
        scanf_s("%d", (int*)variable);
    }
    else if (strcmp(type, "float") == 0) {
        scanf_s("%f", (float*)variable);
    }
    else if (strcmp(type, "double") == 0) {
        scanf_s("%lf", (double*)variable);
    }
    else {
        printf("Unsupported type\n");
    }
    printf("\n");
}


void print_with_delay(const char* text, int char_delay_ms, int line_delay_ms) {
    for (size_t i = 0; i < strlen(text); i++) {
        putchar(text[i]);
        fflush(stdout);

        if (text[i] != ' ') {
            delay(char_delay_ms);
        }

        if (text[i] == '.' || text[i] == '!' || text[i] == '?') {
            delay(line_delay_ms);
        }
    }
}
