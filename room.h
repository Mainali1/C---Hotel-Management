/**
 * Hotel Management System
 * room.h - Room management header file
 * 
 * This file contains declarations for room management functionality
 * including room structure and function prototypes.
 */

#ifndef ROOM_H
#define ROOM_H

#include "auth.h" /* For User type */

/* Room status definitions */
typedef enum {
    ROOM_STATUS_AVAILABLE,
    ROOM_STATUS_OCCUPIED,
    ROOM_STATUS_MAINTENANCE,
    ROOM_STATUS_RESERVED,
    ROOM_STATUS_CLEANING
} RoomStatus;

/* Room type definitions */
typedef enum {
    ROOM_TYPE_STANDARD,
    ROOM_TYPE_DELUXE,
    ROOM_TYPE_SUITE,
    ROOM_TYPE_EXECUTIVE,
    ROOM_TYPE_PRESIDENTIAL
} RoomType;

/* Maximum lengths for strings */
#define MAX_ROOM_DESC_LEN 100
#define MAX_ROOM_FEATURES_LEN 200

/* File paths */
#define ROOMS_FILE "data/rooms.dat"

/* Room structure */
typedef struct {
    int id;                                     /* Room number */
    RoomType type;                              /* Room type */
    RoomStatus status;                          /* Current status */
    double rate;                                /* Rate per night */
    char description[MAX_ROOM_DESC_LEN];        /* Room description */
    char features[MAX_ROOM_FEATURES_LEN];       /* Room features */
    int capacity;                               /* Maximum occupancy */
    int floor;                                  /* Floor number */
    int isActive;                               /* Whether room is active in system */
} Room;

/* Function prototypes */
const char* getRoomStatusString(RoomStatus status);
const char* getRoomTypeString(RoomType type);
int initializeRoomData(void);
int addRoom(User *currentUser);
int modifyRoom(User *currentUser, int roomId);
int deleteRoom(User *currentUser, int roomId);
int changeRoomStatus(User *currentUser, int roomId, int newStatus);
int getRoomById(int roomId, Room *room);
int isRoomAvailable(int roomId, const char *startDate, const char *endDate);
void listRooms(User *currentUser, int statusFilter);
void searchRooms(User *currentUser);

#endif /* ROOM_H */