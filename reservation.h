/**
 * Hotel Management System
 * reservation.h - Reservation management header file
 * 
 * This file contains declarations for reservation management functionality
 * including reservation structure and function prototypes.
 */

#ifndef RESERVATION_H
#define RESERVATION_H

#include "auth.h" /* For User structure */
#include "room.h" /* For Room structure */
#include "guest.h" /* For Guest structure */

/* Reservation status definitions */
#define RESERVATION_STATUS_CONFIRMED 1
#define RESERVATION_STATUS_CHECKED_IN 2
#define RESERVATION_STATUS_CHECKED_OUT 3
#define RESERVATION_STATUS_CANCELLED 4
#define RESERVATION_STATUS_NO_SHOW 5

/* Maximum lengths for strings */
#define MAX_RESERVATION_NOTES_LEN 200

/**
 * Reservation structure - Stores reservation information
 */
typedef struct {
    int id;                                      /* Unique reservation ID */
    int guestId;                                  /* ID of the guest */
    int roomId;                                   /* ID of the room */
    char checkInDate[11];                         /* Check-in date (YYYY-MM-DD) */
    char checkOutDate[11];                        /* Check-out date (YYYY-MM-DD) */
    int status;                                   /* Reservation status */
    int numGuests;                                /* Number of guests */
    double totalAmount;                           /* Total amount for the stay */
    double paidAmount;                            /* Amount already paid */
    char creationDate[20];                        /* Date and time of reservation creation */
    int createdBy;                                /* ID of user who created the reservation */
    char notes[MAX_RESERVATION_NOTES_LEN];        /* Additional notes */
    int isActive;                                 /* Whether the reservation is active in the system */
} Reservation;

/* Function prototypes */

/**
 * Initialize reservation data file
 * 
 * @return 1 if successful, 0 otherwise
 */
int initializeReservationData(void);

/**
 * Check if a guest has any active reservations
 *
 * @param guestId The ID of the guest to check
 * @return 1 if guest has active reservations, 0 otherwise
 */
int hasActiveReservationsByGuest(int guestId);

/**
 * Add a new reservation to the system
 * 
 * @param currentUser The user performing the action
 * @return Reservation ID if successful, 0 otherwise
 */
int addReservation(User *currentUser);

/**
 * Modify an existing reservation
 * 
 * @param currentUser The user performing the action
 * @param reservationId ID of the reservation to modify
 * @return 1 if successful, 0 otherwise
 */
int modifyReservation(User *currentUser, int reservationId);

/**
 * Cancel a reservation
 * 
 * @param currentUser The user performing the action
 * @param reservationId ID of the reservation to cancel
 * @return 1 if successful, 0 otherwise
 */
int cancelReservation(User *currentUser, int reservationId);

/**
 * Check in a guest for a reservation
 * 
 * @param currentUser The user performing the action
 * @param reservationId ID of the reservation
 * @return 1 if successful, 0 otherwise
 */
int checkInReservation(User *currentUser, int reservationId);

/**
 * Check out a guest from a reservation
 * 
 * @param currentUser The user performing the action
 * @param reservationId ID of the reservation
 * @return 1 if successful, 0 otherwise
 */
int checkOutReservation(User *currentUser, int reservationId);

/**
 * List all reservations in the system
 * 
 * @param currentUser The user performing the action
 * @param statusFilter Filter reservations by status (0 for all)
 */
void listReservations(User *currentUser, int statusFilter);

/**
 * Search for reservations based on criteria
 * 
 * @param currentUser The user performing the action
 */
void searchReservations(User *currentUser);

/**
 * Get a reservation by ID
 * 
 * @param reservationId ID of the reservation to get
 * @param reservation Pointer to Reservation structure to store the reservation info
 * @return 1 if found, 0 otherwise
 */
int getReservationById(int reservationId, Reservation *reservation);

/**
 * Check if a room is available for a given date range
 * 
 * @param roomId ID of the room to check
 * @param startDate Start date of the range (YYYY-MM-DD)
 * @param endDate End date of the range (YYYY-MM-DD)
 * @return 1 if available, 0 otherwise
 */
int checkRoomAvailability(int roomId, const char *startDate, const char *endDate);

/**
 * Check if a room has active reservations
 * 
 * @param roomId ID of the room to check
 * @return 1 if has active reservations, 0 otherwise
 */
int hasActiveReservations(int roomId);

/**
 * Check if a guest has active reservations
 * 
 * @param guestId ID of the guest to check
 * @return 1 if has active reservations, 0 otherwise
 */
int hasActiveReservationsByGuest(int guestId);

/**
 * Calculate the number of days between two dates
 * 
 * @param startDate Start date (YYYY-MM-DD)
 * @param endDate End date (YYYY-MM-DD)
 * @return Number of days
 */
int calculateDays(const char *startDate, const char *endDate);

/**
 * Calculate the total amount for a reservation
 * 
 * @param roomId ID of the room
 * @param startDate Check-in date (YYYY-MM-DD)
 * @param endDate Check-out date (YYYY-MM-DD)
 * @return Total amount
 */
double calculateReservationAmount(int roomId, const char *startDate, const char *endDate);

/**
 * Display the reservation management menu
 * 
 * @param currentUser The current logged in user
 */
void reservationManagementMenu(User *currentUser);

/**
 * Get the string representation of a reservation status
 * 
 * @param status The reservation status code
 * @return String representation of the status
 */
const char* getReservationStatusString(int status);

/**
 * Initialize reservation data if it doesn't exist
 * 
 * @return 1 if successful, 0 otherwise
 */
int initializeReservationData(void);

#endif /* RESERVATION_H */