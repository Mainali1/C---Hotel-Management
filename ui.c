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
 #include "billing.h"
 
 
 /**
  * Display the welcome screen
  */
 void displayWelcomeScreen(void) {
     clearScreen();
     printf("\n\n");
     printf("  =======================================================\n");
     printf("  |                                                     |\n");
     printf("  |             HOTEL MANAGEMENT SYSTEM                 |\n");
     printf("  |                                                     |\n");
     printf("  =======================================================\n");
     printf("  |                                                     |\n");
     printf("  |        Welcome to the Hotel Management System       |\n");
     printf("  |                                                     |\n");
     printf("  =======================================================\n\n");
     
     pauseExecution();
 }
 
 /**
  * Display a header for a section
  */
 void displayHeader(const char *title) {
     int len = strlen(title);
     int padding = (58 - len) / 2;
     
     printf("\n");
     for (int i = 0; i < 60; i++) printf("=");
     printf("\n");
     
     printf(" %*s%s%*s \n", padding, "", title, padding, "");
     
     for (int i = 0; i < 60; i++) printf("=");
     printf("\n\n");
 }
 
 /**
  * Display the reports menu and handle user input
  */
 void reportsMenu(User *currentUser) {
     int choice;
     
     do {
         clearScreen();
         displayHeader("Reports Menu");
         
         printf("1. Overdue Payments Report\n");
         printf("2. Occupancy Report (Not Implemented)\n");
         printf("3. Revenue Report (Not Implemented)\n");
         printf("0. Back to Main Menu\n");
         
         choice = getIntInput("\nEnter your choice: ", 0, 3);
         
         switch (choice) {
             case 1:
                 checkOverdueInvoices();
                 // To-do: Create a dedicated listing function for the report
                 printf("\nOverdue invoices check complete. Invoices are now updated.\n");
                 listInvoices(currentUser, INVOICE_STATUS_OVERDUE);
                 break;
             case 2:
             case 3:
                 printf("\nThis report is not yet implemented.\n");
                 break;
             case 0:
                 return;
         }
         
         if (choice != 0) {
             pauseExecution();
         }
     } while (choice != 0);
 }