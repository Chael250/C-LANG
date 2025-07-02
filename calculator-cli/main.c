#include<stdio.h>
#include<string.h> // strncpy, strlen
#include<stdlib.h> // exit
#include<ctype.h> // isspace
#include<math.h> // For future use, e.g., pow, sqrt


#define MAX_INPUT 256
#define HISTORY_SIZE 100

char history[HISTORY_SIZE][MAX_INPUT];
int history_count = 0;

void add_to_history(const char *entry){
    if(history_count < HISTORY_SIZE) {
        strncpy(history[history_count++], entry, MAX_INPUT);
    }
}

void show_history() {
    printf("\033[1;34m--- History ---\033[0m\n");
    for(int i = 0; i < history_count; i++) {
        printf("%d: %s\n", i + 1, history[i]);
    }
}

void trim(char *str) {
    char *end;
    while (isspace((unsigned char)*str)) str++;
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    *(end + 1) = 0;
}

double evaluate_simple(char *expr, int *error) {
    double num1 = 0, num2 = 0;
    char op;

    if(sscanf(expr, "%lf %c %lf", &num1, &op, &num2) == 3) {
        switch(op) {
            case '+': return num1 + num2;
            case '-': return num1 - num2;
            case '*': return num1 * num2;
            case '/':
                if(num2 == 0) {
                    *error = 1; // Division by zero error
                    return 0;
                }
                return num1 / num2;
            default:
                *error = 2; // Invalid operator error
                return 0;
        }
    } else {
        *error = 3; // Parsing error
        return 0;
    }
}

void handle_command(char *input) {
    if(strcmp(input, "exit") == 0) {
        printf("\033[1;31mGoodbye!\033[0m\n");
        exit(0);
    } else if(strcmp(input, "history") == 0) {
        show_history();
    } else {
        int error = 0;
        double result = evaluate_simple(input, &error);

        if(!error) {
            printf("\033[1;32mResult: %.2f\033[0m\n", result);
        } else {
            printf("\033[1;31mError: ");
            switch(error) {
                case 1: printf("Division by zero.\n"); break;
                case 2: printf("Invalid operator.\n"); break;
                case 3: printf("Parsing error. Please use the format 'num1 op num2'.\n"); break;
                default: printf("Unknown error.\n");
            }    
        }
    }
}

int main() {
    char input[MAX_INPUT];
    printf("\033[1;36mWelcome to the Advanced C CLI Calculator! Type 'exit' to quit.\033[0m\n");
    while (1) {
        printf("\033[1;33m> \033[0m");
        if (!fgets(input, MAX_INPUT, stdin)) break;
        trim(input);
        if (strlen(input) == 0) continue;
        add_to_history(input);
        handle_command(input);
    }
    return 0;
}

// Loop
// Add brackets to respect bodmas
// Error handling
// Instead of using scanf, read full line:
// History
// Use the C math library (#include <math.h> and compile with -lm).
// Teaches: Base conversion, strtol(), format specifiers
// Use ANSI escape codes to add color
// Teaches: Handling stdin from a pipe, isatty()
// \033[1;32m // green  
// \033[1;34m // blue  
// \033[1;33m // yellow  
// \033[1;36m // cyan  
// \033[1;37m // white  