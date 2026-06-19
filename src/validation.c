#include "header.h"

int is_valid_name(const char *name) {
    size_t len;
    size_t i;

    if (name == NULL) return 0;
    len = strlen(name);
    if (len < 2 || len > 99) return 0;

    for (i = 0; i < len; i++) {
        char ch = name[i];
        if (!(isalpha((unsigned char)ch) || ch == ' ' || ch == '.' ||
              ch == '-' || ch == '\'')) {
            return 0;
        }
    }
    return 1;
}

int is_valid_phone(const char *phone) {
    size_t len;
    size_t i;

    if (phone == NULL) return 0;
    len = strlen(phone);
    if (len != 10) return 0;

    for (i = 0; i < len; i++) {
        if (!isdigit((unsigned char)phone[i])) return 0;
    }
    return 1;
}

int is_valid_email(const char *email) {
    const char *at;
    const char *dot;

    if (email == NULL) return 0;
    if (strlen(email) < 5 || strlen(email) > 99) return 0;

    at = strchr(email, '@');
    if (at == NULL) return 0;
    if (at == email) return 0;

    dot = strrchr(at, '.');
    if (dot == NULL) return 0;
    if (*(dot + 1) == '\0') return 0;
    if (dot - at < 2) return 0;

    return 1;
}

int is_valid_roll_number(const char *roll) {
    size_t len;
    size_t i;

    if (roll == NULL) return 0;
    len = strlen(roll);
    if (len < 2 || len > 29) return 0;

    for (i = 0; i < len; i++) {
        char ch = roll[i];
        if (!(isalnum((unsigned char)ch) || ch == '-' || ch == '/')) {
            return 0;
        }
    }
    return 1;
}

int is_valid_registration(const char *reg) {
    size_t len;
    size_t i;

    if (reg == NULL) return 0;
    len = strlen(reg);
    if (len < 5 || len > 29) return 0;

    for (i = 0; i < len; i++) {
        char ch = reg[i];
        if (!(isalnum((unsigned char)ch) || ch == '-' || ch == '/')) {
            return 0;
        }
    }
    return 1;
}

int is_valid_room_name(const char *name) {
    size_t len;
    size_t i;

    if (name == NULL) return 0;
    len = strlen(name);
    if (len < 1 || len > 49) return 0;

    for (i = 0; i < len; i++) {
        char ch = name[i];
        if (!(isalnum((unsigned char)ch) || ch == ' ' || ch == '-' || ch == '/')) {
            return 0;
        }
    }
    return 1;
}

int is_valid_subject(const char *subject) {
    size_t len;
    size_t i;

    if (subject == NULL) return 0;
    len = strlen(subject);
    if (len < 2 || len > 99) return 0;

    for (i = 0; i < len; i++) {
        char ch = subject[i];
        if (!(isalpha((unsigned char)ch) || ch == ' ' || ch == '-' ||
              ch == '&' || ch == '/' || ch == '(' || ch == ')' ||
              isdigit((unsigned char)ch))) {
            return 0;
        }
    }
    return 1;
}

int is_valid_date_str(const char *date) {
    int year, month, day;
    int days_in_month[] = {31,28,31,30,31,30,31,31,30,31,30,31};

    if (date == NULL) return 0;
    if (strlen(date) != 10) return 0;
    if (date[4] != '-' || date[7] != '-') return 0;

    if (sscanf(date, "%d-%d-%d", &year, &month, &day) != 3) return 0;
    if (month < 1 || month > 12) return 0;
    if (year < 2020 || year > 2035) return 0;

    if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))
        days_in_month[1] = 29;

    if (day < 1 || day > days_in_month[month - 1]) return 0;

    return 1;
}

int is_valid_time_str(const char *time) {
    int hour, minute, second;

    if (time == NULL) return 0;
    if (strlen(time) != 8) return 0;
    if (time[2] != ':' || time[5] != ':') return 0;

    if (sscanf(time, "%d:%d:%d", &hour, &minute, &second) != 3) return 0;
    if (hour < 0 || hour > 23) return 0;
    if (minute < 0 || minute > 59) return 0;
    if (second < 0 || second > 59) return 0;

    return 1;
}

int is_positive_int(const char *str) {
    size_t i;
    if (str == NULL || *str == '\0') return 0;

    for (i = 0; str[i] != '\0'; i++) {
        if (!isdigit((unsigned char)str[i])) return 0;
    }
    return 1;
}

int is_within_range(int val, int min, int max) {
    return (val >= min && val <= max);
}
