#include "header.h"

void classroom_menu(void) {
    int choice;

    do {
        print_header("CLASSROOM MANAGEMENT");

        printf("  1. Add Classroom\n");
        printf("  2. Update Classroom\n");
        printf("  3. Delete Classroom\n");
        printf("  4. View All Classrooms\n");
        printf("  5. Search Classroom\n");
        printf("  6. Back to Main Menu\n");

        choice = get_valid_int("\n  Enter your choice: ");

        switch (choice) {
            case 1: add_classroom(); break;
            case 2: update_classroom(); break;
            case 3: delete_classroom(); break;
            case 4: view_classrooms(); break;
            case 5: search_classroom(); break;
            case 6: return;
            default:
                set_console_color(COLOR_RED);
                printf("\n  Invalid choice!\n");
                reset_console_color();
                pause_program();
        }
    } while (1);
}

void add_classroom(void) {
    Classroom c;
    char query[MAX_QUERY];
    MYSQL_RES *result;
    char escaped_room[MAX_ROOM_NAME * 2 + 1];
    char escaped_building[MAX_BUILDING * 2 + 1];

    print_header("ADD CLASSROOM");

    get_string_input("  Room Name: ", c.room_name, sizeof(c.room_name));
    if (!is_valid_room_name(c.room_name)) {
        set_console_color(COLOR_RED);
        printf("\n  Invalid room name.\n");
        reset_console_color();
        pause_program();
        return;
    }

    snprintf(query, sizeof(query),
             "SELECT room_id FROM classrooms WHERE room_name = '%s'",
             c.room_name);
    result = db_query(query);
    if (result != NULL) {
        if (mysql_fetch_row(result) != NULL) {
            mysql_free_result(result);
            set_console_color(COLOR_RED);
            printf("\n  Classroom already exists!\n");
            reset_console_color();
            pause_program();
            return;
        }
        mysql_free_result(result);
    }

    get_string_input("  Building: ", c.building, sizeof(c.building));
    if (!is_valid_name(c.building) && strlen(c.building) < 1) {
        set_console_color(COLOR_RED);
        printf("\n  Invalid building name.\n");
        reset_console_color();
        pause_program();
        return;
    }

    c.floor = get_valid_int("  Floor: ");
    c.capacity = get_valid_int("  Capacity: ");

    if (c.capacity < 10 || c.capacity > 500) {
        set_console_color(COLOR_RED);
        printf("\n  Capacity must be between 10 and 500.\n");
        reset_console_color();
        pause_program();
        return;
    }

    db_escape_string(escaped_room, c.room_name, sizeof(escaped_room));
    db_escape_string(escaped_building, c.building, sizeof(escaped_building));

    snprintf(query, sizeof(query),
             "INSERT INTO classrooms (room_name, building, floor, capacity) "
             "VALUES ('%s', '%s', %d, %d)",
             escaped_room, escaped_building, c.floor, c.capacity);

    if (db_execute(query)) {
        set_console_color(COLOR_GREEN);
        printf("\n  Classroom added successfully!\n");
        reset_console_color();
    } else {
        set_console_color(COLOR_RED);
        printf("\n  Failed to add classroom.\n");
        reset_console_color();
    }

    pause_program();
}

void update_classroom(void) {
    int room_id;
    char query[MAX_QUERY];
    MYSQL_RES *result;
    MYSQL_ROW row;
    Classroom c;
    char escaped_room[MAX_ROOM_NAME * 2 + 1];
    char escaped_building[MAX_BUILDING * 2 + 1];

    print_header("UPDATE CLASSROOM");

    view_classrooms();

    room_id = get_valid_int("\n  Enter Room ID to update: ");

    snprintf(query, sizeof(query),
             "SELECT * FROM classrooms WHERE room_id = %d", room_id);

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
        printf("\n  Classroom not found!\n");
        reset_console_color();
        pause_program();
        return;
    }

    snprintf(c.room_name, sizeof(c.room_name), "%s", row[1]);
    snprintf(c.building, sizeof(c.building), "%s", row[2]);
    c.floor = atoi(row[3]);
    c.capacity = atoi(row[4]);
    mysql_free_result(result);

    printf("\n  Current: %s | %s | Floor %d | Capacity %d\n",
           c.room_name, c.building, c.floor, c.capacity);

    printf("\n  Enter new values (press Enter to keep current):\n");

    printf("  Room Name [%s]: ", c.room_name);
    if (fgets(query, sizeof(query), stdin) != NULL) {
        trim_newline(query);
        if (strlen(query) > 0) {
            if (!is_valid_room_name(query)) {
                set_console_color(COLOR_RED);
                printf("\n  Invalid room name.\n");
                reset_console_color();
                pause_program();
                return;
            }
            snprintf(c.room_name, sizeof(c.room_name), "%s", query);
        }
    }

    printf("  Building [%s]: ", c.building);
    if (fgets(query, sizeof(query), stdin) != NULL) {
        trim_newline(query);
        if (strlen(query) > 0) {
            snprintf(c.building, sizeof(c.building), "%s", query);
        }
    }

    printf("  Floor [%d]: ", c.floor);
    if (fgets(query, sizeof(query), stdin) != NULL) {
        trim_newline(query);
        if (strlen(query) > 0) {
            c.floor = atoi(query);
        }
    }

    printf("  Capacity [%d]: ", c.capacity);
    if (fgets(query, sizeof(query), stdin) != NULL) {
        trim_newline(query);
        if (strlen(query) > 0) {
            int val = atoi(query);
            if (val >= 10 && val <= 500) c.capacity = val;
        }
    }

    db_escape_string(escaped_room, c.room_name, sizeof(escaped_room));
    db_escape_string(escaped_building, c.building, sizeof(escaped_building));

    snprintf(query, sizeof(query),
             "UPDATE classrooms SET room_name = '%s', building = '%s', "
             "floor = %d, capacity = %d WHERE room_id = %d",
             escaped_room, escaped_building, c.floor, c.capacity, room_id);

    if (db_execute(query)) {
        set_console_color(COLOR_GREEN);
        printf("\n  Classroom updated successfully!\n");
        reset_console_color();
    } else {
        set_console_color(COLOR_RED);
        printf("\n  Failed to update classroom.\n");
        reset_console_color();
    }

    pause_program();
}

void delete_classroom(void) {
    int room_id;
    char query[MAX_QUERY];
    MYSQL_RES *result;

    print_header("DELETE CLASSROOM");

    view_classrooms();

    room_id = get_valid_int("\n  Enter Room ID to delete: ");

    snprintf(query, sizeof(query),
             "SELECT room_name FROM classrooms WHERE room_id = %d", room_id);

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
        printf("\n  Classroom not found!\n");
        reset_console_color();
        pause_program();
        return;
    }
    mysql_free_result(result);

    if (!confirm_dialog("Are you sure you want to delete this classroom")) {
        printf("\n  Operation cancelled.\n");
        pause_program();
        return;
    }

    snprintf(query, sizeof(query),
             "DELETE FROM classrooms WHERE room_id = %d", room_id);

    if (db_execute(query)) {
        set_console_color(COLOR_GREEN);
        printf("\n  Classroom deleted successfully!\n");
        reset_console_color();
    } else {
        set_console_color(COLOR_RED);
        printf("\n  Failed to delete classroom.\n");
        reset_console_color();
    }

    pause_program();
}

void view_classrooms(void) {
    char query[MAX_QUERY];
    MYSQL_RES *result;
    MYSQL_ROW row;
    int count = 0;

    snprintf(query, sizeof(query),
             "SELECT * FROM classrooms ORDER BY building, floor, room_name");

    result = db_query(query);
    if (result == NULL) {
        set_console_color(COLOR_RED);
        printf("\n  No classrooms found or database error.\n");
        reset_console_color();
        return;
    }

    printf("\n");
    {
        const char *headers[] = {"ID", "Room", "Building", "Floor", "Capacity"};
        int widths[] = {4, 12, 25, 6, 9};
        draw_table_header(headers, widths, 5);
    }

    while ((row = mysql_fetch_row(result)) != NULL) {
        const char *cells[] = {row[0], row[1], row[2], row[3], row[4]};
        int widths[] = {4, 12, 25, 6, 9};
        draw_table_row(cells, widths, 5);
        count++;
    }

    mysql_free_result(result);

    if (count > 0) {
        printf("  ");
        print_line('-', 4 + 12 + 25 + 6 + 9 + 7);
        set_console_color(COLOR_YELLOW);
        printf("  Total Classrooms: %d\n", count);
        reset_console_color();
    }
}

void search_classroom(void) {
    char query[MAX_QUERY];
    MYSQL_RES *result;
    MYSQL_ROW row;
    char term[MAX_BUFFER];
    int found = 0;

    print_header("SEARCH CLASSROOM");

    get_string_input("  Enter room name or building: ", term, sizeof(term));

    snprintf(query, sizeof(query),
             "SELECT * FROM classrooms WHERE room_name LIKE '%%%s%%' "
             "OR building LIKE '%%%s%%' ORDER BY building, floor",
             term, term);

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
        const char *headers[] = {"ID", "Room", "Building", "Floor", "Capacity"};
        int widths[] = {4, 12, 25, 6, 9};
        draw_table_header(headers, widths, 5);
    }

    while ((row = mysql_fetch_row(result)) != NULL) {
        const char *cells[] = {row[0], row[1], row[2], row[3], row[4]};
        int widths[] = {4, 12, 25, 6, 9};
        draw_table_row(cells, widths, 5);
        found++;
    }

    mysql_free_result(result);

    if (found > 0) {
        printf("  ");
        print_line('-', 4 + 12 + 25 + 6 + 9 + 7);
        set_console_color(COLOR_YELLOW);
        printf("  Total Found: %d\n", found);
        reset_console_color();
    } else {
        set_console_color(COLOR_RED);
        printf("\n  No matching classrooms.\n");
        reset_console_color();
    }

    pause_program();
}
