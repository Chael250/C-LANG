#ifndef TASK_H // if not defined TASK_H do as follows
#define TASK_H

#include <time.h>

typedef struct Task {
    int id;
    char title[100];
    char description[256];
    struct tm due_date; // Using struct tm for due dates
    int completed; // 0 for not done, 1 for done
    struct Task* next;
} Task; 

void createTask(const char* title, const char* description, struct tm due_date);
void printTask(Task* task);
void freeTask(Task* head, int task_id);
void markTaskAsDone(Task* head, int task_id);
void editTask(Task* head, int task_id, const char* new_title, const char* new_description, struct tm new_due_date);
void freeAllTasks(Task* head);

#endif // TASK_H