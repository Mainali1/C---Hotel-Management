#ifndef GUEST_H
#define GUEST_H

#include "auth.h" /* For User type */

/* VIP status definitions */
typedef enum {
    VIP_STATUS_REGULAR,
    VIP_STATUS_BRONZE,
    VIP_STATUS_SILVER,
    VIP_STATUS_GOLD,
    VIP_STATUS_PLATINUM
} VipStatus;

/* Maximum lengths for strings */
#define MAX_GUEST_NAME_LEN 50
#define MAX_ADDRESS_LEN 100
#define MAX_PHONE_LEN 20
#define MAX_EMAIL_LEN 50
#define MAX_ID_NUMBER_LEN 20
#define MAX_ID_TYPE_LEN 20
#define MAX_DATE_LEN 11
#define MAX_NOTES_LEN 200

/* File paths */
#define GUESTS_FILE "data/guests.dat"

/* Guest structure */
typedef struct {
    int id;
    char name[MAX_GUEST_NAME_LEN];
    char address[MAX_ADDRESS_LEN];
    char phone[MAX_PHONE_LEN];
    char email[MAX_EMAIL_LEN];
    char idNumber[MAX_ID_NUMBER_LEN];
    char idType[MAX_ID_TYPE_LEN];
    char registrationDate[MAX_DATE_LEN];
    int totalStays;
    double totalSpent;
    char notes[MAX_NOTES_LEN];
    int isActive;
    VipStatus vipStatus;
} Guest;

/* Function prototypes */
const char* getVipStatusString(VipStatus status);
int initializeGuestData(void);
int addGuest(User *currentUser);
int modifyGuest(User *currentUser, int guestId);
int deleteGuest(User *currentUser, int guestId);
int getGuestById(int guestId, Guest *guest);
void listGuests(User *currentUser);
void searchGuests(User *currentUser);
void updateGuestStats(int guestId, int stays, double spent);

#endif /* GUEST_H */