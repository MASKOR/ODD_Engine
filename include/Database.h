//
// Project: ODD Engine
// Description: Header file for the Database class, which manages domain objects and their properties.
//
// This file is part of the ODD Engine and was developed in the Automated Driving Project 
// of the Fahrzeugsoftwarelabor at MASCOR Institute of FH Aachen - University of Applied Sciences.
//
// Maintainer: Moritz Rumpf, Joschua Schulte-Tigges, Till Voss
// 

#ifndef DATABASE_H
#define DATABASE_H

#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <iostream>


#include "Ontology.h"

class Database {

public:
    class DomainObjectDataProperty
    {
    public:
        [[nodiscard]] DataType::value get_type() const
        {
            if(auto lockedPointer = ontologyReference.lock())
            {
                return lockedPointer->range;
            }
            throw std::bad_weak_ptr();
        }

        std::string get_id()
        {
            if(auto lockedPointer=ontologyReference.lock())
            {
                return lockedPointer->id;
            }
            throw std::bad_weak_ptr();
        }

        DataType::value dataType;
        std::pair<DataType::value,int> index;
        std::weak_ptr<OntologyDataProperty> ontologyReference;
    };

    class DomainObject
    {
    public:
        DomainObject(const std::string& objectName, const std::string& classKey, const std::string& valueKey,  Ontology& ontology)
        {
            // Set class
            this->objectName=std::make_shared<std::string>(objectName);
            this->classReference = ontology.get_class_ptr(classKey);
            if(!classReference)
            {
                throw std::out_of_range("No class exisits with this ID: " + classKey);
            }

            // set Value
            set_value(classKey, valueKey, ontology);

       }

        /*
         * set the Value of an Object if it is a Subclass of the class of the Object
         */
        void set_value(const std::string& classKey, const std::string& valueKey,  Ontology& ontology)
        {
            if(valueKey.empty() || valueKey == "none")
            {
                this->valueReference = nullptr;
            }
            else
            {
                this->valueReference = ontology.get_class_ptr(valueKey);
                if(!ontology.find_class(valueKey))
                {
                    throw std::out_of_range(valueKey + " not found");
                }
                for (const auto& topClass: this->valueReference->topClasses)
                {
                    if(topClass->id == this->classReference->id)
                    {
                        return;
                    }
                }
                throw std::domain_error(valueKey + " is not a Subclass of " + classKey);
            }
        }

        std::shared_ptr<OntologyClass> get_value()
        {
            return valueReference;
        }

        std::shared_ptr<OntologyClass> get_class_reference()
        {
            return classReference;
        }

        std::shared_ptr<DomainObjectDataProperty> get_data_property(const std::string& data_property_key)
        {
            for(const auto& ptr: dataProperies)
            {
                std::string id = ptr->get_id();
                if(id == data_property_key || id.substr(id.find('#')+1, std::string::npos) == data_property_key)
                {
                    return ptr;
                }
            }
            throw std::out_of_range("DataProperty " + data_property_key + " not in List");
        }

        std::vector<std::shared_ptr<DomainObjectDataProperty>> dataProperies;

    private:
        std::shared_ptr<std::string> objectName;

        std::shared_ptr<OntologyClass> classReference;
        std::shared_ptr<OntologyClass> valueReference;
    };

    /*
     * Initialize an object in Database
     */
    void init_object(const std::string & objectName, const std::string& domainClassType,  Ontology& ontology);

    /*
     * Initialize an variable in Database
     */
    void init_variable(const std::string& key, DataType::value dataType);

    /*
     *
     */
    void set_object_value(const std::string & key, const std::string& value,  Ontology& ontology);

    /*
     * Set the Value of a Data Property
     * DataType has to be set as Parameter for type Checking
     */
    template <typename T>
    void set_data_property(const std::string& key, const T& value)
    {
        auto pair = find_variable(key);

        if (pair.first == DataType::BOOL)
        {
            *boolVariable[pair.second] = value;
        }
        else if (pair.first == DataType::INT)
        {
            *intVariable[pair.second] = value;
        }
        else if (pair.first == DataType::FLOAT)
        {
            *floatVariable[pair.second] = value;
        }
        else if (pair.first == DataType::DOUBLE)
        {
            *doubleVariable[pair.second] = value;
        }
        else if (pair.first == DataType::STRING)
        {
            *stringVariable[pair.second] = value;
        }
        else
        {
            throw std::invalid_argument("Datatype of " + key + " is not supported!");

        }
    }

    std::shared_ptr<bool> get_bool_ptr(const std::string& key)
    {
        auto pair = find_variable(key);

        if (pair.first != DataType::BOOL)
        {
            throw std::invalid_argument("Identifier " + key + " is not a Boolean!");
        }

        return boolVariable.at(pair.second);
    }

    std::shared_ptr<int> get_int_ptr(const std::string& key)
    {
        auto pair = find_variable(key);

        if (pair.first != DataType::INT)
        {
            throw std::invalid_argument("Identifier " + key + " is not a Integer!");
        }

        return intVariable.at(pair.second);
    }

    std::shared_ptr<float> get_float_ptr(const std::string& key)
    {
        auto pair = find_variable(key);

        if (pair.first != DataType::FLOAT)
        {
            throw std::invalid_argument("Identifier " + key + " is not a Float!");
        }

        return floatVariable.at(pair.second);
    }

    std::shared_ptr<double> get_double_ptr(const std::string& key)
    {
        auto pair = find_variable(key);

        if (pair.first != DataType::DOUBLE)
        {
            throw std::invalid_argument("Identifier " + key + " is not a Double!");
        }

        return doubleVariable.at(pair.second);
    }

    std::pair<DataType::value, int> find_variable(const std::string& key);

    std::pair<DataType::value, int> find_variable_without_error(const std::string& key);

    /*
     * returns the value of a Object or a BoolVariable as String
     */
    std::string get_object_value(const std::string& key);

    std::shared_ptr<DomainObject>  get_domain_obj_shared_ptr (const std::string & key);


    std::vector<std::shared_ptr<bool>>                          boolVariable;
    std::vector<std::shared_ptr<int>>                           intVariable;
    std::vector<std::shared_ptr<float>>                         floatVariable;
    std::vector<std::shared_ptr<double>>                        doubleVariable;
    std::vector<std::shared_ptr<std::string>>                   stringVariable;
    std::vector<std::shared_ptr<DomainObject>> objects;

private:
    /*
     *
     */
    std::unordered_map<std::string, std::pair<DataType::value, int>> indexMap;
    };

#endif //DATABASE_H
