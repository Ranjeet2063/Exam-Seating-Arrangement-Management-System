#include "header.h"

void student_menu(void) {
    int choice;

    do {
        print_header("STUDENT MANAGEMENT");

        printf("  1. Add Student\n");
        printf("  2. Update Student\n");
        printf("  3. Delete Student\n");
        printf("  4. Search Student\n");
        printf("  5. View All Students\n");
        printf("  6. Back to Main Menu\n");

        choice = get_valid_int("\n  Enter your choice: ");

        switch (choice) {
            case 1: add_student(); break;
            case 2: update_student(); break;
            case 3: delete_student(); break;
            case 4: search_student(); break;
            case 5: view_students(); break;
            case 6: return;
            default:
                set_console_color(COLOR_RED);
                printf("\n  Invalid choice!\n");
                reset_console_color();
                pause_program();
        }
    } while (1);
}

void add_student(void) {
    Student s;
    char query[MAX_QUERY];
    MYSQL_RES *result;
    char escaped_roll[MAX_ROLL * 2 + 1];
    char escaped_reg[MAX_REG * 2 + 1];
    char escaped_name[MAX_NAME * 2 + 1];
    char escaped_dept[MAX_DEPT_NAME * 2 + 1];
    char escaped_phone[MAX_PHONE * 2 + 1];
    char escaped_email[MAX_EMAIL * 2 + 1];

    print_header("ADD STUDENT");

    get_string_input("  Roll Number: ", s.roll_number, sizeof(s.roll_number));
    if (!is_valid_roll_number(s.roll_number)) {
        set_console_color(COLOR_RED);
        printf("\n  Invalid roll number. Use letters, numbers, hyphens.\n");
        reset_console_color();
        pause_program();
        return;
    }

    db_escape_string(escaped_roll, s.roll_number, sizeof(escaped_roll));

    snprintf(query, sizeof(query),
             "SELECT student_id FROM students WHERE roll_number = '%s'",
             escaped_roll);
    result = db_query(query);
    if (result != NULL) {
        if (mysql_fetch_row(result) != NULL) {
            mysql_free_result(result);
            set_console_color(COLOR_RED);
            printf("\n  Roll number already exists!\n");
            reset_console_color();
            pause_program();
            return;
        }
        mysql_free_result(result);
    }

    get_string_input("  Registration Number: ", s.registration_number,
                     sizeof(s.registration_number));
    if (!is_valid_registration(s.registration_number)) {
        set_console_color(COLOR_RED);
        printf("\n  Invalid registration number.\n");
        reset_console_color();
        pause_program();
        return;
    }

    db_escape_string(escaped_reg, s.registration_number, sizeof(escaped_reg));

    snprintf(query, sizeof(query),
             "SELECT student_id FROM students WHERE registration_number = '%s'",
             escaped_reg);
    result = db_query(query);
    if (result != NULL) {
        if (mysql_fetch_row(result) != NULL) {
            mysql_free_result(result);
            set_console_color(COLOR_RED);
            printf("\n  Registration number already exists!\n");
            reset_console_color();
            pause_program();
            return;
        }
        mysql_free_result(result);
    }

    get_string_input("  Full Name: ", s.full_name, sizeof(s.full_name));
    if (!is_valid_name(s.full_name)) {
        set_console_color(COLOR_RED);
        printf("\n  Invalid name. Use only letters and spaces.\n");
        reset_console_color();
        pause_program();
        return;
    }

    get_string_input("  Department: ", s.department, sizeof(s.department));
    if (!is_valid_name(s.department)) {
        set_console_color(COLOR_RED);
        printf("\n  Invalid department name.\n");
        reset_console_color();
        pause_program();
        return;
    }

    s.semester = get_valid_int("  Semester (1-8): ");
    if (!is_within_range(s.semester, 1, 8)) {
        set_console_color(COLOR_RED);
        printf("\n  Semester must be between 1 and 8.\n");
        reset_console_color();
        pause_program();
        return;
    }

    s.year = get_valid_int("  Year (1-4): ");
    if (!is_within_range(s.year, 1, 4)) {
        set_console_color(COLOR_RED);
        printf("\n  Year must be between 1 and 4.\n");
        reset_console_color();
        pause_program();
        return;
    }

    get_string_input("  Phone (10 digits): ", s.phone, sizeof(s.phone));
    if (!is_valid_phone(s.phone)) {
        set_console_color(COLOR_RED);
        printf("\n  Invalid phone number. Must be 10 digits.\n");
        reset_console_color();
        pause_program();
        return;
    }

    get_string_input("  Email: ", s.email, sizeof(s.email));
    if (!is_valid_email(s.email)) {
        set_console_color(COLOR_RED);
        printf("\n  Invalid email format.\n");
        reset_console_color();
        pause_program();
        return;
    }

    db_escape_string(escaped_roll, s.roll_number, sizeof(escaped_roll));
    db_escape_string(escaped_reg, s.registration_number, sizeof(escaped_reg));
    db_escape_string(escaped_name, s.full_name, sizeof(escaped_name));
    db_escape_string(escaped_dept, s.department, sizeof(escaped_dept));
    db_escape_string(escaped_phone, s.phone, sizeof(escaped_phone));
    db_escape_string(escaped_email, s.email, sizeof(escaped_email));

    snprintf(query, sizeof(query),
             "INSERT INTO students (roll_number, registration_number, full_name, "
             "department, semester, year, phone, email) "
             "VALUES ('%s', '%s', '%s', '%s', %d, %d, '%s', '%s')",
             escaped_roll, escaped_reg, escaped_name, escaped_dept,
             s.semester, s.year, escaped_phone, escaped_email);

    if (db_execute(query)) {
        set_console_color(COLOR_GREEN);
        printf("\n  Student added successfully!\n");
        reset_console_color();
    } else {
        set_console_color(COLOR_RED);
        printf("\n  Failed to add student.\n");
        reset_console_color();
    }

    pause_program();
}

void update_student(void) {
    int student_id;
    Student s;
    char query[MAX_QUERY];
    MYSQL_RES *result;
    MYSQL_ROW row;
    char escaped_name[MAX_NAME * 2 + 1];
    char escaped_dept[MAX_DEPT_NAME * 2 + 1];
    char escaped_phone[MAX_PHONE * 2 + 1];
    char escaped_email[MAX_EMAIL * 2 + 1];

    print_header("UPDATE STUDENT");

    student_id = get_valid_int("  Enter Student ID to update: ");

    snprintf(query, sizeof(query),
             "SELECT * FROM students WHERE student_id = %d", student_id);

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
        printf("\n  Student not found!\n");
        reset_console_color();
        pause_program();
        return;
    }

    s.student_id = atoi(row[0]);
    snprintf(s.roll_number, sizeof(s.roll_number), "%s", row[1]);
    snprintf(s.registration_number, sizeof(s.registration_number), "%s", row[2]);
    snprintf(s.full_name, sizeof(s.full_name), "%s", row[3]);
    snprintf(s.department, sizeof(s.department), "%s", row[4]);
    s.semester = atoi(row[5]);
    s.year = atoi(row[6]);
    snprintf(s.phone, sizeof(s.phone), "%s", row[7]);
    snprintf(s.email, sizeof(s.email), "%s", row[8]);
    mysql_free_result(result);

    printf("\n  Current Details:\n");
    printf("  Roll Number: %s\n", s.roll_number);
    printf("  Registration: %s\n", s.registration_number);

    printf("\n  Enter new values (press Enter to keep current):\n");

    printf("  Full Name [%s]: ", s.full_name);
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
            snprintf(s.full_name, sizeof(s.full_name), "%s", query);
        }
    }

    printf("  Department [%s]: ", s.department);
    if (fgets(query, sizeof(query), stdin) != NULL) {
        trim_newline(query);
        if (strlen(query) > 0) {
            if (!is_valid_name(query)) {
                set_console_color(COLOR_RED);
                printf("\n  Invalid department name.\n");
                reset_console_color();
                pause_program();
                return;
            }
            snprintf(s.department, sizeof(s.department), "%s", query);
        }
    }

    printf("  Semester [%d]: ", s.semester);
    if (fgets(query, sizeof(query), stdin) != NULL) {
        trim_newline(query);
        if (strlen(query) > 0) {
            int val = atoi(query);
            if (is_within_range(val, 1, 8)) {
                s.semester = val;
            }
        }
    }

    printf("  Year [%d]: ", s.year);
    if (fgets(query, sizeof(query), stdin) != NULL) {
        trim_newline(query);
        if (strlen(query) > 0) {
            int val = atoi(query);
            if (is_within_range(val, 1, 4)) {
                s.year = val;
            }
        }
    }

    printf("  Phone [%s]: ", s.phone);
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
            snprintf(s.phone, sizeof(s.phone), "%s", query);
        }
    }

    printf("  Email [%s]: ", s.email);
    if (fgets(query, sizeof(query), stdin) != NULL) {
        trim_newline(query);
        if (strlen(query) > 0) {
            if (!is_valid_email(query)) {
                set_console_color(COLOR_RED);
                printf("\n  Invalid email.\n");
                reset_console_color();
                pause_program();
                return;
            }
            snprintf(s.email, sizeof(s.email), "%s", query);
        }
    }

    db_escape_string(escaped_name, s.full_name, sizeof(escaped_name));
    db_escape_string(escaped_dept, s.department, sizeof(escaped_dept));
    db_escape_string(escaped_phone, s.phone, sizeof(escaped_phone));
    db_escape_string(escaped_email, s.email, sizeof(escaped_email));

    snprintf(query, sizeof(query),
             "UPDATE students SET full_name = '%s', department = '%s', "
             "semester = %d, year = %d, phone = '%s', email = '%s' "
             "WHERE student_id = %d",
             escaped_name, escaped_dept, s.semester, s.year,
             escaped_phone, escaped_email, student_id);

    if (db_execute(query)) {
        set_console_color(COLOR_GREEN);
        printf("\n  Student updated successfully!\n");
        reset_console_color();
    } else {
        set_console_color(COLOR_RED);
        printf("\n  Failed to update student.\n");
        reset_console_color();
    }

    pause_program();
}

void delete_student(void) {
    int student_id;
    char query[MAX_QUERY];
    MYSQL_RES *result;

    print_header("DELETE STUDENT");

    student_id = get_valid_int("  Enter Student ID to delete: ");

    snprintf(query, sizeof(query),
             "SELECT full_name FROM students WHERE student_id = %d", student_id);

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
        printf("\n  Student not found!\n");
        reset_console_color();
        pause_program();
        return;
    }
    mysql_free_result(result);

    if (!confirm_dialog("Are you sure you want to delete this student")) {
        printf("\n  Operation cancelled.\n");
        pause_program();
        return;
    }

    snprintf(query, sizeof(query),
             "DELETE FROM students WHERE student_id = %d", student_id);

    if (db_execute(query)) {
        set_console_color(COLOR_GREEN);
        printf("\n  Student deleted successfully!\n");
        reset_console_color();
    } else {
        set_console_color(COLOR_RED);
        printf("\n  Failed to delete student.\n");
        reset_console_color();
    }

    pause_program();
}

void search_student(void) {
    char search_term[MAX_NAME];
    char escaped_term[MAX_NAME * 2 + 1];
    char query[MAX_QUERY];
    MYSQL_RES *result;
    MYSQL_ROW row;
    int choice;
    int found = 0;

    print_header("SEARCH STUDENT");

    printf("  Search by:\n");
    printf("  1. Roll Number\n");
    printf("  2. Registration Number\n");
    printf("  3. Name\n");
    printf("  4. Department\n");
    printf("  5. Back\n");

    choice = get_valid_int("\n  Enter choice: ");

    if (choice < 1 || choice > 4) return;

    get_string_input("  Enter search term: ", search_term, sizeof(search_term));
    db_escape_string(escaped_term, search_term, sizeof(escaped_term));

    switch (choice) {
        case 1:
            snprintf(query, sizeof(query),
                     "SELECT * FROM students WHERE roll_number LIKE '%%%s%%' ORDER BY roll_number",
                     escaped_term);
            break;
        case 2:
            snprintf(query, sizeof(query),
                     "SELECT * FROM students WHERE registration_number LIKE '%%%s%%' ORDER BY roll_number",
                     escaped_term);
            break;
        case 3:
            snprintf(query, sizeof(query),
                     "SELECT * FROM students WHERE full_name LIKE '%%%s%%' ORDER BY roll_number",
                     escaped_term);
            break;
        case 4:
            snprintf(query, sizeof(query),
                     "SELECT * FROM students WHERE department LIKE '%%%s%%' ORDER BY roll_number",
                     escaped_term);
            break;
        default:
            return;
    }

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
        const char *headers[] = {"ID", "Roll No", "Reg No", "Name", "Dept", "Sem", "Year", "Phone", "Email"};
        int widths[] = {4, 10, 12, 25, 25, 4, 5, 12, 25};
        draw_table_header(headers, widths, 9);
    }

    while ((row = mysql_fetch_row(result)) != NULL) {
        const char *cells[] = {row[0], row[1], row[2], row[3], row[4],
                               row[5], row[6], row[7], row[8]};
        int widths[] = {4, 10, 12, 25, 25, 4, 5, 12, 25};
        draw_table_row(cells, widths, 9);
        found++;
    }

    mysql_free_result(result);

    if (found > 0) {
        printf("  ");
        print_line('-', 4 + 10 + 12 + 25 + 25 + 4 + 5 + 12 + 25 + 10);
        set_console_color(COLOR_YELLOW);
        printf("  Total Students Found: %d\n", found);
        reset_console_color();
    } else {
        set_console_color(COLOR_RED);
        printf("\n  No matching students found.\n");
        reset_console_color();
    }

    pause_program();
}

void view_students(void) {
    char query[MAX_QUERY];
    MYSQL_RES *result;
    MYSQL_ROW row;
    int count = 0;

    snprintf(query, sizeof(query),
             "SELECT * FROM students ORDER BY roll_number");

    result = db_query(query);
    if (result == NULL) {
        set_console_color(COLOR_RED);
        printf("\n  No students found or database error.\n");
        reset_console_color();
        return;
    }

    printf("\n");
    {
        const char *headers[] = {"ID", "Roll No", "Reg No", "Name", "Dept", "Sem", "Year", "Phone", "Email"};
        int widths[] = {4, 10, 12, 25, 25, 4, 5, 12, 25};
        draw_table_header(headers, widths, 9);
    }

    while ((row = mysql_fetch_row(result)) != NULL) {
        const char *cells[] = {row[0], row[1], row[2], row[3], row[4],
                               row[5], row[6], row[7], row[8]};
        int widths[] = {4, 10, 12, 25, 25, 4, 5, 12, 25};
        draw_table_row(cells, widths, 9);
        count++;
    }

    mysql_free_result(result);

    if (count > 0) {
        printf("  ");
        print_line('-', 4 + 10 + 12 + 25 + 25 + 4 + 5 + 12 + 25 + 10);
        set_console_color(COLOR_YELLOW);
        printf("  Total Students: %d\n", count);
        reset_console_color();
    } else {
        set_console_color(COLOR_RED);
        printf("\n  No students in database.\n");
        reset_console_color();
    }
}
