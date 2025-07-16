/**
 * Hotel Management System
 * utils.h - Utility functions header
 * 
 * This file contains declarations for utility functions used throughout the application
 * including date handling, input validation, and string manipulation.
 */

 #ifndef UTILS_H
 #define UTILS_H
 
 /**
  * Get integer input from user with validation
  */
 int getIntInput(const char *prompt, int min, int max);
 
 /**
  * Get double input from user with validation
  */
 double getDoubleInput(const char *prompt, double min, double max);
 
 /**
  * Get string input from user with validation
  */
 void getStringInput(const char *prompt, char *output, int maxLen);
 
 /**
  * Get password input from user (masked with asterisks)
  */
 void getPassword(char *password, int maxLen);

/**
 * Validate a date string in YYYY-MM-DD format
 */
int validateDate(const char *dateStr);

 /**
  * Get current date in YYYY-MM-DD format
  */
 void getCurrentDate(char *dateStr);
 
 /**
  * Get current date and time in YYYY-MM-DD HH:MM:SS format
  */
 void getCurrentDateTime(char *dateTimeStr);
 
 /**
  * Calculate the number of days between two dates in YYYY-MM-DD format
  */
 int calculateDateDifference(const char *startDate, const char *endDate);
 
 /**
  * Clear the console screen
  */
 void clearScreen(void);
 
 /**
  * Pause execution until user presses a key
  */
 void pauseExecution(void);
 
 #endif /* UTILS_H */