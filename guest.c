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
#include <ctype.h> /* Add this for tolower() */
#include "guest.h"
#include "fileio.h"
#include "utils.h"
#include "ui.h"

#define GUESTS_FILE "data/guests.dat"
#define TEMP_FILE "data/temp.dat"
#define MAX_GUESTS 10000

/* Get the string representation of a VIP status */
const char* getVipStatusString(VipStatus status) {
    switch (status) {
        case VIP_STATUS_REGULAR:
            return "Regular";
        case VIP_STATUS_BRONZE:
            return "Bronze";
        case VIP_STATUS_SILVER:
            return "Silver";
        case VIP_STATUS_GOLD:
            return "Gold";
        case VIP_STATUS_PLATINUM:
            return "Platinum";
        default:
            return "Unknown";
    }
}

/* Initialize guest data file */
int initializeGuestData(void) {
    FILE *fp;
    Guest sampleGuests[3] = {
        {
            .id = 1,
            .name = "John Smith",
            .address = "123 Main St, Anytown, USA",
            .phone = "555-123-4567",
            .email = "john.smith@example.com",
            .idNumber = "AB123456",
            .idType = "Passport",
            .registrationDate = "2023-01-15",
            .totalStays = 3,
            .totalSpent = 1250.75,
            .notes = "Prefers rooms away from elevator. Allergic to feathers.",
            .isActive = 1,
            .vipStatus = VIP_STATUS_SILVER
        },
        {
            .id = 2,
            .name = "Jane Doe",
            .address = "456 Oak Ave, Somewhere, USA",
            .phone = "555-987-6543",
            .email = "jane.doe@example.com",
            .idNumber = "DL789012",
            .idType = "Driver's License",
            .registrationDate = "2023-02-20",
            .totalStays = 1,
            .totalSpent = 450.25,
            .notes = "First-time guest. Interested in spa services.",
            .isActive = 1,
            .vipStatus = VIP_STATUS_REGULAR
        },
        {
            .id = 3,
            .name = "Robert Johnson",
            .address = "789 Pine Blvd, Elsewhere, USA",
            .phone = "555-456-7890",
            .email = "robert.johnson@example.com",
            .idNumber = "PP567890",
            .idType = "Passport",
            .registrationDate = "2022-11-05",
            .totalStays = 8,
            .totalSpent = 4750.50,
            .notes = "Frequent business traveler. Prefers high floor rooms.",
            .isActive = 1,
            .vipStatus = VIP_STATUS_PLATINUM
        }
    };
    int numSampleGuests = sizeof(sampleGuests) / sizeof(sampleGuests[0]);
    
    /* Create data directory if it doesn't exist */
    if (!createDirectoryIfNotExists("data")) {
        printf("\nError: Failed to create data directory\n");
        return 0;
    }

    /* Check if guests file already exists */
    fp = fopen(GUESTS_FILE, "rb");
    if (fp != NULL) {
        /* Check if file has content */
        fseek(fp, 0, SEEK_END);
        if (ftell(fp) > 0) {
            fclose(fp);
            return 1; /* Guests already exist */
        }
        fclose(fp);
    }

    /* Create guests file and write sample data */
    fp = fopen(GUESTS_FILE, "wb");
    if (!fp) {
        printf("\nError: Could not create guests file\n");
        return 0;
    }

    /* Write sample guests to file */
    if (fwrite(sampleGuests, sizeof(Guest), numSampleGuests, fp) != numSampleGuests) {
        printf("\nError: Could not write sample guest data\n");
        fclose(fp);
        return 0;
    }

    fclose(fp);
    printf("\nSample guest data created successfully.\n");
    return 1;
}

/* Add a new guest */
int addGuest(User *currentUser) {
    FILE *fp;
    Guest newGuest;
    int nextId = 1;
    time_t now;
    struct tm *t;
    
    /* Get current date for registration date */
    now = time(NULL);
    t = localtime(&now);
    sprintf(newGuest.registrationDate, "%04d-%02d-%02d", 
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
    
    /* Find next available ID */
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
    
    /* Get guest details */
    clearScreen();
    printf("===== ADD NEW GUEST =====\n");
    
    newGuest.id = nextId;
    
    printf("Full Name: ");
    getchar(); /* Consume newline */
    fgets(newGuest.name, MAX_GUEST_NAME_LEN, stdin);
    newGuest.name[strcspn(newGuest.name, "\n")] = '\0'; /* Remove newline */
    
    printf("Address: ");
    fgets(newGuest.address, MAX_GUEST_ADDRESS_LEN, stdin);
    newGuest.address[strcspn(newGuest.address, "\n")] = '\0'; /* Remove newline */
    
    printf("Phone Number: ");
    fgets(newGuest.phone, MAX_GUEST_PHONE_LEN, stdin);
    newGuest.phone[strcspn(newGuest.phone, "\n")] = '\0'; /* Remove newline */
    
    printf("Email: ");
    fgets(newGuest.email, MAX_GUEST_EMAIL_LEN, stdin);
    newGuest.email[strcspn(newGuest.email, "\n")] = '\0'; /* Remove newline */
    
    printf("ID Type (Passport, Driver's License, etc.): ");
    fgets(newGuest.idType, 20, stdin);
    newGuest.idType[strcspn(newGuest.idType, "\n")] = '\0'; /* Remove newline */
    
    printf("ID Number: ");
    fgets(newGuest.idNumber, MAX_GUEST_ID_LEN, stdin);
    newGuest.idNumber[strcspn(newGuest.idNumber, "\n")] = '\0'; /* Remove newline */
    
    printf("Notes (optional): ");
    fgets(newGuest.notes, MAX_GUEST_NOTES_LEN, stdin);
    newGuest.notes[strcspn(newGuest.notes, "\n")] = '\0'; /* Remove newline */
    
    /* Set default values */
    newGuest.totalStays = 0;
    newGuest.totalSpent = 0.0;
    newGuest.isActive = 1;
    newGuest.vipStatus = 0; /* Regular guest by default */
    
    /* Write new guest to file */
    fp = fopen(GUESTS_FILE, "ab");
    if (fp == NULL) {
        printf("\nError: Could not open guests file.\n");
        return 0;
    }
    
    fwrite(&newGuest, sizeof(Guest), 1, fp);
    fclose(fp);
    
    printf("\nGuest added successfully with ID: %d\n", newGuest.id);
    return newGuest.id;
}

/* Modify an existing guest */
int modifyGuest(User *currentUser, int guestId) {
    FILE *fp, *tempFp;
    Guest tempGuest;
    int found = 0;
    int choice;
    char newValue[MAX_GUEST_NOTES_LEN]; /* Largest possible input */
    int newVipStatus;
    
    /* Open guests file */
    fp = fopen(GUESTS_FILE, "rb");
    if (fp == NULL) {
        printf("\nError: Could not open guests file.\n");
        return 0;
    }
    
    /* Find guest to modify */
    while (fread(&tempGuest, sizeof(Guest), 1, fp) == 1) {
        if (tempGuest.id == guestId && tempGuest.isActive) {
            found = 1;
            break;
        }
    }
    
    fclose(fp);
    
    if (!found) {
        printf("\nGuest with ID %d not found.\n", guestId);
        return 0;
    }
    
    /* Display guest details and modification menu */
    clearScreen();
    printf("===== MODIFY GUEST =====\n");
    printf("ID: %d\n", tempGuest.id);
    printf("Name: %s\n", tempGuest.name);
    printf("Address: %s\n", tempGuest.address);
    printf("Phone: %s\n", tempGuest.phone);
    printf("Email: %s\n", tempGuest.email);
    printf("ID Type: %s\n", tempGuest.idType);
    printf("ID Number: %s\n", tempGuest.idNumber);
    printf("Registration Date: %s\n", tempGuest.registrationDate);
    printf("Total Stays: %d\n", tempGuest.totalStays);
    printf("Total Spent: $%.2f\n", tempGuest.totalSpent);
    printf("VIP Status: %s\n", getVipStatusString(tempGuest.vipStatus));
    printf("Notes: %s\n", tempGuest.notes);
    
    printf("\nWhat would you like to modify?\n");
    printf("1. Name\n");
    printf("2. Address\n");
    printf("3. Phone\n");
    printf("4. Email\n");
    printf("5. ID Type\n");
    printf("6. ID Number\n");
    printf("7. Notes\n");
    printf("8. VIP Status\n");
    printf("0. Cancel\n");
    
    choice = getIntInput("Enter your choice: ", 0, 8);
    
    if (choice == 0) {
        return 0; /* Cancelled */
    }
    
    /* Get new value based on choice */
    if (choice >= 1 && choice <= 7) {
        printf("Enter new value: ");
        getchar(); /* Consume newline */
        fgets(newValue, MAX_GUEST_NOTES_LEN, stdin);
        newValue[strcspn(newValue, "\n")] = '\0'; /* Remove newline */
    } else if (choice == 8) {
        printf("\nVIP Status:\n");
        printf("0. Regular\n");
        printf("1. Bronze\n");
        printf("2. Silver\n");
        printf("3. Gold\n");
        printf("4. Platinum\n");
        printf("5. Diamond\n");
        newVipStatus = getIntInput("Enter new VIP status (0-5): ", 0, 5);
    }
    
    /* Open files for modification */
    fp = fopen(GUESTS_FILE, "rb");
    if (fp == NULL) {
        printf("\nError: Could not open guests file.\n");
        return 0;
    }
    
    tempFp = fopen(TEMP_FILE, "wb");
    if (tempFp == NULL) {
        fclose(fp);
        printf("\nError: Could not create temporary file.\n");
        return 0;
    }
    
    /* Process each guest */
    while (fread(&tempGuest, sizeof(Guest), 1, fp) == 1) {
        if (tempGuest.id == guestId && tempGuest.isActive) {
            /* Modify guest based on choice */
            switch (choice) {
                case 1: /* Name */
                    strcpy(tempGuest.name, newValue);
                    break;
                case 2: /* Address */
                    strcpy(tempGuest.address, newValue);
                    break;
                case 3: /* Phone */
                    strcpy(tempGuest.phone, newValue);
                    break;
                case 4: /* Email */
                    strcpy(tempGuest.email, newValue);
                    break;
                case 5: /* ID Type */
                    strcpy(tempGuest.idType, newValue);
                    break;
                case 6: /* ID Number */
                    strcpy(tempGuest.idNumber, newValue);
                    break;
                case 7: /* Notes */
                    strcpy(tempGuest.notes, newValue);
                    break;
                case 8: /* VIP Status */
                    tempGuest.vipStatus = newVipStatus;
                    break;
            }
        }
        
        /* Write guest to temp file */
        fwrite(&tempGuest, sizeof(Guest), 1, tempFp);
    }
    
    fclose(fp);
    fclose(tempFp);
    
    /* Replace original file with temp file */
    remove(GUESTS_FILE);
    rename(TEMP_FILE, GUESTS_FILE);
    
    printf("\nGuest modified successfully.\n");
    return 1;
}

/* Delete a guest */
int deleteGuest(User *currentUser, int guestId) {
    FILE *fp, *tempFp;
    Guest tempGuest;
    int found = 0;
    char confirm;
    
    /* Check if user has permission */
    if (currentUser->role != ROLE_ADMIN) {
        printf("\nAccess denied. Admin privileges required.\n");
        return 0;
    }
    
    /* Open guests file */
    fp = fopen(GUESTS_FILE, "rb");
    if (fp == NULL) {
        printf("\nError: Could not open guests file.\n");
        return 0;
    }
    
    /* Find guest to delete */
    while (fread(&tempGuest, sizeof(Guest), 1, fp) == 1) {
        if (tempGuest.id == guestId && tempGuest.isActive) {
            found = 1;
            break;
        }
    }
    
    fclose(fp);
    
    if (!found) {
        printf("\nGuest with ID %d not found.\n", guestId);
        return 0;
    }
    
    /* Check if guest has active reservations */
    if (hasActiveReservationsByGuest(guestId)) {
        printf("\nCannot delete guest. They have active reservations.\n");
        return 0;
    }
    
    /* Confirm deletion */
    printf("\nAre you sure you want to delete guest '%s'? (y/n): ", tempGuest.name);
    scanf(" %c", &confirm);
    
    if (confirm != 'y' && confirm != 'Y') {
        printf("\nDeletion cancelled.\n");
        return 0;
    }
    
    /* Open files for deletion */
    fp = fopen(GUESTS_FILE, "rb");
    if (fp == NULL) {
        printf("\nError: Could not open guests file.\n");
        return 0;
    }
    
    tempFp = fopen(TEMP_FILE, "wb");
    if (tempFp == NULL) {
        fclose(fp);
        printf("\nError: Could not create temporary file.\n");
        return 0;
    }
    
    /* Mark guest as inactive instead of deleting */
    while (fread(&tempGuest, sizeof(Guest), 1, fp) == 1) {
        if (tempGuest.id == guestId) {
            tempGuest.isActive = 0; /* Mark as inactive */
        }
        fwrite(&tempGuest, sizeof(Guest), 1, tempFp);
    }
    
    fclose(fp);
    fclose(tempFp);
    
    /* Replace original file with temp file */
    remove(GUESTS_FILE);
    rename(TEMP_FILE, GUESTS_FILE);
    
    printf("\nGuest deleted successfully.\n");
    return 1;
}

/* List all guests */
void listGuests(User *currentUser) {
    FILE *fp;
    Guest tempGuest;
    int count = 0;
    
    /* Open guests file */
    fp = fopen(GUESTS_FILE, "rb");
    if (fp == NULL) {
        printf("\nError: Could not open guests file.\n");
        return;
    }
    
    /* Display header */
    clearScreen();
    printf("===== GUEST LIST =====\n");
    printf("%-5s %-30s %-15s %-25s %-15s %-10s\n", 
           "ID", "Name", "Phone", "Email", "Reg. Date", "VIP Status");
    printf("---------------------------------------------------------------------------------\n");
    
    /* Display each guest */
    while (fread(&tempGuest, sizeof(Guest), 1, fp) == 1) {
        if (tempGuest.isActive) {
            printf("%-5d %-30s %-15s %-25s %-15s %-10s\n", 
                   tempGuest.id, 
                   tempGuest.name, 
                   tempGuest.phone, 
                   tempGuest.email,
                   tempGuest.registrationDate,
                   getVipStatusString(tempGuest.vipStatus));
            count++;
        }
    }
    
    fclose(fp);
    
    /* Display footer */
    printf("---------------------------------------------------------------------------------\n");
    printf("Total guests: %d\n", count);
}

/* Search for guests based on criteria */
void searchGuests(User *currentUser) {
    FILE *fp;
    Guest tempGuest;
    char searchTerm[50];
    int count = 0;
    int searchType;
    
    /* Get search criteria */
    clearScreen();
    printf("===== SEARCH GUESTS =====\n");
    
    printf("Search by:\n");
    printf("1. Name\n");
    printf("2. Phone\n");
    printf("3. Email\n");
    printf("4. ID Number\n");
    printf("5. VIP Status\n");
    
    searchType = getIntInput("Enter your choice: ", 1, 5);
    
    printf("Enter search term: ");
    getchar(); /* Consume newline */
    fgets(searchTerm, 50, stdin);
    searchTerm[strcspn(searchTerm, "\n")] = '\0'; /* Remove newline */
    
    /* Convert search term to lowercase for case-insensitive search */
    for (int i = 0; searchTerm[i]; i++) {
        searchTerm[i] = tolower(searchTerm[i]);
    }
    
    /* Open guests file */
    fp = fopen(GUESTS_FILE, "rb");
    if (fp == NULL) {
        printf("\nError: Could not open guests file.\n");
        return;
    }
    
    /* Display header */
    printf("\n===== SEARCH RESULTS =====\n");
    printf("%-5s %-30s %-15s %-25s %-15s %-10s\n", 
           "ID", "Name", "Phone", "Email", "Reg. Date", "VIP Status");
    printf("---------------------------------------------------------------------------------\n");
    
    /* Search and display matching guests */
    while (fread(&tempGuest, sizeof(Guest), 1, fp) == 1) {
        if (tempGuest.isActive) {
            int match = 0;
            char tempValue[MAX_GUEST_NOTES_LEN];
            
            switch (searchType) {
                case 1: /* Name */
                    strcpy(tempValue, tempGuest.name);
                    break;
                case 2: /* Phone */
                    strcpy(tempValue, tempGuest.phone);
                    break;
                case 3: /* Email */
                    strcpy(tempValue, tempGuest.email);
                    break;
                case 4: /* ID Number */
                    strcpy(tempValue, tempGuest.idNumber);
                    break;
                case 5: /* VIP Status */
                    strcpy(tempValue, getVipStatusString(tempGuest.vipStatus));
                    break;
            }
            
            /* Convert to lowercase for case-insensitive comparison */
            for (int i = 0; tempValue[i]; i++) {
                tempValue[i] = tolower(tempValue[i]);
            }
            
            /* Check if search term is in the value */
            if (strstr(tempValue, searchTerm) != NULL) {
                match = 1;
            }
            
            if (match) {
                printf("%-5d %-30s %-15s %-25s %-15s %-10s\n", 
                       tempGuest.id, 
                       tempGuest.name, 
                       tempGuest.phone, 
                       tempGuest.email,
                       tempGuest.registrationDate,
                       getVipStatusString(tempGuest.vipStatus));
                count++;
            }
        }
    }
    
    fclose(fp);
    
    /* Display footer */
    printf("---------------------------------------------------------------------------------\n");
    printf("Total matching guests: %d\n", count);
}

/* Get a guest by ID */
int getGuestById(int guestId, Guest *guest) {
    FILE *fp;
    Guest tempGuest;
    int found = 0;
    
    /* Open guests file */
    fp = fopen(GUESTS_FILE, "rb");
    if (fp == NULL) {
        return 0; /* File not found */
    }
    
    /* Find guest */
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

/* Update guest stay information after checkout */
int updateGuestStayInfo(int guestId, double amountSpent) {
    FILE *fp, *tempFp;
    Guest tempGuest;
    int found = 0;
    
    /* Open guests file */
    fp = fopen(GUESTS_FILE, "rb");
    if (fp == NULL) {
        return 0; /* File not found */
    }
    
    tempFp = fopen(TEMP_FILE, "wb");
    if (tempFp == NULL) {
        fclose(fp);
        return 0;
    }
    
    /* Update guest information */
    while (fread(&tempGuest, sizeof(Guest), 1, fp) == 1) {
        if (tempGuest.id == guestId && tempGuest.isActive) {
            tempGuest.totalStays++;
            tempGuest.totalSpent += amountSpent;
            
            /* Update VIP status based on total spent */
            if (tempGuest.totalSpent >= 10000.0) {
                tempGuest.vipStatus = 5; /* Diamond */
            } else if (tempGuest.totalSpent >= 5000.0) {
                tempGuest.vipStatus = 4; /* Platinum */
            } else if (tempGuest.totalSpent >= 2500.0) {
                tempGuest.vipStatus = 3; /* Gold */
            } else if (tempGuest.totalSpent >= 1000.0) {
                tempGuest.vipStatus = 2; /* Silver */
            } else if (tempGuest.totalSpent >= 500.0) {
                tempGuest.vipStatus = 1; /* Bronze */
            }
            
            found = 1;
        }
        
        fwrite(&tempGuest, sizeof(Guest), 1, tempFp);
    }
    
    fclose(fp);
    fclose(tempFp);
    
    if (found) {
        /* Replace original file with temp file */
        remove(GUESTS_FILE);
        rename(TEMP_FILE, GUESTS_FILE);
        return 1;
    } else {
        /* Guest not found, remove temp file */
        remove(TEMP_FILE);
        return 0;
    }
}

/* Guest management menu */
void guestManagementMenu(User *currentUser) {
    int choice;
    int guestId;
    
    do {
        clearScreen();
        printf("===== GUEST MANAGEMENT =====\n");
        printf("1. List All Guests\n");
        printf("2. Search Guests\n");
        printf("3. Add Guest\n");
        printf("4. Modify Guest\n");
        printf("5. Delete Guest\n");
        printf("6. View Guest Details\n");
        printf("0. Back to Main Menu\n");
        printf("=============================\n");
        
        choice = getIntInput("Enter your choice: ", 0, 6);
        
        switch (choice) {
            case 1: /* List All Guests */
                listGuests(currentUser);
                break;
                
            case 2: /* Search Guests */
                searchGuests(currentUser);
                break;
                
            case 3: /* Add Guest */
                addGuest(currentUser);
                break;
                
            case 4: /* Modify Guest */
                listGuests(currentUser);
                guestId = getIntInput("\nEnter guest ID to modify (0 to cancel): ", 0, 9999);
                if (guestId != 0) {
                    modifyGuest(currentUser, guestId);
                }
                break;
                
            case 5: /* Delete Guest */
                if (currentUser->role == ROLE_ADMIN) {
                    listGuests(currentUser);
                    guestId = getIntInput("\nEnter guest ID to delete (0 to cancel): ", 0, 9999);
                    if (guestId != 0) {
                        deleteGuest(currentUser, guestId);
                    }
                } else {
                    printf("\nAccess denied. Admin privileges required.\n");
                }
                break;
                
            case 6: /* View Guest Details */
                listGuests(currentUser);
                guestId = getIntInput("\nEnter guest ID to view details (0 to cancel): ", 0, 9999);
                if (guestId != 0) {
                    Guest guest;
                    if (getGuestById(guestId, &guest)) {
                        clearScreen();
                        printf("===== GUEST DETAILS =====\n");
                        printf("ID: %d\n", guest.id);
                        printf("Name: %s\n", guest.name);
                        printf("Address: %s\n", guest.address);
                        printf("Phone: %s\n", guest.phone);
                        printf("Email: %s\n", guest.email);
                        printf("ID Type: %s\n", guest.idType);
                        printf("ID Number: %s\n", guest.idNumber);
                        printf("Registration Date: %s\n", guest.registrationDate);
                        printf("Total Stays: %d\n", guest.totalStays);
                        printf("Total Spent: $%.2f\n", guest.totalSpent);
                        printf("VIP Status: %s\n", getVipStatusString(guest.vipStatus));
                        printf("Notes: %s\n", guest.notes);
                    } else {
                        printf("\nGuest with ID %d not found.\n", guestId);
                    }
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