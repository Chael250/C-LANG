// database.c
#include "main.h"

int init_database() {
    db_connection = mysql_init(NULL);
    
    if (db_connection == NULL) {
        handle_error("mysql_init() failed");
        return -1;
    }
    
    // Connect to MySQL server
    if (mysql_real_connect(db_connection, "localhost", "root", "password", 
                          "contact_book", 0, NULL, 0) == NULL) {
        handle_error("mysql_real_connect() failed");
        mysql_close(db_connection);
        return -1;
    }
    
    // Create tables if they don't exist
    if (create_tables() != 0) {
        handle_error("Failed to create database tables");
        mysql_close(db_connection);
        return -1;
    }
    
    log_action("DATABASE", "Database initialized successfully");
    return 0;
}

void close_database() {
    if (db_connection) {
        mysql_close(db_connection);
        db_connection = NULL;
        log_action("DATABASE", "Database connection closed");
    }
}

int create_tables() {
    const char *create_users_table = 
        "CREATE TABLE IF NOT EXISTS users ("
        "id INT AUTO_INCREMENT PRIMARY KEY,"
        "username VARCHAR(255) UNIQUE NOT NULL,"
        "password VARCHAR(255) NOT NULL,"
        "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
        ")";
    
    const char *create_contacts_table = 
        "CREATE TABLE IF NOT EXISTS contacts ("
        "id INT AUTO_INCREMENT PRIMARY KEY,"
        "name VARCHAR(255) NOT NULL,"
        "phone VARCHAR(20) NOT NULL,"
        "email VARCHAR(100) NOT NULL,"
        "user_id INT NOT NULL,"
        "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
        "updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,"
        "FOREIGN KEY (user_id) REFERENCES users(id) ON DELETE CASCADE"
        ")";
    
    // Create users table
    if (mysql_query(db_connection, create_users_table)) {
        handle_error("Failed to create users table");
        return -1;
    }
    
    // Create contacts table
    if (mysql_query(db_connection, create_contacts_table)) {
        handle_error("Failed to create contacts table");
        return -1;
    }
    
    log_action("DATABASE", "Tables created successfully");
    return 0;
}

int create_contact(const char *name, const char *phone, const char *email) {
    if (current_user_id == -1) {
        handle_error("User not authenticated");
        return -1;
    }
    
    char query[1024];
    snprintf(query, sizeof(query), 
             "INSERT INTO contacts (name, phone, email, user_id) VALUES ('%s', '%s', '%s', %d)",
             name, phone, email, current_user_id);
    
    if (mysql_query(db_connection, query)) {
        handle_error("Failed to create contact");
        return -1;
    }
    
    return 0;
}

int read_contacts(ContactList *list, int page, const char *search_term, const char *sort_by) {
    if (current_user_id == -1) {
        handle_error("User not authenticated");
        return -1;
    }
    
    // Initialize list
    list->contacts = NULL;
    list->count = 0;
    list->current_page = page;
    
    // Build query
    char query[1024];
    char where_clause[512] = "";
    char order_clause[128] = "";
    
    // Add search condition
    if (search_term && strlen(search_term) > 0) {
        snprintf(where_clause, sizeof(where_clause), 
                 " AND (name LIKE '%%%s%%' OR phone LIKE '%%%s%%')",
                 search_term, search_term);
    }
    
    // Add sorting
    if (sort_by && strlen(sort_by) > 0) {
        snprintf(order_clause, sizeof(order_clause), " ORDER BY %s", sort_by);
    }
    
    // Count total records
    char count_query[1024];
    snprintf(count_query, sizeof(count_query), 
             "SELECT COUNT(*) FROM contacts WHERE user_id = %d%s",
             current_user_id, where_clause);
    
    if (mysql_query(db_connection, count_query)) {
        handle_error("Failed to count contacts");
        return -1;
    }
    
    MYSQL_RES *count_result = mysql_store_result(db_connection);
    if (count_result) {
        MYSQL_ROW row = mysql_fetch_row(count_result);
        int total_count = atoi(row[0]);
        list->total_pages = (total_count + CONTACTS_PER_PAGE - 1) / CONTACTS_PER_PAGE;
        mysql_free_result(count_result);
    }
    
    // Fetch contacts for current page
    int offset = (page - 1) * CONTACTS_PER_PAGE;
    snprintf(query, sizeof(query), 
             "SELECT id, name, phone, email, user_id FROM contacts WHERE user_id = %d%s%s LIMIT %d OFFSET %d",
             current_user_id, where_clause, order_clause, CONTACTS_PER_PAGE, offset);
    
    if (mysql_query(db_connection, query)) {
        handle_error("Failed to read contacts");
        return -1;
    }
    
    MYSQL_RES *result = mysql_store_result(db_connection);
    if (result == NULL) {
        handle_error("Failed to store result");
        return -1;
    }
    
    int num_rows = mysql_num_rows(result);
    if (num_rows == 0) {
        mysql_free_result(result);
        return 0;
    }
    
    list->contacts = malloc(num_rows * sizeof(Contact));
    if (list->contacts == NULL) {
        handle_error("Memory allocation failed");
        mysql_free_result(result);
        return -1;
    }
    
    MYSQL_ROW row;
    int i = 0;
    while ((row = mysql_fetch_row(result)) != NULL) {
        list->contacts[i].id = atoi(row[0]);
        strcpy(list->contacts[i].name, row[1]);
        strcpy(list->contacts[i].phone, row[2]);
        strcpy(list->contacts[i].email, row[3]);
        list->contacts[i].user_id = atoi(row[4]);
        i++;
    }
    
    list->count = num_rows;
    mysql_free_result(result);
    
    return 0;
}

int update_contact(int contact_id, const char *name, const char *phone, const char *email) {
    if (current_user_id == -1) {
        handle_error("User not authenticated");
        return -1;
    }
    
    char query[1024];
    snprintf(query, sizeof(query), 
             "UPDATE contacts SET name='%s', phone='%s', email='%s' WHERE id=%d AND user_id=%d",
             name, phone, email, contact_id, current_user_id);
    
    if (mysql_query(db_connection, query)) {
        handle_error("Failed to update contact");
        return -1;
    }
    
    return 0;
}

int delete_contact(int contact_id) {
    if (current_user_id == -1) {
        handle_error("User not authenticated");
        return -1;
    }
    
    char query[256];
    snprintf(query, sizeof(query), 
             "DELETE FROM contacts WHERE id=%d AND user_id=%d",
             contact_id, current_user_id);
    
    if (mysql_query(db_connection, query)) {
        handle_error("Failed to delete contact");
        return -1;
    }
    
    return 0;
}

Contact* get_contact_by_id(int contact_id) {
    if (current_user_id == -1) {
        handle_error("User not authenticated");
        return NULL;
    }
    
    char query[256];
    snprintf(query, sizeof(query), 
             "SELECT id, name, phone, email, user_id FROM contacts WHERE id=%d AND user_id=%d",
             contact_id, current_user_id);
    
    if (mysql_query(db_connection, query)) {
        handle_error("Failed to get contact");
        return NULL;
    }
    
    MYSQL_RES *result = mysql_store_result(db_connection);
    if (result == NULL) {
        handle_error("Failed to store result");
        return NULL;
    }
    
    MYSQL_ROW row = mysql_fetch_row(result);
    if (row == NULL) {
        mysql_free_result(result);
        return NULL;
    }
    
    Contact *contact = malloc(sizeof(Contact));
    if (contact == NULL) {
        handle_error("Memory allocation failed");
        mysql_free_result(result);
        return NULL;
    }
    
    contact->id = atoi(row[0]);
    strcpy(contact->name, row[1]);
    strcpy(contact->phone, row[2]);
    strcpy(contact->email, row[3]);
    contact->user_id = atoi(row[4]);
    
    mysql_free_result(result);
    return contact;
}

int search_contacts(ContactList *list, const char *search_term, int page) {
    return read_contacts(list, page, search_term, "name");
}

int sort_contacts(ContactList *list, const char *sort_by, int page) {
    return read_contacts(list, page, NULL, sort_by);
}