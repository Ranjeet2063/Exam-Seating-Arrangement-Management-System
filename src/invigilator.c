/*
 * ============================================================
 * Exam Seating Arrangement Management System
 * Invigilator (Faculty) Management & Duty Assignment Module
 * ============================================================
 */

#include "header.h"

void invigilator_menu(void) {
    int choice;

    do {
        print_header("INVIGILATOR MANAGEMENT");

        printf("  1. Add Invigilator\n");
        printf("  2. Update Invigilator\n");
        printf("  3. Delete Invigilator\n");
        printf("  4. View All Invigilators\n");
        printf("  5. Search Invigilator\n");
        printf("  6. Assign Invigilator Duty\n");
        printf("  7. View Invigilator Duty Roster\n");
        printf("  8. Back to Main Menu\n");

        choice = get_valid_int("\n  Enter your choice: ");

        switch (choice) {
            case 1: add_invigilator(); break;
            case 2: update_invigilator(); break;
            case 3: delete_invigilator(); break;
            case 4: view_invigilators(); pause_program(); break;
            case 5: search_invigilator(); break;
            case 6: assign_invigilator_duty(); break;
            case 7: view_duty_roster(); pause_program(); break;
            case 8: return;
            default:
                set_console_color(COLOR_RED);
                printf("\n  Invalid choice!\n");
                reset_console_color();
                pause_program();
        }
    } while (1);
}

void add_invigilator(void) {
    Invigilator inv;
    char query[MAX_QUERY];
    MYSQL_RES *result;
    char escaped_name[MAX_NAME * 2 + 1];
    char escaped_dept[MAX_DEPT_NAME * 2 + 1];
    char escaped_email[MAX_EMAIL * 2 + 1];
    char escaped_phone[MAX_PHONE * 2 + 1];

    print_header("ADD INVIGILATOR");

    get_string_input("  Full Name: ", inv.full_name, sizeof(inv.full_name));
    if (!is_valid_name(inv.full_name)) {
        set_console_color(COLOR_RED);
        printf("\n  Invalid name.\n");
        reset_console_color();
        pause_program();
        return;
    }

    get_string_input("  Department: ", inv.department, sizeof(inv.department));
    if (!is_valid_name(inv.department)) {
        set_console_color(COLOR_RED);
        printf("\n  Invalid department name.\n");
        reset_console_color();
        pause_program();
        return;
    }

    get_string_input("  Email: ", inv.email, sizeof(inv.email));
    if (!is_valid_email(inv.email)) {
        set_console_color(COLOR_RED);
        printf("\n  Invalid email address.\n");
        reset_console_color();
        pause_program();
        return;
    }

    db_escape_string(escaped_email, inv.email, sizeof(escaped_email));
    snprintf(query, sizeof(query),
             "SELECT invigilator_id FROM invigilators WHERE email = '%s'",
             escaped_email);
    result = db_query(query);
    if (result != NULL) {
        if (mysql_fetch_row(result) != NULL) {
            mysql_free_result(result);
            set_console_color(COLOR_RED);
            printf("\n  Invigilator with this email already exists!\n");
            reset_console_color();
            pause_program();
            return;
        }
        mysql_free_result(result);
    }

    get_string_input("  Phone: ", inv.phone, sizeof(inv.phone));
    if (!is_valid_phone(inv.phone)) {
        set_console_color(COLOR_RED);
        printf("\n  Invalid phone number. Must be 10 digits.\n");
        reset_console_color();
        pause_program();
        return;
    }

    db_escape_string(escaped_name, inv.full_name, sizeof(escaped_name));
    db_escape_string(escaped_dept, inv.department, sizeof(escaped_dept));
    db_escape_string(escaped_phone, inv.phone, sizeof(escaped_phone));

    snprintf(query, sizeof(query),
             "INSERT INTO invigilators (full_name, department, email, phone) "
             "VALUES ('%s', '%s', '%s', '%s')",
             escaped_name, escaped_dept, escaped_email, escaped_phone);

    if (db_execute(query)) {
        set_console_color(COLOR_GREEN);
        printf("\n  Invigilator added successfully!\n");
        reset_console_color();
    } else {
        set_console_color(COLOR_RED);
        printf("\n  Failed to add invigilator.\n");
        reset_console_color();
    }

    pause_program();
}

void update_invigilator(void) {
    int invigilator_id;
    Invigilator inv;
    char query[MAX_QUERY];
    MYSQL_RES *result;
    MYSQL_ROW row;
    char escaped_name[MAX_NAME * 2 + 1];
    char escaped_dept[MAX_DEPT_NAME * 2 + 1];
    char escaped_email[MAX_EMAIL * 2 + 1];
    char escaped_phone[MAX_PHONE * 2 + 1];

    print_header("UPDATE INVIGILATOR");

    view_invigilators();

    invigilator_id = get_valid_int("\n  Enter Invigilator ID to update: ");

    snprintf(query, sizeof(query),
             "SELECT * FROM invigilators WHERE invigilator_id = %d", invigilator_id);

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
        printf("\n  Invigilator not found!\n");
        reset_console_color();
        pause_program();
        return;
    }

    inv.invigilator_id = atoi(row[0]);
    snprintf(inv.full_name, sizeof(inv.full_name), "%s", row[1]);
    snprintf(inv.department, sizeof(inv.department), "%s", row[2]);
    snprintf(inv.email, sizeof(inv.email), "%s", row[3]);
    snprintf(inv.phone, sizeof(inv.phone), "%s", row[4]);
    mysql_free_result(result);

    printf("\n  Current Details:\n");
    printf("  Name: %s | Dept: %s | Email: %s | Phone: %s\n",
           inv.full_name, inv.department, inv.email, inv.phone);

    printf("\n  Enter new values (press Enter to keep current):\n");

    printf("  Full Name [%s]: ", inv.full_name);
    if (fgets(query, sizeof(query), stdin) != NULL) {
        trim_newline(query);
        if (strlen(query) > 0) {
            if (!is_valid_name(query)) {
                set_console_color(COLOR_RED);
                printf("\n  Invalid name format.\n");
                reset_console_color();
                pause_program();
                return;
            }
            snprintf(inv.full_name, sizeof(inv.full_name), "%s", query);
        }
    }

    printf("  Department [%s]: ", inv.department);
    if (fgets(query, sizeof(query), stdin) != NULL) {
        trim_newline(query);
        if (strlen(query) > 0) {
            snprintf(inv.department, sizeof(inv.department), "%s", query);
        }
    }

    printf("  Email [%s]: ", inv.email);
    if (fgets(query, sizeof(query), stdin) != NULL) {
        trim_newline(query);
        if (strlen(query) > 0) {
            if (!is_valid_email(query)) {
                set_console_color(COLOR_RED);
                printf("\n  Invalid email address.\n");
                reset_console_color();
                pause_program();
                return;
            }
            snprintf(inv.email, sizeof(inv.email), "%s", query);
        }
    }

    printf("  Phone [%s]: ", inv.phone);
    if (fgets(query, sizeof(query), stdin) != NULL) {
        trim_newline(query);
        if (strlen(query) > 0) {
            if (!is_valid_phone(query)) {
                set_console_color(COLOR_RED);
                printf("\n  Invalid phone number.\n");
                reset_console_color();
                pause_program();
                return;
            }
            snprintf(inv.phone, sizeof(inv.phone), "%s", query);
        }
    }

    db_escape_string(escaped_name, inv.full_name, sizeof(escaped_name));
    db_escape_string(escaped_dept, inv.department, sizeof(escaped_dept));
    db_escape_string(escaped_email, inv.email, sizeof(escaped_email));
    db_escape_string(escaped_phone, inv.phone, sizeof(escaped_phone));

    snprintf(query, sizeof(query),
             "UPDATE invigilators SET full_name = '%s', department = '%s', "
             "email = '%s', phone = '%s' WHERE invigilator_id = %d",
             escaped_name, escaped_dept, escaped_email, escaped_phone, invigilator_id);

    if (db_execute(query)) {
        set_console_color(COLOR_GREEN);
        printf("\n  Invigilator updated successfully!\n");
        reset_console_color();
    } else {
        set_console_color(COLOR_RED);
        printf("\n  Failed to update invigilator.\n");
        reset_console_color();
    }

    pause_program();
}

void delete_invigilator(void) {
    int invigilator_id;
    char query[MAX_QUERY];
    MYSQL_RES *result;

    print_header("DELETE INVIGILATOR");

    view_invigilators();

    invigilator_id = get_valid_int("\n  Enter Invigilator ID to delete: ");

    snprintf(query, sizeof(query),
             "SELECT full_name FROM invigilators WHERE invigilator_id = %d", invigilator_id);

    result = db_query(query);
    if (result == NULL || mysql_fetch_row(result) == NULL) {
        if (result) mysql_free_result(result);
        set_console_color(COLOR_RED);
        printf("\n  Invigilator not found!\n");
        reset_console_color();
        pause_program();
        return;
    }
    mysql_free_result(result);

    if (!confirm_dialog("Are you sure you want to delete this invigilator")) {
        printf("\n  Operation cancelled.\n");
        pause_program();
        return;
    }

    snprintf(query, sizeof(query),
             "DELETE FROM invigilators WHERE invigilator_id = %d", invigilator_id);

    if (db_execute(query)) {
        set_console_color(COLOR_GREEN);
        printf("\n  Invigilator deleted successfully!\n");
        reset_console_color();
    } else {
        set_console_color(COLOR_RED);
        printf("\n  Failed to delete invigilator.\n");
        reset_console_color();
    }

    pause_program();
}

void view_invigilators(void) {
    char query[MAX_QUERY];
    MYSQL_RES *result;
    MYSQL_ROW row;
    int count = 0;

    snprintf(query, sizeof(query),
             "SELECT * FROM invigilators ORDER BY full_name");

    result = db_query(query);
    if (result == NULL) {
        set_console_color(COLOR_RED);
        printf("\n  No invigilators found or database error.\n");
        reset_console_color();
        return;
    }

    printf("\n");
    {
        const char *headers[] = {"ID", "Name", "Department", "Email", "Phone"};
        int widths[] = {4, 25, 28, 28, 12};
        draw_table_header(headers, widths, 5);
    }

    while ((row = mysql_fetch_row(result)) != NULL) {
        const char *cells[] = {row[0], row[1], row[2], row[3], row[4]};
        int widths[] = {4, 25, 28, 28, 12};
        draw_table_row(cells, widths, 5);
        count++;
    }

    mysql_free_result(result);

    if (count > 0) {
        printf("  ");
        print_line('-', 4 + 25 + 28 + 28 + 12 + 7);
        set_console_color(COLOR_YELLOW);
        printf("  Total Invigilators: %d\n", count);
        reset_console_color();
    } else {
        set_console_color(COLOR_RED);
        printf("\n  No invigilators in database.\n");
        reset_console_color();
    }
}

void search_invigilator(void) {
    char term[MAX_NAME];
    char escaped_term[MAX_NAME * 2 + 1];
    char query[MAX_QUERY];
    MYSQL_RES *result;
    MYSQL_ROW row;
    int count = 0;

    print_header("SEARCH INVIGILATOR");

    get_string_input("  Enter search name or department: ", term, sizeof(term));
    db_escape_string(escaped_term, term, sizeof(escaped_term));

    snprintf(query, sizeof(query),
             "SELECT * FROM invigilators WHERE full_name LIKE '%%%s%%' "
             "OR department LIKE '%%%s%%' ORDER BY full_name",
             escaped_term, escaped_term);

    result = db_query(query);
    if (result == NULL) {
        set_console_color(COLOR_RED);
        printf("\n  No results found.\n");
        reset_console_color();
        pause_program();
        return;
    }

    printf("\n");
    {
        const char *headers[] = {"ID", "Name", "Department", "Email", "Phone"};
        int widths[] = {4, 25, 28, 28, 12};
        draw_table_header(headers, widths, 5);
    }

    while ((row = mysql_fetch_row(result)) != NULL) {
        const char *cells[] = {row[0], row[1], row[2], row[3], row[4]};
        int widths[] = {4, 25, 28, 28, 12};
        draw_table_row(cells, widths, 5);
        count++;
    }

    mysql_free_result(result);

    if (count > 0) {
        printf("  ");
        print_line('-', 4 + 25 + 28 + 28 + 12 + 7);
        set_console_color(COLOR_YELLOW);
        printf("  Total Invigilators Found: %d\n", count);
        reset_console_color();
    } else {
        set_console_color(COLOR_RED);
        printf("\n  No matching invigilators found.\n");
        reset_console_color();
    }

    pause_program();
}

void assign_invigilator_duty(void) {
    int invigilator_id, exam_id, room_id;
    char query[MAX_QUERY];
    MYSQL_RES *result;

    print_header("ASSIGN INVIGILATOR DUTY");

    view_invigilators();
    invigilator_id = get_valid_int("\n  Enter Invigilator ID: ");

    view_exam_schedule();
    exam_id = get_valid_int("\n  Enter Exam ID: ");

    view_classrooms();
    room_id = get_valid_int("\n  Enter Classroom (Room ID): ");

    /* Check if invigilator already assigned to this exam */
    snprintf(query, sizeof(query),
             "SELECT duty_id FROM invigilator_duties WHERE invigilator_id = %d AND exam_id = %d",
             invigilator_id, exam_id);
    result = db_query(query);
    if (result != NULL) {
        if (mysql_fetch_row(result) != NULL) {
            mysql_free_result(result);
            set_console_color(COLOR_RED);
            printf("\n  This invigilator is already assigned to this exam!\n");
            reset_console_color();
            pause_program();
            return;
        }
        mysql_free_result(result);
    }

    /* Check if room already has an invigilator for this exam */
    snprintf(query, sizeof(query),
             "SELECT duty_id FROM invigilator_duties WHERE exam_id = %d AND room_id = %d",
             exam_id, room_id);
    result = db_query(query);
    if (result != NULL) {
        if (mysql_fetch_row(result) != NULL) {
            mysql_free_result(result);
            set_console_color(COLOR_RED);
            printf("\n  This room already has an assigned invigilator for this exam!\n");
            reset_console_color();
            pause_program();
            return;
        }
        mysql_free_result(result);
    }

    snprintf(query, sizeof(query),
             "INSERT INTO invigilator_duties (invigilator_id, exam_id, room_id) "
             "VALUES (%d, %d, %d)",
             invigilator_id, exam_id, room_id);

    if (db_execute(query)) {
        set_console_color(COLOR_GREEN);
        printf("\n  Invigilator duty assigned successfully!\n");
        reset_console_color();
    } else {
        set_console_color(COLOR_RED);
        printf("\n  Failed to assign invigilator duty.\n");
        reset_console_color();
    }

    pause_program();
}

void view_duty_roster(void) {
    char query[MAX_QUERY];
    MYSQL_RES *result;
    MYSQL_ROW row;
    int count = 0;

    snprintf(query, sizeof(query),
             "SELECT d.duty_id, i.full_name, e.subject, e.exam_date, e.start_time, e.end_time, r.room_name "
             "FROM invigilator_duties d "
             "JOIN invigilators i ON d.invigilator_id = i.invigilator_id "
             "JOIN exams e ON d.exam_id = e.exam_id "
             "JOIN classrooms r ON d.room_id = r.room_id "
             "ORDER BY e.exam_date, e.start_time, r.room_name");

    result = db_query(query);
    if (result == NULL) {
        set_console_color(COLOR_RED);
        printf("\n  No duty roster records found or database error.\n");
        reset_console_color();
        return;
    }

    printf("\n");
    {
        const char *headers[] = {"Duty ID", "Invigilator Name", "Subject", "Date", "Time", "Room"};
        int widths[] = {8, 25, 25, 12, 18, 12};
        draw_table_header(headers, widths, 6);
    }

    while ((row = mysql_fetch_row(result)) != NULL) {
        char time_range[30];
        snprintf(time_range, sizeof(time_range), "%s - %s", row[4], row[5]);
        const char *cells[] = {row[0], row[1], row[2], row[3], time_range, row[6]};
        int widths[] = {8, 25, 25, 12, 18, 12};
        draw_table_row(cells, widths, 6);
        count++;
    }

    mysql_free_result(result);

    if (count > 0) {
        printf("  ");
        print_line('-', 8 + 25 + 25 + 12 + 18 + 12 + 8);
        set_console_color(COLOR_YELLOW);
        printf("  Total Duties Assigned: %d\n", count);
        reset_console_color();
    } else {
        set_console_color(COLOR_RED);
        printf("\n  No invigilator duties assigned yet.\n");
        reset_console_color();
    }
}
