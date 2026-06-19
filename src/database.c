#include "header.h"

#if USE_MYSQL_STUB
/* Stub implementations for testing without MySQL */

int db_connect(void) {
    set_console_color(COLOR_GREEN);
    printf("\n  [STUB] Database connected successfully (Mock Mode)");
    reset_console_color();
    return 1;
}

void db_disconnect(void) {
    printf("\n  [STUB] Database disconnected.");
}

int db_execute(const char *query) {
    printf("\n  [STUB] Executed: %s", query);
    return 1;
}

MYSQL_RES *db_query(const char *query) {
    printf("\n  [STUB] Query: %s", query);
    return NULL;
}

int db_escape_string(char *to, const char *from, size_t len) {
    if (len > 0) {
        strncpy(to, from, len - 1);
        to[len - 1] = '\0';
    }
    return (int)strlen(to);
}

#else
/* Real MySQL connector implementation */

int db_connect(void) {
    g_conn = mysql_init(NULL);
    if (g_conn == NULL) {
        set_console_color(COLOR_RED);
        printf("\n  ERROR: mysql_init() failed. %s", mysql_error(g_conn));
        reset_console_color();
        return 0;
    }

    if (mysql_real_connect(g_conn, DB_HOST, DB_USER, DB_PASS,
                           DB_NAME, DB_PORT, NULL, 0) == NULL) {
        set_console_color(COLOR_RED);
        printf("\n  ERROR: Connection failed. %s", mysql_error(g_conn));
        reset_console_color();
        mysql_close(g_conn);
        g_conn = NULL;
        return 0;
    }

    mysql_set_character_set(g_conn, "utf8mb4");

    set_console_color(COLOR_GREEN);
    printf("\n  Database connected successfully!");
    reset_console_color();
    return 1;
}

void db_disconnect(void) {
    if (g_conn != NULL) {
        mysql_close(g_conn);
        g_conn = NULL;
        printf("\n  Database disconnected.");
    }
}

int db_execute(const char *query) {
    if (g_conn == NULL) return 0;
    if (mysql_query(g_conn, query) != 0) {
        set_console_color(COLOR_RED);
        fprintf(stderr, "\n  SQL Error: %s", mysql_error(g_conn));
        reset_console_color();
        return 0;
    }
    return 1;
}

MYSQL_RES *db_query(const char *query) {
    if (g_conn == NULL) return NULL;
    if (mysql_query(g_conn, query) != 0) {
        set_console_color(COLOR_RED);
        fprintf(stderr, "\n  SQL Error: %s", mysql_error(g_conn));
        reset_console_color();
        return NULL;
    }
    return mysql_store_result(g_conn);
}

int db_escape_string(char *to, const char *from, size_t len) {
    if (g_conn == NULL) {
        if (len > 0) {
            strncpy(to, from, len - 1);
            to[len - 1] = '\0';
        }
        return (int)strlen(to);
    }
    return (int)mysql_real_escape_string(g_conn, to, from,
                                         (unsigned long)strlen(from));
}

#endif /* USE_MYSQL_STUB */
