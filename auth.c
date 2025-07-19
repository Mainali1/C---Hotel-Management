/**
 * Hotel Management System
 * auth.c - User authentication implementation
 * 
 * This file implements the user authentication functionality
 * including login, user management, and password handling.
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <time.h>
 #include "auth.h"
 #include "fileio.h"
 #include "utils.h"
 #include "ui.h"
 
 /* Forward declarations for static functions */
 static void updateUserLastLogin(User *user);
 static int isUsernameExists(const char *username);
 
 #define TEMP_FILE "data/temp_users.dat"
 
 /* Simple password hashing function for academic purposes*/
 void hashPassword(const char *password, char *hashedPassword) {
     unsigned long hash = 5381;
     int c;
 
     while ((c = *password++)) {
         hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
     }
     
     sprintf(hashedPassword, "%lu", hash);
 }
 
 /* Verify if a password matches the stored hash */
 int verifyPassword(const char *password, const char *hashedPassword) {
     char computedHash[MAX_PASSWORD_LEN];
     hashPassword(password, computedHash);
     return strcmp(computedHash, hashedPassword) == 0;
 }
 
 /* Initialize user data with default admin account if file doesn't exist or is empty */
 int initializeUserData(void) {
     FILE *fp;
     User defaultAdmin;
     char hashedPassword[MAX_PASSWORD_LEN];
 
     fp = fopen(USERS_FILE, "rb");
     if (fp != NULL) {
         fseek(fp, 0, SEEK_END);
         if (ftell(fp) > 0) {
             fclose(fp);
             return 1; // File exists and is not empty
         }
         fclose(fp);
     }
     
     printf("\nNo users found. Creating default admin account...");
 
     /* Create users file */
     fp = fopen(USERS_FILE, "wb");
     if (!fp) {
         printf("\nError: Could not create user file.\n");
         return 0;
     }
 
     /* Initialize default admin account */
     defaultAdmin.id = 1;
     strcpy(defaultAdmin.username, "admin");
     hashPassword("admin123", hashedPassword);
     strcpy(defaultAdmin.password, hashedPassword);
     strcpy(defaultAdmin.name, "System Administrator");
     defaultAdmin.role = ROLE_ADMIN;
     strcpy(defaultAdmin.lastLogin, "Never");
     defaultAdmin.active = 1;
 
     /* Write default admin to file */
     if (fwrite(&defaultAdmin, sizeof(User), 1, fp) != 1) {
         printf("\nError: Could not write default admin account.\n");
         fclose(fp);
         return 0;
     }
 
     fclose(fp);
     printf("\nDefault admin account created. User: admin, Pass: admin123\n");
     return 1;
 }
 
 /* Login user function */
 int loginUser(User *user) {
     FILE *fp;
     User tempUser;
     char username[MAX_USERNAME_LEN];
     char password[MAX_PASSWORD_LEN];
     int found = 0;
     
     /* Open users file */
     fp = fopen(USERS_FILE, "rb");
     if (fp == NULL) {
         printf("\nError: Could not open user file. Please ensure the system is initialized.\n");
         return 0;
     }
     
     /* Get username and password */
     clearScreen();
     printf("===== LOGIN =====\n");
     printf("Username: ");
     getStringInput(NULL, username, MAX_USERNAME_LEN);
     printf("Password: ");
     getPassword(password, MAX_PASSWORD_LEN);
     
     /* Search for user */
     while (fread(&tempUser, sizeof(User), 1, fp) == 1) {
         if (strcmp(tempUser.username, username) == 0 && tempUser.active) {
             if (verifyPassword(password, tempUser.password)) {
                 found = 1;
                 *user = tempUser;
                 updateUserLastLogin(user);
                 break;
             }
         }
     }
     
     fclose(fp);
     
     return found;
 }
 
 /* Update user's last login time in memory and file */
 static void updateUserLastLogin(User *user) {
     FILE *fp, *tempFp;
     User tempUser;
     
     // Update the lastLogin field in the user struct passed to the function
     getCurrentDateTime(user->lastLogin);
 
     fp = fopen(USERS_FILE, "rb");
     if (fp == NULL) {
         printf("Error: Cannot open users file to update login time.\n");
         return;
     }
     
     tempFp = fopen(TEMP_FILE, "wb");
     if (tempFp == NULL) {
         printf("Error: Cannot create temp file to update login time.\n");
         fclose(fp);
         return;
     }
     
     /* Copy all users to temp file, updating the matching one */
     while (fread(&tempUser, sizeof(User), 1, fp) == 1) {
         if (tempUser.id == user->id) {
             strcpy(tempUser.lastLogin, user->lastLogin);
         }
         fwrite(&tempUser, sizeof(User), 1, tempFp);
     }
     
     fclose(fp);
     fclose(tempFp);
     
     remove(USERS_FILE);
     rename(TEMP_FILE, USERS_FILE);
 }
 
 /* Add a new user */
 int addUser(User *currentUser) {
     FILE *fp;
     User newUser, tempUser;
     char password[MAX_PASSWORD_LEN];
     char confirmPassword[MAX_PASSWORD_LEN];
     int nextId = 1;
     
     if (currentUser->role != ROLE_ADMIN) {
         printf("\nAccess denied. Admin privileges required.\n");
         return 0;
     }
     
     fp = fopen(USERS_FILE, "rb");
     if (fp != NULL) {
         while (fread(&tempUser, sizeof(User), 1, fp) == 1) {
             if (tempUser.id >= nextId) {
                 nextId = tempUser.id + 1;
             }
         }
         fclose(fp);
     }
     
     clearScreen();
     printf("===== ADD NEW USER =====\n");
     
     newUser.id = nextId;
     
     printf("Username: ");
     getStringInput(NULL, newUser.username, MAX_USERNAME_LEN);
     
     if (isUsernameExists(newUser.username)) {
         printf("\nUsername already exists. Please choose another.\n");
         return 0;
     }
     
     do {
         printf("Password: ");
         getPassword(password, MAX_PASSWORD_LEN);
         
         printf("Confirm Password: ");
         getPassword(confirmPassword, MAX_PASSWORD_LEN);
         
         if (strcmp(password, confirmPassword) != 0) {
             printf("\nPasswords do not match. Please try again.\n");
         }
     } while (strcmp(password, confirmPassword) != 0);
     
     hashPassword(password, newUser.password);
     
     printf("Full Name: ");
     getStringInput(NULL, newUser.name, MAX_NAME_LEN);
     
     printf("Role (1=Admin, 2=Staff): ");
     newUser.role = getIntInput("", 1, 2);
     
     strcpy(newUser.lastLogin, "Never");
     newUser.active = 1;
     
     fp = fopen(USERS_FILE, "ab");
     if (fp == NULL) {
         printf("\nError: Could not open users file.\n");
         return 0;
     }
     
     fwrite(&newUser, sizeof(User), 1, fp);
     fclose(fp);
     
     printf("\nUser added successfully.\n");
     return 1;
 }
 
 /* Check if username already exists */
 static int isUsernameExists(const char *username) {
     FILE *fp;
     User tempUser;
     
     fp = fopen(USERS_FILE, "rb");
     if (fp == NULL) {
         return 0; 
     }
     
     while (fread(&tempUser, sizeof(User), 1, fp) == 1) {
         if (strcmp(tempUser.username, username) == 0) {
             fclose(fp);
             return 1;
         }
     }
     
     fclose(fp);
     return 0;
 }
 
 /* Modify an existing user */
 int modifyUser(User *currentUser, int userId) {
     FILE *fp, *tempFp;
     User tempUser;
     int found = 0;
     
     if (currentUser->role != ROLE_ADMIN) {
         printf("\nAccess denied. Admin privileges required.\n");
         return 0;
     }
     
     if (userId == currentUser->id) {
         printf("\nCannot modify your own account through this menu.\n");
         printf("Use the 'Change Password' option instead.\n");
         return 0;
     }
     
     fp = fopen(USERS_FILE, "rb");
     if (fp == NULL) {
         printf("\nError: Could not open users file.\n");
         return 0;
     }
     
     tempFp = fopen(TEMP_FILE, "wb");
     if (tempFp == NULL) {
         fclose(fp);
         printf("\nError: Could not create temporary file.\n");
         return 0;
     }
     
     while (fread(&tempUser, sizeof(User), 1, fp) == 1) {
         if (tempUser.id == userId) {
             found = 1;
             clearScreen();
             printf("===== MODIFY USER: %s =====\n", tempUser.username);
             printf("1. Change Name (Current: %s)\n", tempUser.name);
             printf("2. Change Role (Current: %s)\n", tempUser.role == ROLE_ADMIN ? "Admin" : "Staff");
             printf("3. Reset Password\n");
             printf("4. Toggle Active Status (Current: %s)\n", tempUser.active ? "Active" : "Inactive");
             printf("0. Cancel\n");
             
             int choice = getIntInput("Enter your choice: ", 0, 4);
 
             switch (choice) {
                 case 1:
                     printf("Enter new name: ");
                     getStringInput(NULL, tempUser.name, MAX_NAME_LEN);
                     break;
                 case 2:
                     printf("Enter new role (1=Admin, 2=Staff): ");
                     tempUser.role = getIntInput("", 1, 2);
                     break;
                 case 3:
                     hashPassword("password123", tempUser.password);
                     printf("\nPassword has been reset to: password123\n");
                     break;
                 case 4:
                     tempUser.active = !tempUser.active;
                     printf("\nUser status is now: %s\n", tempUser.active ? "Active" : "Inactive");
                     break;
                 case 0:
                     break; // No changes
             }
         }
         fwrite(&tempUser, sizeof(User), 1, tempFp);
     }
     
     fclose(fp);
     fclose(tempFp);
     
     if (found) {
         remove(USERS_FILE);
         rename(TEMP_FILE, USERS_FILE);
         printf("\nUser modified successfully.\n");
     } else {
         remove(TEMP_FILE);
         printf("\nUser with ID %d not found.\n", userId);
     }
     return found;
 }
 
 /* Delete a user */
 int deleteUser(User *currentUser, int userId) {
     FILE *fp, *tempFp;
     User tempUser;
     int found = 0;
     char confirm;
     
     if (currentUser->role != ROLE_ADMIN) {
         printf("\nAccess denied. Admin privileges required.\n");
         return 0;
     }
     
     if (userId == currentUser->id) {
         printf("\nCannot delete your own account.\n");
         return 0;
     }
     
     fp = fopen(USERS_FILE, "rb");
     if (fp == NULL) {
         printf("\nError: Could not open users file.\n");
         return 0;
     }
     
     tempFp = fopen(TEMP_FILE, "wb");
     if (tempFp == NULL) {
         fclose(fp);
         printf("\nError: Could not create temporary file.\n");
         return 0;
     }
     
     while (fread(&tempUser, sizeof(User), 1, fp) == 1) {
         if (tempUser.id == userId) {
             found = 1;
             printf("\nAre you sure you want to delete user '%s' (ID: %d)? (y/n): ", tempUser.username, tempUser.id);
             scanf(" %c", &confirm);
             if (confirm == 'y' || confirm == 'Y') {
                 // Do not write this user to the temp file, effectively deleting them
                 printf("User deleted.\n");
             } else {
                 fwrite(&tempUser, sizeof(User), 1, tempFp); // Keep the user
                 printf("Deletion cancelled.\n");
             }
         } else {
             fwrite(&tempUser, sizeof(User), 1, tempFp);
         }
     }
     
     fclose(fp);
     fclose(tempFp);
     
     if (found) {
         remove(USERS_FILE);
         rename(TEMP_FILE, USERS_FILE);
     } else {
         remove(TEMP_FILE);
         printf("\nUser with ID %d not found.\n", userId);
     }
     return found;
 }
 
 /* List all users */
 void listUsers(User *currentUser) {
     FILE *fp;
     User tempUser;
     int count = 0;
     
     if (currentUser->role != ROLE_ADMIN) {
         printf("\nAccess denied. Admin privileges required.\n");
         return;
     }
     
     fp = fopen(USERS_FILE, "rb");
     if (fp == NULL) {
         printf("\nError: Could not open users file.\n");
         return;
     }
     
     clearScreen();
     printf("===== USER LIST =====\n");
     printf("%-5s %-20s %-30s %-15s %-20s %-10s\n", 
            "ID", "Username", "Name", "Role", "Last Login", "Status");
     printf("--------------------------------------------------------------------------------------------------------\n");
     
     while (fread(&tempUser, sizeof(User), 1, fp) == 1) {
         printf("%-5d %-20s %-30s %-15s %-20s %-10s\n", 
                tempUser.id, 
                tempUser.username, 
                tempUser.name, 
                tempUser.role == ROLE_ADMIN ? "Administrator" : "Staff", 
                tempUser.lastLogin, 
                tempUser.active ? "Active" : "Inactive");
         count++;
     }
     
     fclose(fp);
     
     printf("--------------------------------------------------------------------------------------------------------\n");
     printf("Total users: %d\n", count);
 }
 
 /* Change password for current user */
 int changePassword(User *currentUser) {
     FILE *fp, *tempFp;
     User tempUser;
     char currentPassword[MAX_PASSWORD_LEN];
     char newPassword[MAX_PASSWORD_LEN];
     char confirmPassword[MAX_PASSWORD_LEN];
     char hashedPassword[MAX_PASSWORD_LEN];
     
     printf("\nEnter current password: ");
     getPassword(currentPassword, MAX_PASSWORD_LEN);
     
     if (!verifyPassword(currentPassword, currentUser->password)) {
         printf("\nIncorrect password.\n");
         return 0;
     }
     
     do {
         printf("Enter new password: ");
         getPassword(newPassword, MAX_PASSWORD_LEN);
         
         printf("Confirm new password: ");
         getPassword(confirmPassword, MAX_PASSWORD_LEN);
         
         if (strcmp(newPassword, confirmPassword) != 0) {
             printf("\nPasswords do not match. Please try again.\n");
         }
     } while (strcmp(newPassword, confirmPassword) != 0);
     
     hashPassword(newPassword, hashedPassword);
     
     fp = fopen(USERS_FILE, "rb");
     if (fp == NULL) {
         printf("\nError: Could not open users file.\n");
         return 0;
     }
     
     tempFp = fopen(TEMP_FILE, "wb");
     if (tempFp == NULL) {
         fclose(fp);
         printf("\nError: Could not create temporary file.\n");
         return 0;
     }
     
     while (fread(&tempUser, sizeof(User), 1, fp) == 1) {
         if (tempUser.id == currentUser->id) {
             strcpy(tempUser.password, hashedPassword);
             strcpy(currentUser->password, hashedPassword);
         }
         fwrite(&tempUser, sizeof(User), 1, tempFp);
     }
     
     fclose(fp);
     fclose(tempFp);
     
     remove(USERS_FILE);
     rename(TEMP_FILE, USERS_FILE);
     
     printf("\nPassword changed successfully.\n");
     return 1;
 }
 
 /* User management menu */
 void userManagementMenu(User *currentUser) {
     int choice;
     int userId;
     
     if (currentUser->role != ROLE_ADMIN) {
         printf("\nAccess denied. Admin privileges required.\n");
         return;
     }
     
     do {
         clearScreen();
         printf("===== USER MANAGEMENT =====\n");
         printf("1. List Users\n");
         printf("2. Add User\n");
         printf("3. Modify User\n");
         printf("4. Delete User\n");
         printf("0. Back to Main Menu\n");
         printf("============================\n");
         
         choice = getIntInput("Enter your choice: ", 0, 4);
         
         switch (choice) {
             case 1:
                 listUsers(currentUser);
                 break;
             case 2:
                 addUser(currentUser);
                 break;
             case 3:
                 listUsers(currentUser);
                 userId = getIntInput("\nEnter user ID to modify (0 to cancel): ", 0, 9999);
                 if (userId != 0) {
                     modifyUser(currentUser, userId);
                 }
                 break;
             case 4:
                 listUsers(currentUser);
                 userId = getIntInput("\nEnter user ID to delete (0 to cancel): ", 0, 9999);
                 if (userId != 0) {
                     deleteUser(currentUser, userId);
                 }
                 break;
             case 0:
                 return;
             default:
                 printf("\nInvalid choice. Please try again.\n");
         }
         
         pauseExecution();
         
     } while (choice != 0);
 }
