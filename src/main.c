/*
 * ============================================================
 * Exam Seating Arrangement Management System
 * Main Entry Point
 * ============================================================
 */

#include "header.h"

/* Global variable definitions */
MYSQL *g_conn = NULL;
int    g_admin_id = 0;
char   g_username[MAX_NAME] = "";

/* Forward declarations */
void main_menu(void);
void backup_database_menu(void);

int main(void) {
    int logged_in;

    /* Set console title */
    SetConsoleTitle("Exam Seating Arrangement Management System");

    show_splash_screen();

    /* Connect to database */
    if (!db_connect()) {
        set_console_color(COLOR_RED);
        printf("\n  Failed to connect to database. Exiting...\n");
        reset_console_color();
        printf("\n  Press any key to exit...");
        _getch();
        return 1;
    }

    /* Login */
    logged_in = login_menu();
    if (!logged_in) {
        db_disconnect();
        printf("\n  Press any key to exit...");
        _getch();
        return 0;
    }

    /* Main application loop */
    main_menu();

    /* Cleanup */
    db_disconnect();

    set_console_color(COLOR_GREEN);
    printf("\n  Thank you for using the system. Goodbye!\n");
    reset_console_color();
    printf("\n  Press any key to exit...");
    _getch();

    return 0;
}

void main_menu(void) {
    int choice;

    do {
        print_header("EXAM SEATING ARRANGEMENT SYSTEM");

        set_console_color(COLOR_YELLOW);
        printf("  Welcome, %s\n\n", g_username);
        reset_console_color();

        printf("   1.  Dashboard\n");
        printf("   2.  Student Management\n");
        printf("   3.  Department Management\n");
        printf("   4.  Classroom Management\n");
        printf("   5.  Examination Management\n");
        printf("   6.  Invigilator Management\n");
        printf("   7.  Automatic Seating Allocation\n");
        printf("   8.  View Seating Arrangement\n");
        printf("   9.  Search Student\n");
        printf("  10.  Print Seat Card\n");
        printf("  11.  Reports\n");
        printf("  12.  Export Seating Plan\n");
        printf("  13.  Backup Database\n");
        printf("  14.  Change Password\n");
        printf("  15.  Logout\n");
        printf("  16.  Exit\n");

        choice = get_valid_int("\n  Enter your choice: ");

        switch (choice) {
            case 1:
                show_dashboard();
                break;
            case 2:
                student_menu();
                break;
            case 3:
                department_menu();
                break;
            case 4:
                classroom_menu();
                break;
            case 5:
                exam_menu();
                break;
            case 6:
                invigilator_menu();
                break;
            case 7:
                auto_allocate_seats();
                break;
            case 8:
                view_seating_arrangement();
                break;
            case 9:
                search_student();
                break;
            case 10:
                print_seat_card();
                break;
            case 11:
                report_menu();
                break;
            case 12:
                export_seating_plan_txt();
                break;
            case 13:
                backup_database_menu();
                break;
            case 14:
                change_password();
                break;
            case 15:
                if (confirm_dialog("Are you sure you want to logout")) {
                    set_console_color(COLOR_GREEN);
                    printf("\n  Logged out successfully.\n");
                    reset_console_color();
                    return;
                }
                break;
            case 16:
                if (confirm_dialog("Are you sure you want to exit")) {
                    return;
                }
                break;
            default:
                set_console_color(COLOR_RED);
                printf("\n  Invalid choice! Please enter 1-16.\n");
                reset_console_color();
                pause_program();
        }
    } while (1);
}

void backup_database_menu(void) {
    int choice;

    print_header("DATABASE BACKUP / RESTORE");

    printf("  1. Backup Database\n");
    printf("  2. Restore Database\n");
    printf("  3. Back to Main Menu\n");

    choice = get_valid_int("\n  Enter your choice: ");

    switch (choice) {
        case 1: {
            char filename[FILENAME_LEN];
            char command[MAX_QUERY];
            char current_date[DATE_LEN];

            get_current_date_str(current_date, sizeof(current_date));

            snprintf(filename, sizeof(filename), "exports/backup_%s.sql", current_date);

            set_console_color(COLOR_YELLOW);
            printf("\n  This feature requires mysqldump to be installed and in PATH.\n");
            printf("  Backup file: %s\n", filename);
            reset_console_color();

            if (!confirm_dialog("Proceed with backup")) {
                printf("\n  Backup cancelled.\n");
                pause_program();
                return;
            }

            show_loading_animation("Taking database backup", 1000);

            snprintf(command, sizeof(command),
                     "mysqldump -h %s -u %s %s > \"%s\" 2>&1",
                     DB_HOST, DB_USER, DB_NAME, filename);

            int ret = system(command);

            if (ret == 0) {
                set_console_color(COLOR_GREEN);
                printf("\n  Backup successful! File: %s\n", filename);
                reset_console_color();
            } else {
                set_console_color(COLOR_RED);
                printf("\n  Backup failed. Ensure mysqldump is installed.\n");
                reset_console_color();
            }
            pause_program();
            break;
        }

        case 2: {
            char filename[FILENAME_LEN];
            char command[MAX_QUERY];

            get_string_input("  Enter backup filename: ", filename, sizeof(filename));

            if (!confirm_dialog("This will overwrite existing data. Continue")) {
                printf("\n  Restore cancelled.\n");
                pause_program();
                return;
            }

            show_loading_animation("Restoring database", 1000);

            snprintf(command, sizeof(command),
                     "mysql -h %s -u %s %s < \"%s\" 2>&1",
                     DB_HOST, DB_USER, DB_NAME, filename);

            int ret = system(command);

            if (ret == 0) {
                set_console_color(COLOR_GREEN);
                printf("\n  Database restored successfully!\n");
                reset_console_color();
            } else {
                set_console_color(COLOR_RED);
                printf("\n  Restore failed. Check filename and permissions.\n");
                reset_console_color();
            }
            pause_program();
            break;
        }

        case 3:
            return;

        default:
            set_console_color(COLOR_RED);
            printf("\n  Invalid choice.\n");
            reset_console_color();
            pause_program();
    }
}
