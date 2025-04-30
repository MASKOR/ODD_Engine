//
// Created by moritz on 18/06/24.
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

//TODO auslagern in verschiedene Files in diese Blöcke (jeweils mit .hpp .cpp)
//TODO types
//TODO parse
//TODO checkRules

//TODO co endl ersetzen
//TODO zwischen error und Ausgabe entscheiden
//TODO prüfen ob Ausgabe via Stream, RCLCPP_INFO, fstream oder std::cout genutzt werden soll

//TODO prüfen ob list in case WHEN AND überhaupt erlaubt sein soll und wenn ja ob richtig implementiert

//todo check on rekursion in import statement

//todo values in yaml in richtige types casten

/*
 * Datastructure for Ontology
 */
//todo mehr mit weakptr machen bitte

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
    bool inference(std::vector<std::string>& targets);//todo prüfen ob current situation komplexer abgebildet werden soll

private:
    bool check_condition(const Condition& condition);

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

    std::string get_expression_of_variable(const std::string& key);

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

    // Sämtliche Variablen werden hier gespeichert. Database besteht im Kern aus einer unorderdMap von Variablen.
    // Jede Variable besitzt einen Wert vom Typ std::any und den dazugehörigen Datentyp aus ConditionType
    Database variableTable;
    // Die GuardrailMap beinhaltet alle Guardrails mit sämtlichen Constraints und Conditions
    std::unordered_map<int, std::map<std::string, Guardrail>> guardrailMap;
    ExpressionContainer expressionContainer;
    Ontology ontology;
};

#endif //ODD_ENGINE_GUARDRAILENGINE_H
