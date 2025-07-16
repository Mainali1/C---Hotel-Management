/**
 * Hotel Management System
 * reservation.h - Reservation management header file
 * 
 * This file contains declarations for reservation management functionality
 * including reservation structure and function prototypes.
 */

 #ifndef RESERVATION_H
 #define RESERVATION_H
 
 #include "auth.h" 
 #include "room.h" 
 #include "guest.h"
 
 /* Reservation status definitions */
 typedef enum {
     RESERVATION_STATUS_DUMMY = 0, // To make filters start from 1
     RESERVATION_STATUS_CONFIRMED,
     RESERVATION_STATUS_CHECKED_IN,
     RESERVATION_STATUS_CHECKED_OUT,
     RESERVATION_STATUS_CANCELLED,
     RESERVATION_STATUS_NO_SHOW
 } ReservationStatus;
 
 /* Maximum lengths for strings */
 #define MAX_RESERVATION_NOTES_LEN 200
 
 /* File Path */
 #define RESERVATIONS_FILE "data/reservations.dat"
 
 /**
  * Reservation structure - Stores reservation information
  */
 typedef struct {
     int id;                                      /* Unique reservation ID */
     int guestId;                                  /* ID of the guest */
     int roomId;                                   /* ID of the room */
     char checkInDate[11];                         /* Check-in date (YYYY-MM-DD) */
     char checkOutDate[11];                        /* Check-out date (YYYY-MM-DD) */
     ReservationStatus status;                     /* Reservation status */
     int numGuests;                                /* Number of guests */
     double totalAmount;                           /* Total amount for the stay */
     double paidAmount;                            /* Amount already paid */
     char creationDate[20];                        /* Date and time of reservation creation */
     int createdBy;                                /* ID of user who created the reservation */
     char notes[MAX_RESERVATION_NOTES_LEN];        /* Additional notes */
     int isActive;                                 /* Whether the reservation is active in the system */
 } Reservation;
 
 /* Function prototypes */
 
 int initializeReservationData(void);
 int addReservation(User *currentUser);
 int modifyReservation(User *currentUser, int reservationId);
 int cancelReservation(User *currentUser, int reservationId);
 int checkInReservation(User *currentUser, int reservationId);
 int checkOutReservation(User *currentUser, int reservationId);
 void listReservations(User *currentUser, ReservationStatus statusFilter);
 void searchReservations(User *currentUser);
 int getReservationById(int reservationId, Reservation *reservation);
 int checkRoomAvailability(int roomId, const char *startDate, const char *endDate);
 int hasActiveReservations(int roomId);
 int hasActiveReservationsByGuest(int guestId);
 double calculateReservationAmount(int roomId, const char *startDate, const char *endDate);
 void reservationManagementMenu(User *currentUser);
 const char* getReservationStatusString(ReservationStatus status);
 
 #endif /* RESERVATION_H */