//
// Project: ODD Engine
// Description: Header file for the Database class, which manages domain objects and their properties.
//
// This file is part of the ODD Engine and was developed in the Automated Driving Project 
// of the Fahrzeugsoftwarelabor at MASCOR Institute of FH Aachen - University of Applied Sciences.
//
// Maintainer: Moritz Rumpf, Joschua Schulte-Tigges, Till Voss
// 

#include "Guardrail.h"


bool BoolCondition::check_condition()  {
    if(auto lockedPointer = valuePointer.lock()) {
        if(conditionValue == *lockedPointer) {
            return true;
        }
        return false;
    }
    throw std::runtime_error("valuePointer was deleted");
};

bool ObjectCondition::check_condition() {
    if(auto lockedPointer = valuePointer.lock()) {
        for(const auto& conditionValue: conditionValues) {
            std::string result;
            if(lockedPointer->get_value() == nullptr) {
                result = "none";
            }else {
                result = lockedPointer->get_value()->id;
            }
            if(conditionValue == result) {
                return true;
            }
        }
        return false;
    }
    throw std::runtime_error("valuePointer was deleted");
}



void LogicBlock::add_bool_condition(const std::string& conditionName, const std::vector<std::string>& conditionValues,
    std::weak_ptr<bool> databasePointer) {
    auto pointer = std::make_shared<BoolCondition>();
    pointer->conditionName=conditionName;
    pointer->valuePointer=std::move(databasePointer);
    if (conditionValues.size()>1) {
        throw std::runtime_error("Bool Values cant be in a List");
    }
    if(conditionValues[0] == "true") {
        pointer->conditionValue = true;
    }
    else if(conditionValues[0] == "false") {
        pointer->conditionValue = false;
    }
    else {
        throw std::runtime_error("Condition value is not a boolean");
    }

    conditions.emplace_back(pointer);
};

void LogicBlock::add_object_condition(const std::string& conditionName, const std::vector<std::string>& conditionValues,
    std::weak_ptr<Database::DomainObject> databasePointer) {
    auto pointer = std::make_shared<ObjectCondition>();
    pointer->conditionName=conditionName;
    pointer->valuePointer=std::move(databasePointer);
    pointer->conditionValues = conditionValues;
    conditions.emplace_back(pointer);
};


