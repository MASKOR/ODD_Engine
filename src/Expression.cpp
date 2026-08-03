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
#include <algorithm>


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
            std::string str = token.token;
            // cparse cannot handle '.' inside variable names,
            // so use an underscore placeholder in the expression and token map
            std::string identifier = str;
            std::replace(identifier.begin(), identifier.end(), '.', '_');

            if(this->indexMap.find(identifier)!=this->indexMap.end())
            {
                std::cout << str << " already in pairList " << std::endl;
                refinedExpression+=identifier;
                continue;
            }

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
            this->indexMap[identifier] = {pair.first, pos};
            std::cout << str << " now in pairList " << std::endl;
            token.token = identifier;
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
    bool numberWasThere = false;
    bool dotWasThere = false;
    bool charBeforeDot = false;
    bool charAfterDot = false;

    if(s == "true" || s == "false"){
        return stringType::Bool;
    }
    
    while (it != s.end() ) {
        if(*it == '.'){
            dotWasThere = true;
            // Check if there was a character before the dot
            if(charWasThere) {
                charBeforeDot = true;
                // Check if there's a character after the dot for variable pattern (e.g., object.property)
                std::string::const_iterator next_it = it + 1;
                if(next_it != s.end() && 
                   ((int)*next_it > 64 && (int)*next_it < 91 || 
                    (int)*next_it == 95 || 
                    (int)*next_it > 96 && (int)*next_it < 123)) {
                    charAfterDot = true;
                    return stringType::Var;  // Definitely a variable like object.property
                }
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
            // If we had chars before, this is still a variable (e.g., var1, var2)
            if(charWasThere) {
                // Continue, could still be a variable
            }
        }
        else if((int)*it > 64 && (int)*it < 91 || (int)*it == 95  || (int)*it > 96 && (int)*it < 123){
            charWasThere = true;
            // If we're after a dot and we have chars, it's definitely a variable
            if(dotWasThere && charBeforeDot) {
                charAfterDot = true;
            }
        }
        else{
            break;
        }
        ++it;
    }
    if(!s.empty() && it == s.end()){
        // If we have pattern like "object.property", it's a variable
        if(charBeforeDot && charAfterDot && dotWasThere) {
            return stringType::Var;
        }
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