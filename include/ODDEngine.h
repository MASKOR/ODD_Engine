//
// Project: ODD Engine
// Description: Header file for the Database class, which manages domain objects and their properties.
//
// This file is part of the ODD Engine and was developed in the Automated Driving Project 
// of the Fahrzeugsoftwarelabor at MASCOR Institute of FH Aachen - University of Applied Sciences.
//
// Maintainer: Moritz Rumpf, Joschua Schulte-Tigges, Till Voss
// 

#ifndef ODD_ENGINE_GUARDRAILENGINE_H
#define ODD_ENGINE_GUARDRAILENGINE_H

#include <any>
#include <utility>

#include "Ontology.h"
#include "yaml-cpp/yaml.h"
#include "Guardrail.h"
#include "Database.h"
#include "Expression.h"

/*
 * Datastructure for Ontology
 */

#define CHECK_IF_ONTOLOGY_WHILE_RUNTIME false

const std::vector<std::string> KEYWORD_LIST = {"import", "SUITABLE", "MEASURE", "DETERMINE"};


class ODDEngine
{
public:
    // CONSTRUCTOR ----------------------------------
    ODDEngine();

    // PARSER ----------------------------------------
    // recursion
    void parse_odd(const std::string& filename);

    void parse_ontology(const std::string& filename);


private:
    /*
      * returns the height of guardrail
      * returns -1 if not registered
      */
    int get_guardrail_height(const std::string& target);

    void parse_variable(const std::string& key1guardrailName,
                       const YAML::Node& value1guardrailNode);

    int parse_guardrail_or_restriction(const std::string& key1guardrailName,
                       const YAML::Node& value1guardrailNode, bool isRestriction);


public:
    // INFERENCE -------------------------------------
    bool inference(std::vector<std::string>& targets);

private:

    bool check_logic_block(const LogicBlock& logicBlock);

    bool check_guardrail(const std::pair<std::string, Guardrail>& guardrailPair, std::vector<std::string>& targets);


    // HELPER FUNCTIONS --------------------------
public:
    /*
     * calls a Function of Database that prints all variables
     */

    std::string get_default_namespace()
    {
        return dnamespace;
    }

    void print_guardrail_map(bool hideDetail = false);

    // ODD Engine Structure Helper
    void print_all_keys(){
        std::cout << "All keys in Database:" << std::endl;

        for (const auto& pair : variableTable.indexMap) {
            if(is_key_in_restriction(pair.first))
            {
                std::cout << "Restriction: " << pair.first << std::endl;
            }else if(is_key_in_guardrails(pair.first))
            {
                std::cout << "Guardrail: " << pair.first << std::endl;
            }
            else
            {
                std::cout << pair.first << std::endl;
            }
        }
    }

    Database& get_variable_table();

    std::vector<std::string> get_all_keys();

    std::vector<std::string> get_all_keys_without_guardrails();

    std::vector<std::string> get_all_objects();

    std::vector<std::string> get_all_guardrail_keys();

    std::vector<std::string> get_all_restriction_keys();

    std::vector<std::string> get_child_keys(const std::string& key);

    std::string get_target_of_restriction(const std::string& key);

    std::string get_expression_of_variable(const std::string& key, bool getCParse = false);

    Guardrail get_guardrail(const std::string& key)
    {
        for(const auto& heightLevel: guardrailMap)
        {
            try
            {
                return heightLevel.second.at(key);
            }
            catch (const std::out_of_range& e){}
        }
        throw std::out_of_range("Guardrail " + key + " not in List");
    }
     
    bool is_key_in_restriction(const std::string& key)
    {
        for(const auto& heightLevel: guardrailMap)
        {
            auto it = heightLevel.second.find(key);
            if (it != heightLevel.second.end() && it->second.isRestriction)
            {
                return true;
            }
        }
        return false;
    }
    
    bool is_key_variable(const std::string& key)
    {
        if(variableTable.find_variable_without_error(key).first != DataType::ERROR)
        {
            try
            {
                expressionContainer.get_expression(key);
                return true;
            }
            catch(const std::out_of_range& e)
            {
                return false;
            }
            
        }
        else
        {
            return false;
        }
        
    }


    /*
     * Sets the value of an onotologyClass Object
     */
    void set_sub_value(const std::string& key, const std::string& value)
    {
        variableTable.set_object_value(key, value, ontology);
    }

    LogicBlockType::value get_constraint_operation_type(YAML::Node& input);

    template <typename T>
    void set_data_property(const std::string& key, const T& value)
    {
        variableTable.set_data_property(key, value);
    }

    bool is_key_in_guardrails(const std::string& key)
    {
        for(const auto& heightLevel: guardrailMap)
        {
            try
            {
                heightLevel.second.at(key);
                return true;
            }
            catch (const std::out_of_range& e){}
        }
        return false;
    }

    // ATTRIBUTE ---------------------------
private:
    std::string dnamespace;

    // All variables are stored here. The Database essentially consists of an unordered map of variables.
    // Each variable has a value of type std::any and the corresponding data type from ConditionType.
    Database variableTable;
    // The GuardrailMap contains all guardrails with all constraints and conditions.
    std::unordered_map<int, std::map<std::string, Guardrail>> guardrailMap;
    ExpressionContainer expressionContainer;
    Ontology ontology;
};

#endif //ODD_ENGINE_GUARDRAILENGINE_H
