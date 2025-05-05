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
#include <cassert>


/* This function prints all Guardrails.
 * To suppress the details of the Guardrail, use the boolean parameter hideDetails.
 * It also ensures that the height saved in the Guardlines are correct with the height in the processSequence - map
 */
void ODDEngine::print_guardrail_map(bool hideDetails) {
    std::cout<< "--- print Guardrails ----------------" << std::endl;
    for (auto iter = guardrailMap.begin(); iter != guardrailMap.end(); iter++) {
        std::cout<< "Level " << iter->first << ": " << std::endl;
        // iterate through Guardrails
        for (auto &guardrailPair: iter->second) {
            std::cout<< "    " << guardrailPair.first << ":" << std::endl;
            if (hideDetails) continue;
            assert(guardrailPair.second.height == iter->first);
            // iterate through Constraints
            for (auto &constraint: guardrailPair.second.logicBlocks) {
                std::cout<< "        ";
                switch (constraint.operationType) {
                    case LogicBlockType::WHEN_OR:
                        std::cout<< "WHEN OR:" << std::endl;
                        break;
                    case LogicBlockType::WHEN_AND:
                        std::cout<< "WHEN AND:" << std::endl;
                        break;
                    case LogicBlockType::EXCEPT_WHEN_EITHER:
                        std::cout<< "EXCEPT WHEN EITHER:" << std::endl;
                        break;
                }
            }
        }
    }
    std::cout<< "---------------------------\n" << std::endl;
}

bool ODDEngine::check_logic_block(const LogicBlock& logicBlock) {
    for (const auto& condition: logicBlock.conditions) {
        const bool conditionIsTrue = condition->check_condition();
        if(conditionIsTrue && logicBlock.operationType == LogicBlockType::WHEN_OR) {
            return true;
        }
        if(conditionIsTrue && logicBlock.operationType == LogicBlockType::EXCEPT_WHEN_EITHER ||
            !conditionIsTrue && logicBlock.operationType == LogicBlockType::WHEN_AND) {
            return false;
            }
    }
    if(logicBlock.operationType == LogicBlockType::WHEN_OR) {
        return false;
    }
    if(logicBlock.operationType==LogicBlockType::WHEN_AND ||
        logicBlock.operationType == LogicBlockType::EXCEPT_WHEN_EITHER) {
        return true;
        }
    throw std::logic_error("check_logic_block massive error");
}


bool ODDEngine::check_guardrail(const std::pair<std::string, Guardrail>& guardrailPair, std::vector<std::string>& targets){
    for (auto &constraint: guardrailPair.second.logicBlocks) {

        if (!check_logic_block(constraint))
            return false;
    }
    if(guardrailPair.second.isRestriction)
    {
        targets.emplace_back(guardrailPair.second.target);
    }
    return true;
}

bool ODDEngine::inference(std::vector<std::string>& targets){
    // Calculate Expression
    this->expressionContainer.calculate_all();

    // Check Guardrails
    int heightLevel = 0;
    std::map<std::string, Guardrail> guardrailLevel = guardrailMap[heightLevel];

    // iterate through Height Levels
    while (!guardrailLevel.empty()) {
        // iterate through Guardrails
        for (auto &guardrailPair: guardrailLevel) {

            bool g = check_guardrail(guardrailPair, targets);
            if(!guardrailPair.second.isRestriction)
                variableTable.set_data_property(guardrailPair.first, g);
        }

        heightLevel++;
        guardrailLevel = guardrailMap[heightLevel];
    }

    return true;
}
