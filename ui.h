/**
 * Hotel Management System
 * ui.h - User interface header
 * 
 * This file contains declarations for user interface functions
 * for displaying menus, formatting output, and handling screen display.
 */

 #ifndef UI_H
 #define UI_H

#include "auth.h" /* For User type definition */

/**
  * Display the welcome screen
  */
 void displayWelcomeScreen(void);
 
 /**
  * Display a header for a section
  * 
  * @param title The title of the section
  */
 void displayHeader(const char *title);
 
 /**
  * Display a formatted table header
  * 
  * @param columns Array of column titles
  * @param widths Array of column widths
  * @param numCols Number of columns
  */
 void displayTableHeader(const char **columns, const int *widths, int numCols);
 
 /**
  * Display a formatted table row
  * 
  * @param values Array of values for the row
  * @param widths Array of column widths
  * @param numCols Number of columns
  */
 void displayTableRow(const char **values, const int *widths, int numCols);
 
 /**
  * Display a formatted table footer
  * 
  * @param widths Array of column widths
  * @param numCols Number of columns
  */
 void displayTableFooter(const int *widths, int numCols);

/**
 * Display the reports menu and handle user input
 *
 * @param currentUser Pointer to the current user
 */
void reportsMenu(User *currentUser);

/**
  * Display a message box with a title and message
  * 
  * @param title The title of the message box
  * @param message The message to display
  * @param type The type of message (0=info, 1=success, 2=warning, 3=error)
  */
 void displayMessageBox(const char *title, const char *message, int type);
 
 /**
  * Display a progress bar
  * 
  * @param progress The progress value (0-100)
  * @param width The width of the progress bar
  */
 void displayProgressBar(int progress, int width);
 
 /**
  * Display a menu with options
  * 
  * @param title The title of the menu
  * @param options Array of option strings
  * @param numOptions Number of options
  * @return The selected option (1-based)
  */
 int displayMenu(const char *title, const char **options, int numOptions);
 
 #endif /* UI_H */