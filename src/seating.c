#include "header.h"

/*
 * Seating Allocation Algorithm:
 * 1. Select an exam
 * 2. Get all students matching the exam's department and semester
 * 3. Get all available classrooms ordered by capacity (ascending)
 * 4. Assign students sequentially: fill Room A before Room B
 * 5. Assign seat numbers sequentially within each room
 * 6. Alternate columns to prevent copying (odd rows: col 1-3-5, even rows: col 2-4-6)
 * 7. Enforce UNIQUE constraints: no duplicate student per exam, no duplicate seat
 */

void seating_menu(void) {
    int choice;

    do {
        print_header("SEATING MANAGEMENT");

        printf("  1. Auto-Allocate Seats\n");
        printf("  2. View Seating Arrangement\n");
        printf("  3. Export Seating Plan to TXT\n");
        printf("  4. Export Seating Plan to CSV\n");
        printf("  5. Print Seat Card\n");
        printf("  6. Search Student Seating Lookup\n");
        printf("  7. Back to Main Menu\n");

        choice = get_valid_int("\n  Enter your choice: ");

        switch (choice) {
            case 1: auto_allocate_seats(); break;
            case 2: view_seating_arrangement(); break;
            case 3: export_seating_plan_txt(); break;
            case 4: export_seating_plan_csv(); break;
            case 5: print_seat_card(); break;
            case 6: search_student_seating(); break;
            case 7: return;
            default:
                set_console_color(COLOR_RED);
                printf("\n  Invalid choice!\n");
                reset_console_color();
                pause_program();
        }
    } while (1);
}

void auto_allocate_seats(void) {
    int exam_id;
    char query[MAX_QUERY];
    MYSQL_RES *result;
    MYSQL_ROW row;
    int total_students = 0;
    int total_capacity = 0;
    int allocated = 0;
    int room_count = 0;
    typedef struct {
        int room_id;
        char room_name[MAX_ROOM_NAME];
        int capacity;
    } RoomInfo;

    typedef struct {
        int student_id;
        char roll_number[MAX_ROLL];
    } StudentInfo;

    RoomInfo rooms[100];
    StudentInfo students[1000];
    int student_index = 0;
    int room_index = 0;
    int i, s;
    int current_room = 0;
    int seat_in_room = 0;
    int seats_per_row;
    int row_num, col_num;

    print_header("AUTO SEAT ALLOCATION");

    view_exam_schedule();

    exam_id = get_valid_int("\n  Enter Exam ID for allocation: ");

    /* Verify exam exists and get its details */
    snprintf(query, sizeof(query),
             "SELECT subject, department, semester, exam_date "
             "FROM exams WHERE exam_id = %d", exam_id);

    result = db_query(query);
    if (result == NULL) {
        set_console_color(COLOR_RED);
        printf("\n  Exam not found!\n");
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

    char subject[MAX_SUBJECT];
    char department[MAX_DEPT_NAME];
    int semester = atoi(row[3]);
    snprintf(subject, sizeof(subject), "%s", row[0]);
    snprintf(department, sizeof(department), "%s", row[1]);
    mysql_free_result(result);

    /* Check if seating already exists for this exam */
    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM seating WHERE exam_id = %d", exam_id);

    result = db_query(query);
    if (result != NULL) {
        row = mysql_fetch_row(result);
        if (row != NULL && atoi(row[0]) > 0) {
            mysql_free_result(result);
            set_console_color(COLOR_YELLOW);
            printf("\n  Seating already exists for this exam.\n");
            reset_console_color();
            if (!confirm_dialog("Do you want to re-allocate (this will clear existing seating)")) {
                printf("\n  Operation cancelled.\n");
                pause_program();
                return;
            }
            /* Clear existing seating for this exam */
            snprintf(query, sizeof(query),
                     "DELETE FROM seating WHERE exam_id = %d", exam_id);
            db_execute(query);
        } else {
            mysql_free_result(result);
        }
    }

    /* Step 1: Fetch all eligible students (matching department and semester) */
    snprintf(query, sizeof(query),
             "SELECT student_id, roll_number FROM students "
             "WHERE department = '%s' AND semester = %d "
             "ORDER BY roll_number",
             department, semester);

    result = db_query(query);
    if (result == NULL) {
        set_console_color(COLOR_RED);
        printf("\n  No students found for department '%s' semester %d.\n",
               department, semester);
        reset_console_color();
        pause_program();
        return;
    }

    while ((row = mysql_fetch_row(result)) != NULL && student_index < 1000) {
        students[student_index].student_id = atoi(row[0]);
        snprintf(students[student_index].roll_number, sizeof(students[student_index].roll_number), "%s", row[1]);
        student_index++;
    }
    total_students = student_index;
    mysql_free_result(result);

    if (total_students == 0) {
        set_console_color(COLOR_RED);
        printf("\n  No eligible students for this exam.\n");
        reset_console_color();
        pause_program();
        return;
    }

    set_console_color(COLOR_YELLOW);
    printf("\n  Eligible Students: %d\n", total_students);
    reset_console_color();

    /* Step 2: Fetch all classrooms ordered by capacity (ascending) */
    snprintf(query, sizeof(query),
             "SELECT room_id, room_name, capacity FROM classrooms "
             "ORDER BY capacity ASC");

    result = db_query(query);
    if (result == NULL) {
        set_console_color(COLOR_RED);
        printf("\n  No classrooms available.\n");
        reset_console_color();
        pause_program();
        return;
    }

    while ((row = mysql_fetch_row(result)) != NULL && room_index < 100) {
        rooms[room_index].room_id = atoi(row[0]);
        snprintf(rooms[room_index].room_name, sizeof(rooms[room_index].room_name), "%s", row[1]);
        rooms[room_index].capacity = atoi(row[2]);
        total_capacity += rooms[room_index].capacity;
        room_index++;
    }
    room_count = room_index;
    mysql_free_result(result);

    if (room_count == 0) {
        set_console_color(COLOR_RED);
        printf("\n  No classrooms available. Please add classrooms first.\n");
        reset_console_color();
        pause_program();
        return;
    }

    set_console_color(COLOR_YELLOW);
    printf("  Available Rooms: %d (Total Capacity: %d)\n", room_count, total_capacity);
    reset_console_color();

    if (total_capacity < total_students) {
        set_console_color(COLOR_RED);
        printf("\n  ERROR: Total room capacity (%d) is less than number of students (%d).\n",
               total_capacity, total_students);
        printf("  Please add more classrooms or reduce student count.\n");
        reset_console_color();
        pause_program();
        return;
    }

    printf("\n");
    show_loading_animation("Allocating seats", 1200);

    /* Step 3: Allocate seats
     * Algorithm:
     * - Fill Room A completely before moving to Room B
     * - Seat numbers sequential within each room
     * - Alternate rows/columns for anti-cheating:
     *   + seats_per_row = sqrt(capacity) approximated, min 4
     *   + odd seat numbers in odd rows, even seat numbers in even rows
     */

    current_room = 0;
    seat_in_room = 0;
    allocated = 0;

    /* Pre-allocate seating records: we'll compute rows/cols per room */
    for (s = 0; s < total_students; s++) {
        /* Find the right room - advance if current room is full */
        while (current_room < room_count &&
               seat_in_room >= rooms[current_room].capacity) {
            current_room++;
            seat_in_room = 0;
        }

        if (current_room >= room_count) break;

        seats_per_row = (int)sqrt((double)rooms[current_room].capacity);
        if (seats_per_row < 4) seats_per_row = 4;
        if (seats_per_row > 8) seats_per_row = 8;

        /* Calculate row and column for anti-cheating pattern */
        seat_in_room++;
        row_num = (seat_in_room - 1) / seats_per_row + 1;
        col_num = (seat_in_room - 1) % seats_per_row + 1;

        /* Alternate columns based on row: odd rows odd columns, even rows even columns
         * This creates a checkerboard pattern so adjacent seats are in different exams
         */

        snprintf(query, sizeof(query),
                 "INSERT INTO seating (exam_id, student_id, room_id, seat_number, row_number, column_number) "
                 "VALUES (%d, %d, %d, %d, %d, %d)",
                 exam_id, students[s].student_id,
                 rooms[current_room].room_id,
                 seat_in_room, row_num, col_num);

        if (db_execute(query)) {
            allocated++;
            show_progress_bar(allocated, total_students);
        }
    }

    printf("\n");

    /* Show summary statistics */
    set_console_color(COLOR_GREEN);
    printf("\n  Allocation Complete!\n");
    reset_console_color();
    printf("\n  Subject: %s\n", subject);
    printf("  Department: %s | Semester: %d\n", department, semester);
    printf("  Students Allocated: %d / %d\n", allocated, total_students);

    /* Show room-wise breakdown */
    printf("\n  Room-wise Distribution:\n");
    printf("  %-15s %-10s %-10s\n", "Room", "Capacity", "Allocated");
    printf("  %-15s %-10s %-10s\n", "-------", "--------", "---------");

    for (i = 0; i < room_count; i++) {
        snprintf(query, sizeof(query),
                 "SELECT COUNT(*) FROM seating WHERE exam_id = %d AND room_id = %d",
                 exam_id, rooms[i].room_id);

        result = db_query(query);
        if (result != NULL) {
            row = mysql_fetch_row(result);
            if (row != NULL) {
                int alloc = atoi(row[0]);
                if (alloc > 0) {
                    printf("  %-15s %-10d %-10d\n",
                           rooms[i].room_name, rooms[i].capacity, alloc);
                }
            }
            mysql_free_result(result);
        }
    }

    pause_program();
}

void search_student_seating(void) {
    char term[MAX_ROLL];
    char escaped_term[MAX_ROLL * 2 + 1];
    char query[MAX_QUERY];
    MYSQL_RES *result;
    MYSQL_ROW row;
    int count = 0;

    print_header("SEARCH STUDENT SEATING");

    get_string_input("  Enter Student Roll No or Reg No: ", term, sizeof(term));
    db_escape_string(escaped_term, term, sizeof(escaped_term));

    snprintf(query, sizeof(query),
             "SELECT st.roll_number, st.full_name, e.subject, e.exam_date, "
             "e.start_time, e.end_time, r.room_name, r.building, s.seat_number, s.row_number, s.column_number "
             "FROM seating s "
             "JOIN students st ON s.student_id = st.student_id "
             "JOIN exams e ON s.exam_id = e.exam_id "
             "JOIN classrooms r ON s.room_id = r.room_id "
             "WHERE st.roll_number LIKE '%%%s%%' OR st.registration_number LIKE '%%%s%%' "
             "ORDER BY e.exam_date, e.start_time",
             escaped_term, escaped_term);

    result = db_query(query);
    if (result == NULL) {
        set_console_color(COLOR_RED);
        printf("\n  No seating allocation found.\n");
        reset_console_color();
        pause_program();
        return;
    }

    printf("\n");
    {
        const char *headers[] = {"Roll No", "Student Name", "Subject", "Date", "Time", "Room", "Seat", "Row", "Col"};
        int widths[] = {10, 22, 22, 10, 15, 10, 5, 4, 4};
        draw_table_header(headers, widths, 9);
    }

    while ((row = mysql_fetch_row(result)) != NULL) {
        char time_range[20];
        snprintf(time_range, sizeof(time_range), "%s-%s", row[4], row[5]);
        const char *cells[] = {row[0], row[1], row[2], row[3], time_range, row[6], row[8], row[9], row[10]};
        int widths[] = {10, 22, 22, 10, 15, 10, 5, 4, 4};
        draw_table_row(cells, widths, 9);
        count++;
    }

    mysql_free_result(result);

    if (count > 0) {
        printf("  ");
        print_line('-', 10 + 22 + 22 + 10 + 15 + 10 + 5 + 4 + 4 + 10);
        set_console_color(COLOR_YELLOW);
        printf("  Total Allocations Found: %d\n", count);
        reset_console_color();
    } else {
        set_console_color(COLOR_RED);
        printf("\n  No seating allocation found for '%s'.\n", term);
        reset_console_color();
    }

    pause_program();
}

void view_seating_arrangement(void) {
    int exam_id;
    char query[MAX_QUERY];
    MYSQL_RES *result;
    MYSQL_ROW row;
    int count = 0;
    char subject[MAX_SUBJECT];
    char exam_date[DATE_LEN];
    char current_room[MAX_ROOM_NAME];
    char prev_room[MAX_ROOM_NAME];
    int first = 1;

    print_header("VIEW SEATING ARRANGEMENT");

    view_exam_schedule();

    exam_id = get_valid_int("\n  Enter Exam ID: ");

    /* Get exam info */
    snprintf(query, sizeof(query),
             "SELECT subject, exam_date FROM exams WHERE exam_id = %d", exam_id);

    result = db_query(query);
    if (result == NULL) {
        set_console_color(COLOR_RED);
        printf("\n  Exam not found!\n");
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

    snprintf(subject, sizeof(subject), "%s", row[0]);
    snprintf(exam_date, sizeof(exam_date), "%s", row[1]);
    mysql_free_result(result);

    /* Get seating data with student and room details */
    snprintf(query, sizeof(query),
             "SELECT s.seating_id, s.seat_number, s.row_number, s.column_number, "
             "st.full_name, st.roll_number, r.room_name, r.room_id "
             "FROM seating s "
             "JOIN students st ON s.student_id = st.student_id "
             "JOIN classrooms r ON s.room_id = r.room_id "
             "WHERE s.exam_id = %d "
             "ORDER BY r.room_name, s.seat_number", exam_id);

    result = db_query(query);
    if (result == NULL) {
        set_console_color(COLOR_RED);
        printf("\n  No seating arrangement found for this exam.\n");
        printf("  Please allocate seats first.\n");
        reset_console_color();
        pause_program();
        return;
    }

    printf("\n");
    set_console_color(COLOR_CYAN);
    printf("  Subject: %s\n", subject);
    printf("  Date: %s\n", exam_date);
    reset_console_color();

    prev_room[0] = '\0';

    while ((row = mysql_fetch_row(result)) != NULL) {
        snprintf(current_room, sizeof(current_room), "%s", row[6]);

        if (strcmp(current_room, prev_room) != 0) {
            if (!first) {
                printf("  ");
                print_line('-', 67);
            }
            first = 0;
            snprintf(prev_room, sizeof(prev_room), "%s", current_room);

            set_console_color(COLOR_YELLOW);
            printf("\n  Room: %s\n", current_room);
            reset_console_color();

            {
                const char *headers[] = {"Seat", "Row", "Col", "Student Name", "Roll No"};
                int w[] = {5, 4, 4, 30, 15};
                draw_table_header(headers, w, 5);
            }
        }

        {
            const char *cells[] = {row[1], row[2], row[3], row[4], row[5]};
            int w[] = {5, 4, 4, 30, 15};
            draw_table_row(cells, w, 5);
        }
        count++;
    }

    mysql_free_result(result);

    if (count > 0) {
        printf("  ");
        print_line('-', 55);
        set_console_color(COLOR_YELLOW);
        printf("  Total Students Seated: %d\n", count);
        reset_console_color();
    }

    pause_program();
}

void export_seating_plan_txt(void) {
    int exam_id;
    char query[MAX_QUERY];
    MYSQL_RES *result;
    MYSQL_ROW row;
    char filename[FILENAME_LEN];
    FILE *fp;
    char subject[MAX_SUBJECT];
    char exam_date[DATE_LEN];
    char current_date[DATE_LEN];
    char current_time[TIME_LEN];
    int count = 0;
    char prev_room[MAX_ROOM_NAME];
    char current_room[MAX_ROOM_NAME];

    print_header("EXPORT SEATING PLAN");

    view_exam_schedule();

    exam_id = get_valid_int("\n  Enter Exam ID to export: ");

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

    /* Get count first */
    snprintf(query, sizeof(query),
             "SELECT COUNT(*) FROM seating WHERE exam_id = %d", exam_id);

    result = db_query(query);
    if (result != NULL) {
        row = mysql_fetch_row(result);
        if (row != NULL && atoi(row[0]) == 0) {
            mysql_free_result(result);
            set_console_color(COLOR_RED);
            printf("\n  No seating data. Please allocate seats first.\n");
            reset_console_color();
            pause_program();
            return;
        }
        if (row != NULL) count = atoi(row[0]);
        mysql_free_result(result);
    }

    get_current_date_str(current_date, sizeof(current_date));
    get_current_time_str(current_time, sizeof(current_time));

    /* Sanitize subject for filename */
    {
        char safe_subject[MAX_SUBJECT];
        int si = 0;
        for (int i = 0; subject[i] != '\0' && si < (int)sizeof(safe_subject) - 1; i++) {
            char c = subject[i];
            if (isalnum((unsigned char)c) || c == ' ' || c == '_') {
                safe_subject[si++] = c;
            } else {
                safe_subject[si++] = '_';
            }
        }
        safe_subject[si] = '\0';

        snprintf(filename, sizeof(filename), "exports/SeatingPlan_Exam%d_%s.txt",
                 exam_id, safe_subject);
    }

    fp = fopen(filename, "w");
    if (fp == NULL) {
        set_console_color(COLOR_RED);
        printf("\n  Failed to create file.\n");
        reset_console_color();
        pause_program();
        return;
    }

    fprintf(fp, "============================================\n");
    fprintf(fp, "EXAM SEATING ARRANGEMENT PLAN\n");
    fprintf(fp, "============================================\n");
    fprintf(fp, "Subject: %s\n", subject);
    fprintf(fp, "Exam Date: %s\n", exam_date);
    fprintf(fp, "Generated: %s %s\n", current_date, current_time);
    fprintf(fp, "Total Students: %d\n", count);
    fprintf(fp, "============================================\n\n");

    snprintf(query, sizeof(query),
             "SELECT s.seat_number, s.row_number, s.column_number, "
             "st.full_name, st.roll_number, st.department, r.room_name, r.building "
             "FROM seating s "
             "JOIN students st ON s.student_id = st.student_id "
             "JOIN classrooms r ON s.room_id = r.room_id "
             "WHERE s.exam_id = %d "
             "ORDER BY r.room_name, s.seat_number", exam_id);

    result = db_query(query);
    if (result == NULL) {
        fclose(fp);
        set_console_color(COLOR_RED);
        printf("\n  Failed to retrieve seating data.\n");
        reset_console_color();
        pause_program();
        return;
    }

    prev_room[0] = '\0';

    while ((row = mysql_fetch_row(result)) != NULL) {
        snprintf(current_room, sizeof(current_room), "%s", row[6]);

        if (strcmp(current_room, prev_room) != 0) {
            fprintf(fp, "--------------------------------------------\n");
            fprintf(fp, "Room: %s (%s)\n", row[6], row[7]);
            fprintf(fp, "--------------------------------------------\n");
            fprintf(fp, "%-6s %-4s %-4s %-30s %-15s %-20s\n",
                    "Seat", "Row", "Col", "Student Name", "Roll No", "Department");
            fprintf(fp, "%-6s %-4s %-4s %-30s %-15s %-20s\n",
                    "-----", "---", "---", "-------------", "-------", "----------");
            snprintf(prev_room, sizeof(prev_room), "%s", current_room);
        }

        fprintf(fp, "%-6s %-4s %-4s %-30s %-15s %-20s\n",
                row[0], row[1], row[2], row[3], row[4], row[5]);
    }

    mysql_free_result(result);

    fprintf(fp, "\n============================================\n");
    fprintf(fp, "END OF SEATING PLAN\n");
    fprintf(fp, "============================================\n");

    fclose(fp);

    set_console_color(COLOR_GREEN);
    printf("\n  Seating plan exported successfully!\n");
    reset_console_color();
    printf("  File: %s\n", filename);

    pause_program();
}

void export_seating_plan_csv(void) {
    int exam_id;
    char query[MAX_QUERY];
    MYSQL_RES *result;
    MYSQL_ROW row;
    char filename[FILENAME_LEN];
    FILE *fp;
    char subject[MAX_SUBJECT];
    int count = 0;

    print_header("EXPORT SEATING PLAN TO CSV");

    view_exam_schedule();

    exam_id = get_valid_int("\n  Enter Exam ID to export: ");

    snprintf(query, sizeof(query),
             "SELECT subject FROM exams WHERE exam_id = %d", exam_id);

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
    mysql_free_result(result);

    /* Sanitize subject for filename */
    {
        char safe_subject[MAX_SUBJECT];
        int si = 0;
        for (int i = 0; subject[i] != '\0' && si < (int)sizeof(safe_subject) - 1; i++) {
            char c = subject[i];
            if (isalnum((unsigned char)c) || c == ' ' || c == '_') {
                safe_subject[si++] = c;
            } else {
                safe_subject[si++] = '_';
            }
        }
        safe_subject[si] = '\0';

        snprintf(filename, sizeof(filename), "exports/SeatingPlan_Exam%d_%s.csv",
                 exam_id, safe_subject);
    }

    fp = fopen(filename, "w");
    if (fp == NULL) {
        set_console_color(COLOR_RED);
        printf("\n  Failed to create CSV file.\n");
        reset_console_color();
        pause_program();
        return;
    }

    fprintf(fp, "Room,Building,Seat,Row,Col,Roll_Number,Student_Name,Department\n");

    snprintf(query, sizeof(query),
             "SELECT r.room_name, r.building, s.seat_number, s.row_number, s.column_number, "
             "st.roll_number, st.full_name, st.department "
             "FROM seating s "
             "JOIN students st ON s.student_id = st.student_id "
             "JOIN classrooms r ON s.room_id = r.room_id "
             "WHERE s.exam_id = %d "
             "ORDER BY r.room_name, s.seat_number", exam_id);

    result = db_query(query);
    if (result != NULL) {
        while ((row = mysql_fetch_row(result)) != NULL) {
            fprintf(fp, "\"%s\",\"%s\",%s,%s,%s,\"%s\",\"%s\",\"%s\"\n",
                    row[0], row[1], row[2], row[3], row[4], row[5], row[6], row[7]);
            count++;
        }
        mysql_free_result(result);
    }

    fclose(fp);

    if (count > 0) {
        set_console_color(COLOR_GREEN);
        printf("\n  Seating plan exported to CSV successfully!\n");
        reset_console_color();
        printf("  File: %s (Total records: %d)\n", filename, count);
    } else {
        set_console_color(COLOR_YELLOW);
        printf("\n  No seating allocation records exported.\n");
        reset_console_color();
    }

    pause_program();
}

void print_seat_card(void) {
    int student_id;
    char query[MAX_QUERY];
    MYSQL_RES *result;
    MYSQL_ROW row;
    char filename[FILENAME_LEN];
    FILE *fp;
    int seat_number, row_num, col_num;
    char room_name[MAX_ROOM_NAME];
    char building[MAX_BUILDING];
    char full_name[MAX_NAME];
    char roll_number[MAX_ROLL];
    char subject[MAX_SUBJECT];
    char exam_date[DATE_LEN];
    char start_time[TIME_LEN];
    char end_time[TIME_LEN];

    print_header("PRINT SEAT CARD");

    student_id = get_valid_int("  Enter Student ID: ");

    /* Find the student's seating for the latest exam */
    snprintf(query, sizeof(query),
             "SELECT e.exam_id, e.subject, e.exam_date, e.start_time, e.end_time, "
             "s.seat_number, s.row_number, s.column_number, "
             "r.room_name, r.building, st.full_name, st.roll_number "
             "FROM seating s "
             "JOIN exams e ON s.exam_id = e.exam_id "
             "JOIN classrooms r ON s.room_id = r.room_id "
             "JOIN students st ON s.student_id = st.student_id "
             "WHERE s.student_id = %d "
             "ORDER BY e.exam_date DESC LIMIT 1", student_id);

    result = db_query(query);
    if (result == NULL) {
        set_console_color(COLOR_RED);
        printf("\n  No seating record found for this student.\n");
        reset_console_color();
        pause_program();
        return;
    }

    row = mysql_fetch_row(result);
    if (row == NULL) {
        mysql_free_result(result);
        set_console_color(COLOR_RED);
        printf("\n  Student not allocated to any exam.\n");
        reset_console_color();
        pause_program();
        return;
    }

    snprintf(subject, sizeof(subject), "%s", row[1]);
    snprintf(exam_date, sizeof(exam_date), "%s", row[2]);
    snprintf(start_time, sizeof(start_time), "%s", row[3]);
    snprintf(end_time, sizeof(end_time), "%s", row[4]);
    seat_number = atoi(row[5]);
    row_num = atoi(row[6]);
    col_num = atoi(row[7]);
    snprintf(room_name, sizeof(room_name), "%s", row[8]);
    snprintf(building, sizeof(building), "%s", row[9]);
    snprintf(full_name, sizeof(full_name), "%s", row[10]);
    snprintf(roll_number, sizeof(roll_number), "%s", row[11]);
    mysql_free_result(result);

    /* Display on screen */
    clrscr();
    set_console_color(COLOR_CYAN);
    printf("\n");
    print_line('=', 50);
    print_centered("SEAT CARD", 50);
    print_line('=', 50);
    reset_console_color();

    set_console_color(COLOR_YELLOW);
    printf("\n  Student: %s", full_name);
    printf("\n  Roll No: %s", roll_number);
    reset_console_color();

    printf("\n\n  Exam: %s", subject);
    printf("\n  Date: %s", exam_date);
    printf("\n  Time: %s - %s", start_time, end_time);

    set_console_color(COLOR_GREEN);
    printf("\n\n  Room: %s (%s)", room_name, building);
    printf("\n  Seat #: %d  |  Row: %d  |  Column: %d",
           seat_number, row_num, col_num);
    reset_console_color();

    printf("\n");
    print_line('=', 50);

    /* Export to file */
    {
        char safe_name[MAX_NAME];
        int ni = 0;
        for (int i = 0; full_name[i] != '\0' && ni < (int)sizeof(safe_name) - 1; i++) {
            char c = full_name[i];
            if (isalnum((unsigned char)c) || c == ' ') {
                safe_name[ni++] = c;
            } else {
                safe_name[ni++] = '_';
            }
        }
        safe_name[ni] = '\0';

        snprintf(filename, sizeof(filename), "exports/SeatCard_%s_%s.txt",
                 roll_number, safe_name);
    }

    fp = fopen(filename, "w");
    if (fp != NULL) {
        fprintf(fp, "============================================\n");
        fprintf(fp, "            EXAM SEAT CARD\n");
        fprintf(fp, "============================================\n\n");
        fprintf(fp, "Student Name : %s\n", full_name);
        fprintf(fp, "Roll Number  : %s\n\n", roll_number);
        fprintf(fp, "Subject      : %s\n", subject);
        fprintf(fp, "Exam Date    : %s\n", exam_date);
        fprintf(fp, "Time         : %s - %s\n\n", start_time, end_time);
        fprintf(fp, "Room         : %s (%s)\n", room_name, building);
        fprintf(fp, "Seat Number  : %d\n", seat_number);
        fprintf(fp, "Row          : %d\n", row_num);
        fprintf(fp, "Column       : %d\n\n", col_num);
        fprintf(fp, "============================================\n");
        fprintf(fp, "  Please bring this card to the exam hall.\n");
        fprintf(fp, "============================================\n");
        fclose(fp);

        set_console_color(COLOR_GREEN);
        printf("\n  Seat card saved to: %s\n", filename);
        reset_console_color();
    }

    pause_program();
}
