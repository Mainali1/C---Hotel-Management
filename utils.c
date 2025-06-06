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
#include <conio.h> /* Add this for getch() */
 #include "utils.h"
 
 /**
  * Get integer input from user with validation
  * 
  * @param prompt The prompt to display to the user
  * @param min Minimum acceptable value
  * @param max Maximum acceptable value
  * @return The validated integer input
  */
 int getIntInput(const char *prompt, int min, int max) {
     int input;
     char buffer[100];
     int valid = 0;
     
     do {
         printf("%s", prompt);
         fflush(stdin); /* Clear input buffer */
         
         if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
             printf("\nError reading input. Please try again.\n");
             continue;
         }
         
         /* Check if input is a number */
         if (sscanf(buffer, "%d", &input) != 1) {
             printf("\nInvalid input. Please enter a number.\n");
             continue;
         }
         
         /* Check if input is within range */
         if (input < min || input > max) {
             printf("\nInput must be between %d and %d. Please try again.\n", min, max);
             continue;
         }
         
         valid = 1;
     } while (!valid);
     
     return input;
 }
 
 /**
  * Get double input from user with validation
  * 
  * @param prompt The prompt to display to the user
  * @param min Minimum acceptable value
  * @param max Maximum acceptable value
  * @return The validated double input
  */
 double getDoubleInput(const char *prompt, double min, double max) {
     double input;
     char buffer[100];
     int valid = 0;
     
     do {
         printf("%s", prompt);
         fflush(stdin); /* Clear input buffer */
         
         if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
             printf("\nError reading input. Please try again.\n");
             continue;
         }
         
         /* Check if input is a number */
         if (sscanf(buffer, "%lf", &input) != 1) {
             printf("\nInvalid input. Please enter a number.\n");
             continue;
         }
         
         /* Check if input is within range */
         if (input < min || input > max) {
             printf("\nInput must be between %.2f and %.2f. Please try again.\n", min, max);
             continue;
         }
         
         valid = 1;
     } while (!valid);
     
     return input;
 }
 
 /**
  * Get string input from user with validation
  * 
  * @param prompt The prompt to display to the user
  * @param output Buffer to store the input
  * @param maxLen Maximum length of the input
  * @return 1 if successful, 0 otherwise
  */
 int getStringInput(const char *prompt, char *output, int maxLen) {
     char buffer[1000];
     int len;
     
     printf("%s", prompt);
     fflush(stdin); /* Clear input buffer */
     
     if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
         printf("\nError reading input. Please try again.\n");
         return 0;
     }
     
     /* Remove newline character if present */
     len = strlen(buffer);
     if (len > 0 && buffer[len - 1] == '\n') {
         buffer[len - 1] = '\0';
         len--;
     }
     
     /* Check if input is too long */
     if (len >= maxLen) {
         printf("\nInput too long. Maximum length is %d characters.\n", maxLen - 1);
         return 0;
     }
     
     /* Check if input is empty */
     if (len == 0) {
         printf("\nInput cannot be empty. Please try again.\n");
         return 0;
     }
     
     strcpy(output, buffer);
     return 1;
 }
 
 /**
  * Get password input from user (masked with asterisks)
  * 
  * @param password Buffer to store the password
  * @param maxLen Maximum length of the password
  * @return 1 if successful, 0 otherwise
  */
 int getPassword(char *password, int maxLen) {
     int i = 0;
     char ch;
     
     while (1) {
         ch = getch(); /* Get character without echoing */
         
         if (ch == '\r' || ch == '\n') {
             /* Enter key pressed */
             password[i] = '\0';
             printf("\n");
             break;
         } else if (ch == '\b' && i > 0) {
             /* Backspace key pressed */
             i--;
             printf("\b \b"); /* Erase the last asterisk */
         } else if (ch == '\b') {
             /* Backspace at the beginning - do nothing */
         } else if (i < maxLen - 1 && isprint(ch)) {
             /* Printable character */
             password[i++] = ch;
             printf("*"); /* Print asterisk instead of the character */
         }
     }
     
     /* Check if password is empty */
     if (i == 0) {
         printf("\nPassword cannot be empty. Please try again.\n");
         return 0;
     }
     
     return 1;
 }
 
 /**
  * Get current date in YYYY-MM-DD format
  * 
  * @param dateStr Buffer to store the date string
  */
 void getCurrentDate(char *dateStr) {
     time_t now;
     struct tm *t;
     
     time(&now);
     t = localtime(&now);
     
     sprintf(dateStr, "%04d-%02d-%02d", 
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
 }
 
 /**
  * Get current date and time in YYYY-MM-DD HH:MM:SS format
  * 
  * @param dateTimeStr Buffer to store the date and time string
  */
 void getCurrentDateTime(char *dateTimeStr) {
     time_t now;
     struct tm *t;
     
     time(&now);
     t = localtime(&now);
     
     sprintf(dateTimeStr, "%04d-%02d-%02d %02d:%02d:%02d", 
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
             t->tm_hour, t->tm_min, t->tm_sec);
 }
 
 /**
  * Calculate the number of days between two dates in YYYY-MM-DD format
  * 
  * @param startDate Start date in YYYY-MM-DD format
  * @param endDate End date in YYYY-MM-DD format
  * @return Number of days between the dates, or -1 if error
  */
 int calculateDateDifference(const char *startDate, const char *endDate) {
     struct tm start = {0}, end = {0};
     time_t startTime, endTime;
     double diffSeconds;
     int diffDays;
     
     /* Parse start date */
     if (sscanf(startDate, "%d-%d-%d", &start.tm_year, &start.tm_mon, &start.tm_mday) != 3) {
         return -1;
     }
     
     /* Parse end date */
     if (sscanf(endDate, "%d-%d-%d", &end.tm_year, &end.tm_mon, &end.tm_mday) != 3) {
         return -1;
     }
     
     /* Adjust year and month for tm structure */
     start.tm_year -= 1900;
     start.tm_mon -= 1;
     end.tm_year -= 1900;
     end.tm_mon -= 1;
     
     /* Convert to time_t */
     startTime = mktime(&start);
     endTime = mktime(&end);
     
     if (startTime == -1 || endTime == -1) {
         return -1;
     }
     
     /* Calculate difference in seconds and convert to days */
     diffSeconds = difftime(endTime, startTime);
     diffDays = (int)(diffSeconds / (60 * 60 * 24));
     
     return diffDays;
 }
 
 /**
  * Validate a date string in YYYY-MM-DD format
  * 
  * @param dateStr Date string to validate
  * @return 1 if valid, 0 otherwise
  */
 int validateDate(const char *dateStr) {
     int year, month, day;
     struct tm date = {0};
     time_t t;
     char reconstructed[11];
     
     /* Check format */
     if (sscanf(dateStr, "%d-%d-%d", &year, &month, &day) != 3) {
         return 0;
     }
     
     /* Check ranges */
     if (year < 1900 || year > 2100 || month < 1 || month > 12 || day < 1 || day > 31) {
         return 0;
     }
     
     /* Check if date is valid using mktime */
     date.tm_year = year - 1900;
     date.tm_mon = month - 1;
     date.tm_mday = day;
     
     t = mktime(&date);
     if (t == -1) {
         return 0;
     }
     
     /* Verify that the date wasn't adjusted by mktime */
     sprintf(reconstructed, "%04d-%02d-%02d", 
             date.tm_year + 1900, date.tm_mon + 1, date.tm_mday);
     
     return strcmp(dateStr, reconstructed) == 0;
 }
 
 /**
  * Clear the console screen
  */
 void clearScreen(void) {
     system("cls"); /* Windows-specific */
 }
 
 /**
  * Pause execution until user presses a key
  */
 void pauseExecution(void) {
     printf("\nPress any key to continue...");
     getch();
 }
 
 /**
  * Generate a unique ID based on current time
  * 
  * @return A unique ID
  */
 int generateUniqueId(void) {
     static int lastId = 0;
     int newId;
     
     /* Use current time as seed for ID generation */
     time_t now = time(NULL);
     newId = (int)now % 1000000;
     
     /* Ensure ID is unique by incrementing if necessary */
     if (newId <= lastId) {
         newId = lastId + 1;
     }
     
     lastId = newId;
     return newId;
 }

/**
 * Validate a date string in YYYY-MM-DD format
 *
 * @param date The date string to validate
 * @return 1 if valid, 0 otherwise
 */
int isValidDate(const char *date) {
    return validateDate(date);
 }