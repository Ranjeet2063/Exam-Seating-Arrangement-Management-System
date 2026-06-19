#include "header.h"

void exam_menu(void) {
    int choice;

    do {
        print_header("EXAMINATION MANAGEMENT");

        printf("  1. Create Exam\n");
        printf("  2. Edit Exam\n");
        printf("  3. Delete Exam\n");
        printf("  4. View Exam Schedule\n");
        printf("  5. Back to Main Menu\n");

        choice = get_valid_int("\n  Enter your choice: ");

        switch (choice) {
            case 1: create_exam(); break;
            case 2: edit_exam(); break;
            case 3: delete_exam(); break;
            case 4: view_exam_schedule(); break;
            case 5: return;
            default:
                set_console_color(COLOR_RED);
                printf("\n  Invalid choice!\n");
                reset_console_color();
                pause_program();
        }
    } while (1);
}

void create_exam(void) {
    Exam e;
    char query[MAX_QUERY];
    char escaped_subject[MAX_SUBJECT * 2 + 1];
    char escaped_dept[MAX_DEPT_NAME * 2 + 1];

    print_header("CREATE EXAM");

    get_string_input("  Subject: ", e.subject, sizeof(e.subject));
    if (!is_valid_subject(e.subject)) {
        set_console_color(COLOR_RED);
        printf("\n  Invalid subject name.\n");
        reset_console_color();
        pause_program();
        return;
    }

    get_string_input("  Exam Date (YYYY-MM-DD): ", e.exam_date, sizeof(e.exam_date));
    if (!is_valid_date_str(e.exam_date)) {
        set_console_color(COLOR_RED);
        printf("\n  Invalid date. Use YYYY-MM-DD format.\n");
        reset_console_color();
        pause_program();
        return;
    }

    get_string_input("  Start Time (HH:MM:SS): ", e.start_time, sizeof(e.start_time));
    if (!is_valid_time_str(e.start_time)) {
        set_console_color(COLOR_RED);
        printf("\n  Invalid time. Use HH:MM:SS format.\n");
        reset_console_color();
        pause_program();
        return;
    }

    get_string_input("  End Time (HH:MM:SS): ", e.end_time, sizeof(e.end_time));
    if (!is_valid_time_str(e.end_time)) {
        set_console_color(COLOR_RED);
        printf("\n  Invalid time. Use HH:MM:SS format.\n");
        reset_console_color();
        pause_program();
        return;
    }

    if (strcmp(e.start_time, e.end_time) >= 0) {
        set_console_color(COLOR_RED);
        printf("\n  End time must be after start time.\n");
        reset_console_color();
        pause_program();
        return;
    }

    e.semester = get_valid_int("  Semester (1-8): ");
    if (!is_within_range(e.semester, 1, 8)) {
        set_console_color(COLOR_RED);
        printf("\n  Semester must be between 1 and 8.\n");
        reset_console_color();
        pause_program();
        return;
    }

    get_string_input("  Department: ", e.department, sizeof(e.department));
    if (strlen(e.department) < 2) {
        set_console_color(COLOR_RED);
        printf("\n  Invalid department name.\n");
        reset_console_color();
        pause_program();
        return;
    }

    db_escape_string(escaped_subject, e.subject, sizeof(escaped_subject));
    db_escape_string(escaped_dept, e.department, sizeof(escaped_dept));

    snprintf(query, sizeof(query),
             "INSERT INTO exams (subject, exam_date, start_time, end_time, semester, department) "
             "VALUES ('%s', '%s', '%s', '%s', %d, '%s')",
             escaped_subject, e.exam_date, e.start_time, e.end_time,
             e.semester, escaped_dept);

    if (db_execute(query)) {
        set_console_color(COLOR_GREEN);
        printf("\n  Exam created successfully!\n");
        reset_console_color();
    } else {
        set_console_color(COLOR_RED);
        printf("\n  Failed to create exam.\n");
        reset_console_color();
    }

    pause_program();
}

void edit_exam(void) {
    int exam_id;
    Exam e;
    char query[MAX_QUERY];
    MYSQL_RES *result;
    MYSQL_ROW row;
    char escaped_subject[MAX_SUBJECT * 2 + 1];
    char escaped_dept[MAX_DEPT_NAME * 2 + 1];

    print_header("EDIT EXAM");

    view_exam_schedule();

    exam_id = get_valid_int("\n  Enter Exam ID to edit: ");

    snprintf(query, sizeof(query),
             "SELECT * FROM exams WHERE exam_id = %d", exam_id);

    result = db_query(query);
    if (result == NULL) {
        set_console_color(COLOR_RED);
        printf("\n  Database error.\n");
        reset_console_color();
        pause_program();
        return;
    }

    row = mysql_fetch_row(result);
    if (row == NULL) {
        mysql_free_result(result);
        set_console_color(COLOR_RED);
        printf("\n  Exam not found!\n");
        reset_console_color();
        pause_program();
        return;
    }

    snprintf(e.subject, sizeof(e.subject), "%s", row[1]);
    snprintf(e.exam_date, sizeof(e.exam_date), "%s", row[2]);
    snprintf(e.start_time, sizeof(e.start_time), "%s", row[3]);
    snprintf(e.end_time, sizeof(e.end_time), "%s", row[4]);
    e.semester = atoi(row[5]);
    snprintf(e.department, sizeof(e.department), "%s", row[6]);
    mysql_free_result(result);

    printf("\n  Current: %s | %s | %s-%s | Sem %d | %s\n",
           e.subject, e.exam_date, e.start_time, e.end_time,
           e.semester, e.department);

    printf("\n  Enter new values (press Enter to keep current):\n");

    printf("  Subject [%s]: ", e.subject);
    if (fgets(query, sizeof(query), stdin) != NULL) {
        trim_newline(query);
        if (strlen(query) > 0) {
            if (!is_valid_subject(query)) {
                set_console_color(COLOR_RED);
                printf("\n  Invalid subject.\n");
                reset_console_color();
                pause_program();
                return;
            }
            snprintf(e.subject, sizeof(e.subject), "%s", query);
        }
    }

    printf("  Exam Date [%s]: ", e.exam_date);
    if (fgets(query, sizeof(query), stdin) != NULL) {
        trim_newline(query);
        if (strlen(query) > 0) {
            if (!is_valid_date_str(query)) {
                set_console_color(COLOR_RED);
                printf("\n  Invalid date.\n");
                reset_console_color();
                pause_program();
                return;
            }
            snprintf(e.exam_date, sizeof(e.exam_date), "%s", query);
        }
    }

    printf("  Start Time [%s]: ", e.start_time);
    if (fgets(query, sizeof(query), stdin) != NULL) {
        trim_newline(query);
        if (strlen(query) > 0) {
            if (!is_valid_time_str(query)) {
                set_console_color(COLOR_RED);
                printf("\n  Invalid time.\n");
                reset_console_color();
                pause_program();
                return;
            }
            snprintf(e.start_time, sizeof(e.start_time), "%s", query);
        }
    }

    printf("  End Time [%s]: ", e.end_time);
    if (fgets(query, sizeof(query), stdin) != NULL) {
        trim_newline(query);
        if (strlen(query) > 0) {
            if (!is_valid_time_str(query)) {
                set_console_color(COLOR_RED);
                printf("\n  Invalid time.\n");
                reset_console_color();
                pause_program();
                return;
            }
            snprintf(e.end_time, sizeof(e.end_time), "%s", query);
        }
    }

    if (strcmp(e.start_time, e.end_time) >= 0) {
        set_console_color(COLOR_RED);
        printf("\n  End time must be after start time.\n");
        reset_console_color();
        pause_program();
        return;
    }

    printf("  Semester [%d]: ", e.semester);
    if (fgets(query, sizeof(query), stdin) != NULL) {
        trim_newline(query);
        if (strlen(query) > 0) {
            int val = atoi(query);
            if (is_within_range(val, 1, 8)) e.semester = val;
        }
    }

    printf("  Department [%s]: ", e.department);
    if (fgets(query, sizeof(query), stdin) != NULL) {
        trim_newline(query);
        if (strlen(query) > 0) {
            snprintf(e.department, sizeof(e.department), "%s", query);
        }
    }

    db_escape_string(escaped_subject, e.subject, sizeof(escaped_subject));
    db_escape_string(escaped_dept, e.department, sizeof(escaped_dept));

    snprintf(query, sizeof(query),
             "UPDATE exams SET subject = '%s', exam_date = '%s', "
             "start_time = '%s', end_time = '%s', semester = %d, department = '%s' "
             "WHERE exam_id = %d",
             escaped_subject, e.exam_date, e.start_time, e.end_time,
             e.semester, escaped_dept, exam_id);

    if (db_execute(query)) {
        set_console_color(COLOR_GREEN);
        printf("\n  Exam updated successfully!\n");
        reset_console_color();
    } else {
        set_console_color(COLOR_RED);
        printf("\n  Failed to update exam.\n");
        reset_console_color();
    }

    pause_program();
}

void delete_exam(void) {
    int exam_id;
    char query[MAX_QUERY];
    MYSQL_RES *result;

    print_header("DELETE EXAM");

    view_exam_schedule();

    exam_id = get_valid_int("\n  Enter Exam ID to delete: ");

    snprintf(query, sizeof(query),
             "SELECT subject FROM exams WHERE exam_id = %d", exam_id);

    result = db_query(query);
    if (result == NULL) {
        set_console_color(COLOR_RED);
        printf("\n  Database error.\n");
        reset_console_color();
        pause_program();
        return;
    }

    if (mysql_fetch_row(result) == NULL) {
        mysql_free_result(result);
        set_console_color(COLOR_RED);
        printf("\n  Exam not found!\n");
        reset_console_color();
        pause_program();
        return;
    }
    mysql_free_result(result);

    if (!confirm_dialog("Are you sure? This will also delete associated seating data")) {
        printf("\n  Operation cancelled.\n");
        pause_program();
        return;
    }

    snprintf(query, sizeof(query), "DELETE FROM exams WHERE exam_id = %d", exam_id);

    if (db_execute(query)) {
        set_console_color(COLOR_GREEN);
        printf("\n  Exam deleted successfully!\n");
        reset_console_color();
    } else {
        set_console_color(COLOR_RED);
        printf("\n  Failed to delete exam.\n");
        reset_console_color();
    }

    pause_program();
}

void view_exam_schedule(void) {
    char query[MAX_QUERY];
    MYSQL_RES *result;
    MYSQL_ROW row;
    int count = 0;

    snprintf(query, sizeof(query),
             "SELECT * FROM exams ORDER BY exam_date, start_time");

    result = db_query(query);
    if (result == NULL) {
        set_console_color(COLOR_RED);
        printf("\n  No exams found or database error.\n");
        reset_console_color();
        return;
    }

    printf("\n");
    {
        const char *headers[] = {"ID", "Subject", "Date", "Start", "End", "Sem", "Department"};
        int widths[] = {4, 28, 12, 9, 9, 4, 30};
        draw_table_header(headers, widths, 7);
    }

    while ((row = mysql_fetch_row(result)) != NULL) {
        const char *cells[] = {row[0], row[1], row[2], row[3], row[4], row[5], row[6]};
        int widths[] = {4, 28, 12, 9, 9, 4, 30};
        draw_table_row(cells, widths, 7);
        count++;
    }

    mysql_free_result(result);

    if (count > 0) {
        printf("  ");
        print_line('-', 4 + 28 + 12 + 9 + 9 + 4 + 30 + 9);
        set_console_color(COLOR_YELLOW);
        printf("  Total Exams: %d\n", count);
        reset_console_color();
    } else {
        set_console_color(COLOR_RED);
        printf("\n  No exams scheduled.\n");
        reset_console_color();
    }
}
