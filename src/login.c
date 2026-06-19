#include "header.h"

int login_menu(void) {
    char username[MAX_NAME];
    char password[128];
    char escaped_username[MAX_NAME * 2 + 1];
    char escaped_password[257];
    char query[MAX_QUERY];
    MYSQL_RES *result;
    MYSQL_ROW row;
    int attempts = 0;

    print_header("ADMIN LOGIN");

    while (attempts < MAX_ATTEMPTS) {
        printf("\n  Attempt %d of %d\n", attempts + 1, MAX_ATTEMPTS);

        get_string_input("  Username: ", username, sizeof(username));
        printf("  Password: ");
        get_hidden_password(password, sizeof(password));

        db_escape_string(escaped_username, username, sizeof(escaped_username));
        db_escape_string(escaped_password, password, sizeof(escaped_password));

        snprintf(query, sizeof(query),
                 "SELECT admin_id, username FROM admin "
                 "WHERE username = '%s' AND password = '%s'",
                 escaped_username, escaped_password);

        result = db_query(query);
        if (result != NULL) {
            row = mysql_fetch_row(result);
            if (row != NULL) {
                g_admin_id = atoi(row[0]);
                snprintf(g_username, sizeof(g_username), "%s", row[1]);
                mysql_free_result(result);

                set_console_color(COLOR_GREEN);
                printf("\n  Login successful! Welcome, %s.\n", g_username);
                reset_console_color();
                show_loading_animation("Loading Dashboard", 800);
                return 1;
            }
            mysql_free_result(result);
        }

        attempts++;
        set_console_color(COLOR_RED);
        printf("\n  Invalid username or password. (%d/%d attempts)\n",
               attempts, MAX_ATTEMPTS);
        reset_console_color();

        if (attempts < MAX_ATTEMPTS) {
            set_console_color(COLOR_YELLOW);
            printf("\n  [F]orgot Password? ");
            reset_console_color();

            char ch = (char)toupper((unsigned char)_getch());
            printf("%c\n", ch);
            if (ch == 'F') {
                forgot_password();
                return 0;
            }
        }
    }

    set_console_color(COLOR_RED);
    printf("\n  Too many failed attempts. System locked.\n");
    reset_console_color();
    return 0;
}

void forgot_password(void) {
    char username[MAX_NAME];
    char escaped_username[MAX_NAME * 2 + 1];
    char query[MAX_QUERY];
    MYSQL_RES *result;
    MYSQL_ROW row;

    print_header("FORGOT PASSWORD");

    get_string_input("  Enter your username: ", username, sizeof(username));
    db_escape_string(escaped_username, username, sizeof(escaped_username));

    snprintf(query, sizeof(query),
             "SELECT password FROM admin WHERE username = '%s'", escaped_username);

    result = db_query(query);
    if (result != NULL) {
        row = mysql_fetch_row(result);
        if (row != NULL) {
            set_console_color(COLOR_GREEN);
            printf("\n  Your password is: %s\n", row[0]);
            reset_console_color();
        } else {
            set_console_color(COLOR_RED);
            printf("\n  Username not found.\n");
            reset_console_color();
        }
        mysql_free_result(result);
    } else {
        set_console_color(COLOR_RED);
        printf("\n  Database error.\n");
        reset_console_color();
    }

    printf("\n  Press any key to continue...");
    _getch();
}

int change_password(void) {
    char old_password[128];
    char new_password[128];
    char confirm_password[128];
    char escaped_old_password[257];
    char escaped_new_password[257];
    char query[MAX_QUERY];
    MYSQL_RES *result;
    MYSQL_ROW row;

    print_header("CHANGE PASSWORD");

    printf("  Enter current password: ");
    get_hidden_password(old_password, sizeof(old_password));

    db_escape_string(escaped_old_password, old_password, sizeof(escaped_old_password));

    snprintf(query, sizeof(query),
             "SELECT admin_id FROM admin WHERE admin_id = %d AND password = '%s'",
             g_admin_id, escaped_old_password);

    result = db_query(query);
    if (result == NULL) {
        set_console_color(COLOR_RED);
        printf("\n  Database error.\n");
        reset_console_color();
        return 0;
    }

    row = mysql_fetch_row(result);
    if (row == NULL) {
        mysql_free_result(result);
        set_console_color(COLOR_RED);
        printf("\n  Current password is incorrect.\n");
        reset_console_color();
        pause_program();
        return 0;
    }
    mysql_free_result(result);

    printf("  Enter new password: ");
    get_hidden_password(new_password, sizeof(new_password));

    printf("  Confirm new password: ");
    get_hidden_password(confirm_password, sizeof(confirm_password));

    if (strcmp(new_password, confirm_password) != 0) {
        set_console_color(COLOR_RED);
        printf("\n  Passwords do not match.\n");
        reset_console_color();
        pause_program();
        return 0;
    }

    if (strlen(new_password) < 4) {
        set_console_color(COLOR_RED);
        printf("\n  Password must be at least 4 characters.\n");
        reset_console_color();
        pause_program();
        return 0;
    }

    db_escape_string(escaped_new_password, new_password, sizeof(escaped_new_password));

    snprintf(query, sizeof(query),
             "UPDATE admin SET password = '%s' WHERE admin_id = %d",
             escaped_new_password, g_admin_id);

    if (db_execute(query)) {
        set_console_color(COLOR_GREEN);
        printf("\n  Password changed successfully!\n");
        reset_console_color();
    } else {
        set_console_color(COLOR_RED);
        printf("\n  Failed to change password.\n");
        reset_console_color();
    }

    pause_program();
    return 1;
}
