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
  
  #define TEMP_FILE "data/temp_reservations.dat"
  
  /* Get the string representation of a reservation status */
  const char* getReservationStatusString(ReservationStatus status) {
      switch (status) {
          case RESERVATION_STATUS_CONFIRMED: return "Confirmed";
          case RESERVATION_STATUS_CHECKED_IN: return "Checked In";
          case RESERVATION_STATUS_CHECKED_OUT: return "Checked Out";
          case RESERVATION_STATUS_CANCELLED: return "Cancelled";
          case RESERVATION_STATUS_NO_SHOW: return "No Show";
          default: return "Unknown";
      }
  }
  
  /* Initialize reservation data file */
  int initializeReservationData(void) {
      FILE *fp;
      
      fp = fopen(RESERVATIONS_FILE, "rb");
      if (fp != NULL) {
          fseek(fp, 0, SEEK_END);
          if (ftell(fp) > 0) {
              fclose(fp);
              return 1; 
          }
          fclose(fp);
      }
      
      fp = fopen(RESERVATIONS_FILE, "wb");
      if (!fp) {
          printf("\nError: Could not create reservations file.\n");
          return 0;
      }
      fclose(fp);
      return 1;
  }
  
  /* Calculate the total amount for a reservation */
  double calculateReservationAmount(int roomId, const char *startDate, const char *endDate) {
      Room room;
      int days;
      
      if (!getRoomById(roomId, &room)) {
          return 0.0;
      }
      
      days = calculateDateDifference(startDate, endDate);
      if (days <= 0) days = 1;
      
      return room.rate * days;
  }
  
  /* Check if a room is available for a given date range */
  int checkRoomAvailability(int roomId, const char *startDate, const char *endDate) {
      FILE *fp;
      Reservation tempReservation;
      Room room;
  
      if (!getRoomById(roomId, &room)) {
          return 0; 
      }
      
      if (room.status == ROOM_STATUS_MAINTENANCE) {
          return 0;
      }
      
      fp = fopen(RESERVATIONS_FILE, "rb");
      if (fp == NULL) {
          return 1; 
      }
      
      while (fread(&tempReservation, sizeof(Reservation), 1, fp) == 1) {
          if (tempReservation.roomId == roomId && 
              tempReservation.isActive && 
              (tempReservation.status == RESERVATION_STATUS_CONFIRMED || 
               tempReservation.status == RESERVATION_STATUS_CHECKED_IN)) {
              
              if (strcmp(startDate, tempReservation.checkOutDate) < 0 && 
                  strcmp(endDate, tempReservation.checkInDate) > 0) {
                  fclose(fp);
                  return 0; 
              }
          }
      }
      
      fclose(fp);
      return 1;
  }
  
  /* Check if a room has any active/future reservations */
  int hasActiveReservations(int roomId) {
      FILE *fp;
      Reservation tempReservation;
      
      fp = fopen(RESERVATIONS_FILE, "rb");
      if (fp == NULL) return 0;
      
      while (fread(&tempReservation, sizeof(Reservation), 1, fp) == 1) {
          if (tempReservation.roomId == roomId && 
              tempReservation.isActive && 
              (tempReservation.status == RESERVATION_STATUS_CONFIRMED || 
               tempReservation.status == RESERVATION_STATUS_CHECKED_IN)) {
              fclose(fp);
              return 1;
          }
      }
      
      fclose(fp);
      return 0;
  }
  
  /* Check if a guest has any active/future reservations */
  int hasActiveReservationsByGuest(int guestId) {
      FILE *fp;
      Reservation tempReservation;
      
      fp = fopen(RESERVATIONS_FILE, "rb");
      if (fp == NULL) return 0;
      
      while (fread(&tempReservation, sizeof(Reservation), 1, fp) == 1) {
          if (tempReservation.guestId == guestId && 
              tempReservation.isActive && 
              (tempReservation.status == RESERVATION_STATUS_CONFIRMED || 
               tempReservation.status == RESERVATION_STATUS_CHECKED_IN)) {
              fclose(fp);
              return 1;
          }
      }
      
      fclose(fp);
      return 0;
  }
  
  /* Add a new reservation */
  int addReservation(User *currentUser) {
      FILE *fp;
      Reservation newRes;
      Guest guest;
      Room room;
      int nextId = 1;
      int guestId, roomId;
      char startDate[11], endDate[11];
      
      fp = fopen(RESERVATIONS_FILE, "rb");
      if (fp != NULL) {
          while (fread(&newRes, sizeof(Reservation), 1, fp) == 1) {
              if (newRes.id >= nextId) nextId = newRes.id + 1;
          }
          fclose(fp);
      }
      
      clearScreen();
      printf("===== ADD NEW RESERVATION =====\n");
      
      listGuests(currentUser);
      guestId = getIntInput("\nEnter Guest ID (or 0 to add a new guest): ", 0, 9999);
      if (guestId == 0) {
          guestId = addGuest();
          if (guestId == 0) {
              printf("\nFailed to add guest. Reservation cancelled.\n");
              return 0;
          }
      } else if (!getGuestById(guestId, &guest)) {
          printf("\nGuest with ID %d not found. Reservation cancelled.\n", guestId);
          return 0;
      }
      
      listRooms(currentUser, ROOM_STATUS_AVAILABLE);
      roomId = getIntInput("\nEnter Room Number for reservation: ", 1, 9999);
      if (!getRoomById(roomId, &room) || room.status != ROOM_STATUS_AVAILABLE) {
          printf("\nRoom %d is not available or does not exist. Reservation cancelled.\n", roomId);
          return 0;
      }
      
      do {
          printf("\nEnter Check-in Date (YYYY-MM-DD): ");
          getStringInput(NULL, startDate, 11);
          printf("Enter Check-out Date (YYYY-MM-DD): ");
          getStringInput(NULL, endDate, 11);
          if (!validateDate(startDate) || !validateDate(endDate) || strcmp(startDate, endDate) >= 0) {
              printf("\nInvalid dates. Check-out must be after check-in.\n");
          } else if (!checkRoomAvailability(roomId, startDate, endDate)) {
               printf("\nRoom %d is not available for the selected dates.\n", roomId);
          } else {
              break; // Dates are valid and room is available
          }
      } while (1);
      
      newRes.numGuests = getIntInput("Number of Guests: ", 1, room.capacity);
      newRes.totalAmount = calculateReservationAmount(roomId, startDate, endDate);
      printf("\nTotal Amount for stay: $%.2f\n", newRes.totalAmount);
      newRes.paidAmount = getDoubleInput("Enter amount paid now: $", 0.0, newRes.totalAmount);
      
      printf("Notes (optional): ");
      getStringInput(NULL, newRes.notes, MAX_RESERVATION_NOTES_LEN);
  
      newRes.id = nextId;
      newRes.guestId = guestId;
      newRes.roomId = roomId;
      strcpy(newRes.checkInDate, startDate);
      strcpy(newRes.checkOutDate, endDate);
      newRes.status = RESERVATION_STATUS_CONFIRMED;
      getCurrentDateTime(newRes.creationDate);
      newRes.createdBy = currentUser->id;
      newRes.isActive = 1;
      
      fp = fopen(RESERVATIONS_FILE, "ab");
      if (!fp) {
          printf("\nError: Could not save reservation.\n");
          return 0;
      }
      fwrite(&newRes, sizeof(Reservation), 1, fp);
      fclose(fp);
      
      // Room status is updated by check-in/out, not just reservation
      // changeRoomStatus(roomId, ROOM_STATUS_RESERVED);
      
      printf("\nReservation created successfully with ID %d.\n", newRes.id);
      return newRes.id;
  }
  
  
  /* Cancel a reservation */
  int cancelReservation(User *currentUser, int reservationId) {
      FILE *fp, *tempFp;
      Reservation res;
      int found = 0;
      
      fp = fopen(RESERVATIONS_FILE, "rb");
      if (!fp) return 0;
      
      tempFp = fopen(TEMP_FILE, "wb");
      if (!tempFp) { fclose(fp); return 0; }
      
      while (fread(&res, sizeof(Reservation), 1, fp) == 1) {
          if (res.id == reservationId && res.isActive) {
              found = 1;
              if (res.status == RESERVATION_STATUS_CONFIRMED) {
                  printf("\nCancelling reservation %d...", reservationId);
                  res.status = RESERVATION_STATUS_CANCELLED;
                  // Note: a real system might have cancellation fees logic here
                  if(res.paidAmount > 0) {
                      printf("\nNote: $%.2f was paid. Please process refund manually.", res.paidAmount);
                  }
              } else {
                  printf("\nOnly 'Confirmed' reservations can be cancelled. Status is '%s'.", getReservationStatusString(res.status));
              }
          }
          fwrite(&res, sizeof(Reservation), 1, tempFp);
      }
      
      fclose(fp);
      fclose(tempFp);
      
      if (found) {
          remove(RESERVATIONS_FILE);
          rename(TEMP_FILE, RESERVATIONS_FILE);
          printf("\nOperation completed.\n");
      } else {
          remove(TEMP_FILE);
          printf("\nReservation with ID %d not found.\n", reservationId);
      }
      return found;
  }
  
  /* Check in a guest for a reservation */
  int checkInReservation(User *currentUser, int reservationId) {
      FILE *fp, *tempFp;
      Reservation res;
      int found = 0;
      
      fp = fopen(RESERVATIONS_FILE, "rb");
      if (!fp) return 0;
      
      tempFp = fopen(TEMP_FILE, "wb");
      if (!tempFp) { fclose(fp); return 0; }
      
      while (fread(&res, sizeof(Reservation), 1, fp) == 1) {
          if (res.id == reservationId && res.isActive) {
              found = 1;
              if (res.status == RESERVATION_STATUS_CONFIRMED) {
                  res.status = RESERVATION_STATUS_CHECKED_IN;
                  changeRoomStatus(res.roomId, ROOM_STATUS_OCCUPIED);
                  printf("\nGuest checked in successfully for reservation %d.\n", reservationId);
              } else {
                  printf("\nCannot check-in. Reservation status is '%s'.\n", getReservationStatusString(res.status));
              }
          }
          fwrite(&res, sizeof(Reservation), 1, tempFp);
      }
      
      fclose(fp);
      fclose(tempFp);
      
      if (found) {
          remove(RESERVATIONS_FILE);
          rename(TEMP_FILE, RESERVATIONS_FILE);
      } else {
          remove(TEMP_FILE);
          printf("\nReservation with ID %d not found.\n", reservationId);
      }
      return found;
  }
  
  /* Check out a guest from a reservation */
  int checkOutReservation(User *currentUser, int reservationId) {
      FILE *fp, *tempFp;
      Reservation res;
      int found = 0;
      
      if (!getReservationById(reservationId, &res)) {
          printf("\nReservation with ID %d not found.\n", reservationId);
          return 0;
      }
  
      if (res.status != RESERVATION_STATUS_CHECKED_IN) {
          printf("\nOnly checked-in guests can be checked out. Status: %s\n", getReservationStatusString(res.status));
          return 0;
      }
  
      // A real system would link to billing here to check for outstanding balance.
      // We simulate this by checking the paidAmount.
      if (res.paidAmount < res.totalAmount) {
          printf("\nWarning: Outstanding balance of $%.2f.", res.totalAmount - res.paidAmount);
          printf("\nPlease resolve payment in the Billing module before checkout.\n");
          return 0;
      }
      
      fp = fopen(RESERVATIONS_FILE, "rb");
      if (!fp) return 0;
      
      tempFp = fopen(TEMP_FILE, "wb");
      if (!tempFp) { fclose(fp); return 0; }
  
      while (fread(&res, sizeof(Reservation), 1, fp) == 1) {
          if (res.id == reservationId && res.isActive) {
              found = 1;
              res.status = RESERVATION_STATUS_CHECKED_OUT;
              changeRoomStatus(res.roomId, ROOM_STATUS_CLEANING);
              updateGuestStayInfo(res.guestId, res.totalAmount);
              printf("\nGuest checked out successfully. Room %d status set to Cleaning.\n", res.roomId);
          }
          fwrite(&res, sizeof(Reservation), 1, tempFp);
      }
  
      fclose(fp);
      fclose(tempFp);
  
      if (found) {
          remove(RESERVATIONS_FILE);
          rename(TEMP_FILE, RESERVATIONS_FILE);
      } else {
          remove(TEMP_FILE);
      }
      return found;
  }
  
  /* List all reservations */
  void listReservations(User *currentUser, ReservationStatus statusFilter) {
      FILE *fp;
      Reservation res;
      Guest guest;
      int count = 0;
      
      fp = fopen(RESERVATIONS_FILE, "rb");
      if (fp == NULL) {
          printf("\nCould not open reservations file or no reservations exist.\n");
          return;
      }
      
      clearScreen();
      printf("===== RESERVATION LIST =====\n");
      if (statusFilter != 0) {
          printf("Filtered by status: %s\n", getReservationStatusString(statusFilter));
      }
      printf("\n");
      printf("%-5s %-25s %-6s %-12s %-12s %-12s\n", 
             "ID", "Guest Name", "Room", "Check-in", "Check-out", "Status");
      printf("--------------------------------------------------------------------------------\n");
      
      while (fread(&res, sizeof(Reservation), 1, fp) == 1) {
          if (res.isActive && (statusFilter == 0 || res.status == statusFilter)) {
              if (getGuestById(res.guestId, &guest)) {
                  printf("%-5d %-25s %-6d %-12s %-12s %-12s\n", 
                         res.id, 
                         guest.name, 
                         res.roomId, 
                         res.checkInDate,
                         res.checkOutDate,
                         getReservationStatusString(res.status));
                  count++;
              }
          }
      }
      
      fclose(fp);
      
      printf("--------------------------------------------------------------------------------\n");
      printf("Total reservations found: %d\n", count);
  }
  
  /* Get a reservation by ID */
  int getReservationById(int reservationId, Reservation *reservation) {
      FILE *fp;
      Reservation temp;
      int found = 0;
      
      fp = fopen(RESERVATIONS_FILE, "rb");
      if (fp == NULL) return 0;
      
      while (fread(&temp, sizeof(Reservation), 1, fp) == 1) {
          if (temp.id == reservationId && temp.isActive) {
              *reservation = temp;
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
      
      do {
          clearScreen();
          printf("===== RESERVATION MANAGEMENT =====\n");
          printf("1. List All Reservations\n");
          printf("2. Add Reservation\n");
          printf("3. Check-in Guest\n");
          printf("4. Check-out Guest\n");
          printf("5. Cancel Reservation\n");
          printf("0. Back to Main Menu\n");
          printf("==================================\n");
          
          choice = getIntInput("Enter your choice: ", 0, 5);
          
          switch (choice) {
              case 1:
                  listReservations(currentUser, 0);
                  break;
              case 2:
                  addReservation(currentUser);
                  break;
              case 3:
                  listReservations(currentUser, RESERVATION_STATUS_CONFIRMED);
                  reservationId = getIntInput("\nEnter reservation ID to check in (0 to cancel): ", 0, 9999);
                  if (reservationId != 0) checkInReservation(currentUser, reservationId);
                  break;
              case 4:
                  listReservations(currentUser, RESERVATION_STATUS_CHECKED_IN);
                  reservationId = getIntInput("\nEnter reservation ID to check out (0 to cancel): ", 0, 9999);
                  if (reservationId != 0) checkOutReservation(currentUser, reservationId);
                  break;
              case 5:
                  listReservations(currentUser, RESERVATION_STATUS_CONFIRMED);
                  reservationId = getIntInput("\nEnter reservation ID to cancel (0 to cancel): ", 0, 9999);
                  if (reservationId != 0) cancelReservation(currentUser, reservationId);
                  break;
              case 0:
                  return;
              default:
                  printf("\nInvalid choice.\n");
          }
          
          if (choice != 0) {
              pauseExecution();
          }
          
      } while (choice != 0);
  }