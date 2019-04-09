#pragma once

#include "TWBase.h"
#include "TWData.h"
#include "TWPrivateKey.h"

#include "TWSteemTransaction.h"

TW_EXTERN_C_BEGIN

/// Represents a Steem Signer.
TW_EXPORT_CLASS
struct TWSteemSigner;

TW_EXPORT_STATIC_METHOD
struct TWSteemSigner *_Nonnull TWSteemSignerCreate(TWData *_Nonnull chainID);

TW_EXPORT_METHOD
void TWSteemSignerSign(struct TWSteemSigner *_Nonnull signer, struct TWPrivateKey *_Nonnull privateKey, struct TWSteemTransaction *_Nonnull transaction);

TW_EXPORT_METHOD
void TWSteemSignerDelete(struct TWSteemSigner *_Nonnull signer);

TW_EXTERN_C_END