/**
 * Hotel Management System
 * billing.c - Billing management implementation
 * 
 * This file implements the billing management functionality
 * including generating invoices, recording payments, and managing billing records.
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <time.h>
 #include "billing.h"
 #include "fileio.h"
 #include "utils.h"
 #include "ui.h"
 #include "guest.h"
 
 #define INVOICES_FILE "data/invoices.dat"
 #define BILLING_ITEMS_FILE "data/billing_items.dat"
 #define PAYMENTS_FILE "data/payments.dat"
 #define TEMP_FILE "data/temp.dat"
 #define MAX_INVOICES 10000
 #define MAX_ITEMS 10000
 #define MAX_PAYMENTS 10000
 
 /* Get the string representation of an invoice status */
 const char* getInvoiceStatusString(InvoiceStatus status) {
    switch (status) {
        case INVOICE_STATUS_PENDING:
            return "Pending";
        case INVOICE_STATUS_PAID:
            return "Paid";
        case INVOICE_STATUS_OVERDUE:
            return "Overdue";
        case INVOICE_STATUS_CANCELLED:
            return "Cancelled";
        case INVOICE_STATUS_REFUNDED:
            return "Refunded";
        default:
            return "Unknown";
    }
     }
 }
 
/* Initialize billing data if it doesn't exist */
int initializeBillingData(void) {
    FILE *fp;
    int count = 0;
    
    /* Create data directory if it doesn't exist */
    createDirectoryIfNotExists("data");
    
    /* Check if invoices file already exists */
    fp = fopen(INVOICES_FILE, "rb");
    if (fp == NULL) {
        /* Create empty file */
        fp = fopen(INVOICES_FILE, "wb");
        if (fp == NULL) {
            printf("\nError: Could not create invoices file.\n");
            return 0;
        }
        fclose(fp);
    } else {
        fclose(fp);
    }
    
    /* Check if billing items file already exists */
    fp = fopen(BILLING_ITEMS_FILE, "rb");
    if (fp == NULL) {
        /* Create empty file */
        fp = fopen(BILLING_ITEMS_FILE, "wb");
        if (fp == NULL) {
            printf("\nError: Could not create billing items file.\n");
            return 0;
        }
        fclose(fp);
    } else {
        fclose(fp);
    }
    
    /* Check if payments file already exists */
    fp = fopen(PAYMENTS_FILE, "rb");
    if (fp == NULL) {
        /* Create empty file */
        fp = fopen(PAYMENTS_FILE, "wb");
        if (fp == NULL) {
            printf("\nError: Could not create payments file.\n");
            return 0;
        }
        fclose(fp);
    } else {
        fclose(fp);
    }
    
    /* Check for overdue invoices */
    count = checkOverdueInvoices();
    
    return 1;
}

/* Get the string representation of a payment method */
const char* getPaymentMethodString(PaymentMethod method) {
     switch (method) {
         case PAYMENT_METHOD_CASH:
             return "Cash";
         case PAYMENT_METHOD_CREDIT_CARD:
             return "Credit Card";
         case PAYMENT_METHOD_DEBIT_CARD:
             return "Debit Card";
         case PAYMENT_METHOD_BANK_TRANSFER:
             return "Bank Transfer";
         case PAYMENT_METHOD_ONLINE:
             return "Online Payment";
         default:
             return "Unknown";
     }
 }
 
 /* Get the string representation of a payment status */
 const char* getPaymentStatusString(PaymentStatus status) {
     switch (status) {
         case PAYMENT_STATUS_PENDING:
             return "Pending";
         case PAYMENT_STATUS_COMPLETED:
             return "Completed";
         case PAYMENT_STATUS_FAILED:
             return "Failed";
         case PAYMENT_STATUS_REFUNDED:
             return "Refunded";
         default:
             return "Unknown";
     }
 }
 
 /* Get the string representation of a billing item type */
 const char* getBillingItemTypeString(BillingItemType type) {
     switch (type) {
         case BILLING_ITEM_ROOM_CHARGE:
             return "Room Charge";
         case BILLING_ITEM_FOOD_SERVICE:
             return "Food Service";
         case BILLING_ITEM_LAUNDRY:
             return "Laundry";
         case BILLING_ITEM_MINIBAR:
             return "Minibar";
         case BILLING_ITEM_SPA:
             return "Spa Service";
         case BILLING_ITEM_OTHER:
             return "Other";
         default:
             return "Unknown";
     }
 }
 
 /* Initialize billing data if it doesn't exist */
 int initializeBillingData(void) {
     FILE *fp;
     int count = 0;
     
     /* Create data directory if it doesn't exist */
     createDirectoryIfNotExists("data");
     
     /* Check if invoices file already exists */
     fp = fopen(INVOICES_FILE, "rb");
     if (fp == NULL) {
         /* Create empty file */
         fp = fopen(INVOICES_FILE, "wb");
         if (fp == NULL) {
             printf("\nError: Could not create invoices file.\n");
             return 0;
         }
         fclose(fp);
     } else {
         fclose(fp);
     }
     
     /* Check if billing items file already exists */
     fp = fopen(BILLING_ITEMS_FILE, "rb");
     if (fp == NULL) {
         /* Create empty file */
         fp = fopen(BILLING_ITEMS_FILE, "wb");
         if (fp == NULL) {
             printf("\nError: Could not create billing items file.\n");
             return 0;
         }
         fclose(fp);
     } else {
         fclose(fp);
     }
     
     /* Check if payments file already exists */
     fp = fopen(PAYMENTS_FILE, "rb");
     if (fp == NULL) {
         /* Create empty file */
         fp = fopen(PAYMENTS_FILE, "wb");
         if (fp == NULL) {
             printf("\nError: Could not create payments file.\n");
             return 0;
         }
         fclose(fp);
     } else {
         fclose(fp);
     }
     
     /* Check for overdue invoices */
     count = checkOverdueInvoices();
     if (count > 0) {
         printf("\n%d invoice(s) marked as overdue.\n", count);
     }
     
     return 1;
 }
 
 /* Billing Management Menu */
 void billingManagementMenu(User *currentUser) {
     int choice;
     int invoiceId;
     int reservationId;
     int itemId;
     int paymentId;
     int statusFilter;
     int method;
     double amount;
     char transactionId[50];
     char notes[MAX_BILLING_NOTES_LEN];
     char description[MAX_BILLING_DESCRIPTION_LEN];
     double unitPrice;
     int quantity;
     
     do {
         clearScreen();
         printf("===== BILLING MANAGEMENT =====\n");
         printf("1. List All Invoices\n");
         printf("2. List Invoices by Status\n");
         printf("3. Search Invoices\n");
         printf("4. Create Invoice\n");
         printf("5. Modify Invoice\n");
         printf("6. Cancel Invoice\n");
         printf("7. Issue Invoice\n");
         printf("8. Mark Invoice as Paid\n");
         printf("9. Manage Billing Items\n");
         printf("10. Manage Payments\n");
         printf("11. Check Overdue Invoices\n");
         printf("0. Back to Main Menu\n");
         printf("================================\n");
         
         choice = getIntInput("Enter your choice: ", 0, 11);
         
         switch (choice) {
             case 1: /* List All Invoices */
                 listInvoices(currentUser, 0);
                 break;
                 
             case 2: /* List Invoices by Status */
                 clearScreen();
                 printf("===== INVOICE STATUS FILTER =====\n");
                 printf("1. Draft\n");
                 printf("2. Issued\n");
                 printf("3. Paid\n");
                 printf("4. Cancelled\n");
                 printf("5. Overdue\n");
                 printf("0. Cancel\n");
                 
                 statusFilter = getIntInput("Enter status filter: ", 0, 5);
                 
                 if (statusFilter > 0) {
                     listInvoices(currentUser, statusFilter);
                 }
                 break;
                 
             case 3: /* Search Invoices */
                 searchInvoices(currentUser);
                 break;
                 
             case 4: /* Create Invoice */
                 printf("\nEnter Reservation ID: ");
                 scanf("%d", &reservationId);
                 createInvoice(currentUser, reservationId);
                 break;
                 
             case 5: /* Modify Invoice */
                 printf("\nEnter Invoice ID: ");
                 scanf("%d", &invoiceId);
                 modifyInvoice(currentUser, invoiceId);
                 break;
                 
             case 6: /* Cancel Invoice */
                 printf("\nEnter Invoice ID: ");
                 scanf("%d", &invoiceId);
                 cancelInvoice(currentUser, invoiceId);
                 break;
                 
             case 7: /* Issue Invoice */
                 printf("\nEnter Invoice ID: ");
                 scanf("%d", &invoiceId);
                 issueInvoice(currentUser, invoiceId);
                 break;
                 
             case 8: /* Mark Invoice as Paid */
                 printf("\nEnter Invoice ID: ");
                 scanf("%d", &invoiceId);
                 markInvoiceAsPaid(currentUser, invoiceId);
                 break;
                 
             case 9: /* Manage Billing Items */
                 clearScreen();
                 printf("===== BILLING ITEMS MANAGEMENT =====\n");
                 printf("1. List Billing Items for an Invoice\n");
                 printf("2. Add Billing Item\n");
                 printf("3. Modify Billing Item\n");
                 printf("4. Delete Billing Item\n");
                 printf("0. Back\n");
                 
                 int itemChoice = getIntInput("Enter your choice: ", 0, 4);
                 
                 switch (itemChoice) {
                     case 1: /* List Billing Items */
                         printf("\nEnter Invoice ID: ");
                         scanf("%d", &invoiceId);
                         listBillingItems(currentUser, invoiceId);
                         break;
                         
                     case 2: /* Add Billing Item */
                         printf("\nEnter Invoice ID: ");
                         scanf("%d", &invoiceId);
                         
                         /* Get item type */
                         clearScreen();
                         printf("===== BILLING ITEM TYPE =====\n");
                         printf("1. Room Charge\n");
                         printf("2. Food\n");
                         printf("3. Beverage\n");
                         printf("4. Service\n");
                         printf("5. Tax\n");
                         printf("6. Discount\n");
                         printf("7. Other\n");
                         
                         int itemType = getIntInput("Enter item type: ", 1, 7);
                         
                         /* Get item details */
                         printf("\nEnter description: ");
                         getchar(); /* Consume newline */
                         fgets(description, MAX_BILLING_DESCRIPTION_LEN, stdin);
                         description[strcspn(description, "\n")] = '\0'; /* Remove newline */
                         
                         printf("Enter unit price: $");
                         scanf("%lf", &unitPrice);
                         
                         printf("Enter quantity: ");
                         scanf("%d", &quantity);
                         
                         /* Add item */
                         addBillingItem(currentUser, invoiceId, itemType, description, unitPrice, quantity);
                         break;
                         
                     case 3: /* Modify Billing Item */
                         printf("\nEnter Billing Item ID: ");
                         scanf("%d", &itemId);
                         
                         /* Get new details */
                         printf("\nEnter new description (leave empty to keep current): ");
                         getchar(); /* Consume newline */
                         fgets(description, MAX_BILLING_DESCRIPTION_LEN, stdin);
                         description[strcspn(description, "\n")] = '\0'; /* Remove newline */
                         
                         printf("Enter new unit price (0 to keep current): $");
                         scanf("%lf", &unitPrice);
                         
                         printf("Enter new quantity (0 to keep current): ");
                         scanf("%d", &quantity);
                         
                         /* Modify item */
                         modifyBillingItem(currentUser, itemId, description, unitPrice, quantity);
                         break;
                         
                     case 4: /* Delete Billing Item */
                         printf("\nEnter Billing Item ID: ");
                         scanf("%d", &itemId);
                         deleteBillingItem(currentUser, itemId);
                         break;
                 }
                 break;
                 
             case 10: /* Manage Payments */
                 clearScreen();
                 printf("===== PAYMENTS MANAGEMENT =====\n");
                 printf("1. List Payments for an Invoice\n");
                 printf("2. Record Payment\n");
                 printf("3. Modify Payment\n");
                 printf("4. Cancel Payment\n");
                 printf("0. Back\n");
                 
                 int paymentChoice = getIntInput("Enter your choice: ", 0, 4);
                 
                 switch (paymentChoice) {
                     case 1: /* List Payments */
                         printf("\nEnter Invoice ID: ");
                         scanf("%d", &invoiceId);
                         listPayments(currentUser, invoiceId);
                         break;
                         
                     case 2: /* Record Payment */
                         printf("\nEnter Invoice ID: ");
                         scanf("%d", &invoiceId);
                         
                         /* Get payment method */
                         clearScreen();
                         printf("===== PAYMENT METHOD =====\n");
                         printf("1. Cash\n");
                         printf("2. Credit Card\n");
                         printf("3. Debit Card\n");
                         printf("4. Bank Transfer\n");
                         printf("5. Check\n");
                         printf("6. Online Payment\n");
                         
                         method = getIntInput("Enter payment method: ", 1, 6);
                         
                         /* Get payment details */
                         printf("\nEnter amount: $");
                         scanf("%lf", &amount);
                         
                         printf("Enter transaction ID: ");
                         getchar(); /* Consume newline */
                         fgets(transactionId, 50, stdin);
                         transactionId[strcspn(transactionId, "\n")] = '\0'; /* Remove newline */
                         
                         printf("Enter notes: ");
                         fgets(notes, MAX_BILLING_NOTES_LEN, stdin);
                         notes[strcspn(notes, "\n")] = '\0'; /* Remove newline */
                         
                         /* Record payment */
                         recordPayment(currentUser, invoiceId, method, amount, transactionId, notes);
                         break;
                         
                     case 3: /* Modify Payment */
                         printf("\nEnter Payment ID: ");
                         scanf("%d", &paymentId);
                         
                         /* Get new payment method */
                         clearScreen();
                         printf("===== PAYMENT METHOD =====\n");
                         printf("1. Cash\n");
                         printf("2. Credit Card\n");
                         printf("3. Debit Card\n");
                         printf("4. Bank Transfer\n");
                         printf("5. Check\n");
                         printf("6. Online Payment\n");
                         printf("0. Keep Current\n");
                         
                         method = getIntInput("Enter new payment method: ", 0, 6);
                         
                         /* Get new payment details */
                         printf("\nEnter new amount (0 to keep current): $");
                         scanf("%lf", &amount);
                         
                         printf("Enter new transaction ID (leave empty to keep current): ");
                         getchar(); /* Consume newline */
                         fgets(transactionId, 50, stdin);
                         transactionId[strcspn(transactionId, "\n")] = '\0'; /* Remove newline */
                         
                         printf("Enter new notes (leave empty to keep current): ");
                         fgets(notes, MAX_BILLING_NOTES_LEN, stdin);
                         notes[strcspn(notes, "\n")] = '\0'; /* Remove newline */
                         
                         /* Modify payment */
                         modifyPayment(currentUser, paymentId, method, amount, transactionId, notes);
                         break;
                         
                     case 4: /* Cancel Payment */
                         printf("\nEnter Payment ID: ");
                         scanf("%d", &paymentId);
                         cancelPayment(currentUser, paymentId);
                         break;
                 }
                 break;
                 
             case 11: /* Check Overdue Invoices */
                 checkOverdueInvoices();
                 break;
                 
             case 0: /* Back to Main Menu */
                 printf("\nReturning to Main Menu...\n");
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

/* Invoice management functions */

/* Create a new invoice for a reservation */
int createInvoice(User *currentUser, int reservationId) {
    FILE *fp;
    Invoice newInvoice;
    Reservation reservation;
    int nextId = 1;
    time_t now;
    struct tm *t;
    
    /* Check if reservation exists */
    if (!getReservationById(reservationId, &reservation)) {
        printf("\nError: Reservation with ID %d not found.\n", reservationId);
        return 0;
    }
    
    /* Check if invoice already exists for this reservation */
    if (getInvoiceByReservationId(reservationId, &newInvoice)) {
        printf("\nError: Invoice already exists for reservation ID %d.\n", reservationId);
        return 0;
    }
    
    /* Get current date for issue date */
    now = time(NULL);
    t = localtime(&now);
    sprintf(newInvoice.issueDate, "%04d-%02d-%02d", 
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
    
    /* Set due date to 7 days from now */
    now += 7 * 24 * 60 * 60; /* Add 7 days in seconds */
    t = localtime(&now);
    sprintf(newInvoice.dueDate, "%04d-%02d-%02d", 
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
    
    /* Find next available ID */
    fp = fopen(INVOICES_FILE, "rb");
    if (fp != NULL) {
        Invoice tempInvoice;
        while (fread(&tempInvoice, sizeof(Invoice), 1, fp) == 1) {
            if (tempInvoice.id >= nextId) {
                nextId = tempInvoice.id + 1;
            }
        }
        fclose(fp);
    }
    
    /* Initialize invoice */
    newInvoice.id = nextId;
    newInvoice.reservationId = reservationId;
    newInvoice.guestId = reservation.guestId;
    newInvoice.subtotal = 0.0;
    newInvoice.taxAmount = 0.0;
    newInvoice.discountAmount = 0.0;
    newInvoice.totalAmount = 0.0;
    newInvoice.paidAmount = 0.0;
    newInvoice.status = INVOICE_STATUS_DRAFT;
    strcpy(newInvoice.notes, "Auto-generated invoice");
    newInvoice.createdBy = currentUser->id;
    newInvoice.isActive = 1;
    
    /* Add room charge as billing item */
    Room room;
    if (getRoomById(reservation.roomId, &room)) {
        /* Calculate number of days */
        int days = calculateDateDifference(reservation.checkInDate, reservation.checkOutDate);
        if (days <= 0) days = 1; /* Minimum 1 day */
        
        /* Add room charge */
        char description[MAX_BILLING_DESCRIPTION_LEN];
        sprintf(description, "Room %d (%s) for %d night(s)", 
                room.id, getRoomTypeString(room.type), days);
        
        /* Save invoice first */
        fp = fopen(INVOICES_FILE, "ab");
        if (fp == NULL) {
            printf("\nError: Could not open invoices file.\n");
            return 0;
        }
        fwrite(&newInvoice, sizeof(Invoice), 1, fp);
        fclose(fp);
        
        /* Add billing item */
        addBillingItem(currentUser, newInvoice.id, BILLING_ITEM_ROOM, 
                      description, room.rate, days);
        
        /* Update invoice amounts */
        updateInvoiceAmounts(newInvoice.id);
        
        printf("\nInvoice created successfully with ID %d.\n", newInvoice.id);
        return 1;
    } else {
        printf("\nError: Could not find room information.\n");
        return 0;
    }
}

/* Modify an existing invoice */
int modifyInvoice(User *currentUser, int invoiceId) {
    FILE *fp, *tempFp;
    Invoice invoice;
    int found = 0;
    char newNotes[MAX_BILLING_NOTES_LEN];
    
    /* Check if invoice exists */
    if (!getInvoiceById(invoiceId, &invoice)) {
        printf("\nError: Invoice with ID %d not found.\n", invoiceId);
        return 0;
    }
    
    /* Check if invoice can be modified */
    if (invoice.status == INVOICE_STATUS_PAID || 
        invoice.status == INVOICE_STATUS_CANCELLED) {
        printf("\nError: Cannot modify invoice with status '%s'.\n", 
               getInvoiceStatusString(invoice.status));
        return 0;
    }
    
    /* Get new notes */
    printf("\nCurrent notes: %s\n", invoice.notes);
    printf("Enter new notes (leave empty to keep current): ");
    getchar(); /* Consume newline */
    fgets(newNotes, MAX_BILLING_NOTES_LEN, stdin);
    newNotes[strcspn(newNotes, "\n")] = '\0'; /* Remove newline */
    
    /* Update invoice if notes changed */
    if (strlen(newNotes) > 0) {
        /* Open files */
        fp = fopen(INVOICES_FILE, "rb");
        if (fp == NULL) {
            printf("\nError: Could not open invoices file.\n");
            return 0;
        }
        
        tempFp = fopen(TEMP_FILE, "wb");
        if (tempFp == NULL) {
            printf("\nError: Could not create temporary file.\n");
            fclose(fp);
            return 0;
        }
        
        /* Copy invoices to temp file with modification */
        while (fread(&invoice, sizeof(Invoice), 1, fp) == 1) {
            if (invoice.id == invoiceId && invoice.isActive) {
                strcpy(invoice.notes, newNotes);
                found = 1;
            }
            fwrite(&invoice, sizeof(Invoice), 1, tempFp);
        }
        
        /* Close files */
        fclose(fp);
        fclose(tempFp);
        
        /* Replace original file with temp file */
        if (found) {
            remove(INVOICES_FILE);
            rename(TEMP_FILE, INVOICES_FILE);
            printf("\nInvoice updated successfully.\n");
            return 1;
        } else {
            remove(TEMP_FILE);
            printf("\nError: Invoice not found or inactive.\n");
            return 0;
        }
    } else {
        printf("\nNo changes made to invoice.\n");
        return 1;
    }
}

/* Cancel an invoice */
int cancelInvoice(User *currentUser, int invoiceId) {
    FILE *fp, *tempFp;
    Invoice invoice;
    int found = 0;
    
    /* Check if invoice exists */
    if (!getInvoiceById(invoiceId, &invoice)) {
        printf("\nError: Invoice with ID %d not found.\n", invoiceId);
        return 0;
    }
    
    /* Check if invoice can be cancelled */
    if (invoice.status == INVOICE_STATUS_PAID || 
        invoice.status == INVOICE_STATUS_CANCELLED) {
        printf("\nError: Cannot cancel invoice with status '%s'.\n", 
               getInvoiceStatusString(invoice.status));
        return 0;
    }
    
    /* Confirm cancellation */
    printf("\nAre you sure you want to cancel invoice #%d? (1 for Yes, 0 for No): ", invoiceId);
    int confirm = getIntInput("", 0, 1);
    if (!confirm) {
        printf("\nCancellation aborted.\n");
        return 0;
    }
    
    /* Open files */
    fp = fopen(INVOICES_FILE, "rb");
    if (fp == NULL) {
        printf("\nError: Could not open invoices file.\n");
        return 0;
    }
    
    tempFp = fopen(TEMP_FILE, "wb");
    if (tempFp == NULL) {
        printf("\nError: Could not create temporary file.\n");
        fclose(fp);
        return 0;
    }
    
    /* Copy invoices to temp file with modification */
    while (fread(&invoice, sizeof(Invoice), 1, fp) == 1) {
        if (invoice.id == invoiceId && invoice.isActive) {
            invoice.status = INVOICE_STATUS_CANCELLED;
            found = 1;
        }
        fwrite(&invoice, sizeof(Invoice), 1, tempFp);
    }
    
    /* Close files */
    fclose(fp);
    fclose(tempFp);
    
    /* Replace original file with temp file */
    if (found) {
        remove(INVOICES_FILE);
        rename(TEMP_FILE, INVOICES_FILE);
        printf("\nInvoice cancelled successfully.\n");
        return 1;
    } else {
        remove(TEMP_FILE);
        printf("\nError: Invoice not found or inactive.\n");
        return 0;
    }
}

/* Issue an invoice (change status from draft to issued) */
int issueInvoice(User *currentUser, int invoiceId) {
    FILE *fp, *tempFp;
    Invoice invoice;
    int found = 0;
    
    /* Check if invoice exists */
    if (!getInvoiceById(invoiceId, &invoice)) {
        printf("\nError: Invoice with ID %d not found.\n", invoiceId);
        return 0;
    }
    
    /* Check if invoice can be issued */
    if (invoice.status != INVOICE_STATUS_DRAFT) {
        printf("\nError: Only draft invoices can be issued. Current status: %s\n", 
               getInvoiceStatusString(invoice.status));
        return 0;
    }
    
    /* Open files */
    fp = fopen(INVOICES_FILE, "rb");
    if (fp == NULL) {
        printf("\nError: Could not open invoices file.\n");
        return 0;
    }
    
    tempFp = fopen(TEMP_FILE, "wb");
    if (tempFp == NULL) {
        printf("\nError: Could not create temporary file.\n");
        fclose(fp);
        return 0;
    }
    
    /* Copy invoices to temp file with modification */
    while (fread(&invoice, sizeof(Invoice), 1, fp) == 1) {
        if (invoice.id == invoiceId && invoice.isActive) {
            invoice.status = INVOICE_STATUS_ISSUED;
            found = 1;
        }
        fwrite(&invoice, sizeof(Invoice), 1, tempFp);
    }
    
    /* Close files */
    fclose(fp);
    fclose(tempFp);
    
    /* Replace original file with temp file */
    if (found) {
        remove(INVOICES_FILE);
        rename(TEMP_FILE, INVOICES_FILE);
        printf("\nInvoice issued successfully.\n");
        return 1;
    } else {
        remove(TEMP_FILE);
        printf("\nError: Invoice not found or inactive.\n");
        return 0;
    }
}

/* Mark an invoice as paid */
int markInvoiceAsPaid(User *currentUser, int invoiceId) {
    FILE *fp, *tempFp;
    Invoice invoice;
    int found = 0;
    
    /* Check if invoice exists */
    if (!getInvoiceById(invoiceId, &invoice)) {
        printf("\nError: Invoice with ID %d not found.\n", invoiceId);
        return 0;
    }
    
    /* Check if invoice can be marked as paid */
    if (invoice.status != INVOICE_STATUS_ISSUED && 
        invoice.status != INVOICE_STATUS_OVERDUE) {
        printf("\nError: Only issued or overdue invoices can be marked as paid. Current status: %s\n", 
               getInvoiceStatusString(invoice.status));
        return 0;
    }
    
    /* Check if full amount has been paid */
    double paidAmount = calculateInvoicePaid(invoiceId);
    if (paidAmount < invoice.totalAmount) {
        printf("\nWarning: Invoice has not been fully paid.\n");
        printf("Total amount: $%.2f, Paid amount: $%.2f, Remaining: $%.2f\n", 
               invoice.totalAmount, paidAmount, invoice.totalAmount - paidAmount);
        
        printf("Do you still want to mark it as paid? (1 for Yes, 0 for No): ");
        int confirm = getIntInput("", 0, 1);
        if (!confirm) {
            printf("\nOperation cancelled.\n");
            return 0;
        }
    }
    
    /* Open files */
    fp = fopen(INVOICES_FILE, "rb");
    if (fp == NULL) {
        printf("\nError: Could not open invoices file.\n");
        return 0;
    }
    
    tempFp = fopen(TEMP_FILE, "wb");
    if (tempFp == NULL) {
        printf("\nError: Could not create temporary file.\n");
        fclose(fp);
        return 0;
    }
    
    /* Copy invoices to temp file with modification */
    while (fread(&invoice, sizeof(Invoice), 1, fp) == 1) {
        if (invoice.id == invoiceId && invoice.isActive) {
            invoice.status = INVOICE_STATUS_PAID;
            invoice.paidAmount = invoice.totalAmount; /* Mark as fully paid */
            found = 1;
            
            /* Update guest's stay info */
            updateGuestStayInfo(invoice.guestId, invoice.totalAmount);
        }
        fwrite(&invoice, sizeof(Invoice), 1, tempFp);
    }
    
    /* Close files */
    fclose(fp);
    fclose(tempFp);
    
    /* Replace original file with temp file */
    if (found) {
        remove(INVOICES_FILE);
        rename(TEMP_FILE, INVOICES_FILE);
        printf("\nInvoice marked as paid successfully.\n");
        return 1;
    } else {
        remove(TEMP_FILE);
        printf("\nError: Invoice not found or inactive.\n");
        return 0;
    }
}

/* List invoices with optional status filter */
void listInvoices(User *currentUser, int statusFilter) {
    FILE *fp;
    Invoice tempInvoice;
    int count = 0;
    
    /* Open invoices file */
    fp = fopen(INVOICES_FILE, "rb");
    if (fp == NULL) {
        printf("\nError: Could not open invoices file.\n");
        return;
    }
    
    /* Display header */
    clearScreen();
    printf("===== INVOICE LIST =====\n");
    
    if (statusFilter > 0) {
        printf("Filtered by status: %s\n\n", getInvoiceStatusString(statusFilter));
    }
    
    printf("%-5s %-12s %-12s %-12s %-12s %-15s %-10s %-10s\n", 
           "ID", "Reservation", "Guest", "Issue Date", "Due Date", "Status", "Paid", "Total");
    printf("---------------------------------------------------------------------------------\n");
    
    /* Display each invoice */
    while (fread(&tempInvoice, sizeof(Invoice), 1, fp) == 1) {
        if (tempInvoice.isActive && (statusFilter == 0 || tempInvoice.status == statusFilter)) {
            Guest guest;
            getGuestById(tempInvoice.guestId, &guest);
            
            printf("%-5d %-12d %-12d %-12s %-12s %-15s $%-9.2f $%-9.2f\n", 
                   tempInvoice.id, 
                   tempInvoice.reservationId, 
                   tempInvoice.guestId, 
                   tempInvoice.issueDate,
                   tempInvoice.dueDate,
                   getInvoiceStatusString(tempInvoice.status),
                   tempInvoice.paidAmount,
                   tempInvoice.totalAmount);
            count++;
        }
    }
    
    fclose(fp);
    
    /* Display footer */
    printf("---------------------------------------------------------------------------------\n");
    printf("Total invoices: %d\n", count);
}

/* Search for invoices */
void searchInvoices(User *currentUser) {
    FILE *fp;
    Invoice tempInvoice;
    int count = 0;
    int searchChoice;
    int searchId;
    
    /* Open invoices file */
    fp = fopen(INVOICES_FILE, "rb");
    if (fp == NULL) {
        printf("\nError: Could not open invoices file.\n");
        return;
    }
    
    /* Get search criteria */
    clearScreen();
    printf("===== SEARCH INVOICES =====\n");
    printf("1. Search by Invoice ID\n");
    printf("2. Search by Reservation ID\n");
    printf("3. Search by Guest ID\n");
    printf("0. Cancel\n");
    
    searchChoice = getIntInput("Enter your choice: ", 0, 3);
    
    if (searchChoice == 0) {
        fclose(fp);
        return;
    }
    
    printf("\nEnter ID to search for: ");
    scanf("%d", &searchId);
    
    /* Display header */
    clearScreen();
    printf("===== SEARCH RESULTS =====\n");
    printf("%-5s %-12s %-12s %-12s %-12s %-15s %-10s %-10s\n", 
           "ID", "Reservation", "Guest", "Issue Date", "Due Date", "Status", "Paid", "Total");
    printf("---------------------------------------------------------------------------------\n");
    
    /* Display matching invoices */
    while (fread(&tempInvoice, sizeof(Invoice), 1, fp) == 1) {
        if (tempInvoice.isActive) {
            int match = 0;
            
            switch (searchChoice) {
                case 1: /* Invoice ID */
                    match = (tempInvoice.id == searchId);
                    break;
                    
                case 2: /* Reservation ID */
                    match = (tempInvoice.reservationId == searchId);
                    break;
                    
                case 3: /* Guest ID */
                    match = (tempInvoice.guestId == searchId);
                    break;
            }
            
            if (match) {
                Guest guest;
                getGuestById(tempInvoice.guestId, &guest);
                
                printf("%-5d %-12d %-12d %-12s %-12s %-15s $%-9.2f $%-9.2f\n", 
                       tempInvoice.id, 
                       tempInvoice.reservationId, 
                       tempInvoice.guestId, 
                       tempInvoice.issueDate,
                       tempInvoice.dueDate,
                       getInvoiceStatusString(tempInvoice.status),
                       tempInvoice.paidAmount,
                       tempInvoice.totalAmount);
                count++;
            }
        }
    }
    
    fclose(fp);
    
    /* Display footer */
    printf("---------------------------------------------------------------------------------\n");
    printf("Total matching invoices: %d\n", count);
}

/* Get invoice by ID */
int getInvoiceById(int invoiceId, Invoice *invoice) {
    FILE *fp;
    Invoice tempInvoice;
    int found = 0;
    
    /* Open invoices file */
    fp = fopen(INVOICES_FILE, "rb");
    if (fp == NULL) {
        return 0; /* File not found */
    }
    
    /* Search for invoice */
    while (fread(&tempInvoice, sizeof(Invoice), 1, fp) == 1) {
        if (tempInvoice.id == invoiceId && tempInvoice.isActive) {
            *invoice = tempInvoice;
            found = 1;
            break;
        }
    }
    
    fclose(fp);
    return found;
}

/* Get invoice by reservation ID */
int getInvoiceByReservationId(int reservationId, Invoice *invoice) {
    FILE *fp;
    Invoice tempInvoice;
    int found = 0;
    
    /* Open invoices file */
    fp = fopen(INVOICES_FILE, "rb");
    if (fp == NULL) {
        return 0; /* File not found */
    }
    
    /* Search for invoice */
    while (fread(&tempInvoice, sizeof(Invoice), 1, fp) == 1) {
        if (tempInvoice.reservationId == reservationId && tempInvoice.isActive) {
            *invoice = tempInvoice;
            found = 1;
            break;
        }
    }
    
    fclose(fp);
    return found;
}

/* Billing item management functions */

/* Add a billing item to an invoice */
int addBillingItem(User *currentUser, int invoiceId, int type, const char *description, 
                  double unitPrice, int quantity) {
    FILE *fp;
    BillingItem newItem;
    Invoice invoice;
    int nextId = 1;
    
    /* Check if invoice exists */
    if (!getInvoiceById(invoiceId, &invoice)) {
        printf("\nError: Invoice with ID %d not found.\n", invoiceId);
        return 0;
    }
    
    /* Check if invoice can be modified */
    if (invoice.status == INVOICE_STATUS_PAID || 
        invoice.status == INVOICE_STATUS_CANCELLED) {
        printf("\nError: Cannot add items to invoice with status '%s'.\n", 
               getInvoiceStatusString(invoice.status));
        return 0;
    }
    
    /* Find next available ID */
    fp = fopen(BILLING_ITEMS_FILE, "rb");
    if (fp != NULL) {
        BillingItem tempItem;
        while (fread(&tempItem, sizeof(BillingItem), 1, fp) == 1) {
            if (tempItem.id >= nextId) {
                nextId = tempItem.id + 1;
            }
        }
        fclose(fp);
    }
    
    /* Initialize billing item */
    newItem.id = nextId;
    newItem.invoiceId = invoiceId;
    newItem.type = type;
    strcpy(newItem.description, description);
    newItem.unitPrice = unitPrice;
    newItem.quantity = quantity;
    newItem.amount = unitPrice * quantity;
    newItem.isActive = 1;
    
    /* Save billing item */
    fp = fopen(BILLING_ITEMS_FILE, "ab");
    if (fp == NULL) {
        printf("\nError: Could not open billing items file.\n");
        return 0;
    }
    
    fwrite(&newItem, sizeof(BillingItem), 1, fp);
    fclose(fp);
    
    /* Update invoice amounts */
    updateInvoiceAmounts(invoiceId);
    
    printf("\nBilling item added successfully with ID %d.\n", newItem.id);
    return 1;
}

/* Modify a billing item */
int modifyBillingItem(User *currentUser, int itemId, const char *description, 
                     double unitPrice, int quantity) {
    FILE *fp, *tempFp;
    BillingItem item;
    Invoice invoice;
    int found = 0;
    
    /* Check if item exists */
    if (!getBillingItemById(itemId, &item)) {
        printf("\nError: Billing item with ID %d not found.\n", itemId);
        return 0;
    }
    
    /* Check if invoice exists and can be modified */
    if (!getInvoiceById(item.invoiceId, &invoice)) {
        printf("\nError: Associated invoice not found.\n");
        return 0;
    }
    
    if (invoice.status == INVOICE_STATUS_PAID || 
        invoice.status == INVOICE_STATUS_CANCELLED) {
        printf("\nError: Cannot modify items for invoice with status '%s'.\n", 
               getInvoiceStatusString(invoice.status));
        return 0;
    }
    
    /* Open files */
    fp = fopen(BILLING_ITEMS_FILE, "rb");
    if (fp == NULL) {
        printf("\nError: Could not open billing items file.\n");
        return 0;
    }
    
    tempFp = fopen(TEMP_FILE, "wb");
    if (tempFp == NULL) {
        printf("\nError: Could not create temporary file.\n");
        fclose(fp);
        return 0;
    }
    
    /* Copy items to temp file with modification */
    while (fread(&item, sizeof(BillingItem), 1, fp) == 1) {
        if (item.id == itemId && item.isActive) {
            /* Update fields if provided */
            if (strlen(description) > 0) {
                strcpy(item.description, description);
            }
            
            if (unitPrice > 0) {
                item.unitPrice = unitPrice;
            }
            
            if (quantity > 0) {
                item.quantity = quantity;
            }
            
            /* Recalculate amount */
            item.amount = item.unitPrice * item.quantity;
            found = 1;
        }
        fwrite(&item, sizeof(BillingItem), 1, tempFp);
    }
    
    /* Close files */
    fclose(fp);
    fclose(tempFp);
    
    /* Replace original file with temp file */
    if (found) {
        remove(BILLING_ITEMS_FILE);
        rename(TEMP_FILE, BILLING_ITEMS_FILE);
        
        /* Update invoice amounts */
        updateInvoiceAmounts(invoice.id);
        
        printf("\nBilling item updated successfully.\n");
        return 1;
    } else {
        remove(TEMP_FILE);
        printf("\nError: Billing item not found or inactive.\n");
        return 0;
    }
}

/* Delete a billing item */
int deleteBillingItem(User *currentUser, int itemId) {
    FILE *fp, *tempFp;
    BillingItem item;
    Invoice invoice;
    int found = 0;
    
    /* Check if item exists */
    if (!getBillingItemById(itemId, &item)) {
        printf("\nError: Billing item with ID %d not found.\n", itemId);
        return 0;
    }
    
    /* Check if invoice exists and can be modified */
    if (!getInvoiceById(item.invoiceId, &invoice)) {
        printf("\nError: Associated invoice not found.\n");
        return 0;
    }
    
    if (invoice.status == INVOICE_STATUS_PAID || 
        invoice.status == INVOICE_STATUS_CANCELLED) {
        printf("\nError: Cannot delete items for invoice with status '%s'.\n", 
               getInvoiceStatusString(invoice.status));
        return 0;
    }
    
    /* Confirm deletion */
    printf("\nAre you sure you want to delete billing item #%d? (1 for Yes, 0 for No): ", itemId);
    int confirm = getIntInput("", 0, 1);
    if (!confirm) {
        printf("\nDeletion aborted.\n");
        return 0;
    }
    
    /* Open files */
    fp = fopen(BILLING_ITEMS_FILE, "rb");
    if (fp == NULL) {
        printf("\nError: Could not open billing items file.\n");
        return 0;
    }
    
    tempFp = fopen(TEMP_FILE, "wb");
    if (tempFp == NULL) {
        printf("\nError: Could not create temporary file.\n");
        fclose(fp);
        return 0;
    }
    
    /* Copy items to temp file with modification */
    while (fread(&item, sizeof(BillingItem), 1, fp) == 1) {
        if (item.id == itemId && item.isActive) {
            item.isActive = 0; /* Mark as inactive */
            found = 1;
        }
        fwrite(&item, sizeof(BillingItem), 1, tempFp);
    }
    
    /* Close files */
    fclose(fp);
    fclose(tempFp);
    
    /* Replace original file with temp file */
    if (found) {
        remove(BILLING_ITEMS_FILE);
        rename(TEMP_FILE, BILLING_ITEMS_FILE);
        
        /* Update invoice amounts */
        updateInvoiceAmounts(invoice.id);
        
        printf("\nBilling item deleted successfully.\n");
        return 1;
    } else {
        remove(TEMP_FILE);
        printf("\nError: Billing item not found or already inactive.\n");
        return 0;
    }
}

/* List billing items for an invoice */
void listBillingItems(User *currentUser, int invoiceId) {
    FILE *fp;
    BillingItem tempItem;
    int count = 0;
    double total = 0.0;
    
    /* Check if invoice exists */
    Invoice invoice;
    if (!getInvoiceById(invoiceId, &invoice)) {
        printf("\nError: Invoice with ID %d not found.\n", invoiceId);
        return;
    }
    
    /* Open billing items file */
    fp = fopen(BILLING_ITEMS_FILE, "rb");
    if (fp == NULL) {
        printf("\nError: Could not open billing items file.\n");
        return;
    }
    
    /* Display header */
    clearScreen();
    printf("===== BILLING ITEMS FOR INVOICE #%d =====\n", invoiceId);
    printf("Status: %s\n\n", getInvoiceStatusString(invoice.status));
    
    printf("%-5s %-30s %-15s %-10s %-10s %-10s\n", 
           "ID", "Description", "Type", "Unit Price", "Quantity", "Amount");
    printf("---------------------------------------------------------------------------------\n");
    
    /* Display each billing item */
    while (fread(&tempItem, sizeof(BillingItem), 1, fp) == 1) {
        if (tempItem.isActive && tempItem.invoiceId == invoiceId) {
            printf("%-5d %-30s %-15s $%-9.2f %-10d $%-9.2f\n", 
                   tempItem.id, 
                   tempItem.description, 
                   getBillingItemTypeString(tempItem.type), 
                   tempItem.unitPrice,
                   tempItem.quantity,
                   tempItem.amount);
            count++;
            total += tempItem.amount;
        }
    }
    
    fclose(fp);
    
    /* Display footer */
    printf("---------------------------------------------------------------------------------\n");
    printf("Total items: %d\n", count);
    printf("Subtotal: $%.2f\n", total);
    printf("Tax: $%.2f\n", invoice.taxAmount);
    printf("Discount: $%.2f\n", invoice.discountAmount);
    printf("Total: $%.2f\n", invoice.totalAmount);
    printf("Paid: $%.2f\n", invoice.paidAmount);
    printf("Balance: $%.2f\n", invoice.totalAmount - invoice.paidAmount);
}

/* Get billing item by ID */
int getBillingItemById(int itemId, BillingItem *item) {
    FILE *fp;
    BillingItem tempItem;
    int found = 0;
    
    /* Open billing items file */
    fp = fopen(BILLING_ITEMS_FILE, "rb");
    if (fp == NULL) {
        return 0; /* File not found */
    }
    
    /* Search for item */
    while (fread(&tempItem, sizeof(BillingItem), 1, fp) == 1) {
        if (tempItem.id == itemId && tempItem.isActive) {
            *item = tempItem;
            found = 1;
            break;
        }
    }
    
    fclose(fp);
    return found;
}

/* Payment management functions */

/* Record a payment for an invoice */
int recordPayment(User *currentUser, int invoiceId, int method, double amount, 
                 const char *transactionId, const char *notes) {
    FILE *fp;
    Payment newPayment;
    Invoice invoice;
    int nextId = 1;
    time_t now;
    struct tm *t;
    
    /* Check if invoice exists */
    if (!getInvoiceById(invoiceId, &invoice)) {
        printf("\nError: Invoice with ID %d not found.\n", invoiceId);
        return 0;
    }
    
    /* Check if invoice can accept payments */
    if (invoice.status == INVOICE_STATUS_CANCELLED) {
        printf("\nError: Cannot record payment for cancelled invoice.\n");
        return 0;
    }
    
    /* Check if payment would exceed total amount */
    double currentPaid = calculateInvoicePaid(invoiceId);
    if (currentPaid + amount > invoice.totalAmount) {
        printf("\nWarning: This payment would exceed the invoice total.\n");
        printf("Invoice total: $%.2f, Currently paid: $%.2f, This payment: $%.2f\n", 
               invoice.totalAmount, currentPaid, amount);
        
        printf("Do you want to continue? (1 for Yes, 0 for No): ");
        int confirm = getIntInput("", 0, 1);
        if (!confirm) {
            printf("\nPayment cancelled.\n");
            return 0;
        }
    }
    
    /* Get current date for transaction date */
    now = time(NULL);
    t = localtime(&now);
    sprintf(newPayment.transactionDate, "%04d-%02d-%02d", 
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
    
    /* Find next available ID */
    fp = fopen(PAYMENTS_FILE, "rb");
    if (fp != NULL) {
        Payment tempPayment;
        while (fread(&tempPayment, sizeof(Payment), 1, fp) == 1) {
            if (tempPayment.id >= nextId) {
                nextId = tempPayment.id + 1;
            }
        }
        fclose(fp);
    }
    
    /* Initialize payment */
    newPayment.id = nextId;
    newPayment.invoiceId = invoiceId;
    newPayment.method = method;
    newPayment.amount = amount;
    strcpy(newPayment.transactionId, transactionId);
    newPayment.status = PAYMENT_STATUS_COMPLETED;
    strcpy(newPayment.notes, notes);
    newPayment.createdBy = currentUser->id;
    newPayment.isActive = 1;
    
    /* Save payment */
    fp = fopen(PAYMENTS_FILE, "ab");
    if (fp == NULL) {
        printf("\nError: Could not open payments file.\n");
        return 0;
    }
    
    fwrite(&newPayment, sizeof(Payment), 1, fp);
    fclose(fp);
    
    /* Update invoice paid amount */
    updateInvoiceAmounts(invoiceId);
    
    /* Check if invoice is now fully paid */
    if (getInvoiceById(invoiceId, &invoice)) {
        if (invoice.paidAmount >= invoice.totalAmount && 
            invoice.status != INVOICE_STATUS_PAID) {
            printf("\nInvoice is now fully paid. Marking as paid.\n");
            markInvoiceAsPaid(currentUser, invoiceId);
        }
    }
    
    printf("\nPayment recorded successfully with ID %d.\n", newPayment.id);
    return 1;
}

/* Modify a payment */
int modifyPayment(User *currentUser, int paymentId, int method, double amount, 
                 const char *transactionId, const char *notes) {
    FILE *fp, *tempFp;
    Payment payment;
    Invoice invoice;
    int found = 0;
    
    /* Check if payment exists */
    if (!getPaymentById(paymentId, &payment)) {
        printf("\nError: Payment with ID %d not found.\n", paymentId);
        return 0;
    }
    
    /* Check if invoice exists */
    if (!getInvoiceById(payment.invoiceId, &invoice)) {
        printf("\nError: Associated invoice not found.\n");
        return 0;
    }
    
    /* Check if invoice is paid or cancelled */
    if (invoice.status == INVOICE_STATUS_PAID || 
        invoice.status == INVOICE_STATUS_CANCELLED) {
        printf("\nError: Cannot modify payment for invoice with status '%s'.\n", 
               getInvoiceStatusString(invoice.status));
        return 0;
    }
    
    /* Open files */
    fp = fopen(PAYMENTS_FILE, "rb");
    if (fp == NULL) {
        printf("\nError: Could not open payments file.\n");
        return 0;
    }
    
    tempFp = fopen(TEMP_FILE, "wb");
    if (tempFp == NULL) {
        printf("\nError: Could not create temporary file.\n");
        fclose(fp);
        return 0;
    }
    
    /* Copy payments to temp file with modification */
    while (fread(&payment, sizeof(Payment), 1, fp) == 1) {
        if (payment.id == paymentId && payment.isActive) {
            /* Update fields if provided */
            if (method > 0) {
                payment.method = method;
            }
            
            if (amount > 0) {
                payment.amount = amount;
            }
            
            if (strlen(transactionId) > 0) {
                strcpy(payment.transactionId, transactionId);
            }
            
            if (strlen(notes) > 0) {
                strcpy(payment.notes, notes);
            }
            
            found = 1;
        }
        fwrite(&payment, sizeof(Payment), 1, tempFp);
    }
    
    /* Close files */
    fclose(fp);
    fclose(tempFp);
    
    /* Replace original file with temp file */
    if (found) {
        remove(PAYMENTS_FILE);
        rename(TEMP_FILE, PAYMENTS_FILE);
        
        /* Update invoice amounts */
        updateInvoiceAmounts(payment.invoiceId);
        
        printf("\nPayment updated successfully.\n");
        return 1;
    } else {
        remove(TEMP_FILE);
        printf("\nError: Payment not found or inactive.\n");
        return 0;
    }
}

/* Cancel a payment */
int cancelPayment(User *currentUser, int paymentId) {
    FILE *fp, *tempFp;
    Payment payment;
    Invoice invoice;
    int found = 0;
    
    /* Check if payment exists */
    if (!getPaymentById(paymentId, &payment)) {
        printf("\nError: Payment with ID %d not found.\n", paymentId);
        return 0;
    }
    
    /* Check if invoice exists */
    if (!getInvoiceById(payment.invoiceId, &invoice)) {
        printf("\nError: Associated invoice not found.\n");
        return 0;
    }
    
    /* Check if invoice is paid or cancelled */
    if (invoice.status == INVOICE_STATUS_PAID || 
        invoice.status == INVOICE_STATUS_CANCELLED) {
        printf("\nError: Cannot cancel payment for invoice with status '%s'.\n", 
               getInvoiceStatusString(invoice.status));
        return 0;
    }
    
    /* Confirm cancellation */
    printf("\nAre you sure you want to cancel payment #%d for $%.2f? (1 for Yes, 0 for No): ", 
           paymentId, payment.amount);
    int confirm = getIntInput("", 0, 1);
    if (!confirm) {
        printf("\nCancellation aborted.\n");
        return 0;
    }
    
    /* Open files */
    fp = fopen(PAYMENTS_FILE, "rb");
    if (fp == NULL) {
        printf("\nError: Could not open payments file.\n");
        return 0;
    }
    
    tempFp = fopen(TEMP_FILE, "wb");
    if (tempFp == NULL) {
        printf("\nError: Could not create temporary file.\n");
        fclose(fp);
        return 0;
    }
    
    /* Copy payments to temp file with modification */
    while (fread(&payment, sizeof(Payment), 1, fp) == 1) {
        if (payment.id == paymentId && payment.isActive) {
            payment.status = PAYMENT_STATUS_REFUNDED;
            payment.isActive = 0; /* Mark as inactive */
            found = 1;
        }
        fwrite(&payment, sizeof(Payment), 1, tempFp);
    }
    
    /* Close files */
    fclose(fp);
    fclose(tempFp);
    
    /* Replace original file with temp file */
    if (found) {
        remove(PAYMENTS_FILE);
        rename(TEMP_FILE, PAYMENTS_FILE);
        
        /* Update invoice amounts */
        updateInvoiceAmounts(payment.invoiceId);
        
        printf("\nPayment cancelled successfully.\n");
        return 1;
    } else {
        remove(TEMP_FILE);
        printf("\nError: Payment not found or already inactive.\n");
        return 0;
    }
}

/* List payments for an invoice */
void listPayments(User *currentUser, int invoiceId) {
    FILE *fp;
    Payment tempPayment;
    int count = 0;
    double total = 0.0;
    
    /* Check if invoice exists */
    Invoice invoice;
    if (!getInvoiceById(invoiceId, &invoice)) {
        printf("\nError: Invoice with ID %d not found.\n", invoiceId);
        return;
    }
    
    /* Open payments file */
    fp = fopen(PAYMENTS_FILE, "rb");
    if (fp == NULL) {
        printf("\nError: Could not open payments file.\n");
        return;
    }
    
    /* Display header */
    clearScreen();
    printf("===== PAYMENTS FOR INVOICE #%d =====\n", invoiceId);
    printf("Status: %s\n\n", getInvoiceStatusString(invoice.status));
    
    printf("%-5s %-12s %-15s %-10s %-20s %-15s %-30s\n", 
           "ID", "Date", "Method", "Amount", "Transaction ID", "Status", "Notes");
    printf("---------------------------------------------------------------------------------\n");
    
    /* Display each payment */
    while (fread(&tempPayment, sizeof(Payment), 1, fp) == 1) {
        if (tempPayment.invoiceId == invoiceId && 
            (tempPayment.isActive || tempPayment.status == PAYMENT_STATUS_REFUNDED)) {
            printf("%-5d %-12s %-15s $%-9.2f %-20s %-15s %-30s\n", 
                   tempPayment.id, 
                   tempPayment.transactionDate, 
                   getPaymentMethodString(tempPayment.method), 
                   tempPayment.amount,
                   tempPayment.transactionId,
                   getPaymentStatusString(tempPayment.status),
                   tempPayment.notes);
            count++;
            
            if (tempPayment.isActive) {
                total += tempPayment.amount;
            }
        }
    }
    
    fclose(fp);
    
    /* Display footer */
    printf("---------------------------------------------------------------------------------\n");
    printf("Total payments: %d\n", count);
    printf("Total amount paid: $%.2f\n", total);
    printf("Invoice total: $%.2f\n", invoice.totalAmount);
    printf("Balance: $%.2f\n", invoice.totalAmount - total);
}

/* Get payment by ID */
int getPaymentById(int paymentId, Payment *payment) {
    FILE *fp;
    Payment tempPayment;
    int found = 0;
    
    /* Open payments file */
    fp = fopen(PAYMENTS_FILE, "rb");
    if (fp == NULL) {
        return 0; /* File not found */
    }
    
    /* Search for payment */
    while (fread(&tempPayment, sizeof(Payment), 1, fp) == 1) {
        if (tempPayment.id == paymentId && tempPayment.isActive) {
            *payment = tempPayment;
            found = 1;
            break;
        }
    }
    
    fclose(fp);
    return found;
}

/* Utility functions */

/* Calculate the total amount for an invoice */
double calculateInvoiceTotal(int invoiceId) {
    FILE *fp;
    BillingItem tempItem;
    double subtotal = 0.0;
    double taxRate = 0.1; /* 10% tax rate */
    double taxAmount = 0.0;
    double discountAmount = 0.0;
    double total = 0.0;
    
    /* Open billing items file */
    fp = fopen(BILLING_ITEMS_FILE, "rb");
    if (fp == NULL) {
        return 0.0; /* File not found */
    }
    
    /* Calculate subtotal from all active items */
    while (fread(&tempItem, sizeof(BillingItem), 1, fp) == 1) {
        if (tempItem.invoiceId == invoiceId && tempItem.isActive) {
            if (tempItem.type == BILLING_ITEM_DISCOUNT) {
                /* Discount items are subtracted */
                discountAmount += tempItem.amount;
            } else if (tempItem.type == BILLING_ITEM_TAX) {
                /* Tax items are added directly to tax amount */
                taxAmount += tempItem.amount;
            } else {
                /* All other items contribute to subtotal */
                subtotal += tempItem.amount;
            }
        }
    }
    
    fclose(fp);
    
    /* If no explicit tax items, calculate tax based on rate */
    if (taxAmount == 0.0) {
        taxAmount = subtotal * taxRate;
    }
    
    /* Calculate total */
    total = subtotal + taxAmount - discountAmount;
    if (total < 0) total = 0; /* Ensure total is not negative */
    
    return total;
}

/* Calculate the total amount paid for an invoice */
double calculateInvoicePaid(int invoiceId) {
    FILE *fp;
    Payment tempPayment;
    double total = 0.0;
    
    /* Open payments file */
    fp = fopen(PAYMENTS_FILE, "rb");
    if (fp == NULL) {
        return 0.0; /* File not found */
    }
    
    /* Sum all active payments */
    while (fread(&tempPayment, sizeof(Payment), 1, fp) == 1) {
        if (tempPayment.invoiceId == invoiceId && tempPayment.isActive) {
            total += tempPayment.amount;
        }
    }
    
    fclose(fp);
    return total;
}

/* Update invoice amounts (subtotal, tax, discount, total, paid) */
void updateInvoiceAmounts(int invoiceId) {
    FILE *fp, *tempFp;
    Invoice invoice;
    BillingItem tempItem;
    double subtotal = 0.0;
    double taxAmount = 0.0;
    double discountAmount = 0.0;
    double paidAmount = 0.0;
    int found = 0;
    
    /* Calculate subtotal, tax, and discount */
    fp = fopen(BILLING_ITEMS_FILE, "rb");
    if (fp != NULL) {
        while (fread(&tempItem, sizeof(BillingItem), 1, fp) == 1) {
            if (tempItem.invoiceId == invoiceId && tempItem.isActive) {
                if (tempItem.type == BILLING_ITEM_DISCOUNT) {
                    discountAmount += tempItem.amount;
                } else if (tempItem.type == BILLING_ITEM_TAX) {
                    taxAmount += tempItem.amount;
                } else {
                    subtotal += tempItem.amount;
                }
            }
        }
        fclose(fp);
    }
    
    /* If no explicit tax items, calculate tax based on rate */
    if (taxAmount == 0.0) {
        taxAmount = subtotal * 0.1; /* 10% tax rate */
    }
    
    /* Calculate paid amount */
    paidAmount = calculateInvoicePaid(invoiceId);
    
    /* Update invoice */
    fp = fopen(INVOICES_FILE, "rb");
    if (fp == NULL) {
        return; /* File not found */
    }
    
    tempFp = fopen(TEMP_FILE, "wb");
    if (tempFp == NULL) {
        fclose(fp);
        return;
    }
    
    /* Copy invoices to temp file with updated amounts */
    while (fread(&invoice, sizeof(Invoice), 1, fp) == 1) {
        if (invoice.id == invoiceId && invoice.isActive) {
            invoice.subtotal = subtotal;
            invoice.taxAmount = taxAmount;
            invoice.discountAmount = discountAmount;
            invoice.totalAmount = subtotal + taxAmount - discountAmount;
            if (invoice.totalAmount < 0) invoice.totalAmount = 0; /* Ensure total is not negative */
            invoice.paidAmount = paidAmount;
            found = 1;
        }
        fwrite(&invoice, sizeof(Invoice), 1, tempFp);
    }
    
    fclose(fp);
    fclose(tempFp);
    
    /* Replace original file with temp file */
    if (found) {
        remove(INVOICES_FILE);
        rename(TEMP_FILE, INVOICES_FILE);
    } else {
        remove(TEMP_FILE);
    }
}

/* Check for overdue invoices and update their status */
int checkOverdueInvoices(void) {
    FILE *fp, *tempFp;
    Invoice invoice;
    int count = 0;
    time_t now;
    struct tm *t;
    char currentDate[20];
    
    /* Get current date */
    now = time(NULL);
    t = localtime(&now);
    sprintf(currentDate, "%04d-%02d-%02d", 
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
    
    /* Open files */
    fp = fopen(INVOICES_FILE, "rb");
    if (fp == NULL) {
        return 0; /* File not found */
    }
    
    tempFp = fopen(TEMP_FILE, "wb");
    if (tempFp == NULL) {
        fclose(fp);
        return 0;
    }
    
    /* Check each invoice */
    while (fread(&invoice, sizeof(Invoice), 1, fp) == 1) {
        if (invoice.isActive && invoice.status == INVOICE_STATUS_ISSUED) {
            /* Check if due date has passed */
            if (strcmp(currentDate, invoice.dueDate) > 0) {
                invoice.status = INVOICE_STATUS_OVERDUE;
                count++;
            }
        }
        fwrite(&invoice, sizeof(Invoice), 1, tempFp);
    }
    
    fclose(fp);
    fclose(tempFp);
    
    /* Replace original file with temp file */
    if (count > 0) {
        remove(INVOICES_FILE);
        rename(TEMP_FILE, INVOICES_FILE);
    } else {
        remove(TEMP_FILE);
    }
    
    return count;
}