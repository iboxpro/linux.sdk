#ifndef RELEASEOBJECTS_H
#define RELEASEOBJECTS_H
#include "PaymentController.h"
#include "WebObject.h"

void ReleaseIboxAuthenticationResult(Ibox_Result_Authentication *result);
void ReleaseIboxAccount(Ibox_Account *account);
void ReleaseIboxSubmitResult(Ibox_Result_Submit *result);
void ReleaseIboxReaderInfo(Ibox_Result_ReaderInfo *info);
void ReleaseIboxReaderId(Ibox_Result_ReaderId *id);
void ReleaseIboxLinkedCards(Ibox_Result_LinkedCards *cards);
void ReleaseIboxAddLinkedCardResult(Ibox_Result_AddLinkedCard *result);
void ReleaseIboxResult(Ibox_Result *result);
void ReleaseIboxReaderCheckCardResult(Ibox_Result_ReaderCheckCard *result);
void ReleaseIboxLinkedCard(Ibox_LinkedCard *card);
void ReleaseIboxTransaction(Ibox_Transaction *transaction);
void ReleaseIboxTransactions(Ibox_Result_Transactions *transactions);
void ReleaseIboxReverseResult(Ibox_Result_Reverse *result);
void ReleaseIboxMifareResult(Ibox_Result_Mifare *result);
#endif 
