#include <stdio.h>

#include "hw.h"
#include "SDK/PaymentController.h"

#define DEBUG_ENABLED 					1
#define AUTH                    		1
#define GET_READER_INFO         		0
#define TEST_LINKED_CARD 				0
#define TEST_CARD_PAYMENT 				1
#define TEST_SCHEDULE					0
#define TEST_REVERSE					0
#define TEST_PURCHASES					0
#define TEST_PRODUCT					0
#define TEST_TAGS						0
#define TEST_TRANSACTION				0
#define TEST_FISCALIZE					0
#define TEST_PREPARE					0
#define TEST_REVERSE_LAST_TRANSACTION 	0

#define EMAIL            				""
#define SECRET_KEY					""
#define AMOUNT						1.0
#define AMOUNT_REVERSE					0.0
#define TRANSACTION_ID					"2f809619-d26f-48f2-a523-8a5044224400"
#define RECEIPT_EMAIL					"test@mail.de"
#define RECEIPT_PHONE					"+73211232312"
#define ACQUIRER_CODE					NULL		//"ACQ_CODE"
#define PRODUCT_FIELD_TEST_DATA			"field data"

// member variables
const char *mTransactionId = NULL;

void startTransactionAction(const char *transactionId)
{
	mTransactionId = transactionId;
	fprintf(stderr, "TransactionID:%s\n", transactionId);
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
						if (field->defaultValue )
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
			field1->title = "Поле один";
			field1->code = "FIELD_1";
			field1->value = "Тестовое значение";

			Ibox_ProductField *field2 = calloc(1, sizeof(Ibox_ProductField));
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
	Ibox_PaymentController_SetCredentials(EMAIL, SECRET_KEY);
	Ibox_PaymentController_SetDebugEnabled(DEBUG_ENABLED);
	Ibox_PaymentController_ReaderSoundEnabled(1);

	mTransactionId = NULL;

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

		authResult = Ibox_PaymentController_Authentication();
		if (!authResult->errorCode)
		{
			Ibox_Account *account = authResult->account;
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
		paymentContext->description = "Description of transaction";
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
			fprintf(stderr, "Payment done!\n");
		else
			fprintf(stderr, "Payment error: %s\n", submitResult->errorMessage);

		free(submitResult);
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

			free(reverseResult);
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
			}
		}
	}

	if (TEST_FISCALIZE)
	{
		Ibox_Result *fiscalizeResult = Ibox_PaymentController_Fiscalize(TRANSACTION_ID);
		if (!fiscalizeResult->errorCode)
		{
			fprintf(stderr, "Fiscalization done.");
		}
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

				free(prepareResult);
				free(productPrepareData);
			}
		}
	}

	if (authResult)
		free(authResult);

	return 0;
}
