// auth.c
#include "main.h"

int register_user(const char *username, const char *password) {
    if (strlen(username) < 3 || strlen(password) < 6) {
        handle_error("Username must be at least 3 characters and password at least 6 characters");
        return -1;
    }
    
    // Hash the password
    char hashed_password[256];
    if (hash_password(password, hashed_password) != 0) {
        handle_error("Failed to hash password");
        return -1;
    }
    
    // Check if username already exists
    char check_query[512];
    snprintf(check_query, sizeof(check_query), 
             "SELECT id FROM users WHERE username='%s'", username);
    
    if (mysql_query(db_connection, check_query)) {
        handle_error("Failed to check username availability");
        return -1;
    }
    
    MYSQL_RES *result = mysql_store_result(db_connection);
    if (result == NULL) {
        handle_error("Failed to store result");
        return -1;
    }
    
    int user_exists = mysql_num_rows(result) > 0;
    mysql_free_result(result);
    
    if (user_exists) {
        handle_error("Username already exists");
        return -1;
    }
    
    // Insert new user
    char insert_query[1024];
    snprintf(insert_query, sizeof(insert_query), 
             "INSERT INTO users (username, password) VALUES ('%s', '%s')",
             username, hashed_password);
    
    if (mysql_query(db_connection, insert_query)) {
        handle_error("Failed to create user");
        return -1;
    }
    
    char log_msg[512];
    snprintf(log_msg, sizeof(log_msg), "User registered: %s", username);
    log_action("AUTH", log_msg);
    
    return 0;
}

int login_user(const char *username, const char *password) {
    char query[512];
    snprintf(query, sizeof(query), 
             "SELECT id, password FROM users WHERE username='%s'",
             username);
    
    if (mysql_query(db_connection, query)) {
        handle_error("Failed to execute login query");
        return -1;
    }
    
    MYSQL_RES *result = mysql_store_result(db_connection);
    if (result == NULL) {
        handle_error("Failed to store result");
        return -1;
    }
    
    MYSQL_ROW row = mysql_fetch_row(result);
    if (row == NULL) {
        mysql_free_result(result);
        return -1; // User not found
    }
    
    int user_id = atoi(row[0]);
    const char *stored_hash = row[1];
    
    // Verify password
    int password_valid = verify_password(password, stored_hash);
    mysql_free_result(result);
    
    if (password_valid) {
        char log_msg[512];
        snprintf(log_msg, sizeof(log_msg), "User logged in: %s", username);
        log_action("AUTH", log_msg);
        return user_id;
    } else {
        char log_msg[512];
        snprintf(log_msg, sizeof(log_msg), "Failed login attempt for: %s", username);
        log_action("AUTH", log_msg);
        return -1;
    }
}

void logout_user() {
    current_user_id = -1;
    log_action("AUTH", "User logged out");
}

int hash_password(const char *password, char *hashed) {
    char salt[BCRYPT_HASHSIZE];
    
    // Generate salt
    if (bcrypt_gensalt(BCRYPT_WORK_FACTOR, salt) != 0) {
        handle_error("Failed to generate salt");
        return -1;
    }
    
    // Hash password
    if (bcrypt_hashpw(password, salt, hashed) != 0) {
        handle_error("Failed to hash password");
        return -1;
    }
    
    return 0;
}

int verify_password(const char *password, const char *hashed) {
    return bcrypt_checkpw(password, hashed) == 0;
}