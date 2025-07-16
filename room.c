/**
 * Hotel Management System
 * room.c - Room management implementation
 * 
 * This file implements the room management functionality
 * including adding, modifying, and listing rooms.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "room.h"
#include "fileio.h"
#include "utils.h"
#include "ui.h"
#include "reservation.h"

/* Forward declarations for static functions */
static int isRoomExists(int roomId);

#define ROOMS_FILE "data/rooms.dat"
#define TEMP_FILE "data/temp.dat"
#define MAX_ROOMS 1000

/* Get the string representation of a room status */
const char* getRoomStatusString(RoomStatus status) {
    switch (status) {
        case ROOM_STATUS_AVAILABLE:
            return "Available";
        case ROOM_STATUS_OCCUPIED:
            return "Occupied";
        case ROOM_STATUS_MAINTENANCE:
            return "Maintenance";
        case ROOM_STATUS_RESERVED:
            return "Reserved";
        case ROOM_STATUS_CLEANING:
            return "Cleaning";
        default:
            return "Unknown";
    }
}

/* Get the string representation of a room type */
const char* getRoomTypeString(RoomType type) {
    switch (type) {
        case ROOM_TYPE_STANDARD:
            return "Standard";
        case ROOM_TYPE_DELUXE:
            return "Deluxe";
        case ROOM_TYPE_SUITE:
            return "Suite";
        case ROOM_TYPE_EXECUTIVE:
            return "Executive";
        case ROOM_TYPE_PRESIDENTIAL:
            return "Presidential";
        default:
            return "Unknown";
    }
}

/* Initialize room data with sample rooms */
int initializeRoomData(void) {
    FILE *fp;
    Room sampleRooms[10] = {
        /* Standard Rooms */
        {
            .id = 101,
            .type = ROOM_TYPE_STANDARD,
            .status = ROOM_STATUS_AVAILABLE,
            .rate = 100.0,
            .description = "Comfortable standard room with basic amenities",
            .features = "Single bed, TV, Wi-Fi, Basic bathroom",
            .capacity = 1,
            .floor = 1,
            .isActive = 1
        },
        {
            .id = 102,
            .type = ROOM_TYPE_STANDARD,
            .status = ROOM_STATUS_AVAILABLE,
            .rate = 120.0,
            .description = "Standard twin room with city view",
            .features = "Twin beds, TV, Wi-Fi, Basic bathroom",
            .capacity = 2,
            .floor = 1,
            .isActive = 1
        },
        /* Deluxe Rooms */
        {
            .id = 201,
            .type = ROOM_TYPE_DELUXE,
            .status = ROOM_STATUS_AVAILABLE,
            .rate = 150.0,
            .description = "Spacious deluxe room with modern furnishings",
            .features = "Queen bed, TV, Wi-Fi, Mini fridge, Work desk",
            .capacity = 2,
            .floor = 2,
            .isActive = 1
        },
        {
            .id = 202,
            .type = ROOM_TYPE_DELUXE,
            .status = ROOM_STATUS_AVAILABLE,
            .rate = 170.0,
            .description = "Deluxe room with panoramic view",
            .features = "King bed, TV, Wi-Fi, Mini fridge, Work desk, Premium bathroom",
            .capacity = 2,
            .floor = 2,
            .isActive = 1
        },
        /* Suite Rooms */
        {
            .id = 301,
            .type = ROOM_TYPE_SUITE,
            .status = ROOM_STATUS_AVAILABLE,
            .rate = 250.0,
            .description = "Spacious suite with separate living area",
            .features = "King bed, Separate living room, TV, Wi-Fi, Mini bar, Work desk, Premium bathroom, Bathtub",
            .capacity = 3,
            .floor = 3,
            .isActive = 1
        },
        /* Executive Rooms */
        {
            .id = 401,
            .type = ROOM_TYPE_EXECUTIVE,
            .status = ROOM_STATUS_AVAILABLE,
            .rate = 350.0,
            .description = "Luxury executive suite with premium amenities",
            .features = "King bed, Separate living room, Dining area, TV, Wi-Fi, Mini bar, Work desk, Premium bathroom, Bathtub, City view",
            .capacity = 4,
            .floor = 4,
            .isActive = 1
        },
        /* Presidential Suite */
        {
            .id = 501,
            .type = ROOM_TYPE_PRESIDENTIAL,
            .status = ROOM_STATUS_AVAILABLE,
            .rate = 500.0,
            .description = "Luxurious presidential suite with the finest amenities",
            .features = "King bed, Multiple rooms, Living room, Dining area, Kitchenette, Multiple TVs, Wi-Fi, Mini bar, Work desk, Premium bathroom, Jacuzzi, Panoramic view",
            .capacity = 6,
            .floor = 5,
            .isActive = 1
        }
    };
    int numSampleRooms = sizeof(sampleRooms) / sizeof(sampleRooms[0]);

    /* Create data directory if it doesn't exist */
    if (!createDirectoryIfNotExists("data")) {
        printf("\nError: Failed to create data directory\n");
        return 0;
    }

    /* Check if rooms file already exists */
    fp = fopen(ROOMS_FILE, "rb");
    if (fp != NULL) {
        /* Check if file has content */
        fseek(fp, 0, SEEK_END);
        if (ftell(fp) > 0) {
            fclose(fp);
            return 1; /* Rooms already exist */
        }
        fclose(fp);
    }

    /* Create rooms file and write sample data */
    fp = fopen(ROOMS_FILE, "wb");
    if (!fp) {
        printf("\nError: Could not create rooms file\n");
        return 0;
    }

    /* Write sample rooms to file */
    if (fwrite(sampleRooms, sizeof(Room), numSampleRooms, fp) != numSampleRooms) {
        printf("\nError: Could not write sample room data\n");
        fclose(fp);
        return 0;
    }

    fclose(fp);
    printf("\nSample room data created successfully.\n");
    return 1;
}

/* Add a new room */
int addRoom(User *currentUser) {
    FILE *fp;
    Room newRoom;
    int roomId;
    
    /* Check if user has permission */
    if (currentUser->role != ROLE_ADMIN) {
        printf("\nAccess denied. Admin privileges required.\n");
        return 0;
    }
    
    /* Get room details */
    clearScreen();
    printf("===== ADD NEW ROOM =====\n");
    
    /* Get room number */
    printf("Room Number: ");
    roomId = getIntInput("", 1, 9999);
    
    /* Check if room number already exists */
    if (isRoomExists(roomId)) {
        printf("\nRoom number %d already exists.\n", roomId);
        return 0;
    }
    
    newRoom.id = roomId;
    
    /* Get room type */
    printf("\nRoom Type:\n");
    printf("1. Standard\n");
    printf("2. Deluxe\n");
    printf("3. Suite\n");
    printf("4. Executive\n");
    printf("5. Presidential\n");
    newRoom.type = getIntInput("Enter room type (1-5): ", 1, 5);
    
    /* Get room rate */
    printf("\nRoom Rate per Night: $");
    newRoom.rate = getDoubleInput("", 0.0, 10000.0);
    
    /* Get room description */
    printf("\nRoom Description: ");
    getchar(); /* Consume newline */
    fgets(newRoom.description, MAX_ROOM_DESC_LEN, stdin);
    newRoom.description[strcspn(newRoom.description, "\n")] = '\0'; /* Remove newline */
    
    /* Get room features */
    printf("Room Features: ");
    fgets(newRoom.features, MAX_ROOM_FEATURES_LEN, stdin);
    newRoom.features[strcspn(newRoom.features, "\n")] = '\0'; /* Remove newline */
    
    /* Get room capacity */
    printf("Room Capacity (max guests): ");
    newRoom.capacity = getIntInput("", 1, 20);
    
    /* Get floor number */
    printf("Floor Number: ");
    newRoom.floor = getIntInput("", 1, 100);
    
    /* Set default values */
    newRoom.status = ROOM_STATUS_AVAILABLE;
    newRoom.isActive = 1;
    
    /* Write new room to file */
    fp = fopen(ROOMS_FILE, "ab");
    if (fp == NULL) {
        printf("\nError: Could not open rooms file.\n");
        return 0;
    }
    
    fwrite(&newRoom, sizeof(Room), 1, fp);
    fclose(fp);
    
    printf("\nRoom added successfully.\n");
    return 1;
}

/* Check if room already exists */
static int isRoomExists(int roomId) {
    FILE *fp;
    Room tempRoom;
    
    fp = fopen(ROOMS_FILE, "rb");
    if (fp == NULL) {
        return 0; /* File doesn't exist, so room doesn't exist */
    }
    
    while (fread(&tempRoom, sizeof(Room), 1, fp) == 1) {
        if (tempRoom.id == roomId && tempRoom.isActive) {
            fclose(fp);
            return 1; /* Room exists */
        }
    }
    
    fclose(fp);
    return 0; /* Room doesn't exist */
}

/* Modify an existing room */
int modifyRoom(User *currentUser, int roomId) {
    FILE *fp, *tempFp;
    Room tempRoom;
    int choice, newType, newCapacity, statusFilter;
    double newRate;
    char newDescription[MAX_ROOM_DESC_LEN];
    char newFeatures[MAX_ROOM_FEATURES_LEN];
    int found = 0;
    
    /* Check if user has permission */
    if (currentUser->role != ROLE_ADMIN) {
        printf("\nAccess denied. Admin privileges required.\n");
        return 0;
    }
    
    /* Open rooms file */
    fp = fopen(ROOMS_FILE, "rb");
    if (fp == NULL) {
        printf("\nError: Could not open rooms file.\n");
        return 0;
    }
    
    /* Find room to modify */
    while (fread(&tempRoom, sizeof(Room), 1, fp) == 1) {
        if (tempRoom.id == roomId && tempRoom.isActive) {
            found = 1;
            break;
        }
    }
    
    fclose(fp);
    
    if (!found) {
        printf("\nRoom with ID %d not found.\n", roomId);
        return 0;
    }
    
    /* Display room details and modification menu */
    clearScreen();
    printf("===== MODIFY ROOM =====\n");
    printf("Room Number: %d\n", tempRoom.id);
    printf("Type: %s\n", getRoomTypeString(tempRoom.type));
    printf("Status: %s\n", getRoomStatusString(tempRoom.status));
    printf("Rate: $%.2f per night\n", tempRoom.rate);
    printf("Description: %s\n", tempRoom.description);
    printf("Features: %s\n", tempRoom.features);
    printf("Capacity: %d guests\n", tempRoom.capacity);
    printf("Floor: %d\n", tempRoom.floor);
    
    printf("\nWhat would you like to modify?\n");
    printf("1. Room Type\n");
    printf("2. Room Rate\n");
    printf("3. Description\n");
    printf("4. Features\n");
    printf("5. Capacity\n");
    printf("6. Room Status\n");
    printf("0. Cancel\n");
    
    choice = getIntInput("Enter your choice: ", 0, 6);
    
    if (choice == 0) {
        return 0; /* Cancelled */
    }
    
    /* Open files for modification */
    fp = fopen(ROOMS_FILE, "rb");
    if (fp == NULL) {
        printf("\nError: Could not open rooms file.\n");
        return 0;
    }
    
    tempFp = fopen(TEMP_FILE, "wb");
    if (tempFp == NULL) {
        fclose(fp);
        printf("\nError: Could not create temporary file.\n");
        return 0;
    }
    
    /* Process each room */
    while (fread(&tempRoom, sizeof(Room), 1, fp) == 1) {
        if (tempRoom.id == roomId && tempRoom.isActive) {
            /* Modify room based on choice */
            switch (choice) {
                case 1: /* Modify room type */
                    printf("\nRoom Type:\n");
                    printf("1. Standard\n");
                    printf("2. Deluxe\n");
                    printf("3. Suite\n");
                    printf("4. Executive\n");
                    printf("5. Presidential\n");
                    newType = getIntInput("Enter new room type (1-5): ", 1, 5);
                    tempRoom.type = newType;
                    break;
                    
                case 2: /* Modify room rate */
                    printf("Enter new rate per night: $");
                    newRate = getDoubleInput("", 0.0, 10000.0);
                    tempRoom.rate = newRate;
                    break;
                    
                case 3: /* Modify description */
                    printf("Enter new description: ");
                    getchar(); /* Consume newline */
                    fgets(newDescription, MAX_ROOM_DESC_LEN, stdin);
                    newDescription[strcspn(newDescription, "\n")] = '\0'; /* Remove newline */
                    strcpy(tempRoom.description, newDescription);
                    break;
                    
                case 4: /* Modify features */
                    printf("Enter new features: ");
                    getchar(); /* Consume newline */
                    fgets(newFeatures, MAX_ROOM_FEATURES_LEN, stdin);
                    newFeatures[strcspn(newFeatures, "\n")] = '\0'; /* Remove newline */
                    strcpy(tempRoom.features, newFeatures);
                    break;
                    
                case 5: /* Modify capacity */
                    printf("Enter new capacity (max guests): ");
                    newCapacity = getIntInput("", 1, 20);
                    tempRoom.capacity = newCapacity;
                    break;
                    
                case 6: /* Modify room status */
                    printf("\nRoom Status:\n");
                    printf("1. Available\n");
                    printf("2. Occupied\n");
                    printf("3. Maintenance\n");
                    printf("4. Reserved\n");
                    printf("5. Cleaning\n");
                    statusFilter = getIntInput("Enter new status (1-5): ", 1, 5);
                    changeRoomStatus(roomId, statusFilter);
                    break;
            }
        }
        
        /* Write room to temp file */
        fwrite(&tempRoom, sizeof(Room), 1, tempFp);
    }
    
    fclose(fp);
    fclose(tempFp);
    
    /* Replace original file with temp file */
    remove(ROOMS_FILE);
    rename(TEMP_FILE, ROOMS_FILE);
    
    printf("\nRoom modified successfully.\n");
    return 1;
}

/* Delete a room */
int deleteRoom(User *currentUser, int roomId) {
    FILE *fp, *tempFp;
    Room tempRoom;
    int found = 0;
    char confirm;
    
    /* Check if user has permission */
    if (currentUser->role != ROLE_ADMIN) {
        printf("\nAccess denied. Admin privileges required.\n");
        return 0;
    }
    
    /* Open rooms file */
    fp = fopen(ROOMS_FILE, "rb");
    if (fp == NULL) {
        printf("\nError: Could not open rooms file.\n");
        return 0;
    }
    
    /* Find room to delete */
    while (fread(&tempRoom, sizeof(Room), 1, fp) == 1) {
        if (tempRoom.id == roomId && tempRoom.isActive) {
            found = 1;
            break;
        }
    }
    
    fclose(fp);
    
    if (!found) {
        printf("\nRoom with ID %d not found.\n", roomId);
        return 0;
    }
    
    /* Check if room has active reservations */
    if (hasActiveReservations(roomId)) {
        printf("\nCannot delete room. It has active reservations.\n");
        return 0;
    }
    
    /* Confirm deletion */
    printf("\nAre you sure you want to delete room %d? (y/n): ", roomId);
    scanf(" %c", &confirm);
    
    if (confirm != 'y' && confirm != 'Y') {
        printf("\nDeletion cancelled.\n");
        return 0;
    }
    
    /* Open files for deletion */
    fp = fopen(ROOMS_FILE, "rb");
    if (fp == NULL) {
        printf("\nError: Could not open rooms file.\n");
        return 0;
    }
    
    tempFp = fopen(TEMP_FILE, "wb");
    if (tempFp == NULL) {
        fclose(fp);
        printf("\nError: Could not create temporary file.\n");
        return 0;
    }
    
    /* Mark room as inactive instead of deleting */
    while (fread(&tempRoom, sizeof(Room), 1, fp) == 1) {
        if (tempRoom.id == roomId) {
            tempRoom.isActive = 0; /* Mark as inactive */
        }
        fwrite(&tempRoom, sizeof(Room), 1, tempFp);
    }
    
    fclose(fp);
    fclose(tempFp);
    
    /* Replace original file with temp file */
    remove(ROOMS_FILE);
    rename(TEMP_FILE, ROOMS_FILE);
    
    printf("\nRoom deleted successfully.\n");
    return 1;
}

/* List all rooms */
void listRooms(User *currentUser, RoomStatus statusFilter) {
    FILE *fp;
    Room tempRoom;
    int count = 0;
    
    /* Open rooms file */
    fp = fopen(ROOMS_FILE, "rb");
    if (fp == NULL) {
        printf("\nError: Could not open rooms file.\n");
        return;
    }
    
    /* Display header */
    clearScreen();
    printf("===== ROOM LIST =====\n");
    
    if (statusFilter > 0) {
        printf("Filtered by status: %s\n\n", getRoomStatusString(statusFilter));
    }
    
    printf("%-6s %-12s %-12s %-10s %-8s %-10s %-s\n", 
           "Room", "Type", "Status", "Rate", "Capacity", "Floor", "Description");
    printf("---------------------------------------------------------------------------------\n");
    
    /* Display each room */
    while (fread(&tempRoom, sizeof(Room), 1, fp) == 1) {
        if (tempRoom.isActive && (statusFilter == 0 || tempRoom.status == statusFilter)) {
            printf("%-6d %-12s %-12s $%-9.2f %-8d %-10d %s\n", 
                   tempRoom.id, 
                   getRoomTypeString(tempRoom.type), 
                   getRoomStatusString(tempRoom.status), 
                   tempRoom.rate,
                   tempRoom.capacity,
                   tempRoom.floor,
                   tempRoom.description);
            count++;
        }
    }
    
    fclose(fp);
    
    /* Display footer */
    printf("---------------------------------------------------------------------------------\n");
    printf("Total rooms: %d\n", count);
}

/* Search for rooms based on criteria */
void searchRooms(User *currentUser) {
    FILE *fp;
    Room tempRoom;
    int typeFilter = 0;
    int capacityFilter = 0;
    double maxRateFilter = 0.0;
    int count = 0;
    
    /* Get search criteria */
    clearScreen();
    printf("===== SEARCH ROOMS =====\n");
    
    printf("Room Type (0 for any):\n");
    printf("1. Standard\n");
    printf("2. Deluxe\n");
    printf("3. Suite\n");
    printf("4. Executive\n");
    printf("5. Presidential\n");
    typeFilter = getIntInput("Enter room type (0-5): ", 0, 5);
    
    printf("\nMinimum Capacity (0 for any): ");
    capacityFilter = getIntInput("", 0, 20);
    
    printf("\nMaximum Rate per Night (0 for any): $");
    maxRateFilter = getDoubleInput("", 0.0, 10000.0);
    
    /* Open rooms file */
    fp = fopen(ROOMS_FILE, "rb");
    if (fp == NULL) {
        printf("\nError: Could not open rooms file.\n");
        return;
    }
    
    /* Display header */
    printf("\n===== SEARCH RESULTS =====\n");
    printf("%-6s %-12s %-12s %-10s %-8s %-10s %-s\n", 
           "Room", "Type", "Status", "Rate", "Capacity", "Floor", "Description");
    printf("---------------------------------------------------------------------------------\n");
    
    /* Display matching rooms */
    while (fread(&tempRoom, sizeof(Room), 1, fp) == 1) {
        if (tempRoom.isActive && 
            (typeFilter == 0 || tempRoom.type == typeFilter) &&
            (capacityFilter == 0 || tempRoom.capacity >= capacityFilter) &&
            (maxRateFilter == 0.0 || tempRoom.rate <= maxRateFilter)) {
            
            printf("%-6d %-12s %-12s $%-9.2f %-8d %-10d %s\n", 
                   tempRoom.id, 
                   getRoomTypeString(tempRoom.type), 
                   getRoomStatusString(tempRoom.status), 
                   tempRoom.rate,
                   tempRoom.capacity,
                   tempRoom.floor,
                   tempRoom.description);
            count++;
        }
    }
    
    fclose(fp);
    
    /* Display footer */
    printf("---------------------------------------------------------------------------------\n");
    printf("Total matching rooms: %d\n", count);
}

/* Change the status of a room */
int changeRoomStatus(int roomId, RoomStatus newStatus) {
    FILE *fp, *tempFp;
    Room tempRoom;
    int found = 0;
    
    /* Open rooms file */
    fp = fopen(ROOMS_FILE, "rb");
    if (fp == NULL) {
        printf("\nError: Could not open rooms file.\n");
        return 0;
    }
    
    /* Find room to modify */
    while (fread(&tempRoom, sizeof(Room), 1, fp) == 1) {
        if (tempRoom.id == roomId && tempRoom.isActive) {
            found = 1;
            break;
        }
    }
    
    fclose(fp);
    
    if (!found) {
        printf("\nRoom with ID %d not found.\n", roomId);
        return 0;
    }
    
    /* Open files for modification */
    fp = fopen(ROOMS_FILE, "rb");
    if (fp == NULL) {
        printf("\nError: Could not open rooms file.\n");
        return 0;
    }
    
    tempFp = fopen(TEMP_FILE, "wb");
    if (tempFp == NULL) {
        fclose(fp);
        printf("\nError: Could not create temporary file.\n");
        return 0;
    }
    
    /* Process each room */
    while (fread(&tempRoom, sizeof(Room), 1, fp) == 1) {
        if (tempRoom.id == roomId && tempRoom.isActive) {
            tempRoom.status = newStatus;
        }
        fwrite(&tempRoom, sizeof(Room), 1, tempFp);
    }
    
    fclose(fp);
    fclose(tempFp);
    
    /* Replace original file with temp file */
    remove(ROOMS_FILE);
    rename(TEMP_FILE, ROOMS_FILE);
    
    printf("\nRoom status changed to %s.\n", getRoomStatusString(newStatus));
    return 1;
}

/* Get a room by ID */
int getRoomById(int roomId, Room *room) {
    FILE *fp;
    Room tempRoom;
    int found = 0;
    
    /* Open rooms file */
    fp = fopen(ROOMS_FILE, "rb");
    if (fp == NULL) {
        return 0; /* File not found */
    }
    
    /* Find room */
    while (fread(&tempRoom, sizeof(Room), 1, fp) == 1) {
        if (tempRoom.id == roomId && tempRoom.isActive) {
            *room = tempRoom;
            found = 1;
            break;
        }
    }
    
    fclose(fp);
    return found;
}

/* Check if a room is available for a given date range */
int isRoomAvailable(int roomId, const char *startDate, const char *endDate) {
    /* This function will be implemented in reservation.c */
    /* It will check if there are any reservations for this room in the given date range */
    return checkRoomAvailability(roomId, startDate, endDate);
}

/* Room management menu */
void roomManagementMenu(User *currentUser) {
    int choice;
    int roomId;
    int statusFilter;
    
    do {
        clearScreen();
        printf("===== ROOM MANAGEMENT =====\n");
        printf("1. List All Rooms\n");
        printf("2. List Rooms by Status\n");
        printf("3. Search Rooms\n");
        printf("4. Add Room\n");
        printf("5. Modify Room\n");
        printf("6. Change Room Status\n");
        printf("7. Delete Room\n");
        printf("0. Back to Main Menu\n");
        printf("============================\n");
        
        choice = getIntInput("Enter your choice: ", 0, 7);
        
        switch (choice) {
            case 1: /* List All Rooms */
                listRooms(currentUser, 0);
                break;
                
            case 2: /* List Rooms by Status */
                printf("\nRoom Status:\n");
                printf("1. Available\n");
                printf("2. Occupied\n");
                printf("3. Maintenance\n");
                printf("4. Reserved\n");
                printf("5. Cleaning\n");
                statusFilter = getIntInput("Enter status to filter by (1-5): ", 1, 5);
                listRooms(currentUser, statusFilter);
                break;
                
            case 3: /* Search Rooms */
                searchRooms(currentUser);
                break;
                
            case 4: /* Add Room */
                if (currentUser->role == ROLE_ADMIN) {
                    addRoom(currentUser);
                } else {
                    printf("\nAccess denied. Admin privileges required.\n");
                }
                break;
                
            case 5: /* Modify Room */
                if (currentUser->role == ROLE_ADMIN) {
                    listRooms(currentUser, 0);
                    roomId = getIntInput("\nEnter room number to modify (0 to cancel): ", 0, 9999);
                    if (roomId != 0) {
                        modifyRoom(currentUser, roomId);
                    }
                } else {
                    printf("\nAccess denied. Admin privileges required.\n");
                }
                break;
                
            case 6: /* Change Room Status */
                listRooms(currentUser, 0);
                roomId = getIntInput("\nEnter room number to change status (0 to cancel): ", 0, 9999);
                if (roomId != 0) {
                    printf("\nRoom Status:\n");
                    printf("1. Available\n");
                    printf("2. Occupied\n");
                    printf("3. Maintenance\n");
                    printf("4. Reserved\n");
                    printf("5. Cleaning\n");
                    statusFilter = getIntInput("Enter new status (1-5): ", 1, 5);
                    changeRoomStatus(roomId, statusFilter);
                }
                break;
                
            case 7: /* Delete Room */
                if (currentUser->role == ROLE_ADMIN) {
                    listRooms(currentUser, 0);
                    roomId = getIntInput("\nEnter room number to delete (0 to cancel): ", 0, 9999);
                    if (roomId != 0) {
                        deleteRoom(currentUser, roomId);
                    }
                } else {
                    printf("\nAccess denied. Admin privileges required.\n");
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