#include "header.h"

void department_menu(void) {
    int choice;

    do {
        print_header("DEPARTMENT MANAGEMENT");

        printf("  1. Add Department\n");
        printf("  2. Update Department\n");
        printf("  3. Delete Department\n");
        printf("  4. View All Departments\n");
        printf("  5. Back to Main Menu\n");

        choice = get_valid_int("\n  Enter your choice: ");

        switch (choice) {
            case 1: add_department(); break;
            case 2: update_department(); break;
            case 3: delete_department(); break;
            case 4: view_departments(); break;
            case 5: return;
            default:
                set_console_color(COLOR_RED);
                printf("\n  Invalid choice!\n");
                reset_console_color();
                pause_program();
        }
    } while (1);
}

void add_department(void) {
    char name[MAX_DEPT_NAME];
    char query[MAX_QUERY];
    MYSQL_RES *result;

    print_header("ADD DEPARTMENT");

    get_string_input("  Department Name: ", name, sizeof(name));

    if (!is_valid_name(name)) {
        set_console_color(COLOR_RED);
        printf("\n  Invalid department name. Use only letters and spaces.\n");
        reset_console_color();
        pause_program();
        return;
    }

    snprintf(query, sizeof(query),
             "SELECT department_id FROM departments WHERE department_name = '%s'",
             name);

    result = db_query(query);
    if (result != NULL) {
        if (mysql_fetch_row(result) != NULL) {
            mysql_free_result(result);
            set_console_color(COLOR_RED);
            printf("\n  Department already exists!\n");
            reset_console_color();
            pause_program();
            return;
        }
        mysql_free_result(result);
    }

    snprintf(query, sizeof(query),
             "INSERT INTO departments (department_name) VALUES ('%s')", name);

    if (db_execute(query)) {
        set_console_color(COLOR_GREEN);
        printf("\n  Department added successfully!\n");
        reset_console_color();
    } else {
        set_console_color(COLOR_RED);
        printf("\n  Failed to add department.\n");
        reset_console_color();
    }

    pause_program();
}

void update_department(void) {
    int dept_id;
    char new_name[MAX_DEPT_NAME];
    char query[MAX_QUERY];
    MYSQL_RES *result;

    print_header("UPDATE DEPARTMENT");

    view_departments();

    dept_id = get_valid_int("\n  Enter Department ID to update: ");

    snprintf(query, sizeof(query),
             "SELECT department_name FROM departments WHERE department_id = %d",
             dept_id);

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
        printf("\n  Department not found!\n");
        reset_console_color();
        pause_program();
        return;
    }
    mysql_free_result(result);

    get_string_input("  New Department Name: ", new_name, sizeof(new_name));

    if (!is_valid_name(new_name)) {
        set_console_color(COLOR_RED);
        printf("\n  Invalid name format.\n");
        reset_console_color();
        pause_program();
        return;
    }

    snprintf(query, sizeof(query),
             "UPDATE departments SET department_name = '%s' WHERE department_id = %d",
             new_name, dept_id);

    if (db_execute(query)) {
        set_console_color(COLOR_GREEN);
        printf("\n  Department updated successfully!\n");
        reset_console_color();
    } else {
        set_console_color(COLOR_RED);
        printf("\n  Failed to update department.\n");
        reset_console_color();
    }

    pause_program();
}

void delete_department(void) {
    int dept_id;
    char query[MAX_QUERY];
    MYSQL_RES *result;

    print_header("DELETE DEPARTMENT");

    view_departments();

    dept_id = get_valid_int("\n  Enter Department ID to delete: ");

    snprintf(query, sizeof(query),
             "SELECT department_name FROM departments WHERE department_id = %d",
             dept_id);

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
        printf("\n  Department not found!\n");
        reset_console_color();
        pause_program();
        return;
    }
    mysql_free_result(result);

    if (!confirm_dialog("Are you sure you want to delete this department")) {
        printf("\n  Operation cancelled.\n");
        pause_program();
        return;
    }

    snprintf(query, sizeof(query),
             "DELETE FROM departments WHERE department_id = %d", dept_id);

    if (db_execute(query)) {
        set_console_color(COLOR_GREEN);
        printf("\n  Department deleted successfully!\n");
        reset_console_color();
    } else {
        set_console_color(COLOR_RED);
        printf("\n  Failed to delete department. It may have associated records.\n");
        reset_console_color();
    }

    pause_program();
}

void view_departments(void) {
    char query[MAX_QUERY];
    MYSQL_RES *result;
    MYSQL_ROW row;
    int count = 0;

    snprintf(query, sizeof(query),
             "SELECT department_id, department_name FROM departments ORDER BY department_id");

    result = db_query(query);
    if (result == NULL) {
        set_console_color(COLOR_RED);
        printf("\n  No departments found or database error.\n");
        reset_console_color();
        return;
    }

    printf("\n");
    {
        const char *headers[] = {"ID", "Department Name"};
        int widths[] = {5, 50};
        draw_table_header(headers, widths, 2);
    }

    while ((row = mysql_fetch_row(result)) != NULL) {
        const char *cells[] = {row[0], row[1]};
        int widths[] = {5, 50};
        draw_table_row(cells, widths, 2);
        count++;
    }

    mysql_free_result(result);

    {
        int total_width = 5 + 50 + 7;
        printf("  ");
        print_line('-', total_width);
    }

    set_console_color(COLOR_YELLOW);
    printf("  Total Departments: %d\n", count);
    reset_console_color();
}
