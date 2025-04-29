//
// Project: ODD Engine
// Description: Header file for the Database class, which manages domain objects and their properties.
//
// This file is part of the ODD Engine and was developed in the Automated Driving Project 
// of the Fahrzeugsoftwarelabor at MASCOR Institute of FH Aachen - University of Applied Sciences.
//
// Maintainer: Moritz Rumpf, Joschua Schulte-Tigges, Till Voss
// 

#ifndef ODD_ENGINE_GUARDRAIL_TYPES_H
#define ODD_ENGINE_GUARDRAIL_TYPES_H

#include <Database.h>
#include <utility>
#include <vector>
#include <string>
#include <memory>

class Database;

class  Condition{
protected:
    virtual ~Condition() = default;

public:
    virtual bool check_condition()=0;

    std::string conditionName;
};

class  BoolCondition final : public Condition {
    bool check_condition() override ;
public:
    bool conditionValue;
    std::weak_ptr<bool> valuePointer;
};

class ObjectCondition final : public Condition {
    bool check_condition() override;
public:
    std::vector<std::string> conditionValues;
    std::weak_ptr<Database::DomainObject> valuePointer;
};


struct LogicBlockType{
    enum value {EMPTY, WHEN_AND, WHEN_OR, EXCEPT_WHEN_EITHER};
};

struct LogicBlock{
    void add_bool_condition(const std::string& conditionName, const std::vector<std::string>& conditionValues, std::weak_ptr<bool> databasePointer);

    void add_object_condition(const std::string& conditionName, const std::vector<std::string>& conditionValues, std::weak_ptr<Database::DomainObject> databasePointer);


    LogicBlockType::value operationType = LogicBlockType::EMPTY;

    // ConditionType
    std::vector<std::shared_ptr<Condition>> conditions;
};

struct Guardrail{
    std::string title;
    int height = -1;
    bool isRestriction = false;
    std::string target;

    std::vector<LogicBlock> logicBlocks;
    std::vector<Guardrail*> dependencyGuardrails;
};

#endif //ODD_ENGINE_GUARDRAIL_TYPES_H
