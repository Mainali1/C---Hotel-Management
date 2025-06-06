/**
 * Hotel Management System
 * main.c - Entry point of the application
 * 
 * This file contains the main function and the primary menu system
 * for the hotel management application.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "auth.h"
#include "room.h"
#include "guest.h"
#include "reservation.h"
#include "billing.h"
#include "fileio.h"
#include "utils.h"
#include "ui.h"

#define VERSION "1.0.0"

/* Function prototypes */
void displayMainMenu(void);
void initializeSystem(void);
void backupData(void);

/**
 * Main function - Entry point of the application
 */
int main(void) {
    int choice;
    User currentUser;
    int loggedIn = 0;
    
    /* Initialize the system */
    initializeSystem();
    
    /* Display welcome screen */
    displayWelcomeScreen();
    
    /* Authentication loop */
    while (!loggedIn) {
        loggedIn = loginUser(&currentUser);
        if (!loggedIn) {
            printf("\nLogin failed. Please try again.\n");
        }
    }
    
    printf("\nWelcome, %s! (%s)\n", 
           currentUser.name, 
           currentUser.role == ROLE_ADMIN ? "Administrator" : "Staff");
    
    /* Main program loop */
    do {
        displayMainMenu();
        choice = getIntInput("Enter your choice: ", 0, 9);
        
        switch (choice) {
            case 1: /* Room Management */
                roomManagementMenu(&currentUser);
                break;
                
            case 2: /* Guest Management */
                guestManagementMenu(&currentUser);
                break;
                
            case 3: /* Reservation Management */
                reservationManagementMenu(&currentUser);
                break;
                
            case 4: /* Billing and Payments */
                billingManagementMenu(&currentUser);
                break;
                
            case 5: /* Reports */
                if (currentUser.role == ROLE_ADMIN) {
                    reportsMenu(&currentUser);
                } else {
                    printf("\nAccess denied. Admin privileges required.\n");
                }
                break;
                
            case 6: /* User Management */
                if (currentUser.role == ROLE_ADMIN) {
                    userManagementMenu(&currentUser);
                } else {
                    printf("\nAccess denied. Admin privileges required.\n");
                }
                break;
                
            case 7: /* System Backup */
                if (currentUser.role == ROLE_ADMIN) {
                    backupData();
                    printf("\nSystem backup completed successfully.\n");
                } else {
                    printf("\nAccess denied. Admin privileges required.\n");
                }
                break;
                
            case 8: /* Change Password */
                changePassword(&currentUser);
                break;
                
            case 9: /* About */
                printf("\nHotel Management System v%s\n", VERSION);
                printf("Developed for academic purposes\n");
                break;
                
            case 0: /* Exit */
                printf("\nLogging out...\n");
                printf("Thank you for using the Hotel Management System!\n");
                break;
                
            default:
                printf("\nInvalid choice. Please try again.\n");
        }
        
        if (choice != 0) {
            printf("\nPress Enter to continue...");
            getchar(); /* Consume newline */
            getchar(); /* Wait for Enter */
        }
        
    } while (choice != 0);
    
    return 0;
}

/**
 * Display the main menu options
 */
void displayMainMenu(void) {
    clearScreen();
    printf("\n===== HOTEL MANAGEMENT SYSTEM =====\n");
    printf("1. Room Management\n");
    printf("2. Guest Management\n");
    printf("3. Reservation Management\n");
    printf("4. Billing and Payments\n");
    printf("5. Reports\n");
    printf("6. User Management\n");
    printf("7. System Backup\n");
    printf("8. Change Password\n");
    printf("9. About\n");
    printf("0. Exit\n");
    printf("==================================\n");
}

/**
 * Initialize the system by loading necessary data
 * and creating required directories
 */
void initializeSystem(void) {
    /* Create data directory if it doesn't exist */
    createDirectoryIfNotExists("data");
    
    /* Initialize data files if they don't exist */
    initializeDataFiles();
    
    /* Load configuration */
    loadConfiguration();
    
    /* Set random seed */
    srand((unsigned int)time(NULL));
}

/**
 * Backup all system data
 */
void backupData(void) {
    char backupDir[100];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    
    /* Create backup directory with timestamp */
    sprintf(backupDir, "backup_%04d%02d%02d_%02d%02d%02d", 
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec);
    
    createDirectoryIfNotExists(backupDir);
    
    /* Backup all data files */
    backupDataFiles(backupDir);
}