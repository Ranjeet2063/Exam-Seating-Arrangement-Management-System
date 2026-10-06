/*
 * ============================================================
 * Exam Seating Arrangement Management System
 * Header File - All Structures, Constants & Function Prototypes
 * ============================================================
 */

#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <math.h>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <unistd.h>
#include <termios.h>
/* POSIX stubs/types for Windows API functions */
typedef void* HANDLE;
typedef unsigned short WORD;
#define STD_OUTPUT_HANDLE ((void*)(intptr_t)(-11))

static inline void SetConsoleTitle(const char *title) {
    if (title) {
        printf("\033]0;%s\007", title);
    }
}

static inline int _getch(void) {
    struct termios oldt, newt;
    int ch;
    if (tcgetattr(STDIN_FILENO, &oldt) != 0) return getchar();
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

static inline void Sleep(int milliseconds) {
    usleep(milliseconds * 1000);
}
#endif

/* ============================================================
 * DATABASE CONFIGURATION
 * ============================================================ */
#define DB_HOST    "localhost"
#define DB_USER    "root"
#define DB_PASS    ""
#define DB_NAME    "exam_seating_system"
#define DB_PORT    3306

#ifndef USE_MYSQL_STUB
#define USE_MYSQL_STUB 0
#endif

#if USE_MYSQL_STUB
/* Stub MySQL types for compilation without MySQL Connector/C */
typedef int MYSQL;
typedef int MYSQL_RES;
typedef char **MYSQL_ROW;
typedef int MYSQL_FIELD;
/* Stub function declarations */
#define mysql_init(a) 0
#define mysql_real_connect(a,b,c,d,e,f,g,h) 0
#define mysql_close(a) ((void)0)
#define mysql_error(a) "Stub Error"
#define mysql_query(a,b) (-1)
#define mysql_store_result(a) NULL
#define mysql_free_result(a) ((void)0)
#define mysql_fetch_row(a) NULL
#define mysql_real_escape_string(a,b,c,d) (0)
#define mysql_set_character_set(a,b) (-1)
#else
#include <mysql.h>
#endif

/* ============================================================
 * CONSTANTS
 * ============================================================ */
#define MAX_ATTEMPTS     3
#define MAX_NAME         100
#define MAX_PHONE        15
#define MAX_EMAIL        100
#define MAX_SUBJECT      100
#define MAX_BUILDING     100
#define MAX_DEPT_NAME    100
#define MAX_ROOM_NAME    50
#define MAX_ROLL         30
#define MAX_REG          30
#define MAX_BUFFER       256
#define MAX_QUERY        2048
#define DATE_LEN         11
#define TIME_LEN         9
#define FILENAME_LEN     256

/* ============================================================
 * COLOR CODES FOR CONSOLE
 * ============================================================ */
#define COLOR_RESET   7
#define COLOR_RED     12
#define COLOR_GREEN   10
#define COLOR_YELLOW  14
#define COLOR_BLUE    9
#define COLOR_CYAN    11
#define COLOR_WHITE   15
#define COLOR_MAGENTA 13

/* ============================================================
 * STRUCTURES
 * ============================================================ */

typedef struct {
    int    admin_id;
    char   username[MAX_NAME];
    char   password[128];
} Admin;

typedef struct {
    int    student_id;
    char   roll_number[MAX_ROLL];
    char   registration_number[MAX_REG];
    char   full_name[MAX_NAME];
    char   department[MAX_DEPT_NAME];
    int    semester;
    int    year;
    char   phone[MAX_PHONE];
    char   email[MAX_EMAIL];
} Student;

typedef struct {
    int    department_id;
    char   department_name[MAX_DEPT_NAME];
} Department;

typedef struct {
    int    room_id;
    char   room_name[MAX_ROOM_NAME];
    char   building[MAX_BUILDING];
    int    floor;
    int    capacity;
} Classroom;

typedef struct {
    int    exam_id;
    char   subject[MAX_SUBJECT];
    char   exam_date[DATE_LEN];
    char   start_time[TIME_LEN];
    char   end_time[TIME_LEN];
    int    semester;
    char   department[MAX_DEPT_NAME];
} Exam;

typedef struct {
    int    seating_id;
    int    exam_id;
    int    student_id;
    int    room_id;
    int    seat_number;
    int    row_number;
    int    column_number;
    char   student_name[MAX_NAME];
    char   roll_number[MAX_ROLL];
    char   room_name[MAX_ROOM_NAME];
    char   subject[MAX_SUBJECT];
    char   exam_date[DATE_LEN];
} Seating;

typedef struct {
    int    invigilator_id;
    char   full_name[MAX_NAME];
    char   department[MAX_DEPT_NAME];
    char   email[MAX_EMAIL];
    char   phone[MAX_PHONE];
} Invigilator;

typedef struct {
    int    duty_id;
    int    invigilator_id;
    int    exam_id;
    int    room_id;
    char   invigilator_name[MAX_NAME];
    char   subject[MAX_SUBJECT];
    char   room_name[MAX_ROOM_NAME];
    char   exam_date[DATE_LEN];
} InvigilatorDuty;

/* ============================================================
 * GLOBAL VARIABLES (declared in main.c / utility.c)
 * ============================================================ */
extern MYSQL *g_conn;
extern int    g_admin_id;
extern char   g_username[MAX_NAME];

/* ============================================================
 * DATABASE MODULE
 * ============================================================ */
int  db_connect(void);
void db_disconnect(void);
int  db_execute(const char *query);
MYSQL_RES *db_query(const char *query);
int  db_escape_string(char *to, const char *from, size_t len);

/* ============================================================
 * VALIDATION MODULE
 * ============================================================ */
int  is_valid_name(const char *name);
int  is_valid_phone(const char *phone);
int  is_valid_email(const char *email);
int  is_valid_roll_number(const char *roll);
int  is_valid_registration(const char *reg);
int  is_valid_room_name(const char *name);
int  is_valid_subject(const char *subject);
int  is_valid_date_str(const char *date);
int  is_valid_time_str(const char *time);
int  is_positive_int(const char *str);
int  is_within_range(int val, int min, int max);

/* ============================================================
 * UTILITY MODULE
 * ============================================================ */
void set_console_color(int color);
void reset_console_color(void);
void clrscr(void);
void show_splash_screen(void);
void show_loading_animation(const char *message, int duration_ms);
void show_progress_bar(int current, int total);
void print_header(const char *title);
void print_subheader(const char *title);
void print_line(char ch, int len);
void print_centered(const char *text, int width);
void pause_program(void);
int  get_valid_int(const char *prompt);
float get_valid_float(const char *prompt);
void get_string_input(const char *prompt, char *buffer, size_t size);
void get_hidden_password(char *password, size_t size);
int  confirm_dialog(const char *message);
void trim_newline(char *str);
void to_upper_case(char *str);
void get_current_date_str(char *buffer, size_t size);
void get_current_time_str(char *buffer, size_t size);
int  get_console_width(void);
void draw_table_header(const char *headers[], int widths[], int count);
void draw_table_row(const char *cells[], int widths[], int count);

/* ============================================================
 * LOGIN MODULE
 * ============================================================ */
int  login_menu(void);
void forgot_password(void);
int  change_password(void);

/* ============================================================
 * STUDENT MODULE
 * ============================================================ */
void student_menu(void);
void add_student(void);
void update_student(void);
void delete_student(void);
void search_student(void);
void view_students(void);
void import_students_csv(void);

/* ============================================================
 * DEPARTMENT MODULE
 * ============================================================ */
void department_menu(void);
void add_department(void);
void update_department(void);
void delete_department(void);
void view_departments(void);

/* ============================================================
 * CLASSROOM MODULE
 * ============================================================ */
void classroom_menu(void);
void add_classroom(void);
void update_classroom(void);
void delete_classroom(void);
void view_classrooms(void);
void search_classroom(void);

/* ============================================================
 * EXAM MODULE
 * ============================================================ */
void exam_menu(void);
void create_exam(void);
void edit_exam(void);
void delete_exam(void);
void view_exam_schedule(void);

/* ============================================================
 * SEATING MODULE
 * ============================================================ */
void seating_menu(void);
void auto_allocate_seats(void);
void view_seating_arrangement(void);
void export_seating_plan_txt(void);
void export_seating_plan_csv(void);
void print_seat_card(void);
void search_student_seating(void);

/* ============================================================
 * INVIGILATOR MODULE
 * ============================================================ */
void invigilator_menu(void);
void add_invigilator(void);
void update_invigilator(void);
void delete_invigilator(void);
void view_invigilators(void);
void search_invigilator(void);
void assign_invigilator_duty(void);
void view_duty_roster(void);

/* ============================================================
 * REPORT MODULE
 * ============================================================ */
void report_menu(void);
void report_student_list(void);
void report_room_list(void);
void report_department_list(void);
void report_exam_schedule(void);
void report_seating_arrangement(void);
void report_room_occupancy(void);
void report_attendance_sheet(void);
void report_empty_seats(void);
void report_capacity_report(void);
void show_dashboard(void);

/* ============================================================
 * BACKUP MODULE (via SQL script)
 * ============================================================ */
int  backup_database(void);
int  restore_database(void);

#endif /* HEADER_H */
