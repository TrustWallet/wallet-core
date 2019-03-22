// Copyright © 2017-2019 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#pragma once

#include <array>
#include <string>
#include <vector>
#include <stdexcept>

#include <boost/any.hpp>

#include "BinaryCoding.h"

namespace TW {
namespace Ontology {

class ParamsBuilder {

private:

    std::vector<uint8_t> bytes;

public:

    std::vector<uint8_t> getBytes() {
        return bytes;
    }

    void cleanUp() {
        bytes.clear();
    }

    static void buildNeoVmParam(ParamsBuilder &builder, const boost::any &param);

    static void buildNeoVmParam(ParamsBuilder &builder, const std::string &param);

    static void buildNeoVmParam(ParamsBuilder &builder, const std::array<uint8_t, 20> &param);

    static void buildNeoVmParam(ParamsBuilder &builder, const std::vector<uint8_t> &param);

    void pushVar(const std::vector<uint8_t> &data);

    template<typename T>
    void pushVar(T data);

    void push(const std::string &data);

    void push(const std::array<uint8_t, 20> &data);

    void push(const std::vector<uint8_t> &data);

    void push(uint64_t num);

    void push(uint8_t num);

    void pushBack(uint8_t data);

    void pushBack(uint32_t data);

    void pushBack(uint64_t data);

    void pushBack(const std::string &data);

    void pushBack(const std::array<uint8_t, 20> &data);

    template<typename T>
    void pushBack(const std::vector<T> &data);

    static std::vector<uint8_t> buildNativeInvokeCode(const std::vector<uint8_t> &contractAddress, uint8_t version, const std::string &method, const boost::any &params);

};

}} // namespace