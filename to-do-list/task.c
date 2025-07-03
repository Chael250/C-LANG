#include <stdio.h>
#include "task.h"
#include "utils.h"

void createTask(const char* title, const char* description, struct tm due_date) {
    Task* newtask = malloc(sizeof(Task));

    strcpy(newtask->title, title);
    strcpy(newtask->description, description);
    strcpy(newtask->due_date, due_date);

    return newtask;
}

void printTask(Task* task) {
    Task* temp = task;
    if (temp == NULL) {
        printf("No tasks available.\n");
        return;
    }

    while(temp != NULL) {
        printf("Task ID: %d\n", temp->id, BLUE, RESET);
        printf("Title: %s\n", temp->title, GREEN, RESET);
        printf("Description: %s\n", temp->description, GREEN, RESET);
        printf("Due Date: %02d-%02d-%04d\n", temp->due_date.tm_mday, temp->due_date.tm_mon + 1, temp->due_date.tm_year + 1900, GREEN, RESET);
        printf("Completed: %s\n", temp->completed ? "Yes" : "No", YELLOW, RESET);
        printf("-------------------------\n", CYAN, RESET);
        temp = temp->next;
    }
}

void markTaskAsDone(Task* head, int task_id) {
    Task* current = head;
    while (current != NULL) {
        if (current->id == task_id) {
            current->completed = 1; // Mark as done
            return head; // Return the head of the list
        }
        current = current->next;
    }
    printf("Task with ID %d not found.\n", task_id, RED, RESET);
    return head; // Return the head of the list even if task not found
}

void editTask(Task* head, int task_id, const char* new_title, const char* new_description, struct tm new_due_date) {
    Task* current = head;
    while (current != NULL) {
        if (current->id == task_id) {
            strcpy(current->title, new_title);
            strcpy(current->description, new_description);
            current->due_date = new_due_date; // Update due date
            return head; // Return the head of the list
        }
        current = current->next;
    }
    printf("Task with ID %d not found.\n", task_id, RED, RESET);
}

void freeTask(Task* head, int task_id) {
    Task* current = head;
    Task* previous = NULL;

    while (current != NULL) {
        if (current->id == task_id) {
            if (previous == NULL) {
                // Deleting the head of the list
                head = current->next;
            } else {
                previous->next = current->next;
            }
            free(current);
            return; // Exit after freeing the task
        }
        previous = current;
        current = current->next;
    }
    printf("Task with ID %d not found.\n", task_id), RED, RESET;
    // If task not found, do nothing
}

void freeAllTasks(Task* head) {
    Task* current = head;
    Task* next;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }

    printf("All tasks have been freed.\n", GREEN, RESET);
}