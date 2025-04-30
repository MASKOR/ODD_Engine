#include "../include/ODDEngine.h"

/* void ODDEngine::print_all_keys()
{
    variableTable.print_all_keys();
} */

Database& ODDEngine::get_variable_table()
{
    return variableTable;
}

std::vector<std::string> ODDEngine::get_all_keys()
{
    return variableTable.get_all_keys();
}

std::vector<std::string> ODDEngine::get_all_keys_without_guardrails(){
    std::vector<std::string> keys;
    std::vector<std::string> vT = get_all_keys();
    for (std::string str : vT)
    {
        if(!is_key_in_guardrails(str))
        {
            keys.push_back(str);
        }
    }
    return keys;
}

std::vector<std::string> ODDEngine::get_all_objects()
{
    std::vector<std::string> keys;
    
    for (const auto& pair : variableTable.objects)
    {
        keys.push_back(*pair->get_id());
    }

    return keys;
}

std::vector<std::string> ODDEngine::get_all_guardrail_keys()
{
    std::vector<std::string> keys;
    for (const auto& pair : guardrailMap)
    {
        std::cout << "Level " << pair.first << ": " << std::endl;
        for (const std::pair<const std::string, Guardrail>& guardrail : pair.second)
        {
            std::cout << "Guardrail: " << guardrail.first << std::endl;
            keys.push_back(guardrail.first);
        }
    }

    return keys;
}

std::vector<std::string> ODDEngine::get_all_restriction_keys()
{
    std::vector<std::string> keys;
    for (const auto& pair : guardrailMap)
    {
        std::cout << "Level " << pair.first << ": " << std::endl;
        for (const std::pair<const std::string, Guardrail>& guardrail : pair.second)
        {
            std::cout << "Guardrail: " << guardrail.first << std::endl;
            if(guardrail.second.isRestriction){
                keys.push_back(guardrail.first);
            }
        }
    }

    return keys;
}

std::vector<std::string> ODDEngine::get_child_keys(const std::string& key)
{
    if(is_key_in_guardrails(key))
    {
        Guardrail guardrail = get_guardrail(key);
        std::vector<std::string> keys;
        for(LogicBlock logicblock : guardrail.logicBlocks)
        {
            for(std::shared_ptr<Condition> condition : logicblock.conditions)
            {
                keys.push_back(condition->conditionName);
            }
        }
        return keys;
    }
    else
    {
        if(variableTable.find_variable_without_error(key).first != DataType::ERROR)
        {
            std::vector<std::string> keys = expressionContainer.get_dependencies_for_key(key);
            
            return keys;
        }else{
            std::cout << "Key not found in variable table" << std::endl;
            throw std::out_of_range("Identifier " + key + " is not in Database");
        }
    }
}

std::string ODDEngine::get_target_of_restriction(const std::string& key)
{
    if(is_key_in_restriction(key))
    {
        Guardrail rest = get_guardrail(key);
        return rest.target;
    }
    else
    {
        throw std::out_of_range("Identifier " + key + " is not in Database");
    }
}

std::string ODDEngine::get_expression_of_variable(const std::string& key)
{
    if(variableTable.find_variable_without_error(key).first != DataType::ERROR)
    {
        Expression expression = expressionContainer.get_expression(key);
        std::string expr = expression.get_expression_string();
        return expr;
    }else{
        std::cout << "Key not found in variable table" << std::endl;
        throw std::out_of_range("Identifier " + key + " is not in Database");
    }
}