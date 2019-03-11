// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.
//
// This is a GENERATED FILE, changes made here WILL BE LOST.
//

#include <jni.h>
#include <stdio.h>
#include <string.h>

#include <TrustWalletCore/TWBinanceSigner.h>

#include "TWJNI.h"
#include "BinanceSigner.h"

jbyteArray JNICALL Java_wallet_core_jni_BinanceSigner_sign(JNIEnv *env, jclass thisClass, jobject input) {
    jclass inputClass = (*env)->GetObjectClass(env, input);
    jmethodID inputToByteArrayMethodID = (*env)->GetMethodID(env, inputClass, "toByteArray", "()[B");
    jbyteArray inputByteArray = (*env)->CallObjectMethod(env, input, inputToByteArrayMethodID);
    TWData *inputData = TWDataCreateWithJByteArray(env, inputByteArray);
    jbyteArray result = TWDataJByteArray(TWBinanceSignerSign(inputData), env);

    (*env)->DeleteLocalRef(env, inputByteArray);
    (*env)->DeleteLocalRef(env, inputClass);

    return result;
}

