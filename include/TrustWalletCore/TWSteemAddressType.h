#pragma once

#include "TWBase.h"

TW_EXTERN_C_BEGIN

TW_EXPORT_ENUM(uint32_t)
enum TWSteemAddressType {
    TWSteemAddressTypeMainNet = 0,
    TWSteemAddressTypeTestNet = 1,
};

TW_EXTERN_C_END