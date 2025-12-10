#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "../include/utils.h"

// Clear input buffer
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Validate email format
int isValidEmail(const char *email) {
    if (email == NULL || strlen(email) == 0) return 0;
    
    int atCount = 0;
    int dotCount = 0;
    
    for (int i = 0; email[i] != '\0'; i++) {
        if (email[i] == '@') atCount++;
        if (email[i] == '.') dotCount++;
    }
    
    return (atCount == 1 && dotCount >= 1) ? 1 : 0;
}

// Validate phone number
int isValidPhone(const char *phone) {
    if (phone == NULL || strlen(phone) < 10) return 0;
    
    for (int i = 0; phone[i] != '\0'; i++) {
        if (!isdigit(phone[i]) && phone[i] != '+' && phone[i] != '-') {
            return 0;
        }
    }
    return 1;
}

// Generate unique ID
void generateID(char *id, const char *prefix) {
    time_t t;
    time(&t);
    sprintf(id, "%s%lld", prefix, (long long)t);
}

// Get user input safely
void getInput(char *buffer, int size) {
    if (fgets(buffer, size, stdin) != NULL) {
        // Remove trailing newline
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
    }
}

// Display menu
void displayMenu(const char *title, const char **options, int count) {
    printf("\n");
    printf("====== %s ======\n", title);
    for (int i = 0; i < count; i++) {
        printf("%d. %s\n", i + 1, options[i]);
    }
    printf("==============================\n");
}

// Display header
void displayHeader(const char *title) {
    printf("\n");
    printf("╔══════════════════════════════════════╗\n");
    printf("║        %s\n", title);
    printf("╚══════════════════════════════════════╝\n");
}

// Display footer
void displayFooter() {
    printf("\n════════════════════════════════════════\n");
}

// Pause screen
void pauseScreen() {
    printf("\nPress Enter to continue...");
    clearInputBuffer();
}

// Clear screen
void clearScreen() {
    system("cls");
}
