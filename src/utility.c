#include "header.h"

#ifdef _WIN32
HANDLE hConsole = NULL;
#endif

void set_console_color(int color) {
#ifdef _WIN32
    if (hConsole == NULL)
        hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, (WORD)color);
#else
    /* ANSI color mapping for POSIX terminals */
    switch (color) {
        case COLOR_RED:     printf("\033[1;31m"); break;
        case COLOR_GREEN:   printf("\033[1;32m"); break;
        case COLOR_YELLOW:  printf("\033[1;33m"); break;
        case COLOR_BLUE:    printf("\033[1;34m"); break;
        case COLOR_MAGENTA: printf("\033[1;35m"); break;
        case COLOR_CYAN:    printf("\033[1;36m"); break;
        case COLOR_WHITE:   printf("\033[1;37m"); break;
        default:            printf("\033[0m"); break;
    }
#endif
}

void reset_console_color(void) {
#ifdef _WIN32
    set_console_color(COLOR_RESET);
#else
    printf("\033[0m");
#endif
}

void clrscr(void) {
#ifdef _WIN32
    system("cls");
#else
    printf("\033[H\033[J");
#endif
}

void show_splash_screen(void) {
    int width = get_console_width();
    clrscr();

    set_console_color(COLOR_CYAN);
    print_line('=', width);
    printf("\n");

    print_centered("EXAM SEATING ARRANGEMENT MANAGEMENT SYSTEM", width);
    printf("\n");
    print_centered("University Examination Module", width);
    printf("\n");
    print_line('=', width);
    printf("\n");

    set_console_color(COLOR_YELLOW);
    print_centered("Loading", width);
    printf("\n");

    set_console_color(COLOR_GREEN);
    show_loading_animation("Initializing System", 1500);
    reset_console_color();

    clrscr();
}

void show_loading_animation(const char *message, int duration_ms) {
    int i;
    int iterations = 20;
    int sleep_ms = duration_ms / iterations;
    const char spinner[] = "|/-\\";

    printf("\n  %s ", message);
    for (i = 0; i < iterations; i++) {
        printf("\r  %s [%c]", message, spinner[i % 4]);
        fflush(stdout);
        Sleep(sleep_ms);
    }
    printf("\r  %s [Done]  \n", message);
}

void show_progress_bar(int current, int total) {
    int bar_width = 50;
    int pos;
    float percent;
    int i;

    if (total <= 0) return;

    percent = (float)current / total * 100.0f;
    pos = (int)(bar_width * current / total);

    printf("\r  [");
    for (i = 0; i < bar_width; i++) {
        if (i < pos)
            printf("%c", 219);
        else
            printf(" ");
    }
    printf("] %4.1f%%", percent);
    fflush(stdout);
}

void print_header(const char *title) {
    int width = get_console_width();

    clrscr();
    set_console_color(COLOR_CYAN);
    print_line('=', width);
    print_centered(title, width);
    print_line('=', width);
    reset_console_color();
    printf("\n");
}

void print_subheader(const char *title) {
    int width = get_console_width();

    set_console_color(COLOR_YELLOW);
    print_line('-', width);
    print_centered(title, width);
    print_line('-', width);
    reset_console_color();
    printf("\n");
}

void print_line(char ch, int len) {
    int i;
    for (i = 0; i < len; i++)
        putchar(ch);
    printf("\n");
}

void print_centered(const char *text, int width) {
    int len = (int)strlen(text);
    int padding;

    if (len >= width) {
        printf("  %s\n", text);
        return;
    }

    padding = (width - len) / 2;
    printf("%*s%s\n", padding, "", text);
}

void pause_program(void) {
    set_console_color(COLOR_YELLOW);
    printf("\n  Press Enter to continue...");
    reset_console_color();
    fflush(stdin);
    getchar();
}

int get_valid_int(const char *prompt) {
    char buffer[MAX_BUFFER];
    int value;
    char extra;

    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) continue;
        trim_newline(buffer);

        if (sscanf(buffer, "%d%c", &value, &extra) == 1) {
            return value;
        }
        set_console_color(COLOR_RED);
        printf("  Invalid input. Please enter a number.\n");
        reset_console_color();
    }
}

float get_valid_float(const char *prompt) {
    char buffer[MAX_BUFFER];
    float value;
    char extra;

    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) continue;
        trim_newline(buffer);

        if (sscanf(buffer, "%f%c", &value, &extra) == 1) {
            return value;
        }
        set_console_color(COLOR_RED);
        printf("  Invalid input. Please enter a number.\n");
        reset_console_color();
    }
}

void get_string_input(const char *prompt, char *buffer, size_t size) {
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, (int)size, stdin) == NULL) continue;
        trim_newline(buffer);
        if (strlen(buffer) > 0) return;
        set_console_color(COLOR_RED);
        printf("  Input cannot be empty.\n");
        reset_console_color();
    }
}

void get_hidden_password(char *password, size_t size) {
    size_t i = 0;
    char ch;

    while (1) {
        ch = (char)_getch();
        if (ch == '\r') {
            break;
        } else if (ch == '\b') {
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        } else if (i < size - 1) {
            password[i++] = ch;
            printf("*");
        }
    }
    password[i] = '\0';
    printf("\n");
}

int confirm_dialog(const char *message) {
    char choice[MAX_BUFFER];

    set_console_color(COLOR_YELLOW);
    printf("\n  %s (y/n): ", message);
    reset_console_color();

    if (fgets(choice, sizeof(choice), stdin) == NULL) return 0;
    trim_newline(choice);

    return (choice[0] == 'y' || choice[0] == 'Y');
}

void trim_newline(char *str) {
    size_t len;
    if (str == NULL) return;
    len = strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r')) {
        str[--len] = '\0';
    }
}

void to_upper_case(char *str) {
    if (str == NULL) return;
    while (*str) {
        *str = (char)toupper((unsigned char)*str);
        str++;
    }
}

void get_current_date_str(char *buffer, size_t size) {
    time_t t;
    struct tm *tm_info;

    time(&t);
    tm_info = localtime(&t);
    strftime(buffer, size, "%Y-%m-%d", tm_info);
}

void get_current_time_str(char *buffer, size_t size) {
    time_t t;
    struct tm *tm_info;

    time(&t);
    tm_info = localtime(&t);
    strftime(buffer, size, "%H:%M:%S", tm_info);
}

int get_console_width(void) {
    int width = 80;
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    }
#endif
    if (width < 80) width = 80;
    return width;
}

void draw_table_header(const char *headers[], int widths[], int count) {
    int i;

    set_console_color(COLOR_CYAN);
    printf("  ");
    for (i = 0; i < count; i++) {
        printf("+-");
        printf("%*s", -widths[i], "");
        printf("-");
    }
    printf("+\n");

    printf("  ");
    for (i = 0; i < count; i++) {
        printf("| %-*s", widths[i], headers[i]);
    }
    printf("|\n");

    printf("  ");
    for (i = 0; i < count; i++) {
        printf("+-");
        printf("%*s", -widths[i], "");
        printf("-");
    }
    printf("+\n");
    reset_console_color();
}

void draw_table_row(const char *cells[], int widths[], int count) {
    int i;

    printf("  ");
    for (i = 0; i < count; i++) {
        printf("| %-*s", widths[i], cells[i]);
    }
    printf("|\n");
}
