#include <stdio.h>

#include "hw.h"
#include "SDK/PaymentController.h"
#include "SDK/WebObject.h"
#include "SDK/ReleaseObjects.h"
#include "hextools.h"

#define DEBUG_ENABLED 1
#define AUTH 1
#define GET_READER_INFO 0
#define TEST_LINKED_CARD 0
#define TEST_CARD_PAYMENT 1
#define TEST_SCHEDULE 0
#define TEST_REVERSE 0
#define TEST_PURCHASES 0
#define TEST_PRODUCT 0
#define TEST_TAGS 0
#define TEST_TRANSACTION 0
#define TEST_FISCALIZE 0
#define TEST_PREPARE 0
#define TEST_REVERSE_LAST_TRANSACTION 0
#define TEST_MIFARE 0

#define EMAIL ""
#define SECRET_KEY ""
#define AMOUNT 1.0
#define AMOUNT_REVERSE 0.0
#define TRANSACTION_ID "42b5798b-ec5f-4ecd-8428-62bd51b5c176"
#define RECEIPT_EMAIL "test@mail.de"
#define RECEIPT_PHONE "+73211232312"
#define ACQUIRER_CODE NULL //"ACQ_CODE"
#define PRODUCT_FIELD_TEST_DATA "field data"
#define PRINTER_TAPE_WIDTH_CHARS 32

// member variables
Ibox_Account *mAccount = NULL;
const char *mTransactionId = NULL;

// callbacks
void startTransactionAction(const char *transactionId)
{

	if (transactionId) {
		if (mTransactionId)
			free(mTransactionId);
		mTransactionId = calloc(strlen(transactionId) + 1, sizeof(char));
		strcpy(mTransactionId, transactionId);
		fprintf(stderr, "TransactionID:%s\n", transactionId);
	}
	else
		fprintf(stderr, "Transaction started.\n");
}

int selectEmvApplicationAction(char **emvApplications, int emvApplicationsCount)
{
	fprintf(stderr, "selectEmvApplicationAction\n");
	int selectedIndex = 0;
	for (int i = 0; i < emvApplicationsCount; i++)
		fprintf(stderr, "emv app title: %d - %s\n", i, emvApplications[i]);
	return selectedIndex;
}

void reverseTransactionNotFoundAction()
{
	fprintf(stderr, "reverseTransactionNotFoundAction\n");
}

int reverseCancellationTimeoutAction()
{
	fprintf(stderr, "reverseCancellationTimeoutAction\n");
	int makeReturn = 1;
	return makeReturn;
}

int times = 0;
int cancelCheckCardAction()
{
	if (times++ > 3)
	{
		fprintf(stderr, "cancelCheckCardAction: 3 times. Cancelling\n");
		return 1;
	}
	else
		return 0;
}

// print slip
int utf8strlen(const char *p)
{
	int l;
	for (l = 0; *p != 0; ++p)
	{
		if ((*p & 0xC0) != 0x80)
			++l;
	}
	return l;
}

char *createAmountString(double amount, const char *format, const char *currency)
{
	char *returnValue = NULL;

	char *digits = calloc(1, sizeof(double));
	snprintf(digits, sizeof(double), format ? format : "%.2lf", amount);

	if (currency)
	{
		int length = strlen(digits) + strlen(currency) + 1;
		returnValue = calloc(length + 1, sizeof(char));
		strncpy(returnValue, digits, strlen(digits));
		strcat(returnValue, " ");
		strncat(returnValue, currency, strlen(currency));
		strcat(returnValue, "\0");
	}
	else
	{
		returnValue = calloc(strlen(digits), sizeof(char));
		strncpy(returnValue, digits, strlen(digits));
		strcat(returnValue, "\0");
	}

	free(digits);
	return returnValue;
}

void printKeyValue(const char *key, const char *value)
{
	if (key && value)
	{
		int keyLength = utf8strlen(key);
		int valueLength = utf8strlen(value);

		if ((keyLength + valueLength + 1) < PRINTER_TAPE_WIDTH_CHARS)
		{
			int spacesLength = PRINTER_TAPE_WIDTH_CHARS - keyLength - valueLength;
			char *spaces = calloc(spacesLength + 1, sizeof(char));
			for (int i = 0; i < spacesLength; i++)
				strcat(spaces, " ");
			strcat(spaces, "\0");
			fprintf(stderr, "%s%s%s\n", key, spaces, value);
			free(spaces);
		}
	}
}

void printCenterText(const char *text)
{
	if (text && strlen(text))
	{
		int textLength = utf8strlen(text);

		if (textLength == PRINTER_TAPE_WIDTH_CHARS)
			fprintf(stderr, "%s\n", text);
		else if (textLength < PRINTER_TAPE_WIDTH_CHARS)
		{
			int delta = PRINTER_TAPE_WIDTH_CHARS - textLength;
			int spacesLength = (int)(delta / 2.0);

			char *spaces = calloc(spacesLength + 1, sizeof(char));
			for (int i = 0; i < spacesLength; i++)
				strcat(spaces, " ");
			strcat(spaces, "\0");

			fprintf(stderr, "%s%s\n", spaces, text);
			free(spaces);
		}
	}
}

void prinDividertLine(char dividerSign)
{
	if (dividerSign)
	{
		char *dividerLine = calloc(PRINTER_TAPE_WIDTH_CHARS + 1, sizeof(char));
		for (int i = 0; i < PRINTER_TAPE_WIDTH_CHARS; i++)
			dividerLine[i] = dividerSign;
		dividerLine[PRINTER_TAPE_WIDTH_CHARS] = '\0';
		fprintf(stderr, "%s\n", dividerLine);
		free(dividerLine);
	}
}

void printSlip(Ibox_Transaction *transaction, Ibox_Account *account, int requireSignature)
{
	if (transaction && account)
	{
		int isElectronic = 1;

		Ibox_Card *card = transaction->card;
		if (card)
		{
			if (card->type == Ibox_Card_Type_CASH)
				isElectronic = 0;
			else if (card->type == Ibox_Card_Type_PREPAID)
				isElectronic = 0;
			else if (card->type == Ibox_Card_Type_CREDIT)
				isElectronic = 0;
			else if (card->type == Ibox_Card_Type_OUTER_CARD)
				isElectronic = 0;
		}

		if (isElectronic)
		{
			prinDividertLine('-');

			printCenterText(account->bankName);
			printCenterText(account->clientName);
			printCenterText(account->clientLegalName);
			printCenterText(account->branchPhone);
			printCenterText(account->clientWeb);

			if (transaction->withCustomFields)
			{
				Ibox_Product *product = transaction->customFieldsProduct;
				if (product)
					printKeyValue(product->title, "");

				for (int i = 0; i < transaction->customFieldsCount; i++)
				{
					Ibox_ProductField *field = transaction->customFields[i];
					printKeyValue(field->title, field->value);
				}
			}

			char *dateString = calloc(19, sizeof(char));
			time_t currentTime = time(NULL);
			struct tm currentTm = *localtime(&currentTime);
			struct tm tm;

			strptime(transaction->date, "%Y-%m-%dT%H:%M:%S", &tm);
			tm.tm_isdst = currentTm.tm_isdst;
			tm.tm_gmtoff = currentTm.tm_gmtoff;
			tm.tm_zone = currentTm.tm_zone;

			time_t t = mktime(&tm);
			strftime(dateString, 18, "%d.%m.%y %H:%M:%S", &tm);
			strcat(dateString, "\0");

			printKeyValue("Date and time", dateString);
			printKeyValue("Terminal", account->terminalName);
			printKeyValue("Receipt", transaction->invoice);
			printKeyValue("Approval Code", transaction->approvalCode);
			printKeyValue("RRN", transaction->rrn);

			if (card)
			{
				if (card->iin && card->panMasked)
				{
					int length = strlen(card->iin) + strlen(card->panMasked) + 1;
					char *cardString = calloc(length + 1, sizeof(double));
					strncpy(cardString, card->iin, strlen(card->iin));
					strcat(cardString, " ");
					strncat(cardString, card->panMasked, strlen(card->panMasked));
					strcat(cardString, "\0");
					printKeyValue("Card", cardString);
					free(cardString);
				}
			}

			if (transaction->emvDataCount)
			{
				for (int i = 0; i < transaction->emvDataCount; i++)
				{
					Ibox_EmvTag *emvTag = transaction->emvData[i];
					printKeyValue(emvTag->key, emvTag->value);
				}
			}

			printKeyValue("Operation", transaction->operation);

			char *amountString = createAmountString(transaction->amount, NULL, transaction->currencySignSafe);
			printKeyValue("Total", amountString);
			free(amountString);

			char *feeString = createAmountString(0.0, NULL, transaction->currencySignSafe);
			printKeyValue("Fee", feeString);
			free(feeString);

			printKeyValue("State", "Success");

			if (transaction->inputType == Ibox_Transaction_InputType_EMV || transaction->inputType == Ibox_Transaction_InputType_NFC)
				printCenterText("Confirmed by entering PIN");
			else
			{
				if (requireSignature)
				{
					char *signatureTitle = "Customer sign";
					char *signatureString = calloc(PRINTER_TAPE_WIDTH_CHARS + 1, sizeof(char));
					strncpy(signatureString, signatureTitle, strlen(signatureTitle));
					for (int i = 0; i < PRINTER_TAPE_WIDTH_CHARS - strlen(signatureTitle); i++)
						strcat(signatureString, "_");
					strcat(signatureString, "\0");
					printCenterText(" ");
					printCenterText(" ");
					printCenterText(signatureString);
					free(signatureString);
				}
			}

			free(dateString);
		}
	}
}

void addProductData(Ibox_PaymentContext *paymentContext, Ibox_Result_Authentication *authResult)
{
	if (paymentContext)
	{
		if (authResult && authResult->productsCount)
		{
			Ibox_Product *product = NULL;
			for (int i = 0; i < authResult->productsCount; i++)
			{
				Ibox_Product *localProduct = authResult->products[i];
				if (!localProduct->preparable)
				{
					product = localProduct;
					break;
				}
			}

			if (product && product->fieldsCount)
			{
				int productDataCount = 0;
				Ibox_ProductField **buffer = calloc(product->fieldsCount, sizeof(Ibox_ProductField *));

				for (int i = 0; i < product->fieldsCount; i++)
				{
					Ibox_ProductField *field = product->fields[i];
					if (field->state == Ibox_ProductField_State_ENABLED && field->type == Ibox_ProductField_Type_TEXT)
					{
						if (field->defaultValue)
							field->value = field->defaultValue;
						else
							field->value = PRODUCT_FIELD_TEST_DATA;

						memcpy(buffer + productDataCount, &field, sizeof(Ibox_ProductField *));
						productDataCount++;
					}
				}

				if (productDataCount)
				{
					Ibox_ProductField **productData = calloc(productDataCount, sizeof(Ibox_ProductField *));
					for (int i = 0; i < productDataCount; i++)
					{
						Ibox_ProductField *field = buffer[i];
						memcpy(productData + i, &field, sizeof(Ibox_ProductField *));
					}

					paymentContext->product = product;
					paymentContext->productData = productData;
					paymentContext->productDataCount = productDataCount;
				}

				free(buffer);
			}
		}
		else
		{
			Ibox_Product *product = calloc(1, sizeof(Ibox_Product));
			product->title = "Тестовый продукт 1";
			product->code = "PRODUCT_TEST";

			Ibox_ProductField *field1 = calloc(1, sizeof(Ibox_ProductField));
			field1->type = Ibox_ProductField_Type_TEXT;
			field1->title = "Поле один";
			field1->code = "FIELD_1";
			field1->value = "Тестовое значение";

			Ibox_ProductField *field2 = calloc(1, sizeof(Ibox_ProductField));
			field1->type = Ibox_ProductField_Type_TEXT;
			field2->title = "Поле два";
			field2->code = "FIELD_2";
			field2->value = "Тестовое значение";

			int productDataCount = 2;
			Ibox_ProductField **productData = calloc(productDataCount, sizeof(Ibox_ProductField *));
			memcpy(productData, &field1, sizeof(Ibox_ProductField *));
			memcpy(productData + 1, &field2, sizeof(Ibox_ProductField *));

			paymentContext->product = product;
			paymentContext->productData = productData;
			paymentContext->productDataCount = productDataCount;
		}
	}
}

void addPurchases(Ibox_PaymentContext *paymentContext, Ibox_ReverseContext *reverseContext)
{
	if (paymentContext || reverseContext)
	{
		/*
		Ibox_Purchase *purchase1 = calloc(1, sizeof(Ibox_Purchase));

		Ibox_Tag *tag1 = calloc(1, sizeof(Ibox_Tag));
		tag1->code = "1030";
		tag1->value = "Позиция 1";

		Ibox_Tag *tag2 = calloc(1, sizeof(Ibox_Tag));
		tag2->code = "1079";
		tag2->value = "120.0";

		Ibox_Tag *tag3 = calloc(1, sizeof(Ibox_Tag));
		tag3->code = "1023";
		tag3->value = "2";

		Ibox_Tag *tag4 = calloc(1, sizeof(Ibox_Tag));
		tag4->code = "1199";
		tag4->value = "2";

		int tagsCount = 4;
		Ibox_Tag **tags = calloc(tagsCount, sizeof(Ibox_Tag *));
		memcpy(tags, &tag1, sizeof(Ibox_Tag *));
		memcpy(tags + 1, &tag2, sizeof(Ibox_Tag *));
		memcpy(tags + 2, &tag3, sizeof(Ibox_Tag *));
		memcpy(tags + 3, &tag4, sizeof(Ibox_Tag *));

		purchase1->tagsCount = tagsCount;
		purchase1->tags = tags;

		Ibox_Purchase *purchase2 = calloc(1, sizeof(Ibox_Purchase));
		purchase2->title = "Позиция 2";
		purchase2->price = 100.0;
		purchase2->quantity = 1.2;
		int taxesCount = 1;
		char **taxes = calloc(taxesCount, sizeof(char *));
		char *tax = "VAT1800";
		memcpy(taxes, &tax, sizeof(char *));
		purchase2->taxesCount = taxesCount;
		purchase2->taxes = taxes;

		int purchasesCount = 2;
		Ibox_Purchase **purchases = calloc(purchasesCount, sizeof(Ibox_Purchase *));
		memcpy(purchases, &purchase1, sizeof(Ibox_Purchase *));
		memcpy(purchases + 1, &purchase2, sizeof(Ibox_Purchase *));

		if (paymentContext)
		{
			paymentContext->purchasesCount = purchasesCount;
			paymentContext->purchases = purchases;
		}
		else if (reverseContext)
		{
			reverseContext->purchasesCount = purchasesCount;
			reverseContext->purchases = purchases;
		}
		*/

		//amount = 380
		const char *purchasesJson = "{\"Purchases\": [{\"1030\": \"Позиция 1\", \"1079\": 120, \"1023\": 2, \"1199\": 2}, {\"Title\": \"Позиция 2\", \"Price\": 100, \"Quantity\": 1.4, \"TaxCode\": [\"VAT1800\"]}]}";
		//amount = 190
		//const char *purchasesJson = "{\"Purchases\": [{\"1030\": \"Позиция 1\", \"1079\": 120, \"1023\": 1, \"1199\": 2}, {\"Title\": \"Позиция 2\", \"Price\": 100, \"Quantity\": 0.7, \"TaxCode\": [\"VAT1800\"]}]}";
		if (paymentContext)
			paymentContext->purchasesJson = purchasesJson;
		else if (reverseContext)
			reverseContext->purchasesJson = purchasesJson;
	}
}

void addTags(Ibox_PaymentContext *paymentContext, Ibox_ReverseContext *reverseContext)
{
	if (paymentContext || reverseContext)
	{
		/*
		Ibox_Tag *tag1 = calloc(1, sizeof(Ibox_Tag));
		tag1->code = "1005";
		tag1->value = "Тcет666";


		Ibox_Tag *tag2 = calloc(1, sizeof(Ibox_Tag));
		tag2->code = "1008";
		tag2->value = "email@test.moc";

		int tagsCount = 2;
		Ibox_Tag **tags = calloc(tagsCount, sizeof(Ibox_Tag *));
		memcpy(tags, &tag1, sizeof(Ibox_Tag *));
		memcpy(tags + 1, &tag2, sizeof(Ibox_Tag *));

		if (paymentContext)
		{
			paymentContext->tagsCount = tagsCount;
			paymentContext->tags = tags;
		}
		else if (reverseContext)
		{
			reverseContext->tagsCount = tagsCount;
			reverseContext->tags = tags;
		}
		*/

		const char *tagsJson = "{\"1005\": \"тсет джсон\", \"1008\": \"emailJS@test.moc\"}";
		if (paymentContext)
			paymentContext->tagsJson = tagsJson;
		else if (reverseContext)
			reverseContext->tagsJson = tagsJson;
	}
}

int main(void)
{
	Ibox_Result_Authentication *authResult = NULL;
	Ibox_PaymentController_SetSendWebRequestAction(&sendWebRequest);
	Ibox_PaymentController_SetSendReaderRequestAction(&sendReaderRequest);
	Ibox_PaymentController_SetStartTransactionAction(&startTransactionAction);
	Ibox_PaymentController_SetSelectEmvApplicationAction(&selectEmvApplicationAction);
	Ibox_PaymentController_SetReverseTransactionNotFoundAction(&reverseTransactionNotFoundAction);
	Ibox_PaymentController_SetReverseCancellationTimeoutAction(&reverseCancellationTimeoutAction);
	Ibox_PaymentController_SetCancelCheckCardAction(&cancelCheckCardAction);

	Ibox_PaymentController_SetCredentials(EMAIL, SECRET_KEY);
	Ibox_PaymentController_SetDebugEnabled(DEBUG_ENABLED);
	Ibox_PaymentController_ReaderSoundEnabled(0);
	Ibox_PaymentController_SetNFCOnly(1);

	mTransactionId = NULL;

	fprintf(stderr, "Version:%s\n", Ibox_PaymentController_Version());

	if (TEST_MIFARE)
	{

		char uid[7];

		//POLL CARD

		Ibox_Result_Mifare *result = Ibox_PaymentController_DoMifare(MIFARE_POLL_CARD);
		if (result && !result->errorCode)
		{
			char *suid = bin2hex(result->UID, 7);
			char *data = bin2hex(result->cardData, result->cardDataLen);

			fprintf(stderr, "DoMifare: Cmd=%02d, Status=%02d, CardType=%02d, ATQA=%02d%02d, SAK=%02d, CardUid=%s, CardData=%s\n", 
					result->Cmd,
					result->Status,
					result->CardType,
					result->ATQA[0], result->ATQA[1],
					result->SAK,
					suid,
					data);

			if (suid) free (suid);
			if (data) free (data);

			memcpy(uid, result->UID, 7);
		}
		else
		{
			fprintf(stderr, "DoMifare: %s\n", result->errorMessage);
		}

		ReleaseIboxMifareResult(result);

		// VERIFY CARD
		Ibox_PaymentController_setMifareKeyClass(0x60);
		Ibox_PaymentController_setMifareBlockAddr(0x1C);
		Ibox_PaymentController_setMifareCardUid(uid);
		char kv[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
		Ibox_PaymentController_setMifareKeyValue(kv, 6);

		result = Ibox_PaymentController_DoMifare(MIFARE_VERIFY_CARD);

		if (result && !result->errorCode)
		{
			char *suid = bin2hex(result->UID, 7);
			char *data = bin2hex(result->cardData, result->cardDataLen);

			fprintf(stderr, "DoMifare: Cmd=%02d, Status=%02d, CardType=%02d, ATQA=%02d%02d, SAK=%02d, CardUid=%s, CardData=%s\n", 
					result->Cmd,
					result->Status,
					result->CardType,
					result->ATQA[0], result->ATQA[1],
					result->SAK,
					suid,
					data);

			if (suid) free (suid);
			if (data) free (data);
		}
		else
		{
			fprintf(stderr, "DoMifare: %s\n", result->errorMessage);
		}
		ReleaseIboxMifareResult(result);

		// READ CARD
		Ibox_PaymentController_setMifareBlockAddr(0x1C);
		result = Ibox_PaymentController_DoMifare(MIFARE_READ_CARD);

		if (result && !result->errorCode)
		{
			char *suid = bin2hex(result->UID, 7);
			char *data = bin2hex(result->cardData, result->cardDataLen);

			fprintf(stderr, "DoMifare: Cmd=%02d, Status=%02d, CardType=%02d, ATQA=%02d%02d, SAK=%02d, CardUid=%s, CardData=%s\n", 
					result->Cmd,
					result->Status,
					result->CardType,
					result->ATQA[0], result->ATQA[1],
					result->SAK,
					suid,
					data);

			if (suid) free (suid);
			if (data) free (data);
		}
		else
		{
			fprintf(stderr, "DoMifare: %s\n", result->errorMessage);
		}
		ReleaseIboxMifareResult(result);

		// WRITE CARD
		Ibox_PaymentController_setMifareBlockAddr(0x1C);
		char data[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
		Ibox_PaymentController_setMifareCardData(data, 6);
		result = Ibox_PaymentController_DoMifare(MIFARE_WRITE_CARD);

		if (result && !result->errorCode)
		{
			char *suid = bin2hex(result->UID, 7);
			char *data = bin2hex(result->cardData, result->cardDataLen);

			fprintf(stderr, "DoMifare: Cmd=%02d, Status=%02d, CardType=%02d, ATQA=%02d%02d, SAK=%02d, CardUid=%s, CardData=%s\n", 
					result->Cmd,
					result->Status,
					result->CardType,
					result->ATQA[0], result->ATQA[1],
					result->SAK,
					suid,
					data);

			if (suid) free (suid);
			if (data) free (data);
		}
		else
		{
			fprintf(stderr, "DoMifare: %s\n", result->errorMessage);
		}
		ReleaseIboxMifareResult(result);

	}

	if (GET_READER_INFO)
	{
		fprintf(stderr, "Getting reader info.\n");

		Ibox_Result_ReaderId *readerIdResult = Ibox_PaymentController_ReaderId();
		if (!readerIdResult->errorCode)
		{
			fprintf(stderr, "ReaderId:%s\n", readerIdResult->id);
		}
		else
		{
			fprintf(stderr, "Reader id error.\n");
		}
		ReleaseIboxReaderId(readerIdResult);

		Ibox_Result_ReaderInfo *readerInfoResult = Ibox_PaymentController_ReaderInfo();
		if (!readerInfoResult->errorCode)
		{
			fprintf(stderr, "BootloaderVersion:%s\n", readerInfoResult->bootloaderVersion);
			fprintf(stderr, "FirmwareVersion:%s\n", readerInfoResult->firmwareVersion);
			fprintf(stderr, "HardwareVersion:%s\n", readerInfoResult->hardwareVersion);
		}
		else
		{
			fprintf(stderr, "Reader info error.\n");
		}
		ReleaseIboxReaderInfo(readerInfoResult);
		fprintf(stderr, "\n\n");
	}

	if (AUTH)
	{
		fprintf(stderr, "Authenticating.\n");

		authResult = Ibox_PaymentController_Authentication();
		if (!authResult->errorCode)
		{
			Ibox_Account *account = authResult->account;
			if (account)
			{
				mAccount = account;

				fprintf(stderr, "Account data:\n%s\n", account->name);
				fprintf(stderr, "%s\n", account->clientLegalAddress);
				fprintf(stderr, "%s\n", account->branchPhone);

				if (account->paymentOptionsCount)
				{
					Ibox_PaymentOption *paymentOption = account->paymentOptions[0];
					fprintf(stderr, "First acquirer name: %s\n", paymentOption->acquirer->name);
					fprintf(stderr, "First acquirer code: %s\n", paymentOption->acquirer->code);
				}
			}
		}
		else
		{
			fprintf(stderr, "Authentication error:\n%s\n", authResult->errorMessage);
		}

		fprintf(stderr, "\n\n");
	}

	if (TEST_LINKED_CARD)
	{
		fprintf(stderr, "Linked card payment.\n");

		Ibox_Result_LinkedCards *linkedCardsResult = Ibox_PaymentController_LinkedCards();
		if (!linkedCardsResult->errorCode)
		{
			for (int i = 0; i < linkedCardsResult->linkedCardsCount; i++)
			{
				Ibox_LinkedCard *linkedCard = linkedCardsResult->linkedCards[i];
				fprintf(stderr, "PANMasked: %s\n", linkedCard->panMasked);
				fprintf(stderr, "CardExp: %s\n", linkedCard->cardExpiration);
			}
		}
		ReleaseIboxLinkedCards(linkedCardsResult);

		if (!Ibox_PaymentController_IsReaderConected())
		{
			fprintf(stderr, "Reader disconnected.\n");
		}
		else
		{
			fprintf(stderr, "Link a card.\n");
			Ibox_Result_ReaderCheckCard *readerCheckCardResult = Ibox_PaymentController_GetCardData();
			if (!readerCheckCardResult->errorCode)
			{
				Ibox_Result_AddLinkedCard *addLinkedCardResult = Ibox_PaymentController_AddLinkedCardWithReaderData(readerCheckCardResult);
				if (!addLinkedCardResult->errorCode)
				{
					Ibox_LinkedCard *linkedCard = addLinkedCardResult->linkedCard;
					fprintf(stderr, "Linked card data:\n");

					fprintf(stderr, "CardholderName: %s\n", linkedCard->cardholderName);
					fprintf(stderr, "PANMasked: %s\n", linkedCard->panMasked);
					fprintf(stderr, "CardType: %s\n", linkedCard->cardType);
					fprintf(stderr, "CardExp: %s\n", linkedCard->cardExpiration);

					Ibox_PaymentContext *paymentContext = calloc(1, sizeof(Ibox_PaymentContext));
					paymentContext->inputType = Ibox_PaymentController_InputType_LINKED_CARD;
					paymentContext->amount = AMOUNT;
					paymentContext->description = "Description of transaction";
					paymentContext->linkedCardId = linkedCard->id;
					paymentContext->singleStepAuth = 1;
					paymentContext->receiptEmail = RECEIPT_EMAIL;
					paymentContext->receiptPhone = RECEIPT_PHONE;
					paymentContext->acquirerCode = ACQUIRER_CODE;

					if (TEST_PURCHASES)
						addPurchases(paymentContext, NULL);

					if (TEST_PRODUCT)
						addProductData(paymentContext, authResult);

					Ibox_Result_Submit *submitResult = Ibox_PaymentController_StartPayment(paymentContext);
					if (!submitResult->errorCode)
					{
						fprintf(stderr, "Payment done!\n");
					}
					else
					{
						fprintf(stderr, "Payment error: %s\n", submitResult->errorMessage);
					}

					free(paymentContext);
					ReleaseIboxSubmitResult(submitResult);

					Ibox_Result *removeLinkedCardResult = Ibox_PaymentController_RemoveLinkedCard(linkedCard->id);
					if (!removeLinkedCardResult->errorCode)
					{
						fprintf(stderr, "Remove linked card done!\n");
					}
					else
					{
						fprintf(stderr, "Remove linked card error: %s\n", removeLinkedCardResult->errorMessage);
					}
					ReleaseIboxResult(removeLinkedCardResult);
				}
				else
				{
					fprintf(stderr, "Add linked card error:\n%s\n", addLinkedCardResult->errorMessage);
				}
				ReleaseIboxAddLinkedCardResult(addLinkedCardResult);
			}
			else
			{
				fprintf(stderr, "Reader check card error:\n%s\n", readerCheckCardResult->errorMessage);
			}
			ReleaseIboxReaderCheckCardResult(readerCheckCardResult);
		}

		fprintf(stderr, "\n\n");
	}

	if (TEST_CARD_PAYMENT)
	{
		fprintf(stderr, "Card payment.\n");

		Ibox_PaymentContext *paymentContext = calloc(1, sizeof(Ibox_PaymentContext));
		paymentContext->inputType = Ibox_PaymentController_InputType_CARD;
		paymentContext->currencyType = Ibox_PaymentController_CurrencyType_RUB;
		paymentContext->amount = AMOUNT;
		paymentContext->description = "Linux test app";
		paymentContext->singleStepAuth = 1;
		paymentContext->receiptEmail = RECEIPT_EMAIL;
		paymentContext->receiptPhone = RECEIPT_PHONE;
		paymentContext->acquirerCode = ACQUIRER_CODE;

		if (TEST_TAGS)
			addTags(paymentContext, NULL);

		if (TEST_PURCHASES)
			addPurchases(paymentContext, NULL);

		if (TEST_PRODUCT)
			addProductData(paymentContext, authResult);

		Ibox_Result_Submit *submitResult = Ibox_PaymentController_StartPayment(paymentContext);
		if (!submitResult->errorCode)
		{
			fprintf(stderr, "Payment done!\n");
			printSlip(submitResult->transaction, mAccount, submitResult->requireSignature);
		}
		else
			fprintf(stderr, "Payment error: %s\n", submitResult->errorMessage);

		ReleaseIboxSubmitResult(submitResult);

		if (paymentContext->product)
			free(paymentContext->product);

		if (paymentContext->productDataCount > 0)
		{
			for (int i = 0; i < paymentContext->productDataCount; i++)
				free(paymentContext->productData[i]);
			free(paymentContext->productData);
		}
		free(paymentContext);
		fprintf(stderr, "\n");
	}

	if (TEST_REVERSE)
	{
		const char *transactionId = NULL;
		if (TEST_REVERSE_LAST_TRANSACTION && mTransactionId)
			transactionId = mTransactionId;
		else
			transactionId = TRANSACTION_ID;

		if (transactionId)
		{
			Ibox_ReverseContext *reverseContext = calloc(1, sizeof(Ibox_ReverseContext));
			reverseContext->transactionId = transactionId;
			reverseContext->amountReverse = AMOUNT_REVERSE;
			reverseContext->forceReturn = 0;
			reverseContext->purchasesCount = 0;
			reverseContext->purchases = NULL;
			reverseContext->purchasesJson = NULL;

			if (TEST_PURCHASES)
				addPurchases(NULL, reverseContext);

			if (TEST_TAGS)
				addTags(NULL, reverseContext);

			Ibox_Result_Reverse *reverseResult = Ibox_PaymentController_StartReverse(reverseContext);
			if (!reverseResult->errorCode)
				fprintf(stderr, "Reverse done!\n");
			else
				fprintf(stderr, "Reverse error: %s\n", reverseResult->errorMessage);

			ReleaseIboxReverseResult(reverseResult);
			free(reverseContext);
		}
	}

	if (TEST_SCHEDULE)
	{
		Ibox_ScheduleContext *scheduleContext = calloc(1, sizeof(Ibox_ScheduleContext));
		scheduleContext->currencyType = Ibox_PaymentController_CurrencyType_RUB;
		scheduleContext->amount = AMOUNT;
		scheduleContext->description = "Description of schedule";
		scheduleContext->product = NULL;
		scheduleContext->productData = NULL;
		scheduleContext->productDataCount = 0;
		scheduleContext->receiptPhone = NULL;
		scheduleContext->receiptEmail = NULL;

		scheduleContext->scheduleType = Ibox_Schedule_Type_WEEKLY;
		scheduleContext->dayWeek = 1;
		scheduleContext->day = 28;
		scheduleContext->startDate = "2018-09-28";
		scheduleContext->scheduleEndType = Ibox_Schedule_EndType_COUNT;
		scheduleContext->dates = NULL;
		scheduleContext->datesCount = 0;
		scheduleContext->endCount = 1;

		Ibox_Result_ScheduleSubmit *scheduleSubmitResult = Ibox_PaymentController_StartSchedule(scheduleContext);
		if (!scheduleSubmitResult->errorCode)
			fprintf(stderr, "Schedule done!\n");
		else
			fprintf(stderr, "Schedule error: %s\n", scheduleSubmitResult->errorMessage);

		//TODO: Release schedules
		free(scheduleSubmitResult);
		free(scheduleContext);
	}

	if (TEST_TRANSACTION)
	{
		Ibox_Result_Transactions *transactionResult = Ibox_PaymentController_Transaction(TRANSACTION_ID);
		if (!transactionResult->errorCode)
		{
			Ibox_Transaction *transaction = NULL;
			if (transactionResult->transactionsCount)
				transaction = transactionResult->transactions[0];
			if (transaction)
			{
				fprintf(stderr, "Transaction invoice: %s\n", transaction->invoice);
				fprintf(stderr, "Transaction state: %d\n", transaction->state);
				fprintf(stderr, "Transaction substate: %d\n", transaction->substate);

				Ibox_FiscalInfo *fiscalInfo = transaction->fiscalInfo;
				fprintf(stderr, "Fiscal printer serial number: %s\n", fiscalInfo->printerSerialNumber);
				fprintf(stderr, "Fiscal date time: %s\n", fiscalInfo->dateTime);

				Ibox_Card *card = transaction->card;
				fprintf(stderr, "Card iin: %s\n", card->iin);
				fprintf(stderr, "Card pan masked: %s\n", card->panMasked);
				fprintf(stderr, "Card expiration: %s\n", card->expiration);

				if (transaction->emvDataCount)
				{
					fprintf(stderr, "Emv data:\n");
					for (int i = 0; i < transaction->emvDataCount; i++)
					{
						Ibox_EmvTag *emvTag = transaction->emvData[i];
						fprintf(stderr, "%s - %s\n", emvTag->key, emvTag->value);
					}
				}

				if (transaction->withPurchases)
				{
					fprintf(stderr, "Purchases count: %d\n", transaction->purchasesCount);
					for (int i = 0; i < transaction->purchasesCount; i++)
					{
						Ibox_Purchase *purchase = transaction->purchases[i];
						fprintf(stderr, "%s - %f - %d\n", purchase->title, purchase->price, purchase->taxesCount);

						if (purchase->taxesCount)
						{
							fprintf(stderr, "Taxes:\n");
							for (int j = 0; j < purchase->taxesCount; j++)
								fprintf(stderr, "%s\n", purchase->taxes[j]);
						}
					}
				}

				if (transaction->withTags)
				{
					fprintf(stderr, "Tags count: %d\n", transaction->tagsCount);
					for (int i = 0; i < transaction->tagsCount; i++)
					{
						Ibox_Tag *tag = transaction->tags[i];
						fprintf(stderr, "%s - %s\n", tag->code, tag->value);
					}
				}

				if (transaction->withCustomFields)
				{
					fprintf(stderr, "Custom fields count: %d\n", transaction->customFieldsCount);
					for (int i = 0; i < transaction->customFieldsCount; i++)
					{
						Ibox_ProductField *field = transaction->customFields[i];
						fprintf(stderr, "%s - %s\n", field->title, field->value);
					}
				}

				printSlip(transaction, mAccount, 1);

				// ReleaseIboxTransaction(transaction);
			}
		}

		ReleaseIboxTransactions(transactionResult);
	}

	if (TEST_FISCALIZE)
	{
		Ibox_Result *fiscalizeResult = Ibox_PaymentController_Fiscalize(TRANSACTION_ID);
		if (!fiscalizeResult->errorCode)
		{
			fprintf(stderr, "Fiscalization done.");
		}

		ReleaseIboxResult(fiscalizeResult);
	}

	if (TEST_PREPARE)
	{
		if (authResult && authResult->productsCount)
		{
			Ibox_Product *product = NULL;
			for (int i = 0; i < authResult->productsCount; i++)
			{
				Ibox_Product *localProduct = authResult->products[i];
				if (localProduct->preparable)
				{
					product = localProduct;
					break;
				}
			}

			if (product && product->fieldsCount)
			{
				Ibox_ProductField **productPrepareData = calloc(1, sizeof(Ibox_ProductField *));
				Ibox_ProductField *field = product->preparableField;
				if (field->defaultValue)
					field->value = field->defaultValue;
				else
					field->value = PRODUCT_FIELD_TEST_DATA;
				memcpy(productPrepareData, &field, sizeof(Ibox_ProductField *));

				Ibox_Result_Prepare *prepareResult = Ibox_PaymentController_Prepare(product, productPrepareData, 1);
				if (!prepareResult->errorCode)
				{
					Ibox_PaymentContext *paymentContext = calloc(1, sizeof(Ibox_PaymentContext));
					paymentContext->inputType = Ibox_PaymentController_InputType_CARD;
					paymentContext->currencyType = Ibox_PaymentController_CurrencyType_RUB;
					paymentContext->amount = prepareResult->amount;
					paymentContext->receiptEmail = RECEIPT_EMAIL;
					paymentContext->receiptPhone = RECEIPT_PHONE;
					paymentContext->acquirerCode = ACQUIRER_CODE;

					if (prepareResult->fieldsCount)
					{
						int productDataCount = 0;
						Ibox_ProductField **buffer = calloc(product->fieldsCount, sizeof(Ibox_ProductField *));

						for (int i = 0; i < product->fieldsCount; i++)
						{
							Ibox_ProductField *field = product->fields[i];
							for (int i = 0; i < prepareResult->fieldsCount; i++)
							{
								Ibox_ProductField *prepareField = prepareResult->fields[i];
								if (!strcmp(field->code, prepareField->code))
								{
									field->value = prepareField->value;
									memcpy(buffer + productDataCount, &field, sizeof(Ibox_ProductField *));
									productDataCount++;
									break;
								}
							}
						}

						if (productDataCount)
						{
							Ibox_ProductField **productData = calloc(productDataCount, sizeof(Ibox_ProductField *));
							for (int i = 0; i < productDataCount; i++)
							{
								Ibox_ProductField *field = buffer[i];
								memcpy(productData + i, &field, sizeof(Ibox_ProductField *));
							}

							paymentContext->product = product;
							paymentContext->productData = productData;
							paymentContext->productDataCount = productDataCount;

							Ibox_Result_Submit *submitResult = Ibox_PaymentController_StartPayment(paymentContext);
							if (!submitResult->errorCode)
								fprintf(stderr, "Payment done!\n");
							else
								fprintf(stderr, "Payment error: %s\n", submitResult->errorMessage);
						}

						free(buffer);
					}
				}
				else
					fprintf(stderr, "Prepare error: %s\n", prepareResult->errorMessage);

				//TODO: release preparable
				free(prepareResult);
				free(productPrepareData);
			}
		}
	}

	ReleaseIboxAuthenticationResult(authResult);
	//ReleaseIboxAccount(mAccount);
	if (mTransactionId)
		free(mTransactionId);
	Ibox_PaymentController_Cleanup();
	return 0;
}
