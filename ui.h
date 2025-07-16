/**
 * Hotel Management System
 * ui.h - User interface header
 * 
 * This file contains declarations for user interface functions
 * for displaying menus, formatting output, and handling screen display.
 */

 #ifndef UI_H
 #define UI_H

#include "auth.h"

/**
  * Display the welcome screen
  */
 void displayWelcomeScreen(void);
 
 /**
  * Display a header for a section
  */
 void displayHeader(const char *title);
 
/**
 * Display the reports menu and handle user input
 */
void reportsMenu(User *currentUser);

 #endif /* UI_H */
