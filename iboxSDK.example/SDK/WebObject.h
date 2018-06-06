#ifndef WEBOBJECT_H
#define WEBOBJECT_H

// enums
typedef enum
{
	Ibox_Transaction_InputType_MANUAL = 1,
	Ibox_Transaction_InputType_SWIPE = 2,
	Ibox_Transaction_InputType_EMV = 3,
	Ibox_Transaction_InputType_NFC = 4,
	Ibox_Transaction_InputType_PREPAID = 8,
	Ibox_Transaction_InputType_CREDIT = 9,
	Ibox_Transaction_InputType_CASH = 10,
	Ibox_Transaction_InputType_LINK = 30
} Ibox_Transaction_InputType;

typedef enum
{
	Ibox_Transaction_ReverseMode_NONE = 0,
	Ibox_Transaction_ReverseMode_RETURN = 1,
	Ibox_Transaction_ReverseMode_RETURN_PARTIAL = 2,
	Ibox_Transaction_ReverseMode_CANCEL = 3,
	Ibox_Transaction_ReverseMode_CANCEL_PARTIAL = 4,
	Ibox_Transaction_ReverseMode_CANCEL_CNP = 5,
	Ibox_Transaction_ReverseMode_CANCEL_CNP_PARTIAL = 6,
	Ibox_Transaction_ReverseMode_AUTO_REVERSE = 7,
} Ibox_Transaction_ReverseMode;

typedef enum
{
	Ibox_LinkedCard_Status_CREATED = 1,
	Ibox_LinkedCard_Status_VERIFYING = 2,
	Ibox_LinkedCard_Status_VERIFIED = 3,
	Ibox_LinkedCard_Status_FAILED_2_VERIFY = 10
} Ibox_LinkedCard_Status;

typedef enum
{
	Ibox_LinkedCard_State_DISABLED = 0,
	Ibox_LinkedCard_State_ENABLED = 1
} Ibox_LinkedCard_State;

// structures
typedef struct
{
	Ibox_Transaction_InputType inputType;
	Ibox_Transaction_ReverseMode reverseMode;
	const char *id;
	const char *date;
	const char *currencyId;
	const char *amountFormat;
	const char *amountFormatWithoutCurrency;
	const char *currencySign;
	const char *description;
	const char *stateDisplay;
	const char *stateLine1;
	const char *stateLine2;
	const char *invoice;
	double amount;
	double amountEff;
} Ibox_Transaction;

typedef struct
{
	Ibox_LinkedCard_Status status;
	Ibox_LinkedCard_State state;
	const char *alias;
	const char *shortNumber;
	const char *cardholderName;
	const char *panMasked;
	const char *cardExpiration;
	const char *cardType;
	int deleted;
	int visible;
	int id;
} Ibox_LinkedCard;

typedef struct
{
	Ibox_Transaction_InputType inputType;
	const char *id;
	const char *code;
	const char *name;
	const char *imageUrl;
} Ibox_Acquirer;

typedef struct
{
	Ibox_Transaction_InputType inputType;
	Ibox_Acquirer *acquirer;
} Ibox_PaymentOption;

typedef struct
{
	const char *title;
	char **taxes;
	double price;
	double quantity;
	int taxesCount;
} Ibox_Purchase;

typedef struct
{
	const char *title;
	const char *code;
} Ibox_Product;

typedef struct
{
	const char *title;
	const char *code;
	const char *textMask;
	const char *textRegExp;
	const char *defaultValue;
	char *value;
} Ibox_ProductField;

typedef struct
{
	Ibox_PaymentOption **paymentOptions;
	const char *name;
	const char *branchName;
	const char *branchAdress;
	const char *branchPhone;
	const char *clientName;
	const char *clientLegalName;
	const char *clientLegalAddress;
	const char *clientRealAddress;
	const char *clientPhone;
	const char *clientWeb;
	const char *bankName;
	const char *terminalName;
	int id;
	int singleStepAuth;
	int paymentOptionsCount;
} Ibox_Account;

#endif
