//
// Project: ODD Engine
// Description: Header file for the Database class, which manages domain objects and their properties.
//
// This file is part of the ODD Engine and was developed in the Automated Driving Project 
// of the Fahrzeugsoftwarelabor at MASCOR Institute of FH Aachen - University of Applied Sciences.
//
// Maintainer: Moritz Rumpf, Joschua Schulte-Tigges, Till Voss
// 

#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "Database.h"
#include "shunting-yard.h"


struct Token{
    explicit Token(const std::string& token, bool operation = false){
        this->token = token;
        this->isOperation = operation;
    }

    std::string token;
    stringType::value type;
private:
    bool isOperation;
};

const std::vector<std::string> parenthesis{"(",")"};

enum ExpressionOperationType {No_op, Func, And, Or, Equal, Nequal, Less, Great, LessEqual, GreatEqual, Add, Sub, Mul, Div, Pow};

const std::unordered_map<std::string, std::pair<int, ExpressionOperationType>> operatorMap{
                                                  {"&&",{0, And}},{"||",{0, Or}},
                                                  {"==", {1, Equal}}, {"!=", {1, Nequal}}, {"<",{1, Less}}, {">",{1, Great}}, {">=",{1,LessEqual}},{"<=",{1,GreatEqual}},
                                                  {"+",{2,Add}}, {"-",{2,Sub}},
                                                  {"*",{3,Mul}}, {"/",{3,Div}},
                                                  {"^",{4,Pow}}/*, {"!",4}*/};


class Expression
{
public:
    explicit Expression(const std::string& expression, const std::string& resultName, const std::shared_ptr<bool>& ptr)
    {
        this->expressionString = expression;
        this->calc = cparse::calculator(expression.c_str());
        this->resultName=resultName;
        this->dataType = DataType::BOOL;
        this->boolResultPointer = ptr;
    }

    explicit Expression(const std::string& expression, const std::string& resultName, const std::shared_ptr<int>& ptr)
    {
        this->expressionString = expression;
        this->calc = cparse::calculator(expression.c_str());
        this->resultName=resultName;
        this->dataType = DataType::INT;
        this->intResultPointer = ptr;
    }

    explicit Expression(const std::string& expression, const std::string& resultName, const std::shared_ptr<float>& ptr)
    {
        this->expressionString = expression;
        this->calc = cparse::calculator(expression.c_str());
        this->resultName=resultName;
        this->dataType = DataType::FLOAT;
        this->floatResultPointer = ptr;
    }

    explicit Expression(const std::string& expression, const std::string& resultName, const std::shared_ptr<double>& ptr)
    {
        this->expressionString = expression;
        this->calc = cparse::calculator(expression.c_str());
        this->resultName=resultName;
        this->dataType = DataType::DOUBLE;
        this->doubleResultPointer = ptr;
    }

    /*
     * runs the Calculation of the expression with the given TokenMap tm and returns the result
     */
    void calculate(const cparse::TokenMap& tm) const;

    const std::string& get_expression_string() const { return expressionString; }
    const std::string& get_result_name() const { return resultName; }
private:
    std::string resultName;
    DataType::value dataType;
    std::weak_ptr<bool> boolResultPointer;
    std::weak_ptr<int> intResultPointer;
    std::weak_ptr<float> floatResultPointer;
    std::weak_ptr<double> doubleResultPointer;
    cparse::calculator calc;
    std::string expressionString;
};

/*
 * The ExpressionContainer holds a List of Expressions and the associated TokenMap.
 * It also stores every Pointer to the Values that are used for the Expression.
 * The Update - Function updates every Token in the TokenMap with the Value.
 */
class ExpressionContainer
{
public:
    // Main Functions
    void add_expression(const std::string& expressionString, const std::string& resultName, Database& variableTable, DataType::value dataType);

    /*
     * updates the TokenMap with all Values that are necessary for this specific Expression
     */
    void update();

    /*
     * triggers the Calculation of every Expression
     * Every Expression stores the result directly in the associated resultPointer
     */
    void calculate_all();

    // Helper Functions
    static stringType::value get_string_type(const std::string& s);

    /*
    * Method splits a string into a std::vector of tokens.
    * Operators are taken from the operatorMap.
    */
   
    static std::vector<Token> split_string_in_token_list(std::string string);

    Expression get_expression(const std::string& resultName) const
    {
        for (const Expression& expression : expressionList)
        {
            if (expression.get_result_name() == resultName)
            {
                return expression;
            }
        }
        throw std::out_of_range("Expression with name " + resultName + " not found");
    }

    std::vector<std::string> get_dependencies_for_key(const std::string& key);



private:
    // Attribute
    cparse::TokenMap tokenMap;
    std::vector<Expression> expressionList;

    std::vector<std::weak_ptr<bool>>   boolVariables;
    std::vector<std::weak_ptr<int>>    intVariables;
    std::vector<std::weak_ptr<float>>  floatVariables;
    std::vector<std::weak_ptr<double>> doubleVariables;
    std::unordered_map<std::string, std::pair<DataType::value, int>> indexMap;
};
#endif //EXPRESSION_H
