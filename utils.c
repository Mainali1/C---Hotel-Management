/**
 * Hotel Management System
 * utils.c - Utility functions implementation
 * 
 * This file implements utility functions used throughout the application
 * including date handling, input validation, and string manipulation.
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <time.h>
 #include <ctype.h>
 
 #if defined(_WIN32) || defined(_WIN64)
 #include <conio.h>
 #else
 // A simple getch implementation for POSIX systems
 #include <termios.h>
 #include <unistd.h>
 int getch() {
     struct termios oldt, newt;
     int ch;
     tcgetattr(STDIN_FILENO, &oldt);
     newt = oldt;
     newt.c_lflag &= ~(ICANON | ECHO);
     tcsetattr(STDIN_FILENO, TCSANOW, &newt);
     ch = getchar();
     tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
     return ch;
 }
 #endif
 
 #include "utils.h"
 
 // Helper function to clear the input buffer safely
 static void clear_input_buffer() {
     int c;
     while ((c = getchar()) != '\n' && c != EOF);
 }
  
 /**
   * Get integer input from user with validation
   */
 int getIntInput(const char *prompt, int min, int max) {
     int input;
     char buffer[100];
     
     if (prompt != NULL) {
         printf("%s", prompt);
     }
 
     while (1) {
         if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
             printf("\nError reading input. Please try again: ");
             continue;
         }
         
         if (sscanf(buffer, "%d", &input) == 1) {
             if (input >= min && input <= max) {
                 return input;
             } else {
                 printf("Input must be between %d and %d. Try again: ", min, max);
             }
         } else {
             printf("Invalid input. Please enter a number: ");
         }
     }
 }
  
 /**
   * Get double input from user with validation
   */
 double getDoubleInput(const char *prompt, double min, double max) {
     double input;
     char buffer[100];
      
     if (prompt != NULL) {
         printf("%s", prompt);
     }
      
     while (1) {
         if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
             printf("\nError reading input. Please try again: ");
             continue;
         }
 
         if (sscanf(buffer, "%lf", &input) == 1) {
              if (max == 0.0 || (input >= min && input <= max)) { // max 0 means no upper limit
                 return input;
              } else {
                 printf("Input must be between %.2f and %.2f. Try again: ", min, max);
              }
         } else {
             printf("Invalid input. Please enter a number: ");
         }
     }
 }
 
 /**
   * Get string input from user
   */
 void getStringInput(const char *prompt, char *output, int maxLen) {
     if (prompt != NULL) {
         printf("%s", prompt);
     }
 
     if (fgets(output, maxLen, stdin) != NULL) {
         // Remove newline character if present
         output[strcspn(output, "\n")] = '\0';
     } else {
         // Handle potential input error
         output[0] = '\0';
     }
 }
  
 /**
   * Get password input from user (masked with asterisks)
   */
 void getPassword(char *password, int maxLen) {
     int i = 0;
     char ch;
      
     while ((ch = getch()) != '\r' && ch != '\n') {
         if (ch == '\b' || ch == 127) { // Handle backspace
             if (i > 0) {
                 i--;
                 printf("\b \b"); // Erase the last asterisk
             }
         } else if (i < maxLen - 1 && isprint(ch)) {
             password[i++] = ch;
             printf("*");
         }
     }
     password[i] = '\0';
     printf("\n");
 }
  
 /**
   * Get current date in YYYY-MM-DD format
   */
 void getCurrentDate(char *dateStr) {
     time_t now = time(NULL);
     struct tm *t = localtime(&now);
     if (t != NULL) {
         sprintf(dateStr, "%04d-%02d-%02d", t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
     }
 }
  
 /**
   * Get current date and time in YYYY-MM-DD HH:MM:SS format
   */
 void getCurrentDateTime(char *dateTimeStr) {
     time_t now = time(NULL);
     struct tm *t = localtime(&now);
     if (t != NULL) {
         sprintf(dateTimeStr, "%04d-%02d-%02d %02d:%02d:%02d", 
                 t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
                 t->tm_hour, t->tm_min, t->tm_sec);
     }
 }
  
 /**
   * Calculate the number of days between two dates
   */
 int calculateDateDifference(const char *startDate, const char *endDate) {
     struct tm start_tm = {0}, end_tm = {0};
     
     if (sscanf(startDate, "%d-%d-%d", &start_tm.tm_year, &start_tm.tm_mon, &start_tm.tm_mday) != 3 ||
         sscanf(endDate, "%d-%d-%d", &end_tm.tm_year, &end_tm.tm_mon, &end_tm.tm_mday) != 3) {
         return -1; // Invalid format
     }
      
     start_tm.tm_year -= 1900;
     start_tm.tm_mon -= 1;
     end_tm.tm_year -= 1900;
     end_tm.tm_mon -= 1;
      
     time_t start_time = mktime(&start_tm);
     time_t end_time = mktime(&end_tm);
 
     if (start_time == -1 || end_time == -1) {
         return -1; // Invalid date
     }
      
     double difference = difftime(end_time, start_time);
     return (int)(difference / (60 * 60 * 24));
 }
  
 /**
   * Validate a date string in YYYY-MM-DD format
   */
 int validateDate(const char *dateStr) {
     int year, month, day;
     if (strlen(dateStr) != 10 || dateStr[4] != '-' || dateStr[7] != '-') {
         return 0;
     }
     if (sscanf(dateStr, "%d-%d-%d", &year, &month, &day) != 3) {
         return 0;
     }
     if (year < 1900 || year > 2100 || month < 1 || month > 12 || day < 1 || day > 31) {
         return 0;
     }
     // This is a basic check; a more robust one would check days in month.
     return 1;
 }
  
 /**
   * Clear the console screen
   */
 void clearScreen(void) {
     #if defined(_WIN32) || defined(_WIN64)
         system("cls");
     #else
         system("clear");
     #endif
 }
  
 /**
   * Pause execution until user presses Enter
   */
 void pauseExecution(void) {
      printf("\nPress Enter to continue...");
      clear_input_buffer();
 }