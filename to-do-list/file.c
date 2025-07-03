#include <stdio.h>
#include "file.h"
#include "task.h"
#include "utils.h"

void saveTasksToFile(Task* head, const char* filename) {
    FILE* file = fopen(filename, "w"); // open in write mode
    if (file == NULL) {
        printf("❌ Failed to open file for writing.\n", RED, RESET);
        return;
    }

    Task* temp = head;
    while (temp != NULL) {
        fprintf(file, "%d|%s\n", temp->completed, temp->title);
        temp = temp->next;
    }

    fclose(file);
    printf("📁 Tasks saved to %s\n", filename, GREEN, RESET);
}