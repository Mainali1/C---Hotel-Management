/**
 * Hotel Management System
 * reservation.c - Reservation management implementation
 * 
 * This file implements the reservation management functionality
 * including adding, modifying, and listing reservations.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "reservation.h"
#include "fileio.h"
#include "utils.h"
#include "ui.h"

/* Forward declarations for static functions */
static int checkRoomAvailabilityExcludingReservation(int roomId, const char *startDate, const char *endDate, int excludeReservationId);

#define RESERVATIONS_FILE "data/reservations.dat"
#define TEMP_FILE "data/temp.dat"
#define MAX_RESERVATIONS 10000

/* Get the string representation of a reservation status */
const char* getReservationStatusString(int status) {
    switch (status) {
        case RESERVATION_STATUS_CONFIRMED:
            return "Confirmed";
        case RESERVATION_STATUS_CHECKED_IN:
            return "Checked In";
        case RESERVATION_STATUS_CHECKED_OUT:
            return "Checked Out";
        case RESERVATION_STATUS_CANCELLED:
            return "Cancelled";
        case RESERVATION_STATUS_NO_SHOW:
            return "No Show";
        default:
            return "Unknown";
    }
}

/* Initialize reservation data file */
int initializeReservationData(void) {
    FILE *fp;
    Reservation sampleReservations[2] = {
        {
            .id = 1,
            .guestId = 1, /* John Smith */
            .roomId = 101,
            .checkInDate = "2023-05-15",
            .checkOutDate = "2023-05-18",
            .status = RESERVATION_STATUS_CHECKED_IN,
            .numGuests = 2,
            .totalAmount = 300.0,
            .paidAmount = 300.0,
            .creationDate = "2023-05-01 10:30:00",
            .createdBy = 1, /* Admin */
            .notes = "Business trip. Requested early check-in.",
            .isActive = 1
        },
        {
            .id = 2,
            .guestId = 2, /* Jane Doe */
            .roomId = 201,
            .checkInDate = "2023-06-10",
            .checkOutDate = "2023-06-15",
            .status = RESERVATION_STATUS_CONFIRMED,
            .numGuests = 2,
            .totalAmount = 750.0,
            .paidAmount = 200.0, /* Partial payment */
            .creationDate = "2023-05-05 14:45:00",
            .createdBy = 1, /* Admin */
            .notes = "Honeymoon stay. Requested champagne in room.",
            .isActive = 1
        }
    };
    int numSampleReservations = sizeof(sampleReservations) / sizeof(sampleReservations[0]);
    
    /* Create data directory if it doesn't exist */
    createDirectoryIfNotExists("data");
    
    /* Check if reservations file already exists */
    fp = fopen(RESERVATIONS_FILE, "rb");
    if (fp != NULL) {
        /* Check if file has content */
        fseek(fp, 0, SEEK_END);
        if (ftell(fp) > 0) {
            fclose(fp);
            return 1; /* Reservations already exist */
        }
        fclose(fp);
    }
    
    /* Create reservations file and write sample data */
    fp = fopen(RESERVATIONS_FILE, "wb");
    if (!fp) {
        return 0;
    }

    /* Write sample reservations to file */
    if (fwrite(sampleReservations, sizeof(Reservation), numSampleReservations, fp) != numSampleReservations) {
        fclose(fp);
        return 0;
    }

    fclose(fp);
    return 1;
    
    /* Reservation 3 - Checked Out (past) */
    sampleReservations[2].id = 3;
    sampleReservations[2].guestId = 3; /* Robert Johnson */
    sampleReservations[2].roomId = 301;
    strcpy(sampleReservations[2].checkInDate, "2023-04-20");
    strcpy(sampleReservations[2].checkOutDate, "2023-04-25");
    sampleReservations[2].status = RESERVATION_STATUS_CHECKED_OUT;
    sampleReservations[2].numGuests = 3;
    sampleReservations[2].totalAmount = 1250.0;
    sampleReservations[2].paidAmount = 1250.0;
    strcpy(sampleReservations[2].creationDate, "2023-03-15 09:20:00");
    sampleReservations[2].createdBy = 1; /* Admin */
    strcpy(sampleReservations[2].notes, "Family vacation. Late checkout approved.");
    sampleReservations[2].isActive = 1;
    
    /* Write sample reservations to file */
    fp = fopen(RESERVATIONS_FILE, "wb");
    if (fp == NULL) {
        return 0; /* Failed to create file */
    }
    
    fwrite(sampleReservations, sizeof(Reservation), 3, fp);
    fclose(fp);
    
    printf("\nSample reservation data created.\n");
    
    return 1; /* Reservation data created successfully */
}

/* Calculate the number of days between two dates */
int calculateDays(const char *startDate, const char *endDate) {
    struct tm start = {0}, end = {0};
    time_t startTime, endTime;
    double diffSeconds;
    int diffDays;
    
    /* Parse start date */
    sscanf(startDate, "%d-%d-%d", &start.tm_year, &start.tm_mon, &start.tm_mday);
    start.tm_year -= 1900; /* Adjust year */
    start.tm_mon -= 1;     /* Adjust month */
    
    /* Parse end date */
    sscanf(endDate, "%d-%d-%d", &end.tm_year, &end.tm_mon, &end.tm_mday);
    end.tm_year -= 1900; /* Adjust year */
    end.tm_mon -= 1;     /* Adjust month */
    
    /* Convert to time_t */
    startTime = mktime(&start);
    endTime = mktime(&end);
    
    /* Calculate difference in seconds */
    diffSeconds = difftime(endTime, startTime);
    
    /* Convert to days */
    diffDays = (int)(diffSeconds / (60 * 60 * 24));
    
    return diffDays;
}

/* Calculate the total amount for a reservation */
double calculateReservationAmount(int roomId, const char *startDate, const char *endDate) {
    Room room;
    int days;
    
    /* Get room details */
    if (!getRoomById(roomId, &room)) {
        return 0.0; /* Room not found */
    }
    
    /* Calculate number of days */
    days = calculateDays(startDate, endDate);
    
    /* Calculate total amount */
    return room.rate * days;
}

/* Check if a room is available for a given date range */
int checkRoomAvailability(int roomId, const char *startDate, const char *endDate) {
    FILE *fp;
    Reservation tempReservation;
    Room room;
    
    /* Check if room exists and is available */
    if (!getRoomById(roomId, &room)) {
        return 0; /* Room not found */
    }
    
    if (room.status == ROOM_STATUS_MAINTENANCE) {
        return 0; /* Room under maintenance */
    }
    
    /* Open reservations file */
    fp = fopen(RESERVATIONS_FILE, "rb");
    if (fp == NULL) {
        return 1; /* No reservations file, so room is available */
    }
    
    /* Check for overlapping reservations */
    while (fread(&tempReservation, sizeof(Reservation), 1, fp) == 1) {
        if (tempReservation.roomId == roomId && 
            tempReservation.isActive && 
            (tempReservation.status == RESERVATION_STATUS_CONFIRMED || 
             tempReservation.status == RESERVATION_STATUS_CHECKED_IN)) {
            
            /* Check for date overlap */
            if ((strcmp(startDate, tempReservation.checkOutDate) < 0 && 
                 strcmp(endDate, tempReservation.checkInDate) > 0)) {
                fclose(fp);
                return 0; /* Dates overlap, room not available */
            }
        }
    }
    
    fclose(fp);
    return 1; /* Room is available */
}

/* Check if a room has active reservations */
int hasActiveReservations(int roomId) {
    FILE *fp;
    Reservation tempReservation;
    
    /* Open reservations file */
    fp = fopen(RESERVATIONS_FILE, "rb");
    if (fp == NULL) {
        return 0; /* No reservations file */
    }
    
    /* Check for active reservations */
    while (fread(&tempReservation, sizeof(Reservation), 1, fp) == 1) {
        if (tempReservation.roomId == roomId && 
            tempReservation.isActive && 
            (tempReservation.status == RESERVATION_STATUS_CONFIRMED || 
             tempReservation.status == RESERVATION_STATUS_CHECKED_IN)) {
            
            fclose(fp);
            return 1; /* Room has active reservations */
        }
    }
    
    fclose(fp);
    return 0; /* No active reservations */
}

/* Check if a guest has active reservations */
int hasActiveReservationsByGuest(int guestId) {
    FILE *fp;
    Reservation tempReservation;
    
    /* Open reservations file */
    fp = fopen(RESERVATIONS_FILE, "rb");
    if (fp == NULL) {
        return 0; /* No reservations file */
    }
    
    /* Check for active reservations */
    while (fread(&tempReservation, sizeof(Reservation), 1, fp) == 1) {
        if (tempReservation.guestId == guestId && 
            tempReservation.isActive && 
            (tempReservation.status == RESERVATION_STATUS_CONFIRMED || 
             tempReservation.status == RESERVATION_STATUS_CHECKED_IN)) {
            
            fclose(fp);
            return 1; /* Guest has active reservations */
        }
    }
    
    fclose(fp);
    return 0; /* No active reservations */
}

/* Add a new reservation */
int addReservation(User *currentUser) {
    FILE *fp;
    Reservation newReservation;
    Guest guest;
    Room room;
    int nextId = 1;
    int guestId, roomId;
    char startDate[11], endDate[11];
    int validDates = 0;
    time_t now;
    struct tm *t;
    char dateStr[20];
    
    /* Find next available ID */
    fp = fopen(RESERVATIONS_FILE, "rb");
    if (fp != NULL) {
        Reservation tempReservation;
        while (fread(&tempReservation, sizeof(Reservation), 1, fp) == 1) {
            if (tempReservation.id >= nextId) {
                nextId = tempReservation.id + 1;
            }
        }
        fclose(fp);
    }
    
    /* Get reservation details */
    clearScreen();
    printf("===== ADD NEW RESERVATION =====\n");
    
    /* Get guest ID */
    listGuests(currentUser);
    guestId = getIntInput("\nEnter Guest ID (0 to add new guest): ", 0, 9999);
    
    if (guestId == 0) {
        /* Add new guest */
        guestId = addGuest(currentUser);
        if (guestId == 0) {
            printf("\nFailed to add guest. Reservation cancelled.\n");
            return 0;
        }
    } else {
        /* Verify guest exists */
        if (!getGuestById(guestId, &guest)) {
            printf("\nGuest with ID %d not found. Reservation cancelled.\n", guestId);
            return 0;
        }
    }
    
    /* Get room ID */
    listRooms(currentUser, ROOM_STATUS_AVAILABLE);
    roomId = getIntInput("\nEnter Room ID: ", 1, 9999);
    
    /* Verify room exists and is available */
    if (!getRoomById(roomId, &room)) {
        printf("\nRoom with ID %d not found. Reservation cancelled.\n", roomId);
        return 0;
    }
    
    if (room.status != ROOM_STATUS_AVAILABLE) {
        printf("\nRoom %d is not available (current status: %s).\n", 
               roomId, getRoomStatusString(room.status));
        printf("Reservation cancelled.\n");
        return 0;
    }
    
    /* Get check-in and check-out dates */
    do {
        printf("\nEnter Check-in Date (YYYY-MM-DD): ");
        scanf("%10s", startDate);
        
        printf("Enter Check-out Date (YYYY-MM-DD): ");
        scanf("%10s", endDate);
        
        /* Validate dates */
        if (!isValidDate(startDate) || !isValidDate(endDate)) {
            printf("\nInvalid date format. Please use YYYY-MM-DD.\n");
            continue;
        }
        
        if (strcmp(startDate, endDate) >= 0) {
            printf("\nCheck-out date must be after check-in date.\n");
            continue;
        }
        
        /* Check if room is available for these dates */
        if (!checkRoomAvailability(roomId, startDate, endDate)) {
            printf("\nRoom %d is not available for the selected dates.\n", roomId);
            printf("Please choose different dates or a different room.\n");
            continue;
        }
        
        validDates = 1;
        
    } while (!validDates);
    
    /* Get number of guests */
    printf("\nNumber of Guests: ");
    newReservation.numGuests = getIntInput("", 1, room.capacity);
    
    /* Calculate total amount */
    newReservation.totalAmount = calculateReservationAmount(roomId, startDate, endDate);
    printf("\nTotal Amount: $%.2f\n", newReservation.totalAmount);
    
    /* Get paid amount */
    printf("Amount Paid Now: $");
    newReservation.paidAmount = getDoubleInput("", 0.0, newReservation.totalAmount);
    
    /* Get notes */
    printf("\nNotes (optional): ");
    getchar(); /* Consume newline */
    fgets(newReservation.notes, MAX_RESERVATION_NOTES_LEN, stdin);
    newReservation.notes[strcspn(newReservation.notes, "\n")] = '\0'; /* Remove newline */
    
    /* Set reservation details */
    newReservation.id = nextId;
    newReservation.guestId = guestId;
    newReservation.roomId = roomId;
    strcpy(newReservation.checkInDate, startDate);
    strcpy(newReservation.checkOutDate, endDate);
    newReservation.status = RESERVATION_STATUS_CONFIRMED;
    
    /* Set creation date and user */
    now = time(NULL);
    t = localtime(&now);
    sprintf(dateStr, "%04d-%02d-%02d %02d:%02d:%02d", 
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
            t->tm_hour, t->tm_min, t->tm_sec);
    strcpy(newReservation.creationDate, dateStr);
    newReservation.createdBy = currentUser->id;
    newReservation.isActive = 1;
    
    /* Write new reservation to file */
    fp = fopen(RESERVATIONS_FILE, "ab");
    if (fp == NULL) {
        printf("\nError: Could not open reservations file.\n");
        return 0;
    }
    
    fwrite(&newReservation, sizeof(Reservation), 1, fp);
    fclose(fp);
    
    /* Update room status to reserved */
    changeRoomStatus(currentUser, roomId, ROOM_STATUS_RESERVED);
    
    printf("\nReservation added successfully with ID: %d\n", newReservation.id);
    return newReservation.id;
}

/* Modify an existing reservation */
int modifyReservation(User *currentUser, int reservationId) {
    FILE *fp, *tempFp;
    Reservation tempReservation;
    int found = 0;
    int choice;
    char newStartDate[11], newEndDate[11];
    int newRoomId;
    int newNumGuests;
    double newPaidAmount;
    char newNotes[MAX_RESERVATION_NOTES_LEN];
    Room room;
    
    /* Open reservations file */
    fp = fopen(RESERVATIONS_FILE, "rb");
    if (fp == NULL) {
        printf("\nError: Could not open reservations file.\n");
        return 0;
    }
    
    /* Find reservation to modify */
    while (fread(&tempReservation, sizeof(Reservation), 1, fp) == 1) {
        if (tempReservation.id == reservationId && tempReservation.isActive) {
            found = 1;
            break;
        }
    }
    
    fclose(fp);
    
    if (!found) {
        printf("\nReservation with ID %d not found.\n", reservationId);
        return 0;
    }
    
    /* Check if reservation can be modified */
    if (tempReservation.status == RESERVATION_STATUS_CHECKED_OUT || 
        tempReservation.status == RESERVATION_STATUS_CANCELLED || 
        tempReservation.status == RESERVATION_STATUS_NO_SHOW) {
        printf("\nCannot modify a reservation with status: %s\n", 
               getReservationStatusString(tempReservation.status));
        return 0;
    }
    
    /* Display reservation details and modification menu */
    clearScreen();
    printf("===== MODIFY RESERVATION =====\n");
    printf("Reservation ID: %d\n", tempReservation.id);
    
    /* Get guest and room details */
    Guest guest;
    getGuestById(tempReservation.guestId, &guest);
    getRoomById(tempReservation.roomId, &room);
    
    printf("Guest: %s (ID: %d)\n", guest.name, guest.id);
    printf("Room: %d (%s)\n", room.id, getRoomTypeString(room.type));
    printf("Check-in Date: %s\n", tempReservation.checkInDate);
    printf("Check-out Date: %s\n", tempReservation.checkOutDate);
    printf("Status: %s\n", getReservationStatusString(tempReservation.status));
    printf("Number of Guests: %d\n", tempReservation.numGuests);
    printf("Total Amount: $%.2f\n", tempReservation.totalAmount);
    printf("Paid Amount: $%.2f\n", tempReservation.paidAmount);
    printf("Notes: %s\n", tempReservation.notes);
    
    printf("\nWhat would you like to modify?\n");
    printf("1. Check-in/Check-out Dates\n");
    printf("2. Room\n");
    printf("3. Number of Guests\n");
    printf("4. Paid Amount\n");
    printf("5. Notes\n");
    printf("0. Cancel\n");
    
    choice = getIntInput("Enter your choice: ", 0, 5);
    
    if (choice == 0) {
        return 0; /* Cancelled */
    }
    
    /* Handle modification based on choice */
    switch (choice) {
        case 1: /* Modify dates */
            {
                int validDates = 0;
                
                do {
                    printf("\nCurrent Check-in Date: %s\n", tempReservation.checkInDate);
                    printf("Current Check-out Date: %s\n", tempReservation.checkOutDate);
                    
                    printf("\nEnter New Check-in Date (YYYY-MM-DD): ");
                    scanf("%10s", newStartDate);
                    
                    printf("Enter New Check-out Date (YYYY-MM-DD): ");
                    scanf("%10s", newEndDate);
                    
                    /* Validate dates */
                    if (!isValidDate(newStartDate) || !isValidDate(newEndDate)) {
                        printf("\nInvalid date format. Please use YYYY-MM-DD.\n");
                        continue;
                    }
                    
                    if (strcmp(newStartDate, newEndDate) >= 0) {
                        printf("\nCheck-out date must be after check-in date.\n");
                        continue;
                    }
                    
                    /* If dates are the same as before, no need to check availability */
                    if (strcmp(newStartDate, tempReservation.checkInDate) == 0 && 
                        strcmp(newEndDate, tempReservation.checkOutDate) == 0) {
                        printf("\nDates are the same as before. No changes made.\n");
                        return 0;
                    }
                    
                    /* Check if room is available for these dates */
                    /* We need to exclude the current reservation from the check */
                    if (!checkRoomAvailabilityExcludingReservation(
                            tempReservation.roomId, newStartDate, newEndDate, reservationId)) {
                        printf("\nRoom %d is not available for the selected dates.\n", 
                               tempReservation.roomId);
                        printf("Please choose different dates.\n");
                        continue;
                    }
                    
                    validDates = 1;
                    
                } while (!validDates);
                
                /* Calculate new total amount */
                double newTotalAmount = calculateReservationAmount(
                    tempReservation.roomId, newStartDate, newEndDate);
                
                /* Update reservation */
                fp = fopen(RESERVATIONS_FILE, "rb");
                if (fp == NULL) {
                    printf("\nError: Could not open reservations file.\n");
                    return 0;
                }
                
                tempFp = fopen(TEMP_FILE, "wb");
                if (tempFp == NULL) {
                    fclose(fp);
                    printf("\nError: Could not create temporary file.\n");
                    return 0;
                }
                
                while (fread(&tempReservation, sizeof(Reservation), 1, fp) == 1) {
                    if (tempReservation.id == reservationId && tempReservation.isActive) {
                        strcpy(tempReservation.checkInDate, newStartDate);
                        strcpy(tempReservation.checkOutDate, newEndDate);
                        tempReservation.totalAmount = newTotalAmount;
                    }
                    fwrite(&tempReservation, sizeof(Reservation), 1, tempFp);
                }
                
                fclose(fp);
                fclose(tempFp);
                
                /* Replace original file with temp file */
                remove(RESERVATIONS_FILE);
                rename(TEMP_FILE, RESERVATIONS_FILE);
                
                printf("\nReservation dates updated successfully.\n");
                printf("New Total Amount: $%.2f\n", newTotalAmount);
            }
            break;
            
        case 2: /* Modify room */
            {
                /* List available rooms */
                listRooms(currentUser, ROOM_STATUS_AVAILABLE);
                
                newRoomId = getIntInput("\nEnter New Room ID (0 to cancel): ", 0, 9999);
                
                if (newRoomId == 0) {
                    return 0; /* Cancelled */
                }
                
                /* Verify room exists and is available */
                if (!getRoomById(newRoomId, &room)) {
                    printf("\nRoom with ID %d not found.\n", newRoomId);
                    return 0;
                }
                
                /* If room is the same as before, no need to check availability */
                if (newRoomId == tempReservation.roomId) {
                    printf("\nRoom is the same as before. No changes made.\n");
                    return 0;
                }
                
                /* Check if new room is available for the reservation dates */
                if (!checkRoomAvailability(newRoomId, 
                                          tempReservation.checkInDate, 
                                          tempReservation.checkOutDate)) {
                    printf("\nRoom %d is not available for the reservation dates.\n", newRoomId);
                    return 0;
                }
                
                /* Check if number of guests exceeds new room capacity */
                if (tempReservation.numGuests > room.capacity) {
                    printf("\nNumber of guests (%d) exceeds room capacity (%d).\n", 
                           tempReservation.numGuests, room.capacity);
                    return 0;
                }
                
                /* Calculate new total amount */
                double newTotalAmount = calculateReservationAmount(
                    newRoomId, tempReservation.checkInDate, tempReservation.checkOutDate);
                
                /* Update reservation */
                fp = fopen(RESERVATIONS_FILE, "rb");
                if (fp == NULL) {
                    printf("\nError: Could not open reservations file.\n");
                    return 0;
                }
                
                tempFp = fopen(TEMP_FILE, "wb");
                if (tempFp == NULL) {
                    fclose(fp);
                    printf("\nError: Could not create temporary file.\n");
                    return 0;
                }
                
                while (fread(&tempReservation, sizeof(Reservation), 1, fp) == 1) {
                    if (tempReservation.id == reservationId && tempReservation.isActive) {
                        int oldRoomId = tempReservation.roomId;
                        tempReservation.roomId = newRoomId;
                        tempReservation.totalAmount = newTotalAmount;
                        
                        /* Update room statuses */
                        if (tempReservation.status == RESERVATION_STATUS_CONFIRMED) {
                            changeRoomStatus(currentUser, oldRoomId, ROOM_STATUS_AVAILABLE);
                            changeRoomStatus(currentUser, newRoomId, ROOM_STATUS_RESERVED);
                        } else if (tempReservation.status == RESERVATION_STATUS_CHECKED_IN) {
                            changeRoomStatus(currentUser, oldRoomId, ROOM_STATUS_AVAILABLE);
                            changeRoomStatus(currentUser, newRoomId, ROOM_STATUS_OCCUPIED);
                        }
                    }
                    fwrite(&tempReservation, sizeof(Reservation), 1, tempFp);
                }
                
                fclose(fp);
                fclose(tempFp);
                
                /* Replace original file with temp file */
                remove(RESERVATIONS_FILE);
                rename(TEMP_FILE, RESERVATIONS_FILE);
                
                printf("\nReservation room updated successfully.\n");
                printf("New Total Amount: $%.2f\n", newTotalAmount);
            }
            break;
            
        case 3: /* Modify number of guests */
            {
                /* Get room capacity */
                getRoomById(tempReservation.roomId, &room);
                
                printf("\nCurrent Number of Guests: %d\n", tempReservation.numGuests);
                printf("Room Capacity: %d\n", room.capacity);
                
                newNumGuests = getIntInput("Enter New Number of Guests: ", 1, room.capacity);
                
                /* Update reservation */
                fp = fopen(RESERVATIONS_FILE, "rb");
                if (fp == NULL) {
                    printf("\nError: Could not open reservations file.\n");
                    return 0;
                }
                
                tempFp = fopen(TEMP_FILE, "wb");
                if (tempFp == NULL) {
                    fclose(fp);
                    printf("\nError: Could not create temporary file.\n");
                    return 0;
                }
                
                while (fread(&tempReservation, sizeof(Reservation), 1, fp) == 1) {
                    if (tempReservation.id == reservationId && tempReservation.isActive) {
                        tempReservation.numGuests = newNumGuests;
                    }
                    fwrite(&tempReservation, sizeof(Reservation), 1, tempFp);
                }
                
                fclose(fp);
                fclose(tempFp);
                
                /* Replace original file with temp file */
                remove(RESERVATIONS_FILE);
                rename(TEMP_FILE, RESERVATIONS_FILE);
                
                printf("\nNumber of guests updated successfully.\n");
            }
            break;
            
        case 4: /* Modify paid amount */
            {
                printf("\nCurrent Total Amount: $%.2f\n", tempReservation.totalAmount);
                printf("Current Paid Amount: $%.2f\n", tempReservation.paidAmount);
                printf("Balance Due: $%.2f\n", tempReservation.totalAmount - tempReservation.paidAmount);
                
                newPaidAmount = getDoubleInput("Enter New Paid Amount: $", 0.0, tempReservation.totalAmount);
                
                /* Update reservation */
                fp = fopen(RESERVATIONS_FILE, "rb");
                if (fp == NULL) {
                    printf("\nError: Could not open reservations file.\n");
                    return 0;
                }
                
                tempFp = fopen(TEMP_FILE, "wb");
                if (tempFp == NULL) {
                    fclose(fp);
                    printf("\nError: Could not create temporary file.\n");
                    return 0;
                }
                
                while (fread(&tempReservation, sizeof(Reservation), 1, fp) == 1) {
                    if (tempReservation.id == reservationId && tempReservation.isActive) {
                        tempReservation.paidAmount = newPaidAmount;
                    }
                    fwrite(&tempReservation, sizeof(Reservation), 1, tempFp);
                }
                
                fclose(fp);
                fclose(tempFp);
                
                /* Replace original file with temp file */
                remove(RESERVATIONS_FILE);
                rename(TEMP_FILE, RESERVATIONS_FILE);
                
                printf("\nPaid amount updated successfully.\n");
                printf("New Balance Due: $%.2f\n", tempReservation.totalAmount - newPaidAmount);
            }
            break;
            
        case 5: /* Modify notes */
            {
                printf("\nCurrent Notes: %s\n", tempReservation.notes);
                
                printf("Enter New Notes: ");
                getchar(); /* Consume newline */
                fgets(newNotes, MAX_RESERVATION_NOTES_LEN, stdin);
                newNotes[strcspn(newNotes, "\n")] = '\0'; /* Remove newline */
                
                /* Update reservation */
                fp = fopen(RESERVATIONS_FILE, "rb");
                if (fp == NULL) {
                    printf("\nError: Could not open reservations file.\n");
                    return 0;
                }
                
                tempFp = fopen(TEMP_FILE, "wb");
                if (tempFp == NULL) {
                    fclose(fp);
                    printf("\nError: Could not create temporary file.\n");
                    return 0;
                }
                
                while (fread(&tempReservation, sizeof(Reservation), 1, fp) == 1) {
                    if (tempReservation.id == reservationId && tempReservation.isActive) {
                        strcpy(tempReservation.notes, newNotes);
                    }
                    fwrite(&tempReservation, sizeof(Reservation), 1, tempFp);
                }
                
                fclose(fp);
                fclose(tempFp);
                
                /* Replace original file with temp file */
                remove(RESERVATIONS_FILE);
                rename(TEMP_FILE, RESERVATIONS_FILE);
                
                printf("\nNotes updated successfully.\n");
            }
            break;
    }
    
    return 1;
}

/* Check room availability excluding a specific reservation */
static int checkRoomAvailabilityExcludingReservation(
    int roomId, const char *startDate, const char *endDate, int excludeReservationId) {
    
    FILE *fp;
    Reservation tempReservation;
    Room room;
    
    /* Check if room exists and is available */
    if (!getRoomById(roomId, &room)) {
        return 0; /* Room not found */
    }
    
    if (room.status == ROOM_STATUS_MAINTENANCE) {
        return 0; /* Room under maintenance */
    }
    
    /* Open reservations file */
    fp = fopen(RESERVATIONS_FILE, "rb");
    if (fp == NULL) {
        return 1; /* No reservations file, so room is available */
    }
    
    /* Check for overlapping reservations */
    while (fread(&tempReservation, sizeof(Reservation), 1, fp) == 1) {
        if (tempReservation.roomId == roomId && 
            tempReservation.isActive && 
            tempReservation.id != excludeReservationId && 
            (tempReservation.status == RESERVATION_STATUS_CONFIRMED || 
             tempReservation.status == RESERVATION_STATUS_CHECKED_IN)) {
            
            /* Check for date overlap */
            if ((strcmp(startDate, tempReservation.checkOutDate) < 0 && 
                 strcmp(endDate, tempReservation.checkInDate) > 0)) {
                fclose(fp);
                return 0; /* Dates overlap, room not available */
            }
        }
    }
    
    fclose(fp);
    return 1; /* Room is available */
}

/* Cancel a reservation */
int cancelReservation(User *currentUser, int reservationId) {
    FILE *fp, *tempFp;
    Reservation tempReservation;
    int found = 0;
    char confirm;
    
    /* Open reservations file */
    fp = fopen(RESERVATIONS_FILE, "rb");
    if (fp == NULL) {
        printf("\nError: Could not open reservations file.\n");
        return 0;
    }
    
    /* Find reservation to cancel */
    while (fread(&tempReservation, sizeof(Reservation), 1, fp) == 1) {
        if (tempReservation.id == reservationId && tempReservation.isActive) {
            found = 1;
            break;
        }
    }
    
    fclose(fp);
    
    if (!found) {
        printf("\nReservation with ID %d not found.\n", reservationId);
        return 0;
    }
    
    /* Check if reservation can be cancelled */
    if (tempReservation.status != RESERVATION_STATUS_CONFIRMED) {
        printf("\nOnly confirmed reservations can be cancelled.\n");
        printf("Current status: %s\n", getReservationStatusString(tempReservation.status));
        return 0;
    }
    
    /* Confirm cancellation */
    printf("\nAre you sure you want to cancel reservation %d? (y/n): ", reservationId);
    scanf(" %c", &confirm);
    
    if (confirm != 'y' && confirm != 'Y') {
        printf("\nCancellation aborted.\n");
        return 0;
    }
    
    /* Update reservation */
    fp = fopen(RESERVATIONS_FILE, "rb");
    if (fp == NULL) {
        printf("\nError: Could not open reservations file.\n");
        return 0;
    }
    
    tempFp = fopen(TEMP_FILE, "wb");
    if (tempFp == NULL) {
        fclose(fp);
        printf("\nError: Could not create temporary file.\n");
        return 0;
    }
    
    while (fread(&tempReservation, sizeof(Reservation), 1, fp) == 1) {
        if (tempReservation.id == reservationId && tempReservation.isActive) {
            tempReservation.status = RESERVATION_STATUS_CANCELLED;
            
            /* Update room status */
            changeRoomStatus(currentUser, tempReservation.roomId, ROOM_STATUS_AVAILABLE);
        }
        fwrite(&tempReservation, sizeof(Reservation), 1, tempFp);
    }
    
    fclose(fp);
    fclose(tempFp);
    
    /* Replace original file with temp file */
    remove(RESERVATIONS_FILE);
    rename(TEMP_FILE, RESERVATIONS_FILE);
    
    printf("\nReservation cancelled successfully.\n");
    return 1;
}

/* Check in a guest for a reservation */
int checkInReservation(User *currentUser, int reservationId) {
    FILE *fp, *tempFp;
    Reservation tempReservation;
    int found = 0;
    
    /* Open reservations file */
    fp = fopen(RESERVATIONS_FILE, "rb");
    if (fp == NULL) {
        printf("\nError: Could not open reservations file.\n");
        return 0;
    }
    
    /* Find reservation for check-in */
    while (fread(&tempReservation, sizeof(Reservation), 1, fp) == 1) {
        if (tempReservation.id == reservationId && tempReservation.isActive) {
            found = 1;
            break;
        }
    }
    
    fclose(fp);
    
    if (!found) {
        printf("\nReservation with ID %d not found.\n", reservationId);
        return 0;
    }
    
    /* Check if reservation can be checked in */
    if (tempReservation.status != RESERVATION_STATUS_CONFIRMED) {
        printf("\nOnly confirmed reservations can be checked in.\n");
        printf("Current status: %s\n", getReservationStatusString(tempReservation.status));
        return 0;
    }
    
    /* Update reservation */
    fp = fopen(RESERVATIONS_FILE, "rb");
    if (fp == NULL) {
        printf("\nError: Could not open reservations file.\n");
        return 0;
    }
    
    tempFp = fopen(TEMP_FILE, "wb");
    if (tempFp == NULL) {
        fclose(fp);
        printf("\nError: Could not create temporary file.\n");
        return 0;
    }
    
    while (fread(&tempReservation, sizeof(Reservation), 1, fp) == 1) {
        if (tempReservation.id == reservationId && tempReservation.isActive) {
            tempReservation.status = RESERVATION_STATUS_CHECKED_IN;
            
            /* Update room status */
            changeRoomStatus(currentUser, tempReservation.roomId, ROOM_STATUS_OCCUPIED);
        }
        fwrite(&tempReservation, sizeof(Reservation), 1, tempFp);
    }
    
    fclose(fp);
    fclose(tempFp);
    
    /* Replace original file with temp file */
    remove(RESERVATIONS_FILE);
    rename(TEMP_FILE, RESERVATIONS_FILE);
    
    printf("\nGuest checked in successfully.\n");
    return 1;
}

/* Check out a guest from a reservation */
int checkOutReservation(User *currentUser, int reservationId) {
    FILE *fp, *tempFp;
    Reservation tempReservation;
    int found = 0;
    double additionalCharges = 0.0;
    double totalPaid;
    char confirm;
    
    /* Open reservations file */
    fp = fopen(RESERVATIONS_FILE, "rb");
    if (fp == NULL) {
        printf("\nError: Could not open reservations file.\n");
        return 0;
    }
    
    /* Find reservation for check-out */
    while (fread(&tempReservation, sizeof(Reservation), 1, fp) == 1) {
        if (tempReservation.id == reservationId && tempReservation.isActive) {
            found = 1;
            break;
        }
    }
    
    fclose(fp);
    
    if (!found) {
        printf("\nReservation with ID %d not found.\n", reservationId);
        return 0;
    }
    
    /* Check if reservation can be checked out */
    if (tempReservation.status != RESERVATION_STATUS_CHECKED_IN) {
        printf("\nOnly checked-in reservations can be checked out.\n");
        printf("Current status: %s\n", getReservationStatusString(tempReservation.status));
        return 0;
    }
    
    /* Get additional charges */
    printf("\nEnter additional charges (if any): $");
    additionalCharges = getDoubleInput("", 0.0, 10000.0);
    
    /* Calculate total amount */
    tempReservation.totalAmount += additionalCharges;
    
    /* Get payment */
    printf("\nTotal Amount: $%.2f\n", tempReservation.totalAmount);
    printf("Amount Already Paid: $%.2f\n", tempReservation.paidAmount);
    printf("Balance Due: $%.2f\n", tempReservation.totalAmount - tempReservation.paidAmount);
    
    if (tempReservation.totalAmount > tempReservation.paidAmount) {
        printf("\nCollect payment for balance? (y/n): ");
        scanf(" %c", &confirm);
        
        if (confirm == 'y' || confirm == 'Y') {
            totalPaid = tempReservation.totalAmount;
        } else {
            totalPaid = tempReservation.paidAmount;
        }
    } else {
        totalPaid = tempReservation.paidAmount;
    }
    
    /* Update reservation */
    fp = fopen(RESERVATIONS_FILE, "rb");
    if (fp == NULL) {
        printf("\nError: Could not open reservations file.\n");
        return 0;
    }
    
    tempFp = fopen(TEMP_FILE, "wb");
    if (tempFp == NULL) {
        fclose(fp);
        printf("\nError: Could not create temporary file.\n");
        return 0;
    }
    
    while (fread(&tempReservation, sizeof(Reservation), 1, fp) == 1) {
        if (tempReservation.id == reservationId && tempReservation.isActive) {
            tempReservation.status = RESERVATION_STATUS_CHECKED_OUT;
            tempReservation.totalAmount += additionalCharges;
            tempReservation.paidAmount = totalPaid;
            
            /* Update room status */
            changeRoomStatus(currentUser, tempReservation.roomId, ROOM_STATUS_CLEANING);
            
            /* Update guest stay information */
            updateGuestStayInfo(tempReservation.guestId, tempReservation.totalAmount);
        }
        fwrite(&tempReservation, sizeof(Reservation), 1, tempFp);
    }
    
    fclose(fp);
    fclose(tempFp);
    
    /* Replace original file with temp file */
    remove(RESERVATIONS_FILE);
    rename(TEMP_FILE, RESERVATIONS_FILE);
    
    printf("\nGuest checked out successfully.\n");
    return 1;
}

/* List all reservations */
void listReservations(User *currentUser, int statusFilter) {
    FILE *fp;
    Reservation tempReservation;
    int count = 0;
    
    /* Open reservations file */
    fp = fopen(RESERVATIONS_FILE, "rb");
    if (fp == NULL) {
        printf("\nError: Could not open reservations file.\n");
        return;
    }
    
    /* Display header */
    clearScreen();
    printf("===== RESERVATION LIST =====\n");
    
    if (statusFilter > 0) {
        printf("Filtered by status: %s\n\n", getReservationStatusString(statusFilter));
    }
    
    printf("%-5s %-20s %-6s %-12s %-12s %-15s %-10s %-10s\n", 
           "ID", "Guest", "Room", "Check-in", "Check-out", "Status", "Guests", "Total");
    printf("---------------------------------------------------------------------------------\n");
    
    /* Display each reservation */
    while (fread(&tempReservation, sizeof(Reservation), 1, fp) == 1) {
        if (tempReservation.isActive && (statusFilter == 0 || tempReservation.status == statusFilter)) {
            Guest guest;
            getGuestById(tempReservation.guestId, &guest);
            
            printf("%-5d %-20s %-6d %-12s %-12s %-15s %-10d $%-9.2f\n", 
                   tempReservation.id, 
                   guest.name, 
                   tempReservation.roomId, 
                   tempReservation.checkInDate,
                   tempReservation.checkOutDate,
                   getReservationStatusString(tempReservation.status),
                   tempReservation.numGuests,
                   tempReservation.totalAmount);
            count++;
        }
    }
    
    fclose(fp);
    
    /* Display footer */
    printf("---------------------------------------------------------------------------------\n");
    printf("Total reservations: %d\n", count);
}

/* Search for reservations based on criteria */
void searchReservations(User *currentUser) {
    FILE *fp;
    Reservation tempReservation;
    int count = 0;
    int searchType;
    int searchId;
    char searchDate[11];
    
    /* Get search criteria */
    clearScreen();
    printf("===== SEARCH RESERVATIONS =====\n");
    
    printf("Search by:\n");
    printf("1. Guest ID\n");
    printf("2. Room ID\n");
    printf("3. Check-in Date\n");
    printf("4. Check-out Date\n");
    
    searchType = getIntInput("Enter your choice: ", 1, 4);
    
    switch (searchType) {
        case 1: /* Guest ID */
        case 2: /* Room ID */
            printf("Enter ID: ");
            searchId = getIntInput("", 1, 9999);
            break;
            
        case 3: /* Check-in Date */
        case 4: /* Check-out Date */
            printf("Enter Date (YYYY-MM-DD): ");
            scanf("%10s", searchDate);
            
            if (!isValidDate(searchDate)) {
                printf("\nInvalid date format. Please use YYYY-MM-DD.\n");
                return;
            }
            break;
    }
    
    /* Open reservations file */
    fp = fopen(RESERVATIONS_FILE, "rb");
    if (fp == NULL) {
        printf("\nError: Could not open reservations file.\n");
        return;
    }
    
    /* Display header */
    printf("\n===== SEARCH RESULTS =====\n");
    printf("%-5s %-20s %-6s %-12s %-12s %-15s %-10s %-10s\n", 
           "ID", "Guest", "Room", "Check-in", "Check-out", "Status", "Guests", "Total");
    printf("---------------------------------------------------------------------------------\n");
    
    /* Search and display matching reservations */
    while (fread(&tempReservation, sizeof(Reservation), 1, fp) == 1) {
        if (tempReservation.isActive) {
            int match = 0;
            
            switch (searchType) {
                case 1: /* Guest ID */
                    if (tempReservation.guestId == searchId) {
                        match = 1;
                    }
                    break;
                    
                case 2: /* Room ID */
                    if (tempReservation.roomId == searchId) {
                        match = 1;
                    }
                    break;
                    
                case 3: /* Check-in Date */
                    if (strcmp(tempReservation.checkInDate, searchDate) == 0) {
                        match = 1;
                    }
                    break;
                    
                case 4: /* Check-out Date */
                    if (strcmp(tempReservation.checkOutDate, searchDate) == 0) {
                        match = 1;
                    }
                    break;
            }
            
            if (match) {
                Guest guest;
                getGuestById(tempReservation.guestId, &guest);
                
                printf("%-5d %-20s %-6d %-12s %-12s %-15s %-10d $%-9.2f\n", 
                       tempReservation.id, 
                       guest.name, 
                       tempReservation.roomId, 
                       tempReservation.checkInDate,
                       tempReservation.checkOutDate,
                       getReservationStatusString(tempReservation.status),
                       tempReservation.numGuests,
                       tempReservation.totalAmount);
                count++;
            }
        }
    }
    
    fclose(fp);
    
    /* Display footer */
    printf("---------------------------------------------------------------------------------\n");
    printf("Total matching reservations: %d\n", count);
}

/* Get a reservation by ID */
int getReservationById(int reservationId, Reservation *reservation) {
    FILE *fp;
    Reservation tempReservation;
    int found = 0;
    
    /* Open reservations file */
    fp = fopen(RESERVATIONS_FILE, "rb");
    if (fp == NULL) {
        return 0; /* File not found */
    }
    
    /* Find reservation */
    while (fread(&tempReservation, sizeof(Reservation), 1, fp) == 1) {
        if (tempReservation.id == reservationId && tempReservation.isActive) {
            *reservation = tempReservation;
            found = 1;
            break;
        }
    }
    
    fclose(fp);
    return found;
}

/* Reservation management menu */
void reservationManagementMenu(User *currentUser) {
    int choice;
    int reservationId;
    int statusFilter;
    
    do {
        clearScreen();
        printf("===== RESERVATION MANAGEMENT =====\n");
        printf("1. List All Reservations\n");
        printf("2. List Reservations by Status\n");
        printf("3. Search Reservations\n");
        printf("4. Add Reservation\n");
        printf("5. Modify Reservation\n");
        printf("6. Cancel Reservation\n");
        printf("7. Check-in Guest\n");
        printf("8. Check-out Guest\n");
        printf("0. Back to Main Menu\n");
        printf("==================================\n");
        
        choice = getIntInput("Enter your choice: ", 0, 8);
        
        switch (choice) {
            case 1: /* List All Reservations */
                listReservations(currentUser, 0);
                break;
                
            case 2: /* List Reservations by Status */
                printf("\nReservation Status:\n");
                printf("1. Confirmed\n");
                printf("2. Checked In\n");
                printf("3. Checked Out\n");
                printf("4. Cancelled\n");
                printf("5. No Show\n");
                statusFilter = getIntInput("Enter status to filter by (1-5): ", 1, 5);
                listReservations(currentUser, statusFilter);
                break;
                
            case 3: /* Search Reservations */
                searchReservations(currentUser);
                break;
                
            case 4: /* Add Reservation */
                addReservation(currentUser);
                break;
                
            case 5: /* Modify Reservation */
                listReservations(currentUser, 0);
                reservationId = getIntInput("\nEnter reservation ID to modify (0 to cancel): ", 0, 9999);
                if (reservationId != 0) {
                    modifyReservation(currentUser, reservationId);
                }
                break;
                
            case 6: /* Cancel Reservation */
                listReservations(currentUser, RESERVATION_STATUS_CONFIRMED);
                reservationId = getIntInput("\nEnter reservation ID to cancel (0 to cancel): ", 0, 9999);
                if (reservationId != 0) {
                    cancelReservation(currentUser, reservationId);
                }
                break;
                
            case 7: /* Check-in Guest */
                listReservations(currentUser, RESERVATION_STATUS_CONFIRMED);
                reservationId = getIntInput("\nEnter reservation ID to check in (0 to cancel): ", 0, 9999);
                if (reservationId != 0) {
                    checkInReservation(currentUser, reservationId);
                }
                break;
                
            case 8: /* Check-out Guest */
                listReservations(currentUser, RESERVATION_STATUS_CHECKED_IN);
                reservationId = getIntInput("\nEnter reservation ID to check out (0 to cancel): ", 0, 9999);
                if (reservationId != 0) {
                    checkOutReservation(currentUser, reservationId);
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