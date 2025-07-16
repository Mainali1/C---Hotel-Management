/**
 * Hotel Management System
 * auth.h - User authentication header file
 * 
 * This file contains declarations for user authentication functionality
 * including user structure, role definitions, and function prototypes.
 */

 #ifndef AUTH_H
 #define AUTH_H
 
 /* Role definitions */
 #define ROLE_ADMIN 1
 #define ROLE_STAFF 2
 
 /* File paths */
 #define USERS_FILE "data/users.dat"
 
 /* Maximum lengths for strings */
 #define MAX_USERNAME_LEN 20
 #define MAX_PASSWORD_LEN 65 // Increased for better hashing in future
 #define MAX_NAME_LEN 50
 
 /**
  * User structure - Stores user information
  */
 typedef struct {
     int id;                         /* Unique user ID */
     char username[MAX_USERNAME_LEN]; /* Username for login */
     char password[MAX_PASSWORD_LEN]; /* Password (hashed) */
     char name[MAX_NAME_LEN];         /* Full name of the user */
     int role;                        /* User role (admin or staff) */
     char lastLogin[20];              /* Last login timestamp */
     int active;                      /* Whether the account is active */
 } User;
 
 /* Function prototypes */
 
 /**
  * Attempt to log in a user
  * 
  * @param user Pointer to User structure to store logged in user info
  * @return 1 if login successful, 0 otherwise
  */
 int loginUser(User *user);
 
 /**
  * Add a new user to the system
  * 
  * @param currentUser The user performing the action (must be admin)
  * @return 1 if successful, 0 otherwise
  */
 int addUser(User *currentUser);
 
 /**
  * Initialize user data file with default admin account
  * 
  * @return 1 if successful, 0 otherwise
  */
 int initializeUserData(void);
 
 /**
  * Modify an existing user
  * 
  * @param currentUser The user performing the action (must be admin)
  * @param userId ID of the user to modify
  * @return 1 if successful, 0 otherwise
  */
 int modifyUser(User *currentUser, int userId);
 
 /**
  * Delete a user from the system
  * 
  * @param currentUser The user performing the action (must be admin)
  * @param userId ID of the user to delete
  * @return 1 if successful, 0 otherwise
  */
 int deleteUser(User *currentUser, int userId);
 
 /**
  * List all users in the system
  * 
  * @param currentUser The user performing the action (must be admin)
  */
 void listUsers(User *currentUser);
 
 /**
  * Change the password for the current user
  * 
  * @param currentUser The user whose password is being changed
  * @return 1 if successful, 0 otherwise
  */
 int changePassword(User *currentUser);
 
 /**
  * Display the user management menu
  * 
  * @param currentUser The current logged in user
  */
 void userManagementMenu(User *currentUser);
 
 /**
  * Hash a password for secure storage
  * 
  * @param password The plain text password to hash
  * @param hashedPassword Buffer to store the hashed password
  */
 void hashPassword(const char *password, char *hashedPassword);
 
 /**
  * Verify if a password matches the stored hash
  * 
  * @param password The plain text password to verify
  * @param hashedPassword The stored hashed password
  * @return 1 if match, 0 otherwise
  */
 int verifyPassword(const char *password, const char *hashedPassword);
 
 #endif /* AUTH_H */