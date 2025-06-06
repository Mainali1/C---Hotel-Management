#ifndef BILLING_H
#define BILLING_H

/* Invoice status definitions */
typedef enum {
    INVOICE_STATUS_PENDING,
    INVOICE_STATUS_PAID,
    INVOICE_STATUS_OVERDUE,
    INVOICE_STATUS_CANCELLED,
    INVOICE_STATUS_REFUNDED
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
    BILLING_ITEM_OTHER
} BillingItemType;

/* Maximum lengths for strings */
#define MAX_PAYMENT_REF_LEN 50
#define MAX_NOTES_LEN 200

/* File paths */
#define BILLING_FILE "data/billing.dat"
#define PAYMENT_FILE "data/payments.dat"

/* Billing item structure */
typedef struct {
    int id;
    int invoiceId;
    BillingItemType type;
    char description[100];
    double amount;
    char date[11];  /* YYYY-MM-DD */
} BillingItem;

/* Invoice structure */
typedef struct {
    int id;
    int guestId;
    int reservationId;
    char date[11];          /* YYYY-MM-DD */
    char dueDate[11];       /* YYYY-MM-DD */
    double totalAmount;
    InvoiceStatus status;
    char notes[MAX_NOTES_LEN];
} Invoice;

/* Payment structure */
typedef struct {
    int id;
    int invoiceId;
    PaymentMethod method;
    PaymentStatus status;
    double amount;
    char date[11];          /* YYYY-MM-DD */
    char reference[MAX_PAYMENT_REF_LEN];
    char notes[MAX_NOTES_LEN];
} Payment;

/* Function prototypes */
const char* getInvoiceStatusString(InvoiceStatus status);
const char* getPaymentMethodString(PaymentMethod method);
const char* getPaymentStatusString(PaymentStatus status);
const char* getBillingItemTypeString(BillingItemType type);
int initializeBillingData(void);
int createInvoice(int guestId, int reservationId);
int addBillingItem(int invoiceId, BillingItem *item);
int processPayment(int invoiceId, Payment *payment);
int cancelInvoice(int invoiceId);
int refundPayment(int paymentId);
void listInvoices(void);
void searchInvoices(void);
void generateBillingReport(void);
void checkOverdueInvoices(void);

#endif /* BILLING_H */