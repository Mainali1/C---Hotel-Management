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

#define USERS_FILE "data/users.dat"
#define TEMP_FILE "data/temp.dat"
#define MAX_USERS 100

/* Simple password hashing function for academic purposes
   Note: In a real system, use a proper cryptographic hash function */
void hashPassword(const char *password, char *hashedPassword) {
    int i;
    unsigned int hash = 0;
    
    /* Simple hash algorithm */
    for (i = 0; password[i] != '\0'; i++) {
        hash = hash * 31 + password[i];
    }
    
    /* Convert hash to string */
    sprintf(hashedPassword, "%u", hash);
}

/* Verify if a password matches the stored hash */
int verifyPassword(const char *password, const char *hashedPassword) {
    char computedHash[MAX_PASSWORD_LEN];
    hashPassword(password, computedHash);
    return strcmp(computedHash, hashedPassword) == 0;
}

/* Initialize user data with default admin account */
int initializeUserData(void) {
    FILE *fp;
    User defaultAdmin;
    char hashedPassword[MAX_PASSWORD_LEN];

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
    strcpy(defaultAdmin.lastLogin, "");
    defaultAdmin.active = 1;

    /* Write default admin to file */
    if (fwrite(&defaultAdmin, sizeof(User), 1, fp) != 1) {
        printf("\nError: Could not write default admin account.\n");
        fclose(fp);
        return 0;
    }

    fclose(fp);
    printf("\nDefault admin account created successfully.\n");
    return 1;
}

/* Login user function */
int loginUser(User *user) {
    FILE *fp;
    User tempUser;
    char username[MAX_USERNAME_LEN];
    char password[MAX_PASSWORD_LEN];
    int found = 0;
    
    /* Get username and password */
    clearScreen();
    printf("===== LOGIN =====\n");
    printf("Username: ");
    scanf("%19s", username);
    printf("Password: ");
    getPassword(password, MAX_PASSWORD_LEN);
    
    /* Open users file */
    fp = fopen(USERS_FILE, "rb");
    if (fp == NULL) {
        if (!initializeUserData()) {
            printf("\nError: Could not initialize user data.\n");
            return 0;
        }
        fp = fopen(USERS_FILE, "rb");
        if (fp == NULL) {
            printf("\nError: Could not open user file.\n");
            return 0;
        }
    }
    
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
    
    if (!found) {
        printf("\nInvalid username or password.\n");
        return 0;
    }
    
    return 1;
}

/* Update user's last login time */
static void updateUserLastLogin(User *user) {
    FILE *fp, *tempFp;
    User tempUser;
    
    fp = fopen(USERS_FILE, "rb");
    if (fp == NULL) {
        return;
    }
    
    tempFp = fopen(TEMP_FILE, "wb");
    if (tempFp == NULL) {
        fclose(fp);
        return;
    }
    
    /* Copy all users to temp file, updating the matching one */
    while (fread(&tempUser, sizeof(User), 1, fp) == 1) {
        if (tempUser.id == user->id) {
            /* Update last login */
            strcpy(tempUser.lastLogin, user->lastLogin);
        }
        fwrite(&tempUser, sizeof(User), 1, tempFp);
    }
    
    fclose(fp);
    fclose(tempFp);
    
    /* Replace original file with temp file */
    remove(USERS_FILE);
    rename(TEMP_FILE, USERS_FILE);
}

/* Initialize admin user if no users exist */
int initializeAdminUser(void) {
    FILE *fp;
    User adminUser;
    char hashedPassword[MAX_PASSWORD_LEN];
    
    /* Create data directory if it doesn't exist */
    createDirectoryIfNotExists("data");
    
    /* Check if users file already exists */
    fp = fopen(USERS_FILE, "rb");
    if (fp != NULL) {
        /* Check if file has content */
        fseek(fp, 0, SEEK_END);
        if (ftell(fp) > 0) {
            fclose(fp);
            return 0; /* Users already exist */
        }
        fclose(fp);
    }
    
    /* Create admin user */
    adminUser.id = 1;
    strcpy(adminUser.username, "admin");
    hashPassword("admin123", hashedPassword);
    strcpy(adminUser.password, hashedPassword);
    strcpy(adminUser.name, "System Administrator");
    adminUser.role = ROLE_ADMIN;
    strcpy(adminUser.lastLogin, "Never");
    adminUser.active = 1;
    
    /* Write admin user to file */
    fp = fopen(USERS_FILE, "wb");
    if (fp == NULL) {
        return 0; /* Failed to create file */
    }
    
    fwrite(&adminUser, sizeof(User), 1, fp);
    fclose(fp);
    
    printf("\nAdmin user created. Username: admin, Password: admin123\n");
    printf("Please change the password after first login.\n");
    
    return 1; /* Admin user created successfully */
}

/* Add a new user */
int addUser(User *currentUser) {
    FILE *fp;
    User newUser, tempUser;
    char password[MAX_PASSWORD_LEN];
    char confirmPassword[MAX_PASSWORD_LEN];
    int nextId = 1;
    
    /* Check if current user is admin */
    if (currentUser->role != ROLE_ADMIN) {
        printf("\nAccess denied. Admin privileges required.\n");
        return 0;
    }
    
    /* Open users file to find next available ID */
    fp = fopen(USERS_FILE, "rb");
    if (fp != NULL) {
        while (fread(&tempUser, sizeof(User), 1, fp) == 1) {
            if (tempUser.id >= nextId) {
                nextId = tempUser.id + 1;
            }
        }
        fclose(fp);
    }
    
    /* Get user details */
    clearScreen();
    printf("===== ADD NEW USER =====\n");
    
    newUser.id = nextId;
    
    printf("Username: ");
    scanf("%19s", newUser.username);
    
    /* Check if username already exists */
    if (isUsernameExists(newUser.username)) {
        printf("\nUsername already exists. Please choose another.\n");
        return 0;
    }
    
    /* Get and confirm password */
    do {
        printf("Password: ");
        getPassword(password, MAX_PASSWORD_LEN);
        
        printf("Confirm Password: ");
        getPassword(confirmPassword, MAX_PASSWORD_LEN);
        
        if (strcmp(password, confirmPassword) != 0) {
            printf("\nPasswords do not match. Please try again.\n");
        }
    } while (strcmp(password, confirmPassword) != 0);
    
    /* Hash the password */
    hashPassword(password, newUser.password);
    
    /* Get other user details */
    printf("Full Name: ");
    getchar(); /* Consume newline */
    fgets(newUser.name, MAX_NAME_LEN, stdin);
    newUser.name[strcspn(newUser.name, "\n")] = '\0'; /* Remove newline */
    
    printf("Role (1=Admin, 2=Staff): ");
    newUser.role = getIntInput("", 1, 2);
    
    strcpy(newUser.lastLogin, "Never");
    newUser.active = 1;
    
    /* Write new user to file */
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
        return 0; /* File doesn't exist, so username doesn't exist */
    }
    
    while (fread(&tempUser, sizeof(User), 1, fp) == 1) {
        if (strcmp(tempUser.username, username) == 0) {
            fclose(fp);
            return 1; /* Username exists */
        }
    }
    
    fclose(fp);
    return 0; /* Username doesn't exist */
}

/* Modify an existing user */
int modifyUser(User *currentUser, int userId) {
    FILE *fp, *tempFp;
    User tempUser;
    int found = 0;
    int choice;
    char newName[MAX_NAME_LEN];
    int newRole;
    int newActive;
    
    /* Check if current user is admin */
    if (currentUser->role != ROLE_ADMIN) {
        printf("\nAccess denied. Admin privileges required.\n");
        return 0;
    }
    
    /* Open users file */
    fp = fopen(USERS_FILE, "rb");
    if (fp == NULL) {
        printf("\nError: Could not open users file.\n");
        return 0;
    }
    
    /* Find user to modify */
    while (fread(&tempUser, sizeof(User), 1, fp) == 1) {
        if (tempUser.id == userId) {
            found = 1;
            break;
        }
    }
    
    fclose(fp);
    
    if (!found) {
        printf("\nUser with ID %d not found.\n", userId);
        return 0;
    }
    
    /* Prevent modifying own account */
    if (userId == currentUser->id) {
        printf("\nCannot modify your own account through this menu.\n");
        printf("Use the Change Password option instead.\n");
        return 0;
    }
    
    /* Display user details and modification menu */
    clearScreen();
    printf("===== MODIFY USER =====\n");
    printf("ID: %d\n", tempUser.id);
    printf("Username: %s\n", tempUser.username);
    printf("Name: %s\n", tempUser.name);
    printf("Role: %s\n", tempUser.role == ROLE_ADMIN ? "Administrator" : "Staff");
    printf("Last Login: %s\n", tempUser.lastLogin);
    printf("Status: %s\n", tempUser.active ? "Active" : "Inactive");
    
    printf("\nWhat would you like to modify?\n");
    printf("1. Name\n");
    printf("2. Role\n");
    printf("3. Reset Password\n");
    printf("4. Toggle Active Status\n");
    printf("0. Cancel\n");
    
    choice = getIntInput("Enter your choice: ", 0, 4);
    
    if (choice == 0) {
        return 0; /* Cancelled */
    }
    
    /* Open files for modification */
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
    
    /* Process each user */
    while (fread(&tempUser, sizeof(User), 1, fp) == 1) {
        if (tempUser.id == userId) {
            /* Modify user based on choice */
            switch (choice) {
                case 1: /* Modify name */
                    printf("Enter new name: ");
                    getchar(); /* Consume newline */
                    fgets(newName, MAX_NAME_LEN, stdin);
                    newName[strcspn(newName, "\n")] = '\0'; /* Remove newline */
                    strcpy(tempUser.name, newName);
                    break;
                    
                case 2: /* Modify role */
                    printf("Enter new role (1=Admin, 2=Staff): ");
                    newRole = getIntInput("", 1, 2);
                    tempUser.role = newRole;
                    break;
                    
                case 3: /* Reset password */
                    hashPassword("password123", tempUser.password);
                    printf("\nPassword reset to: password123\n");
                    printf("User should change it at next login.\n");
                    break;
                    
                case 4: /* Toggle active status */
                    tempUser.active = !tempUser.active;
                    printf("\nUser status changed to: %s\n", 
                           tempUser.active ? "Active" : "Inactive");
                    break;
            }
        }
        
        /* Write user to temp file */
        fwrite(&tempUser, sizeof(User), 1, tempFp);
    }
    
    fclose(fp);
    fclose(tempFp);
    
    /* Replace original file with temp file */
    remove(USERS_FILE);
    rename(TEMP_FILE, USERS_FILE);
    
    printf("\nUser modified successfully.\n");
    return 1;
}

/* Delete a user */
int deleteUser(User *currentUser, int userId) {
    FILE *fp, *tempFp;
    User tempUser;
    int found = 0;
    char confirm;
    
    /* Check if current user is admin */
    if (currentUser->role != ROLE_ADMIN) {
        printf("\nAccess denied. Admin privileges required.\n");
        return 0;
    }
    
    /* Prevent deleting own account */
    if (userId == currentUser->id) {
        printf("\nCannot delete your own account.\n");
        return 0;
    }
    
    /* Open users file */
    fp = fopen(USERS_FILE, "rb");
    if (fp == NULL) {
        printf("\nError: Could not open users file.\n");
        return 0;
    }
    
    /* Find user to delete */
    while (fread(&tempUser, sizeof(User), 1, fp) == 1) {
        if (tempUser.id == userId) {
            found = 1;
            break;
        }
    }
    
    fclose(fp);
    
    if (!found) {
        printf("\nUser with ID %d not found.\n", userId);
        return 0;
    }
    
    /* Confirm deletion */
    printf("\nAre you sure you want to delete user '%s'? (y/n): ", tempUser.username);
    scanf(" %c", &confirm);
    
    if (confirm != 'y' && confirm != 'Y') {
        printf("\nDeletion cancelled.\n");
        return 0;
    }
    
    /* Open files for deletion */
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
    
    /* Copy all users except the one to delete */
    while (fread(&tempUser, sizeof(User), 1, fp) == 1) {
        if (tempUser.id != userId) {
            fwrite(&tempUser, sizeof(User), 1, tempFp);
        }
    }
    
    fclose(fp);
    fclose(tempFp);
    
    /* Replace original file with temp file */
    remove(USERS_FILE);
    rename(TEMP_FILE, USERS_FILE);
    
    printf("\nUser deleted successfully.\n");
    return 1;
}

/* List all users */
void listUsers(User *currentUser) {
    FILE *fp;
    User tempUser;
    int count = 0;
    
    /* Check if current user is admin */
    if (currentUser->role != ROLE_ADMIN) {
        printf("\nAccess denied. Admin privileges required.\n");
        return;
    }
    
    /* Open users file */
    fp = fopen(USERS_FILE, "rb");
    if (fp == NULL) {
        printf("\nError: Could not open users file.\n");
        return;
    }
    
    /* Display header */
    clearScreen();
    printf("===== USER LIST =====\n");
    printf("%-5s %-20s %-30s %-15s %-20s %-10s\n", 
           "ID", "Username", "Name", "Role", "Last Login", "Status");
    printf("---------------------------------------------------------------------------------\n");
    
    /* Display each user */
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
    
    /* Display footer */
    printf("---------------------------------------------------------------------------------\n");
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
    
    /* Get current password */
    printf("\nEnter current password: ");
    getPassword(currentPassword, MAX_PASSWORD_LEN);
    
    /* Verify current password */
    if (!verifyPassword(currentPassword, currentUser->password)) {
        printf("\nIncorrect password.\n");
        return 0;
    }
    
    /* Get and confirm new password */
    do {
        printf("Enter new password: ");
        getPassword(newPassword, MAX_PASSWORD_LEN);
        
        printf("Confirm new password: ");
        getPassword(confirmPassword, MAX_PASSWORD_LEN);
        
        if (strcmp(newPassword, confirmPassword) != 0) {
            printf("\nPasswords do not match. Please try again.\n");
        }
    } while (strcmp(newPassword, confirmPassword) != 0);
    
    /* Hash the new password */
    hashPassword(newPassword, hashedPassword);
    
    /* Open files for modification */
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
    
    /* Update password in file */
    while (fread(&tempUser, sizeof(User), 1, fp) == 1) {
        if (tempUser.id == currentUser->id) {
            strcpy(tempUser.password, hashedPassword);
            strcpy(currentUser->password, hashedPassword); /* Update in memory too */
        }
        fwrite(&tempUser, sizeof(User), 1, tempFp);
    }
    
    fclose(fp);
    fclose(tempFp);
    
    /* Replace original file with temp file */
    remove(USERS_FILE);
    rename(TEMP_FILE, USERS_FILE);
    
    printf("\nPassword changed successfully.\n");
    return 1;
}

/* User management menu */
void userManagementMenu(User *currentUser) {
    int choice;
    int userId;
    
    /* Check if current user is admin */
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
            case 1: /* List Users */
                listUsers(currentUser);
                break;
                
            case 2: /* Add User */
                addUser(currentUser);
                break;
                
            case 3: /* Modify User */
                listUsers(currentUser);
                userId = getIntInput("\nEnter user ID to modify (0 to cancel): ", 0, 9999);
                if (userId != 0) {
                    modifyUser(currentUser, userId);
                }
                break;
                
            case 4: /* Delete User */
                listUsers(currentUser);
                userId = getIntInput("\nEnter user ID to delete (0 to cancel): ", 0, 9999);
                if (userId != 0) {
                    deleteUser(currentUser, userId);
                }
                break;
                
            case 0: /* Back to Main Menu */
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
}
