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
 
 #define VERSION "1.0.1"
 
 /* Function prototypes */
 void displayMainMenu(User *currentUser);
 void initializeSystem(void);
 void backupData(void);
 void handleAdminMenu(int choice, User *currentUser);
 void handleStaffMenu(int choice, User *currentUser);
 
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
             printf("\nLogin failed. Press Enter to try again...");
             getchar(); // Consume potential leftover newline
             getchar(); // Wait for user to press Enter
         }
     }
     
     printf("\nWelcome, %s! (%s)\n", 
            currentUser.name, 
            currentUser.role == ROLE_ADMIN ? "Administrator" : "Staff");
     pauseExecution();
     
     /* Main program loop */
     do {
         displayMainMenu(&currentUser);
         choice = getIntInput("Enter your choice: ", 0, 9);
         
         if (currentUser.role == ROLE_ADMIN) {
             handleAdminMenu(choice, &currentUser);
         } else {
             handleStaffMenu(choice, &currentUser);
         }
         
         if (choice != 0) {
             pauseExecution();
         }
         
     } while (choice != 0);
     
     return 0;
 }
 
 /**
  * Handle menu choices for Admin users
  */
 void handleAdminMenu(int choice, User *currentUser) {
     switch (choice) {
         case 1: roomManagementMenu(currentUser); break;
         case 2: guestManagementMenu(currentUser); break;
         case 3: reservationManagementMenu(currentUser); break;
         case 4: billingManagementMenu(currentUser); break;
         case 5: reportsMenu(currentUser); break;
         case 6: userManagementMenu(currentUser); break;
         case 7: 
             backupData();
             printf("\nSystem backup completed successfully.\n");
             break;
         case 8: changePassword(currentUser); break;
         case 9:
             printf("\nHotel Management System v%s\n", VERSION);
             printf("Developed for academic purposes.\n");
             break;
         case 0:
             printf("\nLogging out...\n");
             printf("Thank you for using the Hotel Management System!\n");
             break;
         default:
             printf("\nInvalid choice. Please try again.\n");
     }
 }
 
 /**
  * Handle menu choices for Staff users
  */
 void handleStaffMenu(int choice, User *currentUser) {
      switch (choice) {
         case 1: roomManagementMenu(currentUser); break;
         case 2: guestManagementMenu(currentUser); break;
         case 3: reservationManagementMenu(currentUser); break;
         case 4: billingManagementMenu(currentUser); break;
         case 5:
         case 6:
         case 7:
             printf("\nAccess denied. Admin privileges required.\n");
             break;
         case 8: changePassword(currentUser); break;
         case 9:
             printf("\nHotel Management System v%s\n", VERSION);
             printf("Developed for academic purposes.\n");
             break;
         case 0:
             printf("\nLogging out...\n");
             printf("Thank you for using the Hotel Management System!\n");
             break;
         default:
             printf("\nInvalid choice. Please try again.\n");
     }
 }
 
 
 /**
  * Display the main menu options
  */
 void displayMainMenu(User *currentUser) {
     clearScreen();
     printf("\n===== HOTEL MANAGEMENT SYSTEM =====\n");
     printf("1. Room Management\n");
     printf("2. Guest Management\n");
     printf("3. Reservation Management\n");
     printf("4. Billing and Payments\n");
     if (currentUser->role == ROLE_ADMIN) {
         printf("5. Reports\n");
         printf("6. User Management\n");
         printf("7. System Backup\n");
     }
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
     printf("Initializing system...\n");
     /* Create data directory if it doesn't exist */
     createDirectoryIfNotExists("data");
     
     /* Initialize data files if they don't exist */
     initializeDataFiles();
     
     /* Load configuration */
     loadConfiguration();
     
     /* Set random seed */
     srand((unsigned int)time(NULL));
     printf("Initialization complete.\n");
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
     
     printf("Creating backup in directory: %s\n", backupDir);
     backupDataFiles(backupDir);
 }