#include "parser.hpp"

#include <rapidsjon/document.h>

#include <iostream>
#include <cstdio>

namespace nm {

namespace {

//std::vector<std::string> split(const std::string& input, const std::string& regex) {
//    // passing -1 as the submatch index parameter performs splitting
//    std::sregex_token_iterator
//        first{input.begin(), input.end(), regex, -1},
//        last;
//    return {first, last};
//}
optional<SignalType> parseSignalType(const rapidjson::Value &signalTypeValue)
{
    if(!signalTypeValue.IsString()){
        return{};
    }
    int dimensionality{-1};
    if(sscanf(signalTypeValue.GetString(), "%df", &dimensionality) == EOF){
        std::cerr << "Couldn't parse signal type";
        return {};
    }
    return {SignalType{dimensionality}};
}

optional<std::unique_ptr<Module> > parseModule(const rapidjson::Value &moduleValue, const nm::TypeManager &typeManager)
{
    if(!moduleValue["name"].IsString()){
        return {};
    }
    if(!moduleValue["type"].IsString()){
        return {};
    }
    std::string typeString = moduleValue["type"].GetString();
    std::string nameString = moduleValue["name"].GetString();
    auto type = typeManager.getType(typeString);
    if(type == nullptr){
        std::cerr << "Unknown type: " <<  typeString << std::endl;
        return {};
    }
    std::unique_ptr<Module> module = std::unique_ptr<Module>(new Module(*type, nameString));
    return {std::move(module)};
}

optional<std::unique_ptr<CompositeModuleType> > parseModuleType(const rapidjson::Value &type, const TypeManager& typeManager)
{
    auto &nameValue = type["name"];
    if(!nameValue.IsString()){
        std::cerr << "Module missing required field \"name\"\n";
        return {};
    }
    std::string moduleTypeName = nameValue.GetString();
    std::string description;
    auto &descriptionValue = type["description"];
    if(descriptionValue.IsString()){
        description = descriptionValue.GetString();
    }

    auto moduleType = std::unique_ptr<CompositeModuleType>{new CompositeModuleType(moduleTypeName, description)};

    //parse Inputs
    auto &inputsValue = type["inputs"];
    if(!inputsValue.IsNull()){
        if(!inputsValue.IsArray()){
            std::cerr << "inputs is not an array\n";
            return {};
        }
        for(rapidjson::SizeType i = 0; i < inputsValue.Size(); i++){
            auto &inputValue = inputsValue[i];
            std::string name = inputValue["name"].GetString();
            auto maybeSignalType = parseSignalType(inputValue["type"]);
            if(!maybeSignalType){
                return {};
            }
            moduleType->addInput(name, *maybeSignalType);
        }
    }

    //parse submodules
    auto &modulesValue = type["modules"];
    if(!modulesValue.IsNull()){
        if(!modulesValue.IsArray()){
            std::cerr << "modules is not an array\n";
            return {};
        }
        for(rapidjson::SizeType i = 0; i<modulesValue.Size(); i++){
            auto maybeModule = parseModule(modulesValue[i], typeManager);
            if(!maybeModule){
                std::cerr << "Couldn't parse Module\n";
                return {};
            }
            moduleType->addModule(std::move(*maybeModule));
        }
    }


    return {std::move(moduleType)};
}

bool parseModuleTypeArray(const rapidjson::Value &array, TypeManager &typeManager)
{
    if(!array.IsArray()){
        std::cerr << "Tried to parse an array that wasn't an array";
        return {};
    }
    std::map <std::string, std::unique_ptr<ModuleType>> moduleTypes{};
    for(rapidjson::SizeType i = 0; i < array.Size(); i++){
        auto maybeModuleType = parseModuleType(array[i], typeManager);
        if(!maybeModuleType){
            return false;
        }
        std::unique_ptr<CompositeModuleType> &moduleTypePtr = *maybeModuleType;
        if(!typeManager.addUserType(std::move(moduleTypePtr))){
            std::cerr << "Couldn't add type to typeManager.\n";
            return false;
        }
    }
    return true;
}

} // namespace

Parser::Parser()
{
}

optional<std::unique_ptr<TypeManager> > Parser::parseDocument(std::string input)
{
    rapidjson::Document document;
    document.Parse<0>(input.c_str());

    if(document.HasParseError()){
        std::cerr << "Couldn't parse json document\n";
        return {};
    }

    auto &moduleTypes = document["moduleTypes"];

    if(moduleTypes.IsNull()){
        std::cerr << "moduleTypes not present in root of document\n";
        return {};
    }

    std::unique_ptr<TypeManager> typeManager{new TypeManager{}};
    typeManager->initBuiltinTypes();
    if(!parseModuleTypeArray(moduleTypes, *(typeManager.get()))){
        return {};
    }
    return {std::move(typeManager)};
}

} // namespace nm
