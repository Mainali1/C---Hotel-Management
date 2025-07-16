/**
 * Hotel Management System
 * fileio.c - File I/O operations implementation
 * 
 * This file implements file operations for data persistence
 * including file reading, writing, and directory management.
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <sys/stat.h>
 
 // Headers for MS-specific functions
 #if defined(_WIN32) || defined(_WIN64)
 #include <direct.h>
 #define MKDIR(path) _mkdir(path)
 #else
 #include <unistd.h>
 #define MKDIR(path) mkdir(path, 0755)
 #endif

 #include "fileio.h"
 #include "auth.h"
 #include "room.h"
 #include "guest.h"
 #include "reservation.h"
 #include "billing.h"
 
 /**
  * Create a directory if it doesn't exist
  */
 int createDirectoryIfNotExists(const char *dirPath) {
     struct stat st = {0};
     if (stat(dirPath, &st) == -1) {
         if (MKDIR(dirPath) != 0) {
             perror("Error creating directory");
             return 0;
         }
     }
     return 1;
 }
 
 /**
  * Backup a file to the specified backup directory
  */
 int backupFile(const char *sourceFile, const char* backupDir) {
     FILE *source, *dest;
     char destFile[256];
     char buffer[1024];
     size_t bytesRead;
     const char *fileName = strrchr(sourceFile, '/');
     if (!fileName) {
        fileName = strrchr(sourceFile, '\\');
     }
     fileName = fileName ? fileName + 1 : sourceFile;

     sprintf(destFile, "%s/%s", backupDir, fileName);
     
     source = fopen(sourceFile, "rb");
     if (source == NULL) {
         printf("\nWarning: Could not open source file %s for backup (it may not exist yet).\n", sourceFile);
         return 1; // Not a fatal error if a file doesn't exist to be backed up
     }
     
     dest = fopen(destFile, "wb");
     if (dest == NULL) {
         printf("\nError: Could not create backup file %s\n", destFile);
         fclose(source);
         return 0;
     }
     
     while ((bytesRead = fread(buffer, 1, sizeof(buffer), source)) > 0) {
         fwrite(buffer, 1, bytesRead, dest);
     }
     
     fclose(source);
     fclose(dest);
     
     return 1;
 }
 
 /**
  * Check if a file exists
  */
 int fileExists(const char *filePath) {
     FILE *fp = fopen(filePath, "rb");
     if (fp) {
         fclose(fp);
         return 1;
     }
     return 0;
 }
 
/**
 * Initialize all data files for the system
 */
int initializeDataFiles(void) {
    if (!initializeUserData()) return 0;
    if (!initializeRoomData()) return 0;
    if (!initializeGuestData()) return 0;
    if (!initializeReservationData()) return 0;
    if (!initializeBillingData()) return 0;
    return 1;
}

/**
 * Load system configuration from file
 */
int loadConfiguration(void) {
    // Stub for future implementation
    return 1;
}

/**
 * Backup all data files to specified directory
 */
int backupDataFiles(const char *backupDir) {
    createDirectoryIfNotExists(backupDir);
    
    int success = 1;
    if (!backupFile(USERS_FILE, backupDir)) success = 0;
    if (!backupFile(ROOMS_FILE, backupDir)) success = 0;
    if (!backupFile(GUESTS_FILE, backupDir)) success = 0;
    if (!backupFile(RESERVATIONS_FILE, backupDir)) success = 0;
    if (!backupFile(INVOICES_FILE, backupDir)) success = 0;
    if (!backupFile(BILLING_ITEMS_FILE, backupDir)) success = 0;
    if (!backupFile(PAYMENTS_FILE, backupDir)) success = 0;

    return success;
}