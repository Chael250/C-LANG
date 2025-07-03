// intergration with database (mysql)
// CRUD for contacts
// struct(contact) => id, name, phone, email, user_id
// user => id, username, password
// Password encryption (bcrypt)
// authentication (login, register)
// makefile
// log system (log all actions)
// error handling
// modules (main, utils, contact, file, auth)
// Search contacts by name or phone
// Sort contacts by name and phone
// Pagination for large contact list (10 contacts per page)

// main.c
#include "main.h"

MYSQL *db_connection = NULL;
int current_user_id = -1;

int main() {
    printf("=== Contact Book System ===\n");
    
    // Initialize database
    if (init_database() != 0) {
        handle_error("Failed to initialize database");
        return 1;
    }
    
    log_action("SYSTEM", "Application started");
    
    // Main application loop
    int choice;
    char input[10];
    
    while (1) {
        if (current_user_id == -1) {
            printf("\n=== Authentication Required ===\n");
            printf("1. Login\n");
            printf("2. Register\n");
            printf("3. Exit\n");
            printf("Enter your choice: ");
            
            if (get_user_input(input, sizeof(input)) == 0) {
                choice = atoi(input);
                
                switch (choice) {
                    case 1:
                        handle_login();
                        break;
                    case 2:
                        handle_register();
                        break;
                    case 3:
                        printf("Goodbye!\n");
                        log_action("SYSTEM", "Application exited");
                        close_database();
                        return 0;
                    default:
                        printf("Invalid choice. Please try again.\n");
                        break;
                }
            }
        } else {
            show_main_menu();
        }
    }
    
    return 0;
}

void handle_login() {
    char username[MAX_STRING_LENGTH];
    char password[MAX_STRING_LENGTH];
    
    printf("\n=== Login ===\n");
    printf("Username: ");
    if (get_user_input(username, sizeof(username)) == 0) {
        printf("Password: ");
        if (get_user_input(password, sizeof(password)) == 0) {
            int user_id = login_user(username, password);
            if (user_id > 0) {
                current_user_id = user_id;
                printf("Login successful! Welcome, %s\n", username);
                log_action("AUTH", "User logged in");
            } else {
                printf("Invalid username or password.\n");
                log_action("AUTH", "Failed login attempt");
            }
        }
    }
}

void handle_register() {
    char username[MAX_STRING_LENGTH];
    char password[MAX_STRING_LENGTH];
    char confirm_password[MAX_STRING_LENGTH];
    
    printf("\n=== Register ===\n");
    printf("Username: ");
    if (get_user_input(username, sizeof(username)) == 0) {
        printf("Password: ");
        if (get_user_input(password, sizeof(password)) == 0) {
            printf("Confirm Password: ");
            if (get_user_input(confirm_password, sizeof(confirm_password)) == 0) {
                if (strcmp(password, confirm_password) == 0) {
                    if (register_user(username, password) == 0) {
                        printf("Registration successful! Please login.\n");
                        log_action("AUTH", "New user registered");
                    } else {
                        printf("Registration failed. Username might already exist.\n");
                        log_action("AUTH", "Registration failed");
                    }
                } else {
                    printf("Passwords do not match.\n");
                }
            }
        }
    }
}

void show_main_menu() {
    int choice;
    char input[10];
    
    printf("\n=== Main Menu ===\n");
    printf("1. View Contacts\n");
    printf("2. Add Contact\n");
    printf("3. Update Contact\n");
    printf("4. Delete Contact\n");
    printf("5. Search Contacts\n");
    printf("6. Logout\n");
    printf("7. Exit\n");
    printf("Enter your choice: ");
    
    if (get_user_input(input, sizeof(input)) == 0) {
        choice = atoi(input);
        
        switch (choice) {
            case 1:
                handle_contact_listing();
                break;
            case 2:
                handle_contact_creation();
                break;
            case 3:
                handle_contact_update();
                break;
            case 4:
                handle_contact_deletion();
                break;
            case 5:
                handle_contact_search();
                break;
            case 6:
                logout_user();
                current_user_id = -1;
                printf("Logged out successfully.\n");
                log_action("AUTH", "User logged out");
                break;
            case 7:
                printf("Goodbye!\n");
                log_action("SYSTEM", "Application exited");
                close_database();
                exit(0);
            default:
                printf("Invalid choice. Please try again.\n");
                break;
        }
    }
}

void handle_contact_creation() {
    char name[MAX_STRING_LENGTH];
    char phone[MAX_PHONE_LENGTH];
    char email[MAX_EMAIL_LENGTH];
    
    printf("\n=== Add New Contact ===\n");
    printf("Name: ");
    if (get_user_input(name, sizeof(name)) == 0) {
        printf("Phone: ");
        if (get_user_input(phone, sizeof(phone)) == 0) {
            if (!validate_phone(phone)) {
                printf("Invalid phone number format.\n");
                return;
            }
            
            printf("Email: ");
            if (get_user_input(email, sizeof(email)) == 0) {
                if (!validate_email(email)) {
                    printf("Invalid email format.\n");
                    return;
                }
                
                if (create_contact(name, phone, email) == 0) {
                    printf("Contact added successfully!\n");
                    log_action("CONTACT", "Contact created");
                } else {
                    printf("Failed to add contact.\n");
                    log_action("CONTACT", "Failed to create contact");
                }
            }
        }
    }
}

void handle_contact_listing() {
    ContactList list = {0};
    char input[10];
    int page = 1;
    char sort_by[20] = "name";
    
    printf("\n=== Contact List ===\n");
    printf("Sort by (name/phone): ");
    if (get_user_input(input, sizeof(input)) == 0) {
        if (strlen(input) > 0) {
            if (strcmp(input, "name") == 0 || strcmp(input, "phone") == 0) {
                strcpy(sort_by, input);
            }
        }
    }
    
    while (1) {
        if (read_contacts(&list, page, NULL, sort_by) == 0) {
            print_contact_list(&list);
            
            printf("\nPage %d of %d\n", page, list.total_pages);
            printf("Commands: [n]ext, [p]revious, [q]uit: ");
            
            if (get_user_input(input, sizeof(input)) == 0) {
                if (strcmp(input, "n") == 0 && page < list.total_pages) {
                    page++;
                } else if (strcmp(input, "p") == 0 && page > 1) {
                    page--;
                } else if (strcmp(input, "q") == 0) {
                    break;
                }
            }
            
            free_contact_list(&list);
        } else {
            printf("Failed to retrieve contacts.\n");
            break;
        }
    }
}

void handle_contact_update() {
    char input[10];
    int contact_id;
    
    printf("\n=== Update Contact ===\n");
    printf("Enter contact ID: ");
    if (get_user_input(input, sizeof(input)) == 0) {
        contact_id = atoi(input);
        
        Contact *contact = get_contact_by_id(contact_id);
        if (contact) {
            printf("Current contact information:\n");
            print_contact(contact);
            
            char name[MAX_STRING_LENGTH];
            char phone[MAX_PHONE_LENGTH];
            char email[MAX_EMAIL_LENGTH];
            
            printf("\nEnter new information (press Enter to keep current):\n");
            printf("Name (%s): ", contact->name);
            if (get_user_input(name, sizeof(name)) == 0) {
                if (strlen(name) == 0) strcpy(name, contact->name);
                
                printf("Phone (%s): ", contact->phone);
                if (get_user_input(phone, sizeof(phone)) == 0) {
                    if (strlen(phone) == 0) strcpy(phone, contact->phone);
                    if (!validate_phone(phone)) {
                        printf("Invalid phone number format.\n");
                        free(contact);
                        return;
                    }
                    
                    printf("Email (%s): ", contact->email);
                    if (get_user_input(email, sizeof(email)) == 0) {
                        if (strlen(email) == 0) strcpy(email, contact->email);
                        if (!validate_email(email)) {
                            printf("Invalid email format.\n");
                            free(contact);
                            return;
                        }
                        
                        if (update_contact(contact_id, name, phone, email) == 0) {
                            printf("Contact updated successfully!\n");
                            log_action("CONTACT", "Contact updated");
                        } else {
                            printf("Failed to update contact.\n");
                            log_action("CONTACT", "Failed to update contact");
                        }
                    }
                }
            }
            
            free(contact);
        } else {
            printf("Contact not found.\n");
        }
    }
}

void handle_contact_deletion() {
    char input[10];
    int contact_id;
    
    printf("\n=== Delete Contact ===\n");
    printf("Enter contact ID: ");
    if (get_user_input(input, sizeof(input)) == 0) {
        contact_id = atoi(input);
        
        Contact *contact = get_contact_by_id(contact_id);
        if (contact) {
            printf("Contact to delete:\n");
            print_contact(contact);
            
            printf("Are you sure you want to delete this contact? (y/n): ");
            if (get_user_input(input, sizeof(input)) == 0) {
                if (strcmp(input, "y") == 0 || strcmp(input, "Y") == 0) {
                    if (delete_contact(contact_id) == 0) {
                        printf("Contact deleted successfully!\n");
                        log_action("CONTACT", "Contact deleted");
                    } else {
                        printf("Failed to delete contact.\n");
                        log_action("CONTACT", "Failed to delete contact");
                    }
                } else {
                    printf("Deletion cancelled.\n");
                }
            }
            
            free(contact);
        } else {
            printf("Contact not found.\n");
        }
    }
}

void handle_contact_search() {
    char search_term[MAX_STRING_LENGTH];
    ContactList list = {0};
    char input[10];
    int page = 1;
    
    printf("\n=== Search Contacts ===\n");
    printf("Enter search term (name or phone): ");
    if (get_user_input(search_term, sizeof(search_term)) == 0) {
        while (1) {
            if (search_contacts(&list, search_term, page) == 0) {
                printf("\nSearch results for '%s':\n", search_term);
                print_contact_list(&list);
                
                printf("\nPage %d of %d\n", page, list.total_pages);
                printf("Commands: [n]ext, [p]revious, [q]uit: ");
                
                if (get_user_input(input, sizeof(input)) == 0) {
                    if (strcmp(input, "n") == 0 && page < list.total_pages) {
                        page++;
                    } else if (strcmp(input, "p") == 0 && page > 1) {
                        page--;
                    } else if (strcmp(input, "q") == 0) {
                        break;
                    }
                }
                
                free_contact_list(&list);
            } else {
                printf("No contacts found matching '%s'.\n", search_term);
                break;
            }
        }
    }
}


