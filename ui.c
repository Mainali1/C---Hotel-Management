/**
 * Hotel Management System
 * ui.c - User interface implementation
 * 
 * This file implements user interface functions for displaying menus,
 * formatting output, and handling screen display.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui.h"
#include "utils.h"
#include "reservation.h"
#include "billing.h"
#include "guest.h"

/**
 * Display the welcome screen
 */
void displayWelcomeScreen(void) {
    clearScreen();
    printf("\n\n");
    printf("  *******************************************************\n");
    printf("  *                                                     *\n");
    printf("  *             HOTEL MANAGEMENT SYSTEM                 *\n");
    printf("  *                                                     *\n");
    printf("  *******************************************************\n");
    printf("  *                                                     *\n");
    printf("  *        Welcome to the Hotel Management System       *\n");
    printf("  *                                                     *\n");
    printf("  *        Developed as an Academic Project for         *\n");
    printf("  *                 BCA II Semester                      *\n");
    printf("  *                                                     *\n");
    printf("  *******************************************************\n\n");
    
    pauseExecution();
    clearScreen();
}

/**
 * Display a header for a section
 * 
 * @param title The title of the section
 */
void displayHeader(const char *title) {
    int i;
    int len = strlen(title);
    int padding = (60 - len) / 2;
    
    printf("\n");
    for (i = 0; i < 60; i++) printf("=");
    printf("\n");
    
    for (i = 0; i < padding; i++) printf(" ");
    printf("%s", title);
    for (i = 0; i < padding; i++) printf(" ");
    printf("\n");
    
    for (i = 0; i < 60; i++) printf("=");
    printf("\n\n");
}

/**
 * Display a formatted table header
 * 
 * @param columns Array of column titles
 * @param widths Array of column widths
 * @param numCols Number of columns
 */
void displayTableHeader(const char **columns, const int *widths, int numCols) {
    int i, j;
    int totalWidth = 1; /* Start with 1 for the first | */
    
    /* Calculate total width */
    for (i = 0; i < numCols; i++) {
        totalWidth += widths[i] + 3; /* +3 for | and spaces */
    }
    
    /* Print top border */
    printf("+");
    for (i = 0; i < numCols; i++) {
        for (j = 0; j < widths[i] + 2; j++) printf("-");
        printf("+");
    }
    printf("\n");
    
    /* Print column headers */
    printf("|");
    for (i = 0; i < numCols; i++) {
        printf(" %-*s |", widths[i], columns[i]);
    }
    printf("\n");
    
    /* Print separator */
    printf("+");
    for (i = 0; i < numCols; i++) {
        for (j = 0; j < widths[i] + 2; j++) printf("-");
        printf("+");
    }
    printf("\n");
}

/**
 * Display a formatted table row
 * 
 * @param values Array of values for the row
 * @param widths Array of column widths
 * @param numCols Number of columns
 */
void displayTableRow(const char **values, const int *widths, int numCols) {
    int i;
    
    printf("|");
    for (i = 0; i < numCols; i++) {
        printf(" %-*s |", widths[i], values[i]);
    }
    printf("\n");
}

/**
 * Display a formatted table footer
 * 
 * @param widths Array of column widths
 * @param numCols Number of columns
 */
void displayTableFooter(const int *widths, int numCols) {
    int i, j;
    int totalWidth = 1; /* Start with 1 for the first | */
    
    for (i = 0; i < numCols; i++) {
        totalWidth += widths[i] + 3; /* Add width plus spacing and | */
    }
    
    printf("+");
    for (i = 0; i < totalWidth - 2; i++) printf("-");
    printf("+\n");
}

/**
 * Display the reports menu and handle user input
 *
 * @param currentUser Pointer to the current user
 */
void reportsMenu(User *currentUser) {
    int choice;
    
    do {
        clearScreen();
        displayHeader("Reports Menu");
        
        printf("\n1. Occupancy Report");
        printf("\n2. Revenue Report");
        printf("\n3. Guest Statistics");
        printf("\n4. Overdue Payments Report");
        printf("\n0. Back to Main Menu");
        
        choice = getIntInput("\n\nEnter your choice (0-4): ", 0, 4);
        
        switch (choice) {
            case 1:
                /* TODO: Implement occupancy report */
                printf("\nOccupancy report will be implemented in future updates.");
                break;
            case 2:
                /* TODO: Implement revenue report */
                printf("\nRevenue report will be implemented in future updates.");
                break;
            case 3:
                /* TODO: Implement guest statistics */
                printf("\nGuest statistics will be implemented in future updates.");
                break;
            case 4:
                /* Check overdue invoices */
                checkOverdueInvoices();
                break;
        }
        
        if (choice != 0) {
            printf("\n");
            pauseExecution();
        }
    } while (choice != 0);
}

/**
 * Display a message box with a title and message
 * 
 * @param title The title of the message box
 * @param message The message to display
 * @param type The type of message (0=info, 1=success, 2=warning, 3=error)
 */
void displayMessageBox(const char *title, const char *message, int type) {
    int i;
    int titleLen = strlen(title);
    int messageLen = strlen(message);
    int boxWidth = messageLen + 10 > 50 ? messageLen + 10 : 50;
    const char *prefix;
    
    /* Set prefix based on message type */
    switch (type) {
        case 1: prefix = "[SUCCESS] "; break;
        case 2: prefix = "[WARNING] "; break;
        case 3: prefix = "[ERROR] "; break;
        default: prefix = "[INFO] "; break;
    }
    
    /* Print top border */
    printf("\n");
    for (i = 0; i < boxWidth; i++) printf("*");
    printf("\n");
    
    /* Print title */
    printf("*");
    for (i = 0; i < (boxWidth - titleLen - strlen(prefix) - 2) / 2; i++) printf(" ");
    printf("%s%s", prefix, title);
    for (i = 0; i < (boxWidth - titleLen - strlen(prefix) - 2) / 2; i++) printf(" ");
    if ((boxWidth - titleLen - strlen(prefix) - 2) % 2 != 0) printf(" ");
    printf("*\n");
    
    /* Print separator */
    for (i = 0; i < boxWidth; i++) printf("*");
    printf("\n");
    
    /* Print message */
    printf("*");
    for (i = 0; i < (boxWidth - messageLen - 2) / 2; i++) printf(" ");
    printf("%s", message);
    for (i = 0; i < (boxWidth - messageLen - 2) / 2; i++) printf(" ");
    if ((boxWidth - messageLen - 2) % 2 != 0) printf(" ");
    printf("*\n");
    
    /* Print bottom border */
    for (i = 0; i < boxWidth; i++) printf("*");
    printf("\n");
}

/**
 * Display a progress bar
 * 
 * @param progress The progress value (0-100)
 * @param width The width of the progress bar
 */
void displayProgressBar(int progress, int width) {
    int i;
    int pos = width * progress / 100;
    
    printf("[Progress: %3d%%] [", progress);
    for (i = 0; i < width; i++) {
        if (i < pos) printf("#");
        else printf(" ");
    }
    printf("]\r");
    fflush(stdout);
}

/**
 * Display a menu with options
 * 
 * @param title The title of the menu
 * @param options Array of option strings
 * @param numOptions Number of options
 * @return The selected option (1-based)
 */
int displayMenu(const char *title, const char **options, int numOptions) {
    int i;
    int choice;
    
    displayHeader(title);
    
    for (i = 0; i < numOptions; i++) {
        printf("  %d. %s\n", i + 1, options[i]);
    }
    printf("  0. Back\n\n");
    
    choice = getIntInput("Enter your choice: ", 0, numOptions);
    return choice;
}