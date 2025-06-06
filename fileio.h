/**
 * Hotel Management System
 * fileio.h - File I/O operations header
 * 
 * This file contains declarations for file operations
 * including file reading, writing, and directory management.
 */

 #ifndef FILEIO_H
 #define FILEIO_H
 
 /**
  * Create a directory if it doesn't exist
  * 
  * @param dirPath Path to the directory to create
  * @return 1 if successful or directory already exists, 0 otherwise
  */
 int createDirectoryIfNotExists(const char *dirPath);
 
 /**
  * Backup a file to the backup directory
  * 
  * @param sourceFile Path to the source file
  * @return 1 if successful, 0 otherwise
  */
 int backupFile(const char *sourceFile);
 
 /**
  * Check if a file exists
  * 
  * @param filePath Path to the file to check
  * @return 1 if file exists, 0 otherwise
  */
 int fileExists(const char *filePath);
 
 /**
  * Create a system backup of all data files
  * 
  * @return 1 if successful, 0 if any backup failed
  */
 int createSystemBackup(void);

/**
 * Initialize all data files for the system
 *
 * @return 1 if successful, 0 if any initialization failed
 */
int initializeDataFiles(void);

/**
 * Load system configuration from file
 *
 * @return 1 if successful, 0 if failed
 */
int loadConfiguration(void);

/**
 * Backup all data files to specified directory
 *
 * @param backupDir Directory to store backup files
 * @return 1 if successful, 0 if any backup failed
 */
int backupDataFiles(const char *backupDir);

#endif /* FILEIO_H */