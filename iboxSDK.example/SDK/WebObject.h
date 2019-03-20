#ifndef WEBOBJECT_H
#define WEBOBJECT_H

// enums
typedef enum
{
	Ibox_Transaction_InputType_MANUAL = 1,
	Ibox_Transaction_InputType_SWIPE = 2,
	Ibox_Transaction_InputType_EMV = 3,
	Ibox_Transaction_InputType_NFC = 4,
	Ibox_Transaction_InputType_OUTER_CARD = 7,
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
	Ibox_Transaction_ReverseMode_AUTO_REVERSE = 7
} Ibox_Transaction_ReverseMode;

typedef enum
{
	Ibox_Transaction_DisplayMode_DECLINE = 0,
	Ibox_Transaction_DisplayMode_SUCCESS = 1,
	Ibox_Transaction_DisplayMode_REVERSE = 2,
	Ibox_Transaction_DisplayMode_REVERSED = 3,
	Ibox_Transaction_DisplayMode_NON_FINANCIAL = 100
} Ibox_Transaction_DisplayMode;

typedef enum
{
	Ibox_Transaction_SubstateStyle_BLACK,
	Ibox_Transaction_SubstateStyle_RED,
	Ibox_Transaction_SubstateStyle_GREY,
	Ibox_Transaction_SubstateStyle_GREY_STRIKETHROUGH
} Ibox_Transaction_SubstateStyle;

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

typedef enum
{
	Ibox_FiscalInfo_Status_NONE = 0,
	Ibox_FiscalInfo_Status_CREATED = 1,
	Ibox_FiscalInfo_Status_SUCCESS = 2,
	Ibox_FiscalInfo_Status_FAILURE = 4
} Ibox_FiscalInfo_Status;

typedef enum
{
	Ibox_ProductField_Type_NONE = 0,
	Ibox_ProductField_Type_TEXT = 1,
	Ibox_ProductField_Type_IMAGE = 2
} Ibox_ProductField_Type;

typedef enum
{
	Ibox_ProductField_State_DISABLED = 0,
	Ibox_ProductField_State_ENABLED = 1
} Ibox_ProductField_State;

typedef enum
{
	Ibox_Schedule_Type_NONE = -1,
	Ibox_Schedule_Type_DELAYED_ONCE = 0,
	Ibox_Schedule_Type_WEEKLY = 1,
	Ibox_Schedule_Type_MONTHLY = 2,
	Ibox_Schedule_Type_QUARTERLY = 3,
	Ibox_Schedule_Type_ANNUAL = 4,
	Ibox_Schedule_Type_ARBITRARY_DATES = 5,
	Ibox_Schedule_Type_DAILY = 6
} Ibox_Schedule_Type;

typedef enum
{
	Ibox_Schedule_EndType_NONE = 0,
	Ibox_Schedule_EndType_DATE = 1,
	Ibox_Schedule_EndType_COUNT = 2
} Ibox_Schedule_EndType;

// structures
typedef struct
{
	Ibox_FiscalInfo_Status status;
	const char *printerSerialNumber;
	const char *printerRegisterNumber;
	const char *printerShift;
	const char *printerCryptographicVerificationCode;
	const char *printerDocSerialNumber;
	const char *documentNumber;
	const char *documentMark;
	const char *storageNumber;
	const char *dateTime;
	const char *message;
	int error;
} Ibox_FiscalInfo;

typedef struct
{
	Ibox_Transaction_InputType inputType;
	Ibox_Transaction_ReverseMode reverseMode;
	Ibox_Transaction_DisplayMode displayMode;
	Ibox_Transaction_SubstateStyle substateStyle;
	Ibox_FiscalInfo *fiscalInfo;
	const char *id;
	const char *date;
	const char *currencyId;
	const char *currencyIso;
	const char *amountFormat;
	const char *amountFormatWithoutCurrency;
	const char *currencySign;
	const char *currencySignSafe;
	const char *description;
	const char *stateDisplay;
	const char *stateLine1;
	const char *stateLine2;
	const char *invoice;
	const char *purchasesJson;
	double amount;
	double amountEff;
	int currencyDecimalsCount;
	int substate;
	int state;
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
	const char *code;
	const char *value;
	double doubleValue;
	int intValue;
} Ibox_Tag;

typedef struct
{
	Ibox_Tag **tags;
	const char *title;
	char **taxes;
	double price;
	double quantity;
	int taxesCount;
	int tagsCount;
} Ibox_Purchase;

typedef struct
{
	Ibox_ProductField_State state;
	Ibox_ProductField_Type type;
	const char *title;
	const char *code;
	const char *textMask;
	const char *textRegExp;
	const char *defaultValue;
	const char *value;
	int required;
	int preparable;
	int userVisible;
	int numeric;
} Ibox_ProductField;

typedef struct
{
	Ibox_ProductField **fields;
	Ibox_ProductField *preparableField;
	const char *title;
	const char *code;
	int fieldsCount;
	int preparable;
} Ibox_Product;

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
