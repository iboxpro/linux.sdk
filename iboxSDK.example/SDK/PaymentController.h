#ifndef PAYMENTCONTROLLER_H_
#define PAYMENTCONTROLLER_H_

#include "Result.h"
#include "WebObject.h"

// enums
typedef enum
{
	Ibox_PaymentController_InputType_CARD = 0,
	Ibox_PaymentController_InputType_LINKED_CARD = 1,
	Ibox_PaymentController_InputType_PREPAID = 2,
	Ibox_PaymentController_InputType_CREDIT = 3,
	Ibox_PaymentController_InputType_CASH = 4,
	Ibox_PaymentController_InputType_LINK = 5
} Ibox_PaymentController_InputType;

// structures
typedef struct {
    char *data;
    int length;
} Ibox_MemoryStruct;

typedef struct {
	Ibox_PaymentController_InputType inputType;
	Ibox_Purchase **purchases;
	Ibox_Product *product;
	Ibox_ProductField **productData;
	Ibox_Acquirer *acquirer;
	const char *description;
	double amount;
	int linkedCardId;
	int purchasesCount;
	int productDataCount;
} Ibox_PaymentContext;

typedef struct {
	const char *transactionId;
	double amountReverse;
} Ibox_ReverseContext;

// events
typedef Ibox_MemoryStruct *(*IboxSendWebRequestAction) (const char *, const char *);
typedef Ibox_MemoryStruct *(*IboxSendReaderRequestAction) (char *, int);

// setters
void Ibox_PaymentController_SetCredentials(const char *email, const char *key);
void Ibox_PaymentController_SetSendWebRequestAction(IboxSendWebRequestAction action);
void Ibox_PaymentController_SetSendReaderRequestAction(IboxSendReaderRequestAction action);

// authentication
Ibox_Result_Authentication *Ibox_PaymentController_Authentication();

// reader id
Ibox_Result_ReaderId *Ibox_PaymentController_ReaderId();

// reader info
Ibox_Result_ReaderInfo *Ibox_PaymentController_ReaderInfo();

// reader reset
Ibox_Result *Ibox_PaymentController_ReaderReset();

// reader sound
Ibox_Result *Ibox_PaymentController_ReaderSoundEnabled(int enabled);

// linked cards
Ibox_Result_LinkedCards *Ibox_PaymentController_LinkedCards();

// add linked card
Ibox_Result_ReaderCheckCard *Ibox_PaymentController_GetCardData();
Ibox_Result_AddLinkedCard *Ibox_PaymentController_AddLinkedCard();
Ibox_Result_AddLinkedCard *Ibox_PaymentController_AddLinkedCardWithReaderData(Ibox_Result_ReaderCheckCard *readerData);

// remove linked card
Ibox_Result *Ibox_PaymentController_RemoveLinkedCard(int linkedCardId);

// start payment
Ibox_Result_Submit *Ibox_PaymentController_StartPayment(Ibox_PaymentContext *context);

// start reverse
Ibox_Result_Reverse *Ibox_PaymentController_StartReverse(Ibox_ReverseContext *context);

// history
Ibox_Result_Transactions *Ibox_PaymentController_Transaction(const char *transactionId);
Ibox_Result_Transactions *Ibox_PaymentController_Transactions(int page);

// other
void Ibox_PaymentController_SetDebugEnabled(int debugEnabled);
char *Ibox_PaymentController_Version();

#endif
