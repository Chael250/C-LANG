#include "main.h"
#include <ctype.h>
#include <regex.h>

void log_action(const char *action, const char *details) {
    FILE *log_file = fopen("contact_book.log", "a");
    if (log_file == NULL) {
        return; // Silent fail for logging
    }
    
    time_t now = time(NULL);
    struct tm *local_time = localtime(&now);
    
    fprintf(log_file, "[%04d-%02d-%02d %02d:%02d:%02d] [%s] %s\n",
            local_time->tm_year + 1900, local_time->tm_mon + 1, local_time->tm_mday,
            local_time->tm_hour, local_time->tm_min, local_time->tm_sec,
            action, details);
    
    fclose(log_file);
}

void handle_error(const char *error_msg) {
    fprintf(stderr, "Error: %s\n", error_msg);
    
    // Log the error
    char log_msg[512];
    snprintf(log_msg, sizeof(log_msg), "ERROR: %s", error_msg);
    log_action("SYSTEM", log_msg);
    
    // If it's a MySQL error, also log that
    if (db_connection) {
        const char *mysql_error = mysql_error(db_connection);
        if (strlen(mysql_error) > 0) {
            fprintf(stderr, "MySQL Error: %s\n", mysql_error);
            snprintf(log_msg, sizeof(log_msg), "MySQL ERROR: %s", mysql_error);
            log_action("SYSTEM", log_msg);
        }
    }
}

void clear_screen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void print_contact(const Contact *contact) {
    printf("ID: %d\n", contact->id);
    printf("Name: %s\n", contact->name);
    printf("Phone: %s\n", contact->phone);
    printf("Email: %s\n", contact->email);
    printf("-------------------\n");
}

void print_contact_list(const ContactList *list) {
    if (list->count == 0) {
        printf("No contacts found.\n");
        return;
    }
    
    printf("\n%-5s %-20s %-15s %-30s\n", "ID", "Name", "Phone", "Email");
    printf("%-5s %-20s %-15s %-30s\n", "-----", "--------------------", "---------------", "------------------------------");
    
    for (int i = 0; i < list->count; i++) {
        printf("%-5d %-20s %-15s %-30s\n",
               list->contacts[i].id,
               list->contacts[i].name,
               list->contacts[i].phone,
               list->contacts[i].email);
    }
}

void free_contact_list(ContactList *list) {
    if (list->contacts) {
        free(list->contacts);
        list->contacts = NULL;
        list->count = 0;
    }
}

int get_user_input(char *buffer, int max_length) {
    if (fgets(buffer, max_length, stdin) == NULL) {
        return -1;
    }
    
    // Remove newline character
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    }
    
    return 0;
}

int validate_email(const char *email) {
    if (email == NULL || strlen(email) == 0) {
        return 0;
    }
    
    // Basic email validation using regex
    regex_t regex;
    int ret;
    
    const char *pattern = "^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$";
    
    ret = regcomp(&regex, pattern, REG_EXTENDED);
    if (ret) {
        return 0;
    }
    
    ret = regexec(&regex, email, 0, NULL, 0);
    regfree(&regex);
    
    return ret == 0;
}

int validate_phone(const char *phone) {
    if (phone == NULL || strlen(phone) == 0) {
        return 0;
    }
    
    // Check length (minimum 10 digits)
    int digit_count = 0;
    for (int i = 0; phone[i] != '\0'; i++) {
        if (isdigit(phone[i])) {
            digit_count++;
        } else if (phone[i] != ' ' && phone[i] != '-' && phone[i] != '(' && 
                   phone[i] != ')' && phone[i] != '+') {
            return 0; // Invalid character
        }
    }
    
    return digit_count >= 10;
}

// String utilities
void trim_whitespace(char *str) {
    if (str == NULL) return;
    
    // Trim leading whitespace
    char *start = str;
    while (isspace((unsigned char)*start)) start++;
    
    // Trim trailing whitespace
    char *end = str + strlen(str) - 1;
    while (end > start && isspace((unsigned char)*end)) end--;
    
    // Move string and null terminate
    size_t len = end - start + 1;
    memmove(str, start, len);
    str[len] = '\0';
}

void to_lowercase(char *str) {
    if (str == NULL) return;
    
    for (int i = 0; str[i]; i++) {
        str[i] = tolower((unsigned char)str[i]);
    }
}

void to_uppercase(char *str) {
    if (str == NULL) return;
    
    for (int i = 0; str[i]; i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
}

// Memory utilities
void safe_free(void **ptr) {
    if (ptr && *ptr) {
        free(*ptr);
        *ptr = NULL;
    }
}

// File utilities
int file_exists(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file) {
        fclose(file);
        return 1;
    }
    return 0;
}

// Database utilities
char* escape_string(const char *input) {
    if (input == NULL) return NULL;
    
    size_t input_len = strlen(input);
    char *escaped = malloc(input_len * 2 + 1);
    if (escaped == NULL) return NULL;
    
    mysql_real_escape_string(db_connection, escaped, input, input_len);
    return escaped;
}

// Input validation helpers
int is_valid_id(const char *id_str) {
    if (id_str == NULL || strlen(id_str) == 0) return 0;
    
    for (int i = 0; id_str[i]; i++) {
        if (!isdigit(id_str[i])) return 0;
    }
    
    int id = atoi(id_str);
    return id > 0;
}

int is_valid_username(const char *username) {
    if (username == NULL || strlen(username) < 3 || strlen(username) > 50) {
        return 0;
    }
    
    // Username should start with letter and contain only letters, numbers, and underscore
    if (!isalpha(username[0])) return 0;
    
    for (int i = 1; username[i]; i++) {
        if (!isalnum(username[i]) && username[i] != '_') {
            return 0;
        }
    }
    
    return 1;
}

int is_valid_password(const char *password) {
    if (password == NULL || strlen(password) < 6) {
        return 0;
    }
    
    // Password should contain at least one digit and one letter
    int has_digit = 0, has_letter = 0;
    
    for (int i = 0; password[i]; i++) {
        if (isdigit(password[i])) has_digit = 1;
        if (isalpha(password[i])) has_letter = 1;
    }
    
    return has_digit && has_letter;
}

// Pagination utilities
int calculate_total_pages(int total_items, int items_per_page) {
    if (total_items == 0) return 0;
    return (total_items + items_per_page - 1) / items_per_page;
}

int get_offset(int page, int items_per_page) {
    return (page - 1) * items_per_page;
}

// Display utilities
void print_separator() {
    printf("=====================================\n");
}

void print_header(const char *title) {
    print_separator();
    printf("  %s\n", title);
    print_separator();
}

void print_success(const char *message) {
    printf("✓ %s\n", message);
}

void print_error(const char *message) {
    printf("✗ %s\n", message);
}

void print_info(const char *message) {
    printf("ℹ %s\n", message);
}

// Progress indicator
void show_progress(int current, int total) {
    int progress = (current * 100) / total;
    printf("\rProgress: [");
    
    int bars = progress / 5;
    for (int i = 0; i < 20; i++) {
        if (i < bars) {
            printf("=");
        } else {
            printf(" ");
        }
    }
    
    printf("] %d%%", progress);
    fflush(stdout);
    
    if (current == total) {
        printf("\n");
    }
}