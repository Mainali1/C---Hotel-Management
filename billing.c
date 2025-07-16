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
 #include "reservation.h"
 #include "room.h"
 
 #define TEMP_INVOICES_FILE "data/temp_invoices.dat"
 #define TEMP_ITEMS_FILE "data/temp_items.dat"
 #define TEMP_PAYMENTS_FILE "data/temp_payments.dat"
 
 // Forward declarations for local utility functions
 static void updateInvoiceAmounts(int invoiceId);
 static double calculateInvoicePaid(int invoiceId);
 
 /* Get the string representation of an invoice status */
 const char* getInvoiceStatusString(InvoiceStatus status) {
     switch (status) {
         case INVOICE_STATUS_DRAFT: return "Draft";
         case INVOICE_STATUS_ISSUED: return "Issued";
         case INVOICE_STATUS_PAID: return "Paid";
         case INVOICE_STATUS_CANCELLED: return "Cancelled";
         case INVOICE_STATUS_OVERDUE: return "Overdue";
         default: return "Unknown";
     }
 }
  
 /* Get the string representation of a payment method */
 const char* getPaymentMethodString(PaymentMethod method) {
      switch (method) {
          case PAYMENT_METHOD_CASH: return "Cash";
          case PAYMENT_METHOD_CREDIT_CARD: return "Credit Card";
          case PAYMENT_METHOD_DEBIT_CARD: return "Debit Card";
          case PAYMENT_METHOD_BANK_TRANSFER: return "Bank Transfer";
          case PAYMENT_METHOD_ONLINE: return "Online Payment";
          default: return "Unknown";
      }
 }
  
 /* Get the string representation of a payment status */
 const char* getPaymentStatusString(PaymentStatus status) {
      switch (status) {
          case PAYMENT_STATUS_PENDING: return "Pending";
          case PAYMENT_STATUS_COMPLETED: return "Completed";
          case PAYMENT_STATUS_FAILED: return "Failed";
          case PAYMENT_STATUS_REFUNDED: return "Refunded";
          default: return "Unknown";
      }
 }
  
 /* Get the string representation of a billing item type */
 const char* getBillingItemTypeString(BillingItemType type) {
      switch (type) {
          case BILLING_ITEM_ROOM_CHARGE: return "Room Charge";
          case BILLING_ITEM_FOOD_SERVICE: return "Food Service";
          case BILLING_ITEM_LAUNDRY: return "Laundry";
          case BILLING_ITEM_MINIBAR: return "Minibar";
          case BILLING_ITEM_SPA: return "Spa Service";
          case BILLING_ITEM_TAX: return "Tax";
          case BILLING_ITEM_DISCOUNT: return "Discount";
          case BILLING_ITEM_OTHER: return "Other";
          default: return "Unknown";
      }
 }
  
 /* Initialize billing data files if they don't exist */
 int initializeBillingData(void) {
     if (!fileExists(INVOICES_FILE)) {
         FILE *fp = fopen(INVOICES_FILE, "wb");
         if (!fp) { printf("\nError: Could not create invoices file.\n"); return 0; }
         fclose(fp);
     }
     if (!fileExists(BILLING_ITEMS_FILE)) {
         FILE *fp = fopen(BILLING_ITEMS_FILE, "wb");
         if (!fp) { printf("\nError: Could not create billing items file.\n"); return 0; }
         fclose(fp);
     }
     if (!fileExists(PAYMENTS_FILE)) {
         FILE *fp = fopen(PAYMENTS_FILE, "wb");
         if (!fp) { printf("\nError: Could not create payments file.\n"); return 0; }
         fclose(fp);
     }
     
     int count = checkOverdueInvoices();
     if (count > 0) {
         printf("\nSystem check: %d invoice(s) marked as overdue.\n", count);
     }
     return 1;
 }
 
 /* Create a new invoice for a reservation */
 int createInvoice(User *currentUser, int reservationId) {
     Invoice newInvoice;
     Reservation reservation;
 
     if (!getReservationById(reservationId, &reservation)) {
         printf("\nError: Reservation with ID %d not found.\n", reservationId);
         return 0;
     }
     
     if (getInvoiceByReservationId(reservationId, &newInvoice)) {
         printf("\nError: Invoice already exists for reservation ID %d (Invoice ID: %d).\n", reservationId, newInvoice.id);
         return 0;
     }
     
     int nextId = 1;
     FILE *fp = fopen(INVOICES_FILE, "rb");
     if (fp) {
         Invoice temp;
         while(fread(&temp, sizeof(Invoice), 1, fp) == 1) {
             if (temp.id >= nextId) nextId = temp.id + 1;
         }
         fclose(fp);
     }
 
     newInvoice.id = nextId;
     newInvoice.reservationId = reservationId;
     newInvoice.guestId = reservation.guestId;
     newInvoice.status = INVOICE_STATUS_DRAFT;
     newInvoice.paidAmount = 0.0;
     newInvoice.createdBy = currentUser->id;
     newInvoice.isActive = 1;
     strcpy(newInvoice.notes, "Auto-generated invoice.");
     
     getCurrentDate(newInvoice.issueDate);
     // Set due date to reservation check-out date
     strcpy(newInvoice.dueDate, reservation.checkOutDate);
 
     // Save the invoice shell first
     fp = fopen(INVOICES_FILE, "ab");
     if (!fp) {
         printf("\nError: Could not open invoices file for writing.\n");
         return 0;
     }
     fwrite(&newInvoice, sizeof(Invoice), 1, fp);
     fclose(fp);
     
     // Add room charge as the first billing item
     Room room;
     if (getRoomById(reservation.roomId, &room)) {
         int days = calculateDateDifference(reservation.checkInDate, reservation.checkOutDate);
         if (days <= 0) days = 1;
         
         char description[MAX_BILLING_DESCRIPTION_LEN];
         sprintf(description, "Room %d stay (%d nights)", room.id, days);
         addBillingItem(currentUser, newInvoice.id, BILLING_ITEM_ROOM_CHARGE, description, room.rate, days);
     }
     
     updateInvoiceAmounts(newInvoice.id);
     
     printf("\nInvoice #%d created successfully for Reservation #%d.\n", newInvoice.id, reservationId);
     return newInvoice.id;
 }
 
 
 /* Add a billing item to an invoice */
 int addBillingItem(User *currentUser, int invoiceId, BillingItemType type, const char *description, 
                   double unitPrice, int quantity) {
     Invoice invoice;
     if (!getInvoiceById(invoiceId, &invoice)) {
         printf("\nError: Invoice with ID %d not found.\n", invoiceId);
         return 0;
     }
 
     if (invoice.status == INVOICE_STATUS_PAID || invoice.status == INVOICE_STATUS_CANCELLED) {
         printf("\nError: Cannot add items to a %s invoice.\n", getInvoiceStatusString(invoice.status));
         return 0;
     }
     
     int nextId = 1;
     FILE *fp = fopen(BILLING_ITEMS_FILE, "rb");
     if (fp) {
         BillingItem temp;
         while(fread(&temp, sizeof(BillingItem), 1, fp) == 1) {
             if (temp.id >= nextId) nextId = temp.id + 1;
         }
         fclose(fp);
     }
 
     BillingItem newItem;
     newItem.id = nextId;
     newItem.invoiceId = invoiceId;
     newItem.type = type;
     strncpy(newItem.description, description, MAX_BILLING_DESCRIPTION_LEN - 1);
     newItem.description[MAX_BILLING_DESCRIPTION_LEN - 1] = '\0';
     newItem.unitPrice = unitPrice;
     newItem.quantity = quantity;
     newItem.amount = unitPrice * quantity;
     newItem.isActive = 1;
     
     fp = fopen(BILLING_ITEMS_FILE, "ab");
     if (!fp) {
         printf("\nError: Could not open billing items file.\n");
         return 0;
     }
     fwrite(&newItem, sizeof(BillingItem), 1, fp);
     fclose(fp);
     
     updateInvoiceAmounts(invoiceId);
     printf("\nBilling item added to Invoice #%d.\n", invoiceId);
     return 1;
 }
 
 /* Record a payment for an invoice */
 int recordPayment(User *currentUser, int invoiceId, PaymentMethod method, double amount, 
                  const char *transactionId, const char *notes) {
     Invoice invoice;
     if (!getInvoiceById(invoiceId, &invoice)) {
         printf("\nError: Invoice with ID %d not found.\n", invoiceId);
         return 0;
     }
 
     if (invoice.status == INVOICE_STATUS_PAID || invoice.status == INVOICE_STATUS_CANCELLED) {
         printf("\nError: Cannot record payment for a %s invoice.\n", getInvoiceStatusString(invoice.status));
         return 0;
     }
     
     int nextId = 1;
     FILE* fp = fopen(PAYMENTS_FILE, "rb");
     if (fp) {
         Payment temp;
         while(fread(&temp, sizeof(Payment), 1, fp) == 1) {
             if (temp.id >= nextId) nextId = temp.id + 1;
         }
         fclose(fp);
     }
     
     Payment newPayment;
     newPayment.id = nextId;
     newPayment.invoiceId = invoiceId;
     newPayment.method = method;
     newPayment.amount = amount;
     newPayment.status = PAYMENT_STATUS_COMPLETED;
     getCurrentDate(newPayment.transactionDate);
     strncpy(newPayment.transactionId, transactionId, MAX_PAYMENT_REF_LEN - 1);
     newPayment.transactionId[MAX_PAYMENT_REF_LEN - 1] = '\0';
     strncpy(newPayment.notes, notes, MAX_NOTES_LEN - 1);
     newPayment.notes[MAX_NOTES_LEN - 1] = '\0';
     newPayment.createdBy = currentUser->id;
     newPayment.isActive = 1;
     
     fp = fopen(PAYMENTS_FILE, "ab");
     if (!fp) {
         printf("\nError: Could not open payments file.\n");
         return 0;
     }
     fwrite(&newPayment, sizeof(Payment), 1, fp);
     fclose(fp);
     
     updateInvoiceAmounts(invoiceId);
     
     // Refresh invoice data to check if it's now paid
     getInvoiceById(invoiceId, &invoice);
     if (invoice.paidAmount >= invoice.totalAmount) {
         printf("\nInvoice is now fully paid. Marking as PAID.\n");
         markInvoiceAsPaid(currentUser, invoiceId);
     }
     
     printf("\nPayment of $%.2f recorded for Invoice #%d.\n", amount, invoiceId);
     return 1;
 }
 
 /* Update invoice total amounts based on its items and payments */
 static void updateInvoiceAmounts(int invoiceId) {
     double subtotal = 0.0, tax = 0.0, discount = 0.0;
     
     FILE *fp_items = fopen(BILLING_ITEMS_FILE, "rb");
     if (fp_items) {
         BillingItem item;
         while(fread(&item, sizeof(BillingItem), 1, fp_items) == 1) {
             if (item.invoiceId == invoiceId && item.isActive) {
                 if (item.type == BILLING_ITEM_DISCOUNT) {
                     discount += item.amount;
                 } else if (item.type == BILLING_ITEM_TAX) {
                     tax += item.amount;
                 } else {
                     subtotal += item.amount;
                 }
             }
         }
         fclose(fp_items);
     }
     
     double paid = calculateInvoicePaid(invoiceId);
     
     FILE *fp_inv = fopen(INVOICES_FILE, "rb");
     if (!fp_inv) return;
     FILE *fp_temp = fopen(TEMP_INVOICES_FILE, "wb");
     if (!fp_temp) { fclose(fp_inv); return; }
 
     Invoice inv;
     while(fread(&inv, sizeof(Invoice), 1, fp_inv) == 1) {
         if (inv.id == invoiceId) {
             inv.subtotal = subtotal;
             inv.taxAmount = tax;
             inv.discountAmount = discount;
             inv.totalAmount = (subtotal + tax) - discount;
             if (inv.totalAmount < 0) inv.totalAmount = 0;
             inv.paidAmount = paid;
         }
         fwrite(&inv, sizeof(Invoice), 1, fp_temp);
     }
     
     fclose(fp_inv);
     fclose(fp_temp);
     remove(INVOICES_FILE);
     rename(TEMP_INVOICES_FILE, INVOICES_FILE);
 }
 
 /* Calculate the total amount paid for an invoice */
 static double calculateInvoicePaid(int invoiceId) {
     double total = 0.0;
     FILE *fp = fopen(PAYMENTS_FILE, "rb");
     if (fp) {
         Payment p;
         while(fread(&p, sizeof(Payment), 1, fp) == 1) {
             if (p.invoiceId == invoiceId && p.isActive && p.status == PAYMENT_STATUS_COMPLETED) {
                 total += p.amount;
             }
         }
         fclose(fp);
     }
     return total;
 }
 
 /* List invoices with optional status filter */
 void listInvoices(User *currentUser, int statusFilter) {
     FILE *fp = fopen(INVOICES_FILE, "rb");
     if (!fp) {
         printf("\nNo invoices found or error opening file.\n");
         return;
     }
     
     clearScreen();
     printf("===== INVOICE LIST =====\n");
     printf("%-5s %-10s %-10s %-12s %-12s %-15s %-12s %-12s\n", 
            "ID", "Guest ID", "Resv. ID", "Issue Date", "Due Date", "Status", "Paid", "Total");
     printf("--------------------------------------------------------------------------------------------------\n");
     
     Invoice inv;
     int count = 0;
     while (fread(&inv, sizeof(Invoice), 1, fp) == 1) {
         // statusFilter -1 means no filter
         if (inv.isActive && (statusFilter == -1 || inv.status == statusFilter)) {
             printf("%-5d %-10d %-10d %-12s %-12s %-15s $%-11.2f $%-11.2f\n", 
                    inv.id, inv.guestId, inv.reservationId, inv.issueDate, inv.dueDate,
                    getInvoiceStatusString(inv.status), inv.paidAmount, inv.totalAmount);
             count++;
         }
     }
     fclose(fp);
     printf("--------------------------------------------------------------------------------------------------\n");
     printf("Total invoices found: %d\n", count);
 }
 
 /* Get invoice by ID */
 int getInvoiceById(int invoiceId, Invoice *invoice) {
     FILE *fp = fopen(INVOICES_FILE, "rb");
     if (!fp) return 0;
     
     int found = 0;
     Invoice temp;
     while (fread(&temp, sizeof(Invoice), 1, fp) == 1) {
         if (temp.id == invoiceId && temp.isActive) {
             *invoice = temp;
             found = 1;
             break;
         }
     }
     fclose(fp);
     return found;
 }
 
 /* Get invoice by reservation ID */
 int getInvoiceByReservationId(int reservationId, Invoice *invoice) {
     FILE *fp = fopen(INVOICES_FILE, "rb");
     if (!fp) return 0;
     
     int found = 0;
     Invoice temp;
     while (fread(&temp, sizeof(Invoice), 1, fp) == 1) {
         if (temp.reservationId == reservationId && temp.isActive) {
             *invoice = temp;
             found = 1;
             break;
         }
     }
     fclose(fp);
     return found;
 }
 
 /* Mark an invoice as paid */
 int markInvoiceAsPaid(User *currentUser, int invoiceId) {
     Invoice inv;
     if (!getInvoiceById(invoiceId, &inv)) {
         printf("\nError: Invoice not found.\n");
         return 0;
     }
     
     if (inv.status != INVOICE_STATUS_ISSUED && inv.status != INVOICE_STATUS_OVERDUE) {
         printf("\nError: Only Issued or Overdue invoices can be marked as paid.\n");
         return 0;
     }
     
     if (inv.paidAmount < inv.totalAmount) {
         printf("\nWarning: Invoice not fully paid (Balance: $%.2f).", inv.totalAmount - inv.paidAmount);
         char choice;
         printf("\nMark as paid anyway? (y/n): ");
         scanf(" %c", &choice);
         if (choice != 'y' && choice != 'Y') {
             printf("Operation cancelled.\n");
             return 0;
         }
     }
     
     FILE *fp = fopen(INVOICES_FILE, "rb+");
     if (!fp) {
         printf("\nError opening invoices file.\n");
         return 0;
     }
     
     int found = 0;
     while (fread(&inv, sizeof(Invoice), 1, fp) == 1) {
         if (inv.id == invoiceId) {
             fseek(fp, -sizeof(Invoice), SEEK_CUR);
             inv.status = INVOICE_STATUS_PAID;
             fwrite(&inv, sizeof(Invoice), 1, fp);
             found = 1;
             break;
         }
     }
     fclose(fp);
     
     if (found) {
         printf("\nInvoice #%d marked as PAID.\n", invoiceId);
         updateGuestStayInfo(inv.guestId, inv.totalAmount);
     }
     return found;
 }
 
 
 /* Check for overdue invoices and update their status */
 int checkOverdueInvoices(void) {
     FILE *fp = fopen(INVOICES_FILE, "rb+");
     if (!fp) return 0;
 
     int count = 0;
     char currentDate[11];
     getCurrentDate(currentDate);
 
     Invoice inv;
     while (fread(&inv, sizeof(Invoice), 1, fp) == 1) {
         if (inv.isActive && inv.status == INVOICE_STATUS_ISSUED && strcmp(currentDate, inv.dueDate) > 0) {
             inv.status = INVOICE_STATUS_OVERDUE;
             fseek(fp, -sizeof(Invoice), SEEK_CUR);
             fwrite(&inv, sizeof(Invoice), 1, fp);
             fseek(fp, 0, SEEK_CUR); // Reset stream state
             count++;
         }
     }
     fclose(fp);
     return count;
 }
 
 
 /* List billing items for a specific invoice */
 void listBillingItems(User *currentUser, int invoiceId) {
     Invoice invoice;
     if (!getInvoiceById(invoiceId, &invoice)) {
         printf("\nError: Invoice with ID %d not found.\n", invoiceId);
         return;
     }
     
     FILE *fp = fopen(BILLING_ITEMS_FILE, "rb");
     if (!fp) {
         printf("\nError opening billing items file.\n");
         return;
     }
     
     clearScreen();
     printf("===== BILLING ITEMS FOR INVOICE #%d =====\n", invoiceId);
     printf("Status: %s\n\n", getInvoiceStatusString(invoice.status));
     
     printf("%-5s %-30s %-15s %-12s %-10s %-12s\n", 
            "ID", "Description", "Type", "Unit Price", "Quantity", "Amount");
     printf("------------------------------------------------------------------------------------------\n");
     
     BillingItem item;
     int count = 0;
     while (fread(&item, sizeof(BillingItem), 1, fp) == 1) {
         if (item.invoiceId == invoiceId && item.isActive) {
             printf("%-5d %-30s %-15s $%-11.2f %-10d $%-11.2f\n", 
                    item.id, item.description, getBillingItemTypeString(item.type),
                    item.unitPrice, item.quantity, item.amount);
             count++;
         }
     }
     fclose(fp);
     
     printf("------------------------------------------------------------------------------------------\n");
     printf("Subtotal: $%.2f | Tax: $%.2f | Discount: $%.2f\n", 
            invoice.subtotal, invoice.taxAmount, invoice.discountAmount);
     printf("TOTAL: $%.2f | PAID: $%.2f | BALANCE: $%.2f\n",
            invoice.totalAmount, invoice.paidAmount, invoice.totalAmount - invoice.paidAmount);
 }
 
 /* List payments for a specific invoice */
 void listPayments(User *currentUser, int invoiceId) {
     Invoice invoice;
     if (!getInvoiceById(invoiceId, &invoice)) {
         printf("\nError: Invoice with ID %d not found.\n", invoiceId);
         return;
     }
 
     FILE *fp = fopen(PAYMENTS_FILE, "rb");
     if (!fp) {
         printf("\nError opening payments file.\n");
         return;
     }
 
     clearScreen();
     printf("===== PAYMENTS FOR INVOICE #%d =====\n", invoiceId);
     printf("Balance due: $%.2f\n\n", invoice.totalAmount - invoice.paidAmount);
     printf("%-5s %-12s %-15s %-12s %-20s %-15s\n", 
            "ID", "Date", "Method", "Amount", "Transaction ID", "Status");
     printf("----------------------------------------------------------------------------------\n");
 
     Payment p;
     while (fread(&p, sizeof(Payment), 1, fp) == 1) {
         if (p.invoiceId == invoiceId && p.isActive) {
             printf("%-5d %-12s %-15s $%-11.2f %-20s %-15s\n", 
                    p.id, p.transactionDate, getPaymentMethodString(p.method),
                    p.amount, p.transactionId, getPaymentStatusString(p.status));
         }
     }
     fclose(fp);
     printf("----------------------------------------------------------------------------------\n");
 }
 
 
 // Dummy implementations for functions called by the menu that were missing
 int modifyInvoice(User* u, int id) { printf("\nFunction not implemented.\n"); return 0; }
 int cancelInvoice(User* u, int id) { printf("\nFunction not implemented.\n"); return 0; }
 int issueInvoice(User* u, int id) { printf("\nFunction not implemented.\n"); return 0; }
 
 
 /* Billing Management Menu */
 void billingManagementMenu(User *currentUser) {
      int choice, invoiceId, reservationId;
      
      do {
          clearScreen();
          printf("===== BILLING MANAGEMENT =====\n");
          printf("1. List All Invoices\n");
          printf("2. View Invoice Details (Items & Payments)\n");
          printf("3. Create Invoice from Reservation\n");
          printf("4. Add Billing Item to Invoice\n");
          printf("5. Record Payment for Invoice\n");
          printf("6. Mark Invoice as Paid\n");
          printf("7. Check for Overdue Invoices\n");
          printf("0. Back to Main Menu\n");
          printf("================================\n");
          
          choice = getIntInput("Enter your choice: ", 0, 7);
          
          switch (choice) {
              case 1:
                  listInvoices(currentUser, -1); // -1 for no filter
                  break;
              case 2:
                 listInvoices(currentUser, -1);
                 invoiceId = getIntInput("\nEnter Invoice ID to view details: ", 1, 99999);
                 listBillingItems(currentUser, invoiceId);
                 printf("\n");
                 listPayments(currentUser, invoiceId);
                  break;
              case 3:
                 listReservations(currentUser, 0); // Show all reservations
                 reservationId = getIntInput("\nEnter Reservation ID to create invoice for: ", 1, 99999);
                 createInvoice(currentUser, reservationId);
                  break;
              case 4: { // Add Billing Item
                 listInvoices(currentUser, -1);
                 invoiceId = getIntInput("\nEnter Invoice ID to add item to: ", 1, 99999);
                 
                 printf("\nItem Type:\n1-Room Charge, 2-Food, 3-Laundry, 4-Minibar, 5-Spa, 6-Tax, 7-Discount, 8-Other\n");
                 BillingItemType itemType = (BillingItemType)getIntInput("Enter type (1-8): ", 1, 8);
                 
                 char desc[MAX_BILLING_DESCRIPTION_LEN];
                 double price;
                 int qty;
                 printf("Enter description: ");
                 getStringInput(NULL, desc, MAX_BILLING_DESCRIPTION_LEN);
                 price = getDoubleInput("Enter unit price: $", 0.0, 10000.0);
                 qty = getIntInput("Enter quantity: ", 1, 100);
                 
                 addBillingItem(currentUser, invoiceId, itemType, desc, price, qty);
                 break;
              }
              case 5: { // Record Payment
                 listInvoices(currentUser, -1);
                 invoiceId = getIntInput("\nEnter Invoice ID to record payment for: ", 1, 99999);
                 
                 printf("\nPayment Method:\n0-Cash, 1-Credit Card, 2-Debit Card, 3-Bank Transfer, 4-Online\n");
                 PaymentMethod payMethod = (PaymentMethod)getIntInput("Enter method (0-4): ", 0, 4);
                 
                 double amount;
                 char ref[MAX_PAYMENT_REF_LEN];
                 char notes[MAX_NOTES_LEN];
 
                 amount = getDoubleInput("Enter amount paid: $", 0.01, 100000.0);
                 printf("Enter reference/transaction ID: ");
                 getStringInput(NULL, ref, MAX_PAYMENT_REF_LEN);
                 printf("Enter notes (optional): ");
                 getStringInput(NULL, notes, MAX_NOTES_LEN);
 
                 recordPayment(currentUser, invoiceId, payMethod, amount, ref, notes);
                 break;
              }
              case 6:
                 listInvoices(currentUser, -1);
                 invoiceId = getIntInput("\nEnter Invoice ID to mark as paid: ", 1, 99999);
                 markInvoiceAsPaid(currentUser, invoiceId);
                 break;
             case 7:
                 printf("\nChecking for overdue invoices...\n");
                 int count = checkOverdueInvoices();
                 printf("%d invoice(s) updated to 'Overdue' status.\n", count);
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