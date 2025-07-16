#ifndef BILLING_H
#define BILLING_H

#include "auth.h" // For User type

/* Invoice status definitions */
typedef enum {
    INVOICE_STATUS_DRAFT,
    INVOICE_STATUS_ISSUED,
    INVOICE_STATUS_PAID,
    INVOICE_STATUS_CANCELLED,
    INVOICE_STATUS_OVERDUE
} InvoiceStatus;

/* Payment method definitions */
typedef enum {
    PAYMENT_METHOD_CASH,
    PAYMENT_METHOD_CREDIT_CARD,
    PAYMENT_METHOD_DEBIT_CARD,
    PAYMENT_METHOD_BANK_TRANSFER,
    PAYMENT_METHOD_ONLINE
} PaymentMethod;

/* Payment status definitions */
typedef enum {
    PAYMENT_STATUS_PENDING,
    PAYMENT_STATUS_COMPLETED,
    PAYMENT_STATUS_FAILED,
    PAYMENT_STATUS_REFUNDED
} PaymentStatus;

/* Billing item type definitions */
typedef enum {
    BILLING_ITEM_ROOM_CHARGE,
    BILLING_ITEM_FOOD_SERVICE,
    BILLING_ITEM_LAUNDRY,
    BILLING_ITEM_MINIBAR,
    BILLING_ITEM_SPA,
    BILLING_ITEM_TAX,
    BILLING_ITEM_DISCOUNT,
    BILLING_ITEM_OTHER
} BillingItemType;

/* Maximum lengths for strings */
#define MAX_PAYMENT_REF_LEN 50
#define MAX_NOTES_LEN 200
#define MAX_BILLING_DESCRIPTION_LEN 100

/* File paths */
#define INVOICES_FILE "data/invoices.dat"
#define BILLING_ITEMS_FILE "data/billing_items.dat"
#define PAYMENTS_FILE "data/payments.dat"

/* Billing item structure */
typedef struct {
    int id;
    int invoiceId;
    BillingItemType type;
    char description[MAX_BILLING_DESCRIPTION_LEN];
    double unitPrice;
    int quantity;
    double amount; // unitPrice * quantity
    int isActive;
} BillingItem;

/* Invoice structure */
typedef struct {
    int id;
    int guestId;
    int reservationId;
    char issueDate[11];      /* YYYY-MM-DD */
    char dueDate[11];        /* YYYY-MM-DD */
    double subtotal;
    double taxAmount;
    double discountAmount;
    double totalAmount;
    double paidAmount;
    InvoiceStatus status;
    char notes[MAX_NOTES_LEN];
    int createdBy;
    int isActive;
} Invoice;

/* Payment structure */
typedef struct {
    int id;
    int invoiceId;
    PaymentMethod method;
    PaymentStatus status;
    double amount;
    char transactionDate[11]; /* YYYY-MM-DD */
    char transactionId[MAX_PAYMENT_REF_LEN];
    char notes[MAX_NOTES_LEN];
    int createdBy;
    int isActive;
} Payment;

/* Function prototypes */
const char* getInvoiceStatusString(InvoiceStatus status);
const char* getPaymentMethodString(PaymentMethod method);
const char* getPaymentStatusString(PaymentStatus status);
const char* getBillingItemTypeString(BillingItemType type);
int initializeBillingData(void);
int createInvoice(User *currentUser, int reservationId);
int modifyInvoice(User *currentUser, int invoiceId);
int cancelInvoice(User *currentUser, int invoiceId);
int issueInvoice(User *currentUser, int invoiceId);
int markInvoiceAsPaid(User *currentUser, int invoiceId);
void listInvoices(User *currentUser, int statusFilter);
void searchInvoices(User *currentUser);
int getInvoiceById(int invoiceId, Invoice *invoice);
int getInvoiceByReservationId(int reservationId, Invoice *invoice);
int addBillingItem(User *currentUser, int invoiceId, BillingItemType type, const char *description, double unitPrice, int quantity);
void listBillingItems(User *currentUser, int invoiceId);
int recordPayment(User *currentUser, int invoiceId, PaymentMethod method, double amount, const char *transactionId, const char *notes);
void listPayments(User *currentUser, int invoiceId);
int checkOverdueInvoices(void);
void billingManagementMenu(User *currentUser);

#endif /* BILLING_H */