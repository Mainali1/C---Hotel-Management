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
  * 
  * @param prompt The prompt to display to the user
  * @param min Minimum acceptable value
  * @param max Maximum acceptable value
  * @return The validated integer input
  */
 int getIntInput(const char *prompt, int min, int max);
 
 /**
  * Get double input from user with validation
  * 
  * @param prompt The prompt to display to the user
  * @param min Minimum acceptable value
  * @param max Maximum acceptable value
  * @return The validated double input
  */
 double getDoubleInput(const char *prompt, double min, double max);
 
 /**
  * Get string input from user with validation
  * 
  * @param prompt The prompt to display to the user
  * @param output Buffer to store the input
  * @param maxLen Maximum length of the input
  * @return 1 if successful, 0 otherwise
  */
 int getStringInput(const char *prompt, char *output, int maxLen);
 
 /**
  * Get password input from user (masked with asterisks)
  * 
  * @param password Buffer to store the password
  * @param maxLen Maximum length of the password
  * @return 1 if successful, 0 otherwise
  */
 int getPassword(char *password, int maxLen);

/**
 * Validate a date string in YYYY-MM-DD format
 *
 * @param date The date string to validate
 * @return 1 if valid, 0 otherwise
 */
int isValidDate(const char *date);

 /**
  * Get current date in YYYY-MM-DD format
  * 
  * @param dateStr Buffer to store the date string
  */
 void getCurrentDate(char *dateStr);
 
 /**
  * Get current date and time in YYYY-MM-DD HH:MM:SS format
  * 
  * @param dateTimeStr Buffer to store the date and time string
  */
 void getCurrentDateTime(char *dateTimeStr);
 
 /**
  * Calculate the number of days between two dates in YYYY-MM-DD format
  * 
  * @param startDate Start date in YYYY-MM-DD format
  * @param endDate End date in YYYY-MM-DD format
  * @return Number of days between the dates, or -1 if error
  */
 int calculateDateDifference(const char *startDate, const char *endDate);
 
 /**
  * Validate a date string in YYYY-MM-DD format
  * 
  * @param dateStr Date string to validate
  * @return 1 if valid, 0 otherwise
  */
 int validateDate(const char *dateStr);
 
 /**
  * Clear the console screen
  */
 void clearScreen(void);
 
 /**
  * Pause execution until user presses a key
  */
 void pauseExecution(void);
 
 /**
  * Generate a unique ID based on current time
  * 
  * @return A unique ID
  */
 int generateUniqueId(void);
 
 #endif /* UTILS_H */