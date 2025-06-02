//
// Project: ODD Engine
// Description: Header file for the Database class, which manages domain objects and their properties.
//
// This file is part of the ODD Engine and was developed in the Automated Driving Project 
// of the Fahrzeugsoftwarelabor at MASCOR Institute of FH Aachen - University of Applied Sciences.
//
// Maintainer: Moritz Rumpf, Joschua Schulte-Tigges, Till Voss
// 

#include "../include/Expression.h"

#include <Database.h>


// Expression ----------------------------------------------------------
void Expression::calculate( cparse::TokenMap& tm) const
{
    
    if(dataType==DataType::BOOL)
    {
        if(auto lockedPointer= boolResultPointer.lock())
        {
            *lockedPointer=calc.eval(tm).asBool();
            tm[resultName] = *lockedPointer;
        }
    }
    else if(dataType==DataType::INT)
    {
        if(auto lockedPointer= intResultPointer.lock())
        {
            *lockedPointer=calc.eval(tm).asInt();
            tm[resultName] = *lockedPointer;
        }
    }
    else if(dataType==DataType::FLOAT)
    {
        if(auto lockedPointer= floatResultPointer.lock())
        {
            *lockedPointer=calc.eval(tm).asDouble();
            tm[resultName] = *lockedPointer;
        }
    }
    else if(dataType==DataType::DOUBLE)
    {
        if(auto lockedPointer= doubleResultPointer.lock())
        {
            *lockedPointer=calc.eval(tm).asDouble();
            tm[resultName] = *lockedPointer;
        }
    }
    else //Should never happen
    {
        throw std::logic_error("Expression::calculate impossible Error");
    }
}



// ExpressionContainer -------------------------------------------------

// Main Functions
void ExpressionContainer::add_expression(const std::string& expressionString, const std::string& resultName, Database& variableTable, DataType::value dataType)
{
    if(variableTable.find_variable_without_error(resultName).first != DataType::ERROR)
    {
        throw std::logic_error("Identifier " + resultName + " is already in Database");
    }

    // initalise variable of the expression
    variableTable.init_variable(resultName, dataType);

    // refinedExpression for filtering points
    std::string refinedExpression;
    std::vector<Token> tokenList = split_string_in_token_list(expressionString);
    for(Token& token: tokenList)
    {
        if(token.type == stringType::Var)
        {
            auto indexIter = this->indexMap.find(token.token);
            if(this->indexMap.find(token.token)!=this->indexMap.end())
            {
                std::cout << token.token << " already in pairList " << std::endl;
                refinedExpression+=token.token;
                continue;
            }

            std::weak_ptr<double> ptr;
            std::string str = token.token;
            str.erase(std::remove(str.begin(), str.end(), '.'), str.end());
            token.token = str;

            size_t pos;
            std::pair<DataType::value, int> pair;
            try{
                pair = variableTable.find_variable(str);
            }catch (std::out_of_range& e){
                throw std::out_of_range("Variable " + str + " not found in Database, did you forget to add it?");
            }
            if(pair.first == DataType::BOOL)
            {
                pos = boolVariables.size();
                boolVariables.emplace_back(variableTable.boolVariable.at(pair.second));
            }
            else if(pair.first == DataType::INT)
            {
                pos = intVariables.size();
                intVariables.emplace_back(variableTable.intVariable.at(pair.second));

            }
            else if(pair.first == DataType::FLOAT)
            {
                pos = floatVariables.size();
                floatVariables.emplace_back(variableTable.floatVariable.at(pair.second));
            }
            else if(pair.first == DataType::DOUBLE)
            {
                pos = doubleVariables.size();
                doubleVariables.emplace_back(variableTable.doubleVariable.at(pair.second));
            }
            else
            {
                throw std::invalid_argument("Datatype " + std::to_string(pair.first) + " not supported for Expression");
            }
            this->indexMap[str] = {pair.first, pos};
            std::cout << str << " now in pairList " << std::endl;
        }
        refinedExpression+=token.token;
    }

    // create expression with refined Expression
    std::cout << "Expression: " << refinedExpression << std::endl;
    if(dataType==DataType::BOOL)
    {
        expressionList.emplace_back(refinedExpression, resultName, variableTable.get_bool_ptr(resultName));
    }
    else if(dataType==DataType::INT)
    {
        expressionList.emplace_back(refinedExpression, resultName, variableTable.get_int_ptr(resultName));
    }
    else if(dataType==DataType::FLOAT)
    {
        expressionList.emplace_back(refinedExpression, resultName, variableTable.get_float_ptr(resultName));
    }
    else if(dataType==DataType::DOUBLE)
    {
        expressionList.emplace_back(refinedExpression, resultName, variableTable.get_double_ptr(resultName));
    }
    else
    {
        throw std::invalid_argument("Datatype " + std::to_string(dataType) + " not supported");
    }

    update();
}

void ExpressionContainer::update()
{
    for(const auto& pair: this->indexMap)
    {
        if(pair.second.first == DataType::BOOL)
        {
            if(auto lockedPtr = this->boolVariables.at(pair.second.second).lock())
            {
                tokenMap[pair.first] = *lockedPtr;
            }
        }
        else if(pair.second.first == DataType::INT)
        {
            if(auto lockedPtr = this->intVariables.at(pair.second.second).lock())
            {
                tokenMap[pair.first] = *lockedPtr;
            }
        }
        else if(pair.second.first == DataType::FLOAT)
        {
            if(auto lockedPtr = this->floatVariables.at(pair.second.second).lock())
            {
                tokenMap[pair.first] = *lockedPtr;
            }
        }
        else if(pair.second.first == DataType::DOUBLE)
        {
            if(auto lockedPtr = this->doubleVariables.at(pair.second.second).lock())
            {
                tokenMap[pair.first] = *lockedPtr;
            }
        }
    }
}

void ExpressionContainer::calculate_all()
{
    update();
    for (const Expression& expression: expressionList)
    {
        expression.calculate(tokenMap);
    }
}

// Helper Functions
stringType::value ExpressionContainer::get_string_type(const std::string& s){
    std::string::const_iterator it = s.begin();
    int couldBeFloat = -1;
    bool charWasThere = false;
    bool numberWasThere=false;

    if(s == "true" || s == "false"){
        return stringType::Bool;
    }
    
    while (it != s.end() ) {
        if(*it == '.'){
            if(charWasThere) {
                return stringType::Var;
            }
            if(numberWasThere){
                couldBeFloat++;
            }
            else{
                break;
            }
            if(couldBeFloat>0){
                break;
            }
        }
        else if (std::isdigit(*it) ){
            numberWasThere = true;

        }
        else if((int)*it > 64 && (int)*it < 91 || (int)*it == 95  || (int)*it > 96 && (int)*it < 123){
            charWasThere= true;
        }
        else{
            break;
        }
        ++it;
    }
    if(!s.empty() && it == s.end()){
        if(charWasThere){
            return stringType::Var;
        }
        if(couldBeFloat == 0){
            return stringType::Float;
        }
        return stringType::Int;

    }
    return stringType::NaN;
}

std::vector<Token> ExpressionContainer::split_string_in_token_list(std::string string) {
    std::vector<Token> tokenList;

    for(auto iter = string.begin(); iter != string.end(); ++iter){
        if(*iter == ' '){
            string.erase(iter);
            --iter;
        }
    }

    tokenList.emplace_back(string);
    std::vector<Token> newTokenList;

    std::vector<std::string>  delimiterList = parenthesis;
    for (auto operatorIter = operatorMap.begin(); operatorIter != operatorMap.end(); ++operatorIter)
    {
        delimiterList.emplace_back(operatorIter->first);
    }

    for (const std::string & delimiter: delimiterList) {
        for (Token token: tokenList) {
            if(token.token.length() < 2){
                newTokenList.emplace_back(token);
                continue;
            }
            size_t pos = 0;
            while ((pos = token.token.find(delimiter)) != std::string::npos) {
                std::string substr = token.token.substr(0, pos);
                if(!substr.empty()) {
                    newTokenList.emplace_back(substr);
                }

                Token delimiterToken(delimiter, true);
                newTokenList.emplace_back(delimiterToken);
                token.token.erase(0, pos + delimiter.length());
            }
            if (!token.token.empty()) {
                newTokenList.emplace_back(token);
            }
        }
        tokenList = newTokenList;
        newTokenList.clear();
    }

    for (Token& token: tokenList)
    {
        token.type = get_string_type(token.token);
        if(token.type == stringType::Bool){
            token.token = token.token == "true" ? "1" : "0";
        }
    }
    return tokenList;
}

std::vector<std::string> ExpressionContainer::get_dependencies_for_key(const std::string& key) {
    std::vector<std::string> dependencies;
    try
    {
        Expression expression = get_expression(key);
        std::string expr = expression.get_expression_string();
        std::vector<Token> tokens = split_string_in_token_list(expr);
        
        bool dependsOnKey = false;
        for (const Token& token : tokens) {
            if (token.type == stringType::Var) {
                dependencies.push_back(token.token);
            }
        }

        return dependencies;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return dependencies;
    }
    

    
}