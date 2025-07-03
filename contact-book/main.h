#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <mysql/mysql.h>
#include <bcrypt/bcrypt.h>

#define MAX_STRING_LENGHT 256
#define MAX_PHONE_LENGTH 20
#define MAX_EMAIL_LENGTH 100
#define CONTACTS_PER_PAGE 10
#define BCRYPT_WORK_FACTOR 12

typedef struct {
    int id;
    char name[MAX_STRING_LENGHT];
    char phone[MAX_PHONE_LENGTH];
    char email[MAX_EMAIL_LENGTH];
    int user_id;
} Contact;

typedef struct {
    int id;
    char username[MAX_STRING_LENGHT];
    char password[MAX_STRING_LENGHT]; // Store plain text password for bcrypt
    // char password_hash[60]; // bcrypt hash length
} User;

typedef struct {
    Contact *contacts;
    int count;
    int total_pages;
    int current_page;
} ContactList;

extern MYSQL *db_connection;
extern int current_user_id;

//Database functions

int initialize_database();
void close_database();
int create_tables();

// Authentication functions

int register_user(const char *username, const char *password);
int login_user(const char *username, const char *password);
void logout_user();
int hash_password(const char *password, char *hash);
int verify_password(const char *password, const char *hash);

// Contact CRUD functions

int create_contact(const char *name, const char *phone, const char *email);
int read_contacts(ContactList *list, int page, const char *search_term, const char *sort_by);
int update_contact(int contact_id, const char *name, const char *phone, const char *email);
int delete_contact(int contact_id);
Contact* get_contact_by_id(int contact_id);

// Search and sort functions

int search_contacts(ContactList *list, const char *search_term, int page);
int sort_contacts(ContactList *list, const char *sort_by, int page);

// Utility functions
void log_action(const char *action, const char *details);
void handle_error(const char *message);
void clear_screen();
void print_menu();
void print_contact(const Contact *contact);
void print_contact_list(const ContactList *list);
void free_contact_list(ContactList *list);
int get_user_input(char *buffer, int max_length);
int validate_email(const char *email);
int validate_phone(const char *phone);

// Menu functions
void show_main_menu();
void show_contact_menu();
void handle_contact_creation();
void handle_contact_listing();
void handle_contact_update();
void handle_contact_deletion();
void handle_contact_search();

#endif