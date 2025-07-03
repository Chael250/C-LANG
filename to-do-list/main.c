#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "task.h"
#include "utils.h"
#include "file.h"

void showMenu() {
    printf("\n%s======= TO-DO MENU =======%s\n", CYAN, RESET);
    printf("%s1.%s Add Task\n", GREEN, RESET);
    printf("%s2.%s Show All Tasks\n", GREEN, RESET);
    printf("%s3.%s Mark Task as Done\n", GREEN, RESET);
    printf("%s4.%s Delete Task\n", GREEN, RESET);
    printf("%s5.%s Save in file\n", GREEN, RESET);
    printf("%s6.%s Exit\n", RED, RESET);
    printf("%s==========================%s\n", CYAN, RESET);
}

int main() {
    Task* head = NULL;
    int choice;
    char title[100];

    while (1) {
        showMenu();
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); // clear newline

        switch (choice) {
            case 1:
                printf("Enter task title: ");
                fgets(title, sizeof(title), stdin);
                title[strcspn(title, "\n")] = '\0';
                head = addTask(head, title);
                break;

            case 2:
                printTasks(head);
                break;

            case 3:
                printf("Enter title to mark as done: ");
                fgets(title, sizeof(title), stdin);
                title[strcspn(title, "\n")] = '\0';
                head = markTaskDone(head, title);
                break;

            case 4:
                printf("Enter title to delete: ");
                fgets(title, sizeof(title), stdin);
                title[strcspn(title, "\n")] = '\0';
                head = deleteTask(head, title);
                break;

            case 5:
                saveTasksToFile("tasks.txt", head);
                break;

            case 6:
                freeTasks(head);
                printf("👋 Goodbye, hacker!\n");
                return 0;

            default:
                printf("❗ Invalid choice.\n");
        }
    }
}