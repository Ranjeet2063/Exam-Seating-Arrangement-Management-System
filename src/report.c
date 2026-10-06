#include "header.h"

void report_menu(void) {
    int choice;

    do {
        print_header("REPORTS");

        printf("  1. Dashboard\n");
        printf("  2. Student List\n");
        printf("  3. Room List\n");
        printf("  4. Department List\n");
        printf("  5. Exam Schedule\n");
        printf("  6. Seating Arrangement\n");
        printf("  7. Room Occupancy\n");
        printf("  8. Attendance Sheet\n");
        printf("  9. Empty Seats Report\n");
        printf(" 10. Capacity Report\n");
        printf(" 11. Back to Main Menu\n");

        choice = get_valid_int("\n  Enter your choice: ");

        switch (choice) {
            case 1:  show_dashboard(); break;
            case 2:  report_student_list(); break;
            case 3:  report_room_list(); break;
            case 4:  report_department_list(); break;
            case 5:  report_exam_schedule(); break;
            case 6:  report_seating_arrangement(); break;
            case 7:  report_room_occupancy(); break;
            case 8:  report_attendance_sheet(); break;
            case 9:  report_empty_seats(); break;
            case 10: report_capacity_report(); break;
            case 11: return;
            default:
                set_console_color(COLOR_RED);
                printf("\n  Invalid choice!\n");
                reset_console_color();
                pause_program();
        }
    } while (1);
}

void show_dashboard(void) {
    char query[MAX_QUERY];
    MYSQL_RES *result;
    MYSQL_ROW row;
    int student_count = 0;
    int dept_count = 0;
    int room_count = 0;
    int exam_count = 0;
    int seating_count = 0;
    int total_capacity = 0;

    print_header("DASHBOARD");

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM students");
    result = db_query(query);
    if (result != NULL) {
        row = mysql_fetch_row(result);
        if (row) student_count = atoi(row[0]);
        mysql_free_result(result);
    }

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM departments");
    result = db_query(query);
    if (result != NULL) {
        row = mysql_fetch_row(result);
        if (row) dept_count = atoi(row[0]);
        mysql_free_result(result);
    }

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM classrooms");
    result = db_query(query);
    if (result != NULL) {
        row = mysql_fetch_row(result);
        if (row) room_count = atoi(row[0]);
        mysql_free_result(result);
    }

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM exams");
    result = db_query(query);
    if (result != NULL) {
        row = mysql_fetch_row(result);
        if (row) exam_count = atoi(row[0]);
        mysql_free_result(result);
    }

    snprintf(query, sizeof(query), "SELECT COUNT(*) FROM seating");
    result = db_query(query);
    if (result != NULL) {
        row = mysql_fetch_row(result);
        if (row) seating_count = atoi(row[0]);
        mysql_free_result(result);
    }

    snprintf(query, sizeof(query), "SELECT SUM(capacity) FROM classrooms");
    result = db_query(query);
    if (result != NULL) {
        row = mysql_fetch_row(result);
        if (row && row[0]) total_capacity = atoi(row[0]);
        mysql_free_result(result);
    }

    set_console_color(COLOR_CYAN);
    printf("  +------------------------------------------+\n");
    printf("  |           SYSTEM OVERVIEW                |\n");
    printf("  +------------------------------------------+\n");
    reset_console_color();
    printf("  | %-30s %10d |\n", "Total Students", student_count);
    printf("  | %-30s %10d |\n", "Total Departments", dept_count);
    printf("  | %-30s %10d |\n", "Total Classrooms", room_count);
    printf("  | %-30s %10d |\n", "Total Exams", exam_count);
    printf("  | %-30s %10d |\n", "Seats Allocated", seating_count);
    printf("  | %-30s %10d |\n", "Total Room Capacity", total_capacity);
    set_console_color(COLOR_CYAN);
    printf("  +------------------------------------------+\n");
    reset_console_color();

    /* Upcoming exams */
    printf("\n");
    print_subheader("UPCOMING EXAMS");

    snprintf(query, sizeof(query),
             "SELECT exam_id, subject, exam_date, start_time, department "
             "FROM exams WHERE exam_date >= CURDATE() "
             "ORDER BY exam_date LIMIT 5");

    result = db_query(query);
    if (result != NULL) {
        int count = 0;
        const char *headers[] = {"ID", "Subject", "Date", "Time", "Department"};
        int widths[] = {4, 28, 12, 9, 30};
        draw_table_header(headers, widths, 5);

        while ((row = mysql_fetch_row(result)) != NULL) {
            const char *cells[] = {row[0], row[1], row[2], row[3], row[4]};
            draw_table_row(cells, widths, 5);
            count++;
        }

        if (count > 0) {
            printf("  ");
            print_line('-', 4 + 28 + 12 + 9 + 30 + 7);
        } else {
            printf("\n  No upcoming exams.\n");
        }
        mysql_free_result(result);
    }

    pause_program();
}

void report_student_list(void) {
    char query[MAX_QUERY];
    MYSQL_RES *result;
    MYSQL_ROW row;
    int count = 0;
    char filename[FILENAME_LEN];
    FILE *fp;

    print_header("STUDENT LIST REPORT");

    snprintf(query, sizeof(query),
             "SELECT student_id, roll_number, full_name, department, semester, year, phone, email "
             "FROM students ORDER BY roll_number");

    result = db_query(query);
    if (result == NULL) {
        set_console_color(COLOR_RED);
        printf("\n  No students found.\n");
        reset_console_color();
        pause_program();
        return;
    }

    printf("\n");
    {
        const char *headers[] = {"ID", "Roll No", "Name", "Department", "Sem", "Year", "Phone"};
        int widths[] = {4, 12, 28, 30, 4, 5, 12};
        draw_table_header(headers, widths, 7);
    }

    while ((row = mysql_fetch_row(result)) != NULL) {
        const char *cells[] = {row[0], row[1], row[2], row[3], row[4], row[5], row[6]};
        int widths[] = {4, 12, 28, 30, 4, 5, 12};
        draw_table_row(cells, widths, 7);
        count++;
    }

    mysql_free_result(result);

    if (count > 0) {
        printf("  ");
        print_line('-', 4 + 12 + 28 + 30 + 4 + 5 + 12 + 8);
        set_console_color(COLOR_YELLOW);
        printf("  Total: %d\n", count);
        reset_console_color();

        if (confirm_dialog("Export to CSV file")) {
            snprintf(filename, sizeof(filename), "exports/StudentList.csv");

            fp = fopen(filename, "w");
            if (fp != NULL) {
                fprintf(fp, "ID,Roll_Number,Name,Department,Semester,Year,Phone,Email\n");

                MYSQL_RES *res2 = db_query(
                    "SELECT student_id, roll_number, full_name, department, "
                    "semester, year, phone, email FROM students ORDER BY roll_number");

                if (res2 != NULL) {
                    MYSQL_ROW r2;
                    while ((r2 = mysql_fetch_row(res2)) != NULL) {
                        fprintf(fp, "%s,\"%s\",\"%s\",\"%s\",%s,%s,\"%s\",\"%s\"\n",
                                r2[0], r2[1], r2[2], r2[3], r2[4], r2[5], r2[6], r2[7]);
                    }
                    mysql_free_result(res2);
                }

                fclose(fp);

                set_console_color(COLOR_GREEN);
                printf("\n  Exported successfully to %s\n", filename);
                reset_console_color();
            }
        }
    } else {
        printf("\n  No students in database.\n");
    }

    pause_program();
}

void report_room_list(void) {
    view_classrooms();
    pause_program();
}

void report_department_list(void) {
    view_departments();
    pause_program();
}

void report_exam_schedule(void) {
    view_exam_schedule();
    pause_program();
}

void report_seating_arrangement(void) {
    view_seating_arrangement();
}

void report_room_occupancy(void) {
    char query[MAX_QUERY];
    MYSQL_RES *result;
    MYSQL_ROW row;
    int total_rooms = 0;
    int total_capacity = 0;
    int total_allocated = 0;
    int widths[] = {4, 12, 25, 6, 9, 10, 10, 8};

    print_header("ROOM OCCUPANCY REPORT");

    snprintf(query, sizeof(query),
             "SELECT r.room_id, r.room_name, r.building, r.floor, r.capacity, "
             "COALESCE(s.allocated, 0) as allocated "
             "FROM classrooms r "
             "LEFT JOIN ("
             "  SELECT room_id, COUNT(*) as allocated FROM seating "
             "  GROUP BY room_id"
             ") s ON r.room_id = s.room_id "
             "ORDER BY r.building, r.room_name");

    result = db_query(query);
    if (result == NULL) {
        set_console_color(COLOR_RED);
        printf("\n  No data available.\n");
        reset_console_color();
        pause_program();
        return;
    }

    printf("\n");
    {
        const char *headers[] = {"ID", "Room", "Building", "Floor", "Capacity", "Allocated", "Available", "Usage%"};
        draw_table_header(headers, widths, 8);
    }

    while ((row = mysql_fetch_row(result)) != NULL) {
        int capacity = atoi(row[4]);
        int allocated = atoi(row[5]);
        int available = capacity - allocated;
        float pct = capacity > 0 ? (float)allocated / capacity * 100.0f : 0.0f;
        char pct_str[10];
        char avail_str[10];
        char alloc_str[10];

        snprintf(alloc_str, sizeof(alloc_str), "%d", allocated);
        snprintf(avail_str, sizeof(avail_str), "%d", available);
        snprintf(pct_str, sizeof(pct_str), "%.1f%%", pct);

        const char *cells[] = {row[0], row[1], row[2], row[3], row[4],
                               alloc_str, avail_str, pct_str};
        draw_table_row(cells, widths, 8);

        total_capacity += capacity;
        total_allocated += allocated;
        total_rooms++;
    }

    mysql_free_result(result);

    printf("  ");
    print_line('-', 4 + 12 + 25 + 6 + 9 + 10 + 10 + 8 + 11);

    set_console_color(COLOR_YELLOW);
    printf("  Total Rooms: %d | Total Capacity: %d | Total Allocated: %d\n",
           total_rooms, total_capacity, total_allocated);
    reset_console_color();

    pause_program();
}

void report_attendance_sheet(void) {
    int exam_id;
    char query[MAX_QUERY];
    MYSQL_RES *result;
    MYSQL_ROW row;
    char subject[MAX_SUBJECT];
    char exam_date[DATE_LEN];
    int count = 0;
    int widths[] = {3, 12, 28, 12, 5, 5, 15};

    print_header("ATTENDANCE SHEET");

    view_exam_schedule();

    exam_id = get_valid_int("\n  Enter Exam ID: ");

    snprintf(query, sizeof(query),
             "SELECT subject, exam_date FROM exams WHERE exam_id = %d", exam_id);

    result = db_query(query);
    if (result == NULL || mysql_fetch_row(result) == NULL) {
        if (result) mysql_free_result(result);
        set_console_color(COLOR_RED);
        printf("\n  Exam not found!\n");
        reset_console_color();
        pause_program();
        return;
    }

    row = mysql_fetch_row(result);
    snprintf(subject, sizeof(subject), "%s", row[0]);
    snprintf(exam_date, sizeof(exam_date), "%s", row[1]);
    mysql_free_result(result);

    snprintf(query, sizeof(query),
             "SELECT st.roll_number, st.full_name, st.department, "
             "r.room_name, s.seat_number, s.row_number, s.column_number "
             "FROM seating s "
             "JOIN students st ON s.student_id = st.student_id "
             "JOIN classrooms r ON s.room_id = r.room_id "
             "WHERE s.exam_id = %d "
             "ORDER BY r.room_name, s.seat_number", exam_id);

    result = db_query(query);
    if (result == NULL) {
        set_console_color(COLOR_RED);
        printf("\n  No seating data. Allocate seats first.\n");
        reset_console_color();
        pause_program();
        return;
    }

    printf("\n");
    set_console_color(COLOR_CYAN);
    printf("  Subject: %s | Date: %s\n", subject, exam_date);
    printf("  ============================================\n");
    printf("  Attendance Sheet - Mark present (P) or absent (A)\n");
    reset_console_color();

    printf("\n");
    {
        const char *headers[] = {"#", "Roll No", "Name", "Room", "Seat", "Sign", "Remarks"};
        draw_table_header(headers, widths, 7);
    }

    while ((row = mysql_fetch_row(result)) != NULL) {
        char num[10];
        snprintf(num, sizeof(num), "%d", ++count);

        char sign[6] = "___";
        char remarks[16] = "";

        const char *cells[] = {num, row[0], row[1], row[3], row[4], sign, remarks};
        draw_table_row(cells, widths, 7);
    }

    mysql_free_result(result);

    if (count > 0) {
        printf("  ");
        print_line('-', 3 + 12 + 28 + 12 + 5 + 5 + 15 + 9);
        set_console_color(COLOR_YELLOW);
        printf("  Total Students: %d\n", count);
        reset_console_color();
    }

    pause_program();
}

void report_empty_seats(void) {
    char query[MAX_QUERY];
    MYSQL_RES *result;
    MYSQL_ROW row;
    int widths[] = {4, 12, 25, 9, 10, 6};

    print_header("EMPTY SEATS REPORT");

    snprintf(query, sizeof(query),
             "SELECT r.room_id, r.room_name, r.building, r.capacity, "
             "COALESCE(s.allocated, 0) as allocated "
             "FROM classrooms r "
             "LEFT JOIN ("
             "  SELECT room_id, COUNT(*) as allocated FROM seating "
             "  GROUP BY room_id"
             ") s ON r.room_id = s.room_id "
             "WHERE r.capacity > COALESCE(s.allocated, 0) "
             "ORDER BY (r.capacity - COALESCE(s.allocated, 0)) DESC");

    result = db_query(query);
    if (result == NULL) {
        set_console_color(COLOR_RED);
        printf("\n  No data available.\n");
        reset_console_color();
        pause_program();
        return;
    }

    {
        const char *headers[] = {"ID", "Room", "Building", "Capacity", "Allocated", "Empty"};
        draw_table_header(headers, widths, 6);
    }

    while ((row = mysql_fetch_row(result)) != NULL) {
        int capacity = atoi(row[3]);
        int allocated = atoi(row[4]);
        int empty = capacity - allocated;
        char empty_str[10];
        char alloc_str[10];

        snprintf(empty_str, sizeof(empty_str), "%d", empty);
        snprintf(alloc_str, sizeof(alloc_str), "%d", allocated);

        const char *cells[] = {row[0], row[1], row[2], row[3], alloc_str, empty_str};
        draw_table_row(cells, widths, 6);
    }

    mysql_free_result(result);

    pause_program();
}

void report_capacity_report(void) {
    char query[MAX_QUERY];
    MYSQL_RES *result;
    MYSQL_ROW row;
    int widths[] = {12, 25, 6, 9, 10, 10, 12};

    print_header("CAPACITY REPORT");

    snprintf(query, sizeof(query),
             "SELECT r.room_name, r.building, r.floor, r.capacity, "
             "COALESCE(s.allocated, 0) as allocated, "
             "(r.capacity - COALESCE(s.allocated, 0)) as available "
             "FROM classrooms r "
             "LEFT JOIN ("
             "  SELECT room_id, COUNT(*) as allocated FROM seating "
             "  GROUP BY room_id"
             ") s ON r.room_id = s.room_id "
             "ORDER BY r.building, r.floor");

    result = db_query(query);
    if (result == NULL) {
        set_console_color(COLOR_RED);
        printf("\n  No data available.\n");
        reset_console_color();
        pause_program();
        return;
    }

    {
        const char *headers[] = {"Room", "Building", "Floor", "Capacity", "Allocated", "Available", "Utilization"};
        draw_table_header(headers, widths, 7);
    }

    while ((row = mysql_fetch_row(result)) != NULL) {
        int capacity = atoi(row[3]);
        int allocated = atoi(row[4]);
        int available = atoi(row[5]);
        float pct = capacity > 0 ? (float)allocated / capacity * 100.0f : 0.0f;
        char pct_str[13];
        char avail_str[11];
        char alloc_str[11];

        snprintf(alloc_str, sizeof(alloc_str), "%d", allocated);
        snprintf(avail_str, sizeof(avail_str), "%d", available);
        snprintf(pct_str, sizeof(pct_str), "%.1f%%", pct);

        const char *cells[] = {row[0], row[1], row[2], row[3], alloc_str, avail_str, pct_str};
        draw_table_row(cells, widths, 7);
    }

    mysql_free_result(result);

    pause_program();
}
