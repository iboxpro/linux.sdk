#ifndef RESULT_H_
#define RESULT_H_

#include "WebObject.h"

// enums common

// enums reader
typedef enum
{
	Ibox_Result_ReaderCheckCardType_SWIPE = 0,
	Ibox_Result_ReaderCheckCardType_INSERT = 1,
	Ibox_Result_ReaderCheckCardType_TAP = 2
} Ibox_Result_ReaderCheckCardType;

// enums web

// structures common
typedef struct {
	const char *errorMessage;
	int errorCode;
} Ibox_Result;

// structures reader
typedef struct {
    const char *errorMessage;
    const char *id;
    int errorCode;
} Ibox_Result_ReaderId;

typedef struct {
    const char *errorMessage;
    const char *bootloaderVersion;
    const char *firmwareVersion;
    const char *hardwareVersion;
    int errorCode;
} Ibox_Result_ReaderInfo;

typedef struct {
	Ibox_Result_ReaderCheckCardType type;
    const char *errorMessage;
    const char *data;
    int errorCode;
} Ibox_Result_ReaderCheckCard;

// structures web
typedef struct {
	Ibox_Account *account;
	Ibox_Product **products;
    const char *errorMessage;
    int errorCode;
    int productsCount;
} Ibox_Result_Authentication;

typedef struct {
	Ibox_Transaction *transaction;
	const char *errorMessage;
	const char *emvOnlineProcessResult;
	int errorCode;
	int requireSignature;
} Ibox_Result_Submit;

typedef struct {
	Ibox_LinkedCard **linkedCards;
	const char *errorMessage;
	int errorCode;
	int linkedCardsCount;
} Ibox_Result_LinkedCards;

typedef struct {
    Ibox_LinkedCard *linkedCard;
    const char *errorMessage;
    int errorCode;
} Ibox_Result_AddLinkedCard;

typedef struct {
	Ibox_Transaction *transaction;
    const char *errorMessage;
    const char *emvOnlineProcessResult;
    int errorCode;
} Ibox_Result_Reverse;

typedef struct {
    const char *errorMessage;
    const char *scheduleId;
    int errorCode;
} Ibox_Result_ScheduleSubmit;

typedef struct {
	Ibox_Transaction **transactions;
	const char *errorMessage;
	int errorCode;
	int transactionsCount;
} Ibox_Result_Transactions;

typedef struct {
	Ibox_ProductField **fields;
    const char *errorMessage;
    double amount;
    int errorCode;
    int fieldsCount;
} Ibox_Result_Prepare;

typedef struct
{
	char Cmd;
	char Status;
	char CardType;
	char ATQA[2];
	char SAK;
	char UID[7];
	char *cardData;
	int cardDataLen;

	const char *errorMessage;
	int errorCode;
} Ibox_Result_Mifare;


#endif
