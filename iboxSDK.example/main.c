#include <stdio.h>

#include "hw.h"
#include "SDK/PaymentController.h"

#define DEBUG_ENABLED 			1
#define AUTH                    	1
#define GET_READER_INFO         	1
#define TEST_LINKED_CARD 		0
#define TEST_CARD_PAYMENT 		1
#define TEST_PURCHASES			0
#define TEST_PRODUCT			0

#define EMAIL            		""
#define SECRET_KEY			""

int main(void)
{
	Ibox_PaymentController_SetSendWebRequestAction(&sendWebRequest);
	Ibox_PaymentController_SetSendReaderRequestAction(&sendReaderRequest);
	Ibox_PaymentController_SetCredentials(EMAIL, SECRET_KEY);
	Ibox_PaymentController_SetDebugEnabled(DEBUG_ENABLED);
	Ibox_PaymentController_ReaderSoundEnabled(1);

	fprintf(stderr, "Version:%s\n", Ibox_PaymentController_Version());

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
		free(readerIdResult);

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
		free(readerInfoResult);
		fprintf(stderr, "\n\n");
	}

	if (AUTH)
	{
		fprintf(stderr, "Authenticating.\n");

		Ibox_Result_Authentication *authenticationResul = Ibox_PaymentController_Authentication();
		if (!authenticationResul->errorCode)
		{
			Ibox_Account *account = authenticationResul->account;
			if (account)
			{
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
			fprintf(stderr, "Authentication error:\n%s\n", authenticationResul->errorMessage);
		}

		free(authenticationResul);

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

				Ibox_PaymentContext *paymentContext = malloc(sizeof(Ibox_PaymentContext));
				paymentContext->inputType = Ibox_PaymentController_InputType_LINKED_CARD;
				paymentContext->amount = 1.0;
				paymentContext->description = "Description of transaction";
				paymentContext->linkedCardId = linkedCard->id;
				paymentContext->purchasesCount = 0;
				paymentContext->purchases = NULL;
				paymentContext->product = NULL;
				paymentContext->productData = NULL;
				paymentContext->productDataCount = 0;
				paymentContext->acquirer = NULL;

				if (TEST_PURCHASES)
				{
					Ibox_Purchase *purchase1 = malloc(sizeof(Ibox_Purchase));
					purchase1->title = "Позиция 1";
					purchase1->price = 120.0;
					purchase1->quantity = 2.5;
					int taxes1Count = 1;
					char **taxes1 = calloc(taxes1Count, sizeof(char *));
					char *tax = "VAT1800";
					memcpy(taxes1, &tax, sizeof(char *));
					purchase1->taxesCount = taxes1Count;
					purchase1->taxes = taxes1;

					Ibox_Purchase *purchase2 = malloc(sizeof(Ibox_Purchase));
					purchase2->title = "Позиция 2";
					purchase2->price = 100.0;
					purchase2->quantity = 1.2;
					purchase2->taxesCount = 0;
					purchase2->taxes = NULL;

					int purchasesCount = 2;
					Ibox_Purchase **purchases = calloc(purchasesCount, sizeof(Ibox_Purchase *));
					memcpy(purchases, &purchase1, sizeof(Ibox_Purchase *));
					memcpy(purchases + 1, &purchase2, sizeof(Ibox_Purchase *));

					paymentContext->purchasesCount = purchasesCount;
					paymentContext->purchases = purchases;
				}

				if (TEST_PRODUCT)
				{
					Ibox_Product *product = malloc(sizeof(Ibox_Product));
					product->title = "Тестовый продукт 1";
					product->code = "PRODUCT_TEST";

					Ibox_ProductField *field1 = malloc(sizeof(Ibox_ProductField));
					field1->title = "Поле один";
					field1->code = "FIELD_1";
					field1->value = "Тестовое значение";

					Ibox_ProductField *field2 = malloc(sizeof(Ibox_ProductField));
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

				Ibox_Result_Submit *submitResult = Ibox_PaymentController_StartPayment(paymentContext);
				if (!submitResult->errorCode)
				{
					fprintf(stderr, "Payment done!\n");
				}
				else
				{
					fprintf(stderr, "Payment error: %s\n", submitResult->errorMessage);
				}

				free(submitResult);
				free(paymentContext);

				Ibox_Result *removeLinkedCardResult = Ibox_PaymentController_RemoveLinkedCard(linkedCard->id);
				if (!removeLinkedCardResult->errorCode)
				{
					fprintf(stderr, "Remove linked card done!\n");
				}
				else
				{
					fprintf(stderr, "Remove linked card error: %s\n", removeLinkedCardResult->errorMessage);
				}
				free(removeLinkedCardResult);
			}
			else
			{
				fprintf(stderr, "Add linked card error:\n%s\n", addLinkedCardResult->errorMessage);
			}
		}
		else
		{
			fprintf(stderr, "Reader check card error:\n%s\n", readerCheckCardResult->errorMessage);
		}
		free(readerCheckCardResult);

		fprintf(stderr, "\n\n");

	}

	if (TEST_CARD_PAYMENT)
	{
		fprintf(stderr, "Card payment.\n");

		const char *transactionId = NULL;
		Ibox_PaymentContext *paymentContext = malloc(sizeof(Ibox_PaymentContext));
		paymentContext->inputType = Ibox_PaymentController_InputType_CARD;
		paymentContext->amount = 42.0;
		paymentContext->description = "Description of transaction";
		paymentContext->linkedCardId = 0;
		paymentContext->purchasesCount = 0;
		paymentContext->purchases = NULL;
		paymentContext->product = NULL;
		paymentContext->productData = NULL;
		paymentContext->productDataCount = 0;
		paymentContext->acquirer = NULL;

		if (TEST_PURCHASES)
		{
			Ibox_Purchase *purchase1 = malloc(sizeof(Ibox_Purchase));
			purchase1->title = "Позиция 1";
			purchase1->price = 120.0;
			purchase1->quantity = 2.5;
			int taxes1Count = 1;
			char **taxes1 = calloc(taxes1Count, sizeof(char *));
			char *tax = "VAT1800";
			memcpy(taxes1, &tax, sizeof(char *));
			purchase1->taxesCount = taxes1Count;
			purchase1->taxes = taxes1;

			Ibox_Purchase *purchase2 = malloc(sizeof(Ibox_Purchase));
			purchase2->title = "Позиция 2";
			purchase2->price = 100.0;
			purchase2->quantity = 1.2;
			purchase2->taxesCount = 0;
			purchase2->taxes = NULL;

			int purchasesCount = 2;
			Ibox_Purchase **purchases = calloc(purchasesCount, sizeof(Ibox_Purchase *));
			memcpy(purchases, &purchase1, sizeof(Ibox_Purchase *));
			memcpy(purchases + 1, &purchase2, sizeof(Ibox_Purchase *));

			paymentContext->purchasesCount = purchasesCount;
			paymentContext->purchases = purchases;
		}

		if (TEST_PRODUCT)
		{
			Ibox_Product *product = malloc(sizeof(Ibox_Product));
			product->title = "Тестовый продукт 1";
			product->code = "PRODUCT_TEST";

			Ibox_ProductField *field1 = malloc(sizeof(Ibox_ProductField));
			field1->title = "Поле один";
			field1->code = "FIELD_1";
			field1->value = "Тестовое значение";

			Ibox_ProductField *field2 = malloc(sizeof(Ibox_ProductField));
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

		Ibox_Result_Submit *submitResult = Ibox_PaymentController_StartPayment(paymentContext);
		if (!submitResult->errorCode)
		{
			transactionId = submitResult->transaction->id;
			fprintf(stderr, "Payment done!\n");
		}
		else
		{
			fprintf(stderr, "Payment error: %s\n", submitResult->errorMessage);
		}

		free(submitResult);
		free(paymentContext);

		if (transactionId)
		{
			Ibox_ReverseContext *reverseContext = malloc(sizeof(Ibox_ReverseContext));
			reverseContext->transactionId = transactionId;
			reverseContext->amountReverse = 0.0f;

			Ibox_Result_Reverse *reverseResult = Ibox_PaymentController_StartReverse(reverseContext);
			if (!reverseResult->errorCode)
			{
				fprintf(stderr, "Reverse done!\n");
			}
			else
			{
				fprintf(stderr, "Reverse error: %s\n", reverseResult->errorMessage);
			}

			free(reverseResult);
			free(reverseContext);
		}

		fprintf(stderr, "\n\n");
	}

	return 0;
}
