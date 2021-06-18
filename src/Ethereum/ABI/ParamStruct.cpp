// Copyright © 2017-2021 Trust Wallet.
//
// This file is part of Trust. The full Trust copyright notice, including
// terms governing use, modification, and redistribution, is contained in the
// file LICENSE at the root of the source code distribution tree.

#include "ParamStruct.h"
#include "ValueEncoder.h"
#include "ParamFactory.h"
#include "ParamAddress.h"
#include <Hash.h>
#include <HexCoding.h>

#include <nlohmann/json.hpp>

#include <cassert>
#include <string>

using namespace TW::Ethereum::ABI;
using namespace TW;
using json = nlohmann::json;

std::string ParamNamed::getType() const {
    return _param->getType() + " " + _name;
}

ParamSetNamed::~ParamSetNamed() {
    _params.clear();
}

/// Returns the index of the parameter
int ParamSetNamed::addParam(const std::shared_ptr<ParamNamed>& param) {
    assert(param.get() != nullptr);
    if (param.get() == nullptr) {
        return -1;
    }
    _params.push_back(param);
    return static_cast<int>(_params.size() - 1);
}

void ParamSetNamed::addParams(const std::vector<std::shared_ptr<ParamNamed>>& params) {
    for (auto p : params) {
        addParam(p);
    }
}

std::string ParamSetNamed::getType() const {
    std::string t = "(";
    int cnt = 0;
    for (auto p : _params) {
        if (cnt++ > 0) {
            t += ",";
        }
        t += p->getType();
    }
    t += ")";
    return t;
}

Data ParamSetNamed::encodeHashes() const {
    Data hashes;
    for (auto p: _params) {
        append(hashes, p->hashStruct());
    }
    return hashes;
}

std::string ParamSetNamed::getExtraTypes(std::vector<std::string>& ignoreList) const {
    std::string types;
    for (auto& p: _params) {
        auto pType = p->_param->getType();
        if (std::find(ignoreList.begin(), ignoreList.end(), pType) == ignoreList.end()) {
            types += p->getExtraTypes(ignoreList);
            ignoreList.push_back(pType);
        }
    }
    return types;
}

std::shared_ptr<ParamNamed> ParamSetNamed::findParamByName(const std::string& name) const {
    for (auto& p: _params) {
        if (p->_name == name) {
            return p;
        }
    }
    return nullptr;
}

Data ParamStruct::hashType() const {
    return Hash::keccak256(TW::data(encodeType()));
}

Data ParamStruct::encodeHashes() const {
    Data hashes;
    Data paramsHashes = _params.encodeHashes();
    if (paramsHashes.size() > 0) {
        auto fullType = encodeType();
        hashes = Hash::keccak256(TW::data(fullType));
        append(hashes, paramsHashes);
    }
    return hashes;
}

Data ParamStruct::hashStruct() const {
    Data hash(32);
    Data hashes = encodeHashes();
    if (hashes.size() > 0) {
        hash = Hash::keccak256(hashes);
    }
    return hash;
}

std::string ParamStruct::getExtraTypes(std::vector<std::string>& ignoreList) const {
    std::string types;
    if (std::find(ignoreList.begin(), ignoreList.end(), _name) == ignoreList.end()) {
        types += _name + _params.getType();
        ignoreList.push_back(_name);
    }
    types += _params.getExtraTypes(ignoreList);
    return types;
}

Data ParamStruct::hashStructJson(const std::string& structType, const std::string& valueJson, const std::string& typesJson) {
    auto str = makeStruct(structType, valueJson, typesJson);
    assert(str);
    return str->hashStruct();
}

std::shared_ptr<ParamStruct> findType(const std::string& typeName, const std::vector<std::shared_ptr<ParamStruct>>& types) {
    for (auto& t: types) {
        if (t->getType() == typeName) {
            return t;
        }
    }
    return nullptr;
}

std::shared_ptr<ParamStruct> ParamStruct::makeStruct(const std::string& structType, const std::string& valueJson, const std::string& typesJson) {
    try {
        // parse types
        auto types = makeTypes(typesJson);
        // find type info
        auto typeInfo = findType(structType, types);
        if (!typeInfo) {
            throw std::invalid_argument("Type not found, " + structType);
        }
        auto values = json::parse(valueJson, nullptr, false);
        if (values.is_discarded()) {
            throw std::invalid_argument("Could not parse value Json");
        }
        if (!values.is_object()) {
            throw std::invalid_argument("Expecting object");
        }
        std::vector<std::shared_ptr<ParamNamed>> params;
        const auto& typeParams = typeInfo->getParams();
        // iterate through the type; order is important and field order in the value json is not defined
        for (int i = 0; i < typeParams.getCount(); ++i) {
            auto name = typeParams.getParam(i)->getName();
            auto type = typeParams.getParam(i)->getParam()->getType();
            // look for it in value (may throw)
            auto value = values[name];
            // first try simple params
            auto paramVal = ParamFactory::make(type);
            if (paramVal) {
                if (!values.is_null()) {
                    std::string valueString = (!value.is_array() && !value.is_object()) ? value.get<std::string>() : value.dump();
                    if (!paramVal->setValueJson(valueString)) {
                        throw std::invalid_argument("Could not set type for param " + name);
                    }
                }
                params.push_back(std::make_shared<ParamNamed>(name, paramVal));
            } else if (type.length() >= 2 && type.substr(type.length() - 2, 2) == "[]") {
                // array of struct
                auto arrayType = type.substr(0, type.length() - 2);
                auto subTypeInfo = findType(arrayType, types);
                if (!subTypeInfo) {
                    throw std::invalid_argument("Could not find type for array sub-struct " + arrayType);
                }
                if (!value.is_array()) {
                    throw std::invalid_argument("Value must be array for type " + type);
                }
                std::vector<std::shared_ptr<ParamBase>> paramsArray;
                for (const auto& e: value) {
                    auto subStruct = makeStruct(arrayType, e.dump(), typesJson);
                    if (!subStruct) {
                        throw std::invalid_argument("Could not process array sub-struct " + arrayType + " " + e.dump());
                    }
                    assert(subStruct);
                    paramsArray.push_back(subStruct);
                }
                params.push_back(std::make_shared<ParamNamed>(name, std::make_shared<ParamArray>(paramsArray)));
            } else {
                // try if sub struct
                auto subTypeInfo = findType(type, types);
                if (!subTypeInfo) {
                    throw std::invalid_argument("Could not find type for sub-struct " + type);
                }
                if (value.is_null()) {
                    params.push_back(std::make_shared<ParamNamed>(name, std::make_shared<ParamStruct>(type, std::vector<std::shared_ptr<ParamNamed>>{})));
                } else {
                    auto subStruct = makeStruct(type, value.dump(), typesJson);
                    if (!subStruct) {
                        throw std::invalid_argument("Could not process sub-struct " + type);
                    }
                    assert(subStruct);
                    params.push_back(std::make_shared<ParamNamed>(name, subStruct));
                }
            }
        }
        return std::make_shared<ParamStruct>(structType, params);
    } catch (const std::invalid_argument& ex) {
        throw;
    } catch (...) {
        throw std::invalid_argument("Could not process Json");
    }
}

std::shared_ptr<ParamStruct> ParamStruct::makeType(const std::string& structName, const std::string& structJson, const std::vector<std::shared_ptr<ParamStruct>>& extraTypes) {
    try {
        if (structName.empty()) {
            throw std::invalid_argument("Missing type name");
        }
        auto jsonValue = json::parse(structJson, nullptr, false);
        if (jsonValue.is_discarded()) {
            throw std::invalid_argument("Could not parse type Json");
        }
        if (!jsonValue.is_array()) {
            throw std::invalid_argument("Expecting array");
        }
        std::vector<std::shared_ptr<ParamNamed>> params;
        for(auto& p2: jsonValue) {
            auto name = p2["name"].get<std::string>();
            auto type = p2["type"].get<std::string>();
            if (name.empty() || type.empty()) {
                throw std::invalid_argument("Expecting 'name' and 'type', in " + structName);
            }
            auto named = ParamFactory::makeNamed(name, type);
            if (named) {
                // simple type
                params.push_back(named);
            } else if (type == structName) {
                // recursive to self
                params.push_back(std::make_shared<ParamNamed>(name, std::make_shared<ParamStruct>(type, std::vector<std::shared_ptr<ParamNamed>>{})));
            } else if (type.length() >= 2 && type.substr(type.length() - 2, 2) == "[]") {
                // array of struct
                auto arrayType = type.substr(0, type.length() - 2);
                // try array struct from extra types
                auto p2struct = findType(arrayType, extraTypes);
                if (!p2struct) {
                    throw std::invalid_argument("Unknown struct array type " + arrayType);
                }
                params.push_back(std::make_shared<ParamNamed>(name, std::make_shared<ParamArray>(p2struct)));
            } else {
                // try struct from extra types
                auto p2struct = findType(type, extraTypes);
                if (!p2struct) {
                    throw std::invalid_argument("Unknown type " + type);
                }
                params.push_back(std::make_shared<ParamNamed>(name, p2struct));
            }
        }
        if (params.size() == 0) {
            throw std::invalid_argument("No valid params found");
        }
        return std::make_shared<ParamStruct>(structName, params);
    } catch (const std::invalid_argument& ex) {
        throw;
    } catch (...) {
        throw std::invalid_argument("Could not process Json");
    }
}

std::vector<std::shared_ptr<ParamStruct>> ParamStruct::makeTypes(const std::string& structTypes) {
    try {
        std::vector<std::shared_ptr<ParamStruct>> types;
        auto jsonValue = json::parse(structTypes, nullptr, false);
        if (jsonValue.is_discarded()) {
            throw std::invalid_argument("Could not parse types Json");
        }
        if (!jsonValue.is_array()) {
            throw std::invalid_argument("Expecting array");
        }
        for (auto& t: jsonValue) {
            if (t.is_object()) {
                // take first elem
                for (json::iterator it = t.begin(); it != t.end(); ) {
                    // may throw
                    auto struct1 = makeType(it.key(), it.value().dump(), types);
                    types.push_back(struct1);
                    break;
                }
            }
        }
        return types;
    } catch (std::exception& ex) {
        throw;
    } catch (...) {
        throw std::invalid_argument("Could not process Json");
    }
}