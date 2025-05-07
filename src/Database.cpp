//
// Project: ODD Engine
// Description: Header file for the Database class, which manages domain objects and their properties.
//
// This file is part of the ODD Engine and was developed in the Automated Driving Project 
// of the Fahrzeugsoftwarelabor at MASCOR Institute of FH Aachen - University of Applied Sciences.
//
// Maintainer: Moritz Rumpf, Joschua Schulte-Tigges, Till Voss
// 

#include "../include/Database.h"


/*
 * function initializing a variable
 */
void Database::init_object(const std::string & objectName, const std::string& domainClassType,  Ontology& ontology)
{
    if(this->find_variable_without_error(objectName).first != DataType::ERROR)
    {
        throw std::invalid_argument("Variable " + objectName + " already initialized");
    }
    if(!ontology.find_class(domainClassType))
    {
        throw std::invalid_argument("Class " + domainClassType + " not in Ontology!");
    }

    indexMap[objectName] = {DataType::OBJECT, objects.size()};
    objects.emplace_back(std::make_shared<DomainObject>(objectName, domainClassType, "", ontology));

    for(const auto& dataProperty: ontology.get_properties_of_class(domainClassType))
    {
        DomainObjectDataProperty domain_object_data_property;

        domain_object_data_property.dataType= dataProperty->range;
        domain_object_data_property.ontologyReference = dataProperty;

        DataType::value type;
        if (dataProperty->range==DataType::BOOL)
        {
            type=DataType::BOOL;
        }
        else if (dataProperty->range==DataType::INT)
        {
            type=DataType::INT;
        }
        else if (dataProperty->range==DataType::FLOAT)
        {
            type=DataType::FLOAT;
        }
        else if (dataProperty->range==DataType::DOUBLE)
        {
            type=DataType::DOUBLE;
        }
        else if (dataProperty->range==DataType::STRING)
        {
            type=DataType::STRING;
        }
        else
        {
            throw std::invalid_argument("Type " + std::to_string(dataProperty->range) + " is not supported");
        }

        std::string id = dataProperty->id;
        size_t pos = id.find('#');

        // If the character is found, delete everything before and including '#'
        if (pos != std::string::npos) {
            id.erase(0, pos + 1);
        }

        std::string key = objectName+id;

        this->init_variable(key, type);
        objects[objects.size()-1]->dataProperies.emplace_back(std::make_shared<DomainObjectDataProperty>(domain_object_data_property));
    }
}

/*
 * function initializing a variable that stores Values beside Domain specific classes
 */
void Database::init_variable(const std::string& key, DataType::value dataType)
{
    if(this->find_variable_without_error(key).first != DataType::ERROR)
    {
        throw std::invalid_argument("Guardrail " + key + " already initialized");
    }

    if(dataType == DataType::INT)
    {
        indexMap[key] = {dataType, intVariable.size()};
        intVariable.emplace_back(std::make_shared<int>(0));
    }
    else if(dataType == DataType::BOOL)
    {
        indexMap[key] = {dataType, boolVariable.size()};
        boolVariable.emplace_back(std::make_shared<bool>(false));
    }
    else if(dataType == DataType::DOUBLE)
    {
        indexMap[key] = {dataType, doubleVariable.size()};
        doubleVariable.emplace_back(std::make_shared<double>(0.0));
    }
    else if(dataType == DataType::FLOAT)
    {
        indexMap[key] = {dataType, floatVariable.size()};
        floatVariable.emplace_back(std::make_shared<float>(0.0));
    }
    else if(dataType == DataType::STRING)
    {
        indexMap[key] = {dataType, stringVariable.size()};
        stringVariable.emplace_back(std::make_shared<std::string>(""));
    }
    std::cout << key << " now in Variable" << std::endl;
}

/*
 *
 */
std::string Database::get_object_value(const std::string& key)
{
    std::pair<DataType::value, int> pair;
    try
    {
        pair= indexMap[key];
    }
    catch(const std::out_of_range& e)
    {
        throw std::out_of_range(key + " is not initialized");
    }

    if(pair.first ==DataType::OBJECT)
    {
        std::weak_ptr<DomainObject> weakPtr = objects.at(pair.second);
        if(auto lockedPtr = weakPtr.lock())
        {
            auto ptr = lockedPtr->get_value();
            if(!ptr)
            {
                return "none";
            }
            return lockedPtr->get_value()->id;
        }
    }
    throw std::invalid_argument(key + " is not an Object");
}

void Database::set_object_value(const std::string & key, const std::string& value,  Ontology& ontology){
    auto pair = find_variable(key);
    if(pair.first != DataType::OBJECT)
    {
        throw std::invalid_argument("Identifier " + key + " is not an Object");
    }
    std::shared_ptr<DomainObject> objectPtr = objects[pair.second];
    if(!objectPtr){
        throw std::out_of_range("Variable " + key + " not initialized");;
    }
    objectPtr->set_value(key, value, ontology);
}

std::shared_ptr<Database::DomainObject>  Database::get_domain_obj_shared_ptr (const std::string & key){
    std::pair<DataType::value, int> pair;
    try
    {
        pair = indexMap.at(key);
    }
    catch (const std::out_of_range& e)
    {
        throw std::out_of_range("Object " + key + " not initialized");
    }
    if (pair.first!=DataType::OBJECT)
    {
        throw std::domain_error("[get_domain_obj_shared_ptr] Identifier " + key + " is not an Object");
    }

    return objects.at(pair.second);
}

std::pair<DataType::value, int> Database::find_variable(const std::string& key)
{
    try
    {
        return indexMap.at(key);
    }
    catch (const std::out_of_range& e)
    {
        throw std::out_of_range("Identifier " + key + " not in Database");
    }
}

std::pair<DataType::value, int> Database::find_variable_without_error(const std::string& key)
{
    try
    {
        return indexMap.at(key);
    }
    catch (const std::out_of_range& e)
    {
        return {DataType::ERROR,-1};
    }
}

/*
 * function that prints all variables
 */
void Database::print_all_keys(){
    std::cout << "All keys in Database:" << std::endl;
    for (const auto& pair : indexMap) {
        std::cout << pair.first << std::endl;
    }
}

std::vector<std::string> Database::get_all_keys(){
    std::vector<std::string> keys;
    for (const auto& pair : indexMap) {
        keys.push_back(pair.first);
    }
    return keys;
}