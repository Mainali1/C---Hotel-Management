/**
 * Hotel Management System
 * guest.c - Guest management implementation
 * 
 * This file implements the guest management functionality
 * including adding, modifying, and listing guests.
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <time.h>
 #include <ctype.h>
 #include "guest.h"
 #include "reservation.h" // For hasActiveReservationsByGuest
 #include "fileio.h"
 #include "utils.h"
 #include "ui.h"
 
 #define TEMP_FILE "data/temp_guests.dat"
 
 /* Get the string representation of a VIP status */
 const char* getVipStatusString(VipStatus status) {
     switch (status) {
         case VIP_STATUS_REGULAR: return "Regular";
         case VIP_STATUS_BRONZE: return "Bronze";
         case VIP_STATUS_SILVER: return "Silver";
         case VIP_STATUS_GOLD: return "Gold";
         case VIP_STATUS_PLATINUM: return "Platinum";
         default: return "Unknown";
     }
 }
 
 /* Initialize guest data file if it doesn't exist or is empty */
 int initializeGuestData(void) {
     FILE *fp;
     
     fp = fopen(GUESTS_FILE, "rb");
     if (fp != NULL) {
         fseek(fp, 0, SEEK_END);
         if (ftell(fp) > 0) {
             fclose(fp);
             return 1;
         }
         fclose(fp);
     }
     
     // If we are here, file is empty or doesn't exist, so create it
     fp = fopen(GUESTS_FILE, "wb");
     if (!fp) {
         printf("\nError: Could not create guests file.\n");
         return 0;
     }
     fclose(fp);
     return 1;
 }
 
 /* Add a new guest */
 int addGuest(void) {
     FILE *fp;
     Guest newGuest;
     int nextId = 1;
 
     fp = fopen(GUESTS_FILE, "rb");
     if (fp != NULL) {
         Guest tempGuest;
         while (fread(&tempGuest, sizeof(Guest), 1, fp) == 1) {
             if (tempGuest.id >= nextId) {
                 nextId = tempGuest.id + 1;
             }
         }
         fclose(fp);
     }
     
     clearScreen();
     printf("===== ADD NEW GUEST =====\n");
     
     newGuest.id = nextId;
     
     printf("Full Name: ");
     getStringInput(NULL, newGuest.name, MAX_GUEST_NAME_LEN);
     
     printf("Address: ");
     getStringInput(NULL, newGuest.address, MAX_ADDRESS_LEN);
     
     printf("Phone Number: ");
     getStringInput(NULL, newGuest.phone, MAX_PHONE_LEN);
     
     printf("Email: ");
     getStringInput(NULL, newGuest.email, MAX_EMAIL_LEN);
     
     printf("ID Type (e.g., Passport, License): ");
     getStringInput(NULL, newGuest.idType, MAX_ID_TYPE_LEN);
     
     printf("ID Number: ");
     getStringInput(NULL, newGuest.idNumber, MAX_ID_NUMBER_LEN);
     
     printf("Notes (optional): ");
     getStringInput(NULL, newGuest.notes, MAX_NOTES_LEN);
     
     getCurrentDate(newGuest.registrationDate);
     newGuest.totalStays = 0;
     newGuest.totalSpent = 0.0;
     newGuest.isActive = 1;
     newGuest.vipStatus = VIP_STATUS_REGULAR;
     
     fp = fopen(GUESTS_FILE, "ab");
     if (fp == NULL) {
         printf("\nError: Could not open guests file for writing.\n");
         return 0;
     }
     
     fwrite(&newGuest, sizeof(Guest), 1, fp);
     fclose(fp);
     
     printf("\nGuest '%s' added successfully with ID: %d\n", newGuest.name, newGuest.id);
     return newGuest.id;
 }
 
 /* Modify an existing guest */
 int modifyGuest(User *currentUser, int guestId) {
     FILE *fp, *tempFp;
     Guest tempGuest;
     int found = 0;
 
     fp = fopen(GUESTS_FILE, "rb");
     if (!fp) {
         printf("\nError: Could not open guests file.\n");
         return 0;
     }
     
     tempFp = fopen(TEMP_FILE, "wb");
     if (!tempFp) {
         fclose(fp);
         printf("\nError: Could not create temporary file.\n");
         return 0;
     }
     
     while (fread(&tempGuest, sizeof(Guest), 1, fp) == 1) {
         if (tempGuest.id == guestId && tempGuest.isActive) {
             found = 1;
             clearScreen();
             printf("===== MODIFYING GUEST: %s (ID: %d) =====\n", tempGuest.name, tempGuest.id);
             printf("\n-- Contact Information --\n");
             printf("Current Address: %s\nEnter new address: ", tempGuest.address);
             getStringInput(NULL, tempGuest.address, MAX_ADDRESS_LEN);
 
             printf("Current Phone: %s\nEnter new phone: ", tempGuest.phone);
             getStringInput(NULL, tempGuest.phone, MAX_PHONE_LEN);
 
             printf("Current Email: %s\nEnter new email: ", tempGuest.email);
             getStringInput(NULL, tempGuest.email, MAX_EMAIL_LEN);
 
             printf("\n-- Notes --\n");
             printf("Current Notes: %s\nEnter new notes: ", tempGuest.notes);
             getStringInput(NULL, tempGuest.notes, MAX_NOTES_LEN);
         }
         fwrite(&tempGuest, sizeof(Guest), 1, tempFp);
     }
     
     fclose(fp);
     fclose(tempFp);
     
     if (found) {
         remove(GUESTS_FILE);
         rename(TEMP_FILE, GUESTS_FILE);
         printf("\nGuest information updated successfully.\n");
     } else {
         remove(TEMP_FILE);
         printf("\nGuest with ID %d not found.\n", guestId);
     }
     return found;
 }
 
 /* "Delete" a guest by marking them inactive */
 int deleteGuest(User *currentUser, int guestId) {
     FILE *fp, *tempFp;
     Guest tempGuest;
     int found = 0;
     char confirm;
     
     if (currentUser->role != ROLE_ADMIN) {
         printf("\nAccess denied. Admin privileges required.\n");
         return 0;
     }
 
     if (hasActiveReservationsByGuest(guestId)) {
         printf("\nCannot delete guest %d. They have active or future reservations.\n", guestId);
         return 0;
     }
     
     fp = fopen(GUESTS_FILE, "rb");
     if (!fp) {
         printf("\nError: Could not open guests file.\n");
         return 0;
     }
     
     tempFp = fopen(TEMP_FILE, "wb");
     if (!tempFp) {
         fclose(fp);
         printf("\nError: Could not create temporary file.\n");
         return 0;
     }
 
     while (fread(&tempGuest, sizeof(Guest), 1, fp) == 1) {
         if (tempGuest.id == guestId && tempGuest.isActive) {
             found = 1;
             printf("\nAre you sure you want to delete guest '%s'? (y/n): ", tempGuest.name);
             scanf(" %c", &confirm);
             if (confirm == 'y' || confirm == 'Y') {
                 tempGuest.isActive = 0;
                 printf("Guest has been marked as inactive.\n");
             }
         }
         fwrite(&tempGuest, sizeof(Guest), 1, tempFp);
     }
     
     fclose(fp);
     fclose(tempFp);
     
     if (found) {
         remove(GUESTS_FILE);
         rename(TEMP_FILE, GUESTS_FILE);
     } else {
         remove(TEMP_FILE);
         printf("\nGuest with ID %d not found.\n", guestId);
     }
     return found;
 }
 
 /* List all active guests */
 void listGuests(User *currentUser) {
     FILE *fp;
     Guest tempGuest;
     int count = 0;
     
     fp = fopen(GUESTS_FILE, "rb");
     if (fp == NULL) {
         printf("\nNo guests found or error opening file.\n");
         return;
     }
     
     clearScreen();
     printf("===== GUEST LIST =====\n");
     printf("%-5s %-30s %-15s %-25s %-12s\n", 
            "ID", "Name", "Phone", "Email", "VIP Status");
     printf("--------------------------------------------------------------------------------------\n");
     
     while (fread(&tempGuest, sizeof(Guest), 1, fp) == 1) {
         if (tempGuest.isActive) {
             printf("%-5d %-30s %-15s %-25s %-12s\n", 
                    tempGuest.id, 
                    tempGuest.name, 
                    tempGuest.phone, 
                    tempGuest.email,
                    getVipStatusString(tempGuest.vipStatus));
             count++;
         }
     }
     
     fclose(fp);
     
     printf("--------------------------------------------------------------------------------------\n");
     if (count == 0) {
         printf("No active guests in the system.\n");
     } else {
         printf("Total active guests: %d\n", count);
     }
 }
 
 /* Search for guests */
 void searchGuests(User *currentUser) {
     FILE *fp;
     Guest tempGuest;
     char searchTerm[MAX_GUEST_NAME_LEN];
     int count = 0;
     
     clearScreen();
     printf("===== SEARCH GUESTS =====\n");
     printf("Enter name or phone to search for: ");
     getStringInput(NULL, searchTerm, MAX_GUEST_NAME_LEN);
     
     for (int i = 0; searchTerm[i]; i++) {
         searchTerm[i] = tolower(searchTerm[i]);
     }
     
     fp = fopen(GUESTS_FILE, "rb");
     if (fp == NULL) {
         printf("\nError: Could not open guests file.\n");
         return;
     }
     
     printf("\n===== SEARCH RESULTS =====\n");
     printf("%-5s %-30s %-15s %-25s %-12s\n", 
            "ID", "Name", "Phone", "Email", "VIP Status");
     printf("--------------------------------------------------------------------------------------\n");
     
     while (fread(&tempGuest, sizeof(Guest), 1, fp) == 1) {
         if (tempGuest.isActive) {
             char lowerName[MAX_GUEST_NAME_LEN];
             strcpy(lowerName, tempGuest.name);
             for (int i = 0; lowerName[i]; i++) {
                 lowerName[i] = tolower(lowerName[i]);
             }
             
             if (strstr(lowerName, searchTerm) != NULL || strstr(tempGuest.phone, searchTerm) != NULL) {
                 printf("%-5d %-30s %-15s %-25s %-12s\n", 
                        tempGuest.id, 
                        tempGuest.name, 
                        tempGuest.phone, 
                        tempGuest.email,
                        getVipStatusString(tempGuest.vipStatus));
                 count++;
             }
         }
     }
     
     fclose(fp);
     
     printf("--------------------------------------------------------------------------------------\n");
     printf("Found %d matching guests.\n", count);
 }
 
 /* Get a guest by ID */
 int getGuestById(int guestId, Guest *guest) {
     FILE *fp;
     Guest tempGuest;
     int found = 0;
     
     fp = fopen(GUESTS_FILE, "rb");
     if (fp == NULL) return 0;
     
     while (fread(&tempGuest, sizeof(Guest), 1, fp) == 1) {
         if (tempGuest.id == guestId && tempGuest.isActive) {
             *guest = tempGuest;
             found = 1;
             break;
         }
     }
     
     fclose(fp);
     return found;
 }
 
 /* Update guest stay info after checkout and payment */
 int updateGuestStayInfo(int guestId, double amountSpent) {
     FILE *fp, *tempFp;
     Guest tempGuest;
     int found = 0;
     
     fp = fopen(GUESTS_FILE, "rb");
     if (!fp) return 0;
     
     tempFp = fopen(TEMP_FILE, "wb");
     if (!tempFp) {
         fclose(fp);
         return 0;
     }
     
     while (fread(&tempGuest, sizeof(Guest), 1, fp) == 1) {
         if (tempGuest.id == guestId && tempGuest.isActive) {
             found = 1;
             tempGuest.totalStays++;
             tempGuest.totalSpent += amountSpent;
             
             if (tempGuest.totalSpent >= 5000.0) tempGuest.vipStatus = VIP_STATUS_PLATINUM;
             else if (tempGuest.totalSpent >= 2500.0) tempGuest.vipStatus = VIP_STATUS_GOLD;
             else if (tempGuest.totalSpent >= 1000.0) tempGuest.vipStatus = VIP_STATUS_SILVER;
             else if (tempGuest.totalSpent >= 500.0) tempGuest.vipStatus = VIP_STATUS_BRONZE;
         }
         fwrite(&tempGuest, sizeof(Guest), 1, tempFp);
     }
     
     fclose(fp);
     fclose(tempFp);
     
     if (found) {
         remove(GUESTS_FILE);
         rename(TEMP_FILE, GUESTS_FILE);
     } else {
         remove(TEMP_FILE);
     }
     return found;
 }
 
 /* Guest management menu */
 void guestManagementMenu(User *currentUser) {
     int choice;
     int guestId;
     
     do {
         clearScreen();
         printf("===== GUEST MANAGEMENT =====\n");
         printf("1. List All Guests\n");
         printf("2. Add New Guest\n");
         printf("3. Search Guests\n");
         printf("4. Modify Guest Information\n");
         printf("5. Delete Guest (Admin only)\n");
         printf("0. Back to Main Menu\n");
         printf("=============================\n");
         
         choice = getIntInput("Enter your choice: ", 0, 5);
         
         switch (choice) {
             case 1:
                 listGuests(currentUser);
                 break;
             case 2:
                 addGuest();
                 break;
             case 3:
                 searchGuests(currentUser);
                 break;
             case 4:
                 listGuests(currentUser);
                 guestId = getIntInput("\nEnter guest ID to modify (0 to cancel): ", 0, 9999);
                 if (guestId != 0) {
                     modifyGuest(currentUser, guestId);
                 }
                 break;
             case 5:
                 listGuests(currentUser);
                 guestId = getIntInput("\nEnter guest ID to delete (0 to cancel): ", 0, 9999);
                 if (guestId != 0) {
                     deleteGuest(currentUser, guestId);
                 }
                 break;
             case 0:
                 return;
             default:
                 printf("\nInvalid choice. Please try again.\n");
         }
         
         if (choice != 0) {
             pauseExecution();
         }
         
     } while (choice != 0);
 }