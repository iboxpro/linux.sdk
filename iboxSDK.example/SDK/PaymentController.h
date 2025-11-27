#ifndef PAYMENTCONTROLLER_H_
#define PAYMENTCONTROLLER_H_

#define MIFARE_POLL_CARD 0x01
#define MIFARE_VERIFY_CARD 0x02
#define MIFARE_READ_CARD 0x03
#define MIFARE_WRITE_CARD 0x04
#define MIFARE_FINISH_TRANSACTION 0x0E

#define MIFARE_OK                                                               0x00
#define MIFARE_PARAM_ERROR                                                      0x01
#define MIFARE_TIMEOUT_ERROR                                                    0x02
#define MIFARE_CRC_ERROR                                                        0x03
#define MIFARE_NACK_ERROR                                                       0x04
#define MIFARE_POLL_ERROR                                                       0x05
#define MIFARE_OPERATION_ERROR                                                  0x06

#include "Result.h"

// enums
typedef enum
{
	Ibox_PaymentController_InputType_CARD = 0,
	Ibox_PaymentController_InputType_LINKED_CARD = 1,
	Ibox_PaymentController_InputType_PREPAID = 2,
	Ibox_PaymentController_InputType_CREDIT = 3,
	Ibox_PaymentController_InputType_CASH = 4,
	Ibox_PaymentController_InputType_LINK = 5,
	Ibox_PaymentController_InputType_OUTER_CARD = 6
} Ibox_PaymentController_InputType;

typedef enum
{
	Ibox_PaymentController_CurrencyType_VND = 0,
	Ibox_PaymentController_CurrencyType_RUB = 1
} Ibox_PaymentController_CurrencyType;

// structures
typedef struct
{
	char *data;
	int length;
} Ibox_MemoryStruct;

typedef struct
{
	Ibox_PaymentController_InputType inputType;
	Ibox_PaymentController_CurrencyType currencyType;
	Ibox_Purchase **purchases;
	Ibox_Product *product;
	Ibox_ProductField **productData;
	Ibox_Acquirer *acquirer;
	Ibox_Tag **tags;
	const char *receiptEmail;
	const char *receiptPhone;
	const char *acquirerCode;
	const char *purchasesJson;
	const char *description;
	const char *imagePath;
	const char *tagsJson;
	const char *extId;
	double amount;
	double amountCashGot;
	int linkedCardId;
	int purchasesCount;
	int productDataCount;
	int tagsCount;
	int singleStepAuth;
} Ibox_PaymentContext;

typedef struct
{
	Ibox_Purchase **purchases;
	Ibox_Tag **tags;
	const char *receiptEmail;
	const char *receiptPhone;
	const char *transactionId;
	const char *purchasesJson;
	const char *tagsJson;
	const char *extId;
	double amountReverse;
	int purchasesCount;
	int tagsCount;
	int forceReturn;
} Ibox_ReverseContext;

typedef struct
{
	Ibox_PaymentController_CurrencyType currencyType;
	Ibox_Schedule_Type scheduleType;
	Ibox_Schedule_EndType scheduleEndType;
	Ibox_Product *product;
	Ibox_ProductField **productData;
	const char *description;
	const char *receiptEmail;
	const char *receiptPhone;
	const char *scheduleId;
	const char *image;
	const char *startDate;
	const char *endDate;
	char **dates;
	double amount;
	int productDataCount;
	int datesCount;
	int endCount;
	int month;
	int day;
	int dayWeek;
	int hour;
	int minute;
} Ibox_ScheduleContext;

// events
typedef Ibox_MemoryStruct *(*IboxSendWebRequestAction)(const char *, const char *);
typedef Ibox_MemoryStruct *(*IboxSendReaderRequestAction)(char *, int);
typedef void (*IboxStartTransactionAction)(const char *);
typedef void (*IboxWaiting4CardAction)();
typedef void (*IboxEmvTransactionStartAction)();
typedef int (*IboxSelectEmvApplicationAction)(char **, int);
typedef void (*IboxReverseTransactionNotFoundAction)();
typedef int (*IboxReverseCancellationTimeoutAction)();
typedef int (*IboxCancelCardCheckAction)();

// setters
void Ibox_PaymentController_SetCredentials(const char *email, const char *key);
void Ibox_PaymentController_SetClientProductCode(const char *code);
void Ibox_PaymentController_SetSendWebRequestAction(IboxSendWebRequestAction action);
void Ibox_PaymentController_SetSendReaderRequestAction(IboxSendReaderRequestAction action);
void Ibox_PaymentController_SetStartTransactionAction(IboxStartTransactionAction action);
void Ibox_PaymentController_SetWaiting4CardAction(IboxWaiting4CardAction action);
void Ibox_PaymentController_SetEmvTransactionStartAction(IboxEmvTransactionStartAction action);
void Ibox_PaymentController_SetSelectEmvApplicationAction(IboxSelectEmvApplicationAction action);
void Ibox_PaymentController_SetReverseTransactionNotFoundAction(IboxReverseTransactionNotFoundAction action);
void Ibox_PaymentController_SetReverseCancellationTimeoutAction(IboxReverseCancellationTimeoutAction action);
void Ibox_PaymentController_SetCancelCheckCardAction(IboxCancelCardCheckAction action);

// authentication
Ibox_Result_Authentication *Ibox_PaymentController_Authentication();

// reader id
Ibox_Result_ReaderId *Ibox_PaymentController_ReaderId();

// reader info
Ibox_Result_ReaderInfo *Ibox_PaymentController_ReaderInfo();

// reader reset
void Ibox_PaymentController_ReaderReset();

// reader sound
void Ibox_PaymentController_ReaderSoundEnabled(int enabled);

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

// start schedule
Ibox_Result_ScheduleSubmit *Ibox_PaymentController_StartSchedule(Ibox_ScheduleContext *context);

// history
Ibox_Result_Transactions *Ibox_PaymentController_Transaction(const char *transactionId);
Ibox_Result_Transactions *Ibox_PaymentController_TransactionByInvoice(const char *invoice);
Ibox_Result_Transactions *Ibox_PaymentController_TransactionByRRN(const char *rrn);
Ibox_Result_Transactions *Ibox_PaymentController_Transactions(int page);

// adjust
Ibox_Result *Ibox_PaymentController_AdjustPayment(const char *transactionId, const char *receiptPhone, const char *receiptEmail);
Ibox_Result *Ibox_PaymentController_AdjustReverse(const char *transactionId, const char *receiptPhone, const char *receiptEmail);
Ibox_Result *Ibox_PaymentController_AdjustSchedule(const char *scheduleId, const char *receiptPhone, const char *receiptEmail);

// fiscalize
Ibox_Result *Ibox_PaymentController_Fiscalize(const char *transactionId);

// prepare
Ibox_Result_Prepare *Ibox_PaymentController_Prepare(Ibox_Product *product, Ibox_ProductField **productData, int productDataCount);

// other
char *Ibox_PaymentController_Version();
void Ibox_PaymentController_CancelGetCardData();
void Ibox_PaymentController_SetDebugEnabled(int debugEnabled);
int Ibox_PaymentController_IsReaderConected();
void Ibox_PaymentController_SetNFCOnly(int nfcOnly);

void Ibox_PaymentController_Cleanup();

Ibox_Result_Mifare *Ibox_PaymentController_DoMifare(int code);
void Ibox_PaymentController_setMifareKeyClass(int keyClass);
void Ibox_PaymentController_setMifareBlockAddr(int addr);
void Ibox_PaymentController_setMifareOperation(int cmd);
void Ibox_PaymentController_setMifareKeyValue(char *keyValue, int len);
void Ibox_PaymentController_setMifareCardUid(char *cardUid);
void Ibox_PaymentController_setMifareCardData(char *cardData, int len);
void Ibox_PaymentController_setMifareQuickAddr(int startAddr, int endAddr);
void Ibox_PaymentController_mifareTranstransmission(char *data, int len);

#endif
