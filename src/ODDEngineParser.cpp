//
// Project: ODD Engine
// Description: Header file for the Database class, which manages domain objects and their properties.
//
// This file is part of the ODD Engine and was developed in the Automated Driving Project 
// of the Fahrzeugsoftwarelabor at MASCOR Institute of FH Aachen - University of Applied Sciences.
//
// Maintainer: Moritz Rumpf, Joschua Schulte-Tigges, Till Voss
// 

#include "../include/ODDEngine.h"

ODDEngine::ODDEngine() {}

int ODDEngine::get_guardrail_height(const std::string& target){
    for(auto iter = guardrailMap.begin(); iter != guardrailMap.end(); iter++){
        int key = iter->first;
        std::map<std::string, Guardrail>& map = iter->second;

        if(map.find(target) != map.end()){
            return key;
        }
    }
    return -1;
}

LogicBlockType::value ODDEngine::get_constraint_operation_type(YAML::Node& input){
    if (input.Scalar() == "WHEN AND"){
        return LogicBlockType::WHEN_AND;
    }
    if (input.Scalar() == "WHEN OR"){
        return LogicBlockType::WHEN_OR;
    }
    if (input.Scalar() == "EXCEPT WHEN EITHER"){
        return LogicBlockType::EXCEPT_WHEN_EITHER;

    }
    return LogicBlockType::EMPTY;
}

void ODDEngine::parse_variable(const std::string& key1guardrailName,
                      const YAML::Node& value1guardrailNode){

    DataType::value dataType;
    if(value1guardrailNode.begin()->first.Scalar() == "bool")
    {
        dataType = DataType::BOOL;
    }
    else if(value1guardrailNode.begin()->first.Scalar() == "double")
    {
        dataType = DataType::DOUBLE;
    }
    else if(value1guardrailNode.begin()->first.Scalar() == "int")
    {
        dataType = DataType::INT;
    }
    else
    {
        throw std::out_of_range("Type " + value1guardrailNode.begin()->first.Scalar() + " not supported");
    }

    expressionContainer.add_expression(value1guardrailNode.begin()->second.Scalar(), key1guardrailName, variableTable, dataType);
}

int ODDEngine::parse_guardrail_or_restriction(const std::string& key1guardrailName,
                    const YAML::Node& value1guardrailNode, bool isRestriction){

    int height = get_guardrail_height(key1guardrailName);
    if (height != -1){
        throw std::out_of_range(" already exists");
    }

    Guardrail newGuardrail;
    newGuardrail.isRestriction = isRestriction;

    // iterate through Title, Constraints
    for (auto it = value1guardrailNode.begin(); it != value1guardrailNode.end(); ++it) {
        auto key2statementInGuardrail = it->first;
        auto value2NodeInGuardrail = it->second;

        // Title
        if (key2statementInGuardrail.Scalar() == "TITLE") {
            newGuardrail.title = it->second.Scalar(); // Set title of Guardrail
        }
        // TARGET
        if (key2statementInGuardrail.Scalar() == "TARGET")
        {
            newGuardrail.target = it->second.Scalar(); // Set Target of Guardrail
        }
        // Constraints
        else if (get_constraint_operation_type(key2statementInGuardrail) != LogicBlockType::EMPTY) {

            LogicBlock logicBlock;
            logicBlock.operationType = get_constraint_operation_type(key2statementInGuardrail);

            // iterate through conditions
            for (auto it = value2NodeInGuardrail.begin(); it != value2NodeInGuardrail.end(); ++it) {
                auto key3conditionName = it->first;
                auto value3conditionValue = it->second;

                // check if illegal keyword  is used
                if( std::find(KEYWORD_LIST.begin(), KEYWORD_LIST.end(),key3conditionName.Scalar()) != KEYWORD_LIST.end() ){
                    throw std::out_of_range(key3conditionName.Scalar() + " is not allowed here");
                }

                // check if operand is part of Variables
                auto pair = this->variableTable.find_variable(key3conditionName.Scalar());

                std::vector<std::string> list;
                if(value3conditionValue.IsScalar()) {
                    list.emplace_back(value3conditionValue.Scalar());
                }
                else {
                    for (auto entry: value3conditionValue) {
                        list.emplace_back(entry.Scalar());
                    }
                }

                if (pair.first==DataType::BOOL) {
                    logicBlock.add_bool_condition(key3conditionName.Scalar(), list,
                                    this->variableTable.get_bool_ptr(key3conditionName.Scalar()));
                }
                else if (pair.first==DataType::OBJECT) {
                    logicBlock.add_object_condition(key3conditionName.Scalar(), list,
                                    this->variableTable.get_domain_obj_shared_ptr(key3conditionName.Scalar()));
                }
                else {
                    throw std::out_of_range("Datatype " + std::to_string(pair.first) + " not supported");
                }

                // Set Guardrail height
                try{
                    int dependencyGuardrailHeight = get_guardrail_height(key3conditionName.Scalar());
                    if(dependencyGuardrailHeight > height){
                        height = dependencyGuardrailHeight;
                    }
                }
                catch (const std::out_of_range& e){}
            }
            newGuardrail.logicBlocks.emplace_back(logicBlock);
        }
        else {
            throw std::out_of_range("[ERROR] undeclared identifier: " + key2statementInGuardrail.Scalar() + "\nDid you forget Constraint (WHEN AND, WHEN OR, EXCEPT WHEN EITHER) ?");
        }
    }
    std::cout << "" << std::endl;
    newGuardrail.height = height+1;
    guardrailMap[newGuardrail.height][key1guardrailName] = newGuardrail;

    return newGuardrail.height;
}

// recursion
void ODDEngine::parse_odd(const std::string& filename){
    YAML::Node config = YAML::LoadFile(filename);

    std::cout << "--- Parse Guardlines ---------------" << std::endl;
    if (config.Type() == YAML::NodeType::Map) {

        bool importStillAllowed = true;

        // iterate through Keyword
        for (auto keywordPair = config.begin(); keywordPair != config.end(); keywordPair++) {
            auto value0content = keywordPair->second;

            // Parse Import
            if (keywordPair->first.Scalar() == "import") {
                if (!importStillAllowed) {
                    std::cout << "[ERROR] import is not allowed here" << std::endl;
                    throw std::out_of_range("import is not allowed here");
                }

                if (!keywordPair->second.IsSequence()){
                    std::cout << "[ERROR] import value is no Sequence" << std::endl;
                    throw std::out_of_range("import value is no Sequence");
                }


                // iterate through all import file
                for (YAML::detail::iterator_value fileIter: keywordPair->second){
                    std::string importFilepath;
                    std::string importType = filename.substr(filename.find_last_of('.') );
                    const std::string& importFilename = fileIter.Scalar();

                    // check for supported file types
                    if (importType != ".yaml"){
                        throw std::out_of_range(importType + " is not supported");
                    }

                    // check if filename is just a file in same folder or a relative or absolute path
                    if (importFilename.find('/') == std::string::npos){ // filename only contains filename => file should be in same folder
                        importFilepath = filename.substr(0, filename.find_last_of("\\/")) + "/" + fileIter.Scalar();
                    }
                    else{ // filename contains '/' - char, so it should be a relative or absolute path
                        importFilepath = fileIter.Scalar();
                    }

                    std::cout << "import: " << importFilepath << "\n" << std::endl;

                    parse_odd(importFilepath);
                }
            }

            // Parse Default Namespace
            else if (keywordPair->first.Scalar() == "DNAMESPACE")
            {
                std::string namespaceString = keywordPair->second.Scalar();
                if(namespaceString.empty())
                {
                    dnamespace="";
                }
                else if (namespaceString.find('#') == std::string::npos)
                {
                    throw std::out_of_range("Namespace wrong / # missing");
                }


                dnamespace=namespaceString;

                std::cout << "Default Namespace: " << dnamespace << std::endl;
            }

            // Parse Domain specific Objects
            else if (keywordPair->first.Scalar() == "OBJECT")
            {
                std::cout << "\n" << keywordPair->first.Scalar() << std::endl;
                for(auto it = value0content.begin(); it != value0content.end(); ++it)
                {
                    std::cout << it->first << "  " << it->second.Scalar() << std::endl;
                    if (it->second.Scalar().empty())
                    {
                        throw std::out_of_range("Variable " + it->first.Scalar() + " doesn't have a domain type");
                    }
                    if(it->second.Scalar().find('#')==std::string::npos)
                    {
                        variableTable.init_object(it->first.Scalar(), dnamespace + it->second.Scalar(), ontology);
                    }
                    else{
                        variableTable.init_object(it->first.Scalar(), it->second.Scalar(), ontology);
                    }
                }
            }

            // Parse Measurements
            else if(keywordPair->first.Scalar() == "VARIABLE"){
                std::cout << "\n" << keywordPair->first.Scalar() << std::endl;
                // iterate through Measurements
                for (auto it = value0content.begin(); it != value0content.end(); ++it) {
                    auto key1measurementName = it->first;
                    auto value1measurementNode = it->second;

                    parse_variable(key1measurementName.Scalar(), value1measurementNode);
                }
            }

            // Parse Guardrails
            else if ( keywordPair->first.Scalar() == "RULE") {
                std::cout << "\n" <<keywordPair->first.Scalar() << std::endl;
                importStillAllowed = false;
                auto tmp = value0content;
                auto tmp_iter = value0content.begin();

                // iterate through Guardrails
                for (auto it = tmp.begin(); it != tmp.end(); ++it) {
                    auto key1guardrailName = it->first;
                    auto value1guardrailNode = it->second;

                    variableTable.init_variable(key1guardrailName.Scalar(), DataType::BOOL);
                    parse_guardrail_or_restriction(key1guardrailName.Scalar(), value1guardrailNode, false);
                }
            }
            // Parse Guardrails
            else if (keywordPair->first.Scalar() == "RESTRICTION") {
                std::cout << "\n" <<keywordPair->first.Scalar() << std::endl;
                importStillAllowed = false;
                auto tmp = value0content;
                auto tmp_iter = value0content.begin();


                // iterate through Guardrails
                for (auto it = tmp.begin(); it != tmp.end(); ++it) {
                    auto key1guardrailName = it->first;
                    auto value1guardrailNode = it->second;

                    variableTable.init_variable(key1guardrailName.Scalar(), DataType::BOOL);
                    parse_guardrail_or_restriction(key1guardrailName.Scalar(), value1guardrailNode, true);
                }
            }

            else{
                std::cout << "[ERROR] undeclared Keyword: " << keywordPair->first << std::endl;
            }
        }
    }
    else{
        std::cout << "YAML - Node is no map" << std::endl;
    }
    std::cout << "-------------------" << std::endl;
}

void ODDEngine::parse_ontology(const std::string& filename) {
    ontology=create_ontology(parse_rdf_file(filename));
}
