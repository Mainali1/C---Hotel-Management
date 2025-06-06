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
 #include <direct.h> /* For _mkdir on Windows */
 #include <sys/stat.h> /* For stat */
 #include <time.h>
 #include "fileio.h"
 
 /**
  * Create a directory if it doesn't exist
  * 
  * @param dirPath Path to the directory to create
  * @return 1 if successful or directory already exists, 0 otherwise
  */
 int createDirectoryIfNotExists(const char *dirPath) {
     struct stat st = {0};
     
     /* Check if directory already exists */
     if (stat(dirPath, &st) == -1) {
         /* Directory doesn't exist, create it */
         if (_mkdir(dirPath) == -1) {
             printf("\nError: Could not create directory %s\n", dirPath);
             return 0;
         }
     }
     
     return 1;
 }
 
 /**
  * Backup a file to the backup directory
  * 
  * @param sourceFile Path to the source file
  * @return 1 if successful, 0 otherwise
  */
 int backupFile(const char *sourceFile) {
     FILE *source, *dest;
     char destFile[256];
     char buffer[1024];
     size_t bytesRead;
     time_t now;
     struct tm *t;
     char timestamp[20];
     
     /* Get current time for timestamp */
     time(&now);
     t = localtime(&now);
     strftime(timestamp, sizeof(timestamp), "%Y%m%d_%H%M%S", t);
     
     /* Create backup directory if it doesn't exist */
     if (!createDirectoryIfNotExists("backup")) {
         return 0;
     }
     
     /* Construct destination filename with timestamp */
     sprintf(destFile, "backup/%s_%s", timestamp, strrchr(sourceFile, '/') ? strrchr(sourceFile, '/') + 1 : sourceFile);
     
     /* Open source file for reading */
     source = fopen(sourceFile, "rb");
     if (source == NULL) {
         printf("\nError: Could not open source file %s for backup\n", sourceFile);
         return 0;
     }
     
     /* Open destination file for writing */
     dest = fopen(destFile, "wb");
     if (dest == NULL) {
         printf("\nError: Could not create backup file %s\n", destFile);
         fclose(source);
         return 0;
     }
     
     /* Copy file contents */
     while ((bytesRead = fread(buffer, 1, sizeof(buffer), source)) > 0) {
         fwrite(buffer, 1, bytesRead, dest);
     }
     
     /* Close files */
     fclose(source);
     fclose(dest);
     
     return 1;
 }
 
 /**
  * Check if a file exists
  * 
  * @param filePath Path to the file to check
  * @return 1 if file exists, 0 otherwise
  */
 int fileExists(const char *filePath) {
     FILE *fp = fopen(filePath, "rb");
     if (fp != NULL) {
         fclose(fp);
         return 1;
     }
     return 0;
 }
 
 /**
  * Create a system backup of all data files
  * 
  * @return 1 if successful, 0 if any backup failed
  */
 int createSystemBackup(void) {
     /* TODO: Implement system backup functionality */
     return 1;
 }

/**
 * Initialize all data files for the system
 *
 * @return 1 if successful, 0 if any initialization failed
 */
int initializeDataFiles(void) {
    /* Create data directory if it doesn't exist */
    if (!createDirectoryIfNotExists("data")) {
        printf("\nError: Failed to create data directory\n");
        return 0;
    }

    /* Initialize all data files */
    if (!initializeUserData()) {
        printf("\nError: Failed to initialize user data\n");
        return 0;
    }

    if (!initializeRoomData()) {
        printf("\nError: Failed to initialize room data\n");
        return 0;
    }

    if (!initializeGuestData()) {
        printf("\nError: Failed to initialize guest data\n");
        return 0;
    }

    if (!initializeBillingData()) {
        printf("\nError: Failed to initialize billing data\n");
        return 0;
    }

    printf("\nAll data files initialized successfully.\n");
    return 1;
}

/**
 * Load system configuration from file
 *
 * @return 1 if successful, 0 if failed
 */
int loadConfiguration(void) {
    /* TODO: Implement configuration loading */
    return 1;
}

/**
 * Backup all data files to specified directory
 *
 * @param backupDir Directory to store backup files
 * @return 1 if successful, 0 if any backup failed
 */
int backupDataFiles(const char *backupDir) {
    /* Create backup directory if it doesn't exist */
    if (!createDirectoryIfNotExists(backupDir)) {
        return 0;
    }
    
    /* Backup all data files */
    if (!backupFile("data/users.dat") || 
        !backupFile("data/rooms.dat") || 
        !backupFile("data/guests.dat") || 
        !backupFile("data/reservations.dat") || 
        !backupFile("data/invoices.dat") || 
        !backupFile("data/payments.dat")) {
        return 0;
    }
    
    return 1;
}