//
// Project: ODD Engine
// Description: Header file for the Database class, which manages domain objects and their properties.
//
// This file is part of the ODD Engine and was developed in the Automated Driving Project 
// of the Fahrzeugsoftwarelabor at MASCOR Institute of FH Aachen - University of Applied Sciences.
//
// Maintainer: Moritz Rumpf, Joschua Schulte-Tigges, Till Voss
// 

#include "../include/Ontology.h"
#include <iostream>
#include <redland.h>


bool Ontology::add_class(const std::string& id) {
    if(ontologyClasses[id]!=nullptr) {
       return false;
    }

    std::shared_ptr<OntologyClass> ptr = std::make_shared<OntologyClass>();
    ptr->id = id;


    ontologyClasses[id]=ptr;

    classSize++;
    return true;
}

/*
 * declares subclass connection of class
 * function also sets id as topclass of subclass
 */
bool Ontology::add_subclass(const std::string& id, const std::string& subclass) {
    if(ontologyClasses[id]==nullptr || ontologyClasses[subclass]==nullptr) {
        return false;
    }

    ontologyClasses[subclass]->topClasses.emplace_back(ontologyClasses[id]);
    ontologyClasses[id]->subClasses.emplace_back(ontologyClasses[subclass]);
    return true;
}

/*
 * redundant function
 * calls add_subclass but in different direction
 */
bool Ontology::add_topclass(const std::string& id, const std::string& topclass) {
    return add_subclass(topclass, id);
}

/*
 * returns true if id points to a class
 */
bool Ontology::find_class(const std::string& id ) {
    if (ontologyClasses[id]!=nullptr) {
        return true;
    }
    return false;
}

/*
 * returns the shared pointer of an Ontology that is linked to the key
 */
std::shared_ptr<OntologyClass> Ontology::get_class_ptr(const std::string& key)
{
    if (ontologyClasses[key])
    {
        return ontologyClasses.at(key);
    }

    return nullptr;
}

/*
 * adds a Data Property to dataProperties Map if not exists yet
 */
bool Ontology::add_data_property(const std::string& id) {
    if(dataProperties[id]!=nullptr) {
        return false;
    }

    std::shared_ptr<OntologyDataProperty> ptr = std::make_shared<OntologyDataProperty>();
    ptr->id = id;

    dataProperties[id]=ptr;

    dataPropertySize++;
    return true;
}

/*
 *
 */
bool Ontology::add_domain_to_data_property(const std::string& dataPropertyKey, const std::string& domainKey)
{
    if(!dataProperties[dataPropertyKey])
    {
        std::cout << dataPropertyKey << " not found" << std::endl;
        return false;
    }
    if(!ontologyClasses[domainKey])
    {
        std::cout << domainKey << " not found" << std::endl;
        return false;
    }
    
    for (const auto& domain: dataProperties[dataPropertyKey]->domains)
    {
        if (domain->id == domainKey)
        {
            std::cout << domainKey << " already exists" << std::endl;
            return false;
        }
    }
    std::shared_ptr<OntologyClass> classPtr = this->get_class_ptr(domainKey);
    dataProperties[dataPropertyKey]->domains.emplace_back(classPtr);
    classPtr->properties.emplace_back(dataProperties[dataPropertyKey]);
    return true;
}

/*
 * set range of DataProperty
 * Datatypes must be converted in ConditionDataType
 * at the moment only one range per DataProperty
 */
bool Ontology::set_range_of_data_property(const std::string& dataPropertyKey, DataType::value dataType)
{
    if(!dataProperties[dataPropertyKey])
    {
        return false;
    }
    dataProperties[dataPropertyKey]->range = dataType;
    return true;
}

/*
 *
 */
std::vector<std::shared_ptr<OntologyDataProperty>> Ontology::get_properties_of_class(const std::string& key)
{
    if(!ontologyClasses[key])
    {
        return {};
    }
    std::vector<std::shared_ptr<OntologyClass>> classPtrQueue = {ontologyClasses[key]};
    std::vector<std::string> classAlreadyVisited = {};
    std::vector<std::shared_ptr<OntologyDataProperty>> resultDataProperties;
    while(!classPtrQueue.empty())
    {
        std::shared_ptr<OntologyClass> tmpClass = classPtrQueue[0];
        classPtrQueue.erase(classPtrQueue.begin());
        classAlreadyVisited.emplace_back(tmpClass->id);

        for(const auto& dataProperty: tmpClass->properties)
        {
            resultDataProperties.emplace_back(dataProperty);
        }
        for(const auto& topClass: tmpClass->topClasses)
        {
            classPtrQueue.emplace_back(topClass);
        }
    }
    return resultDataProperties;
}

/*
 * returns a Map with all Tripels per subject/Keyword
 */
std::unordered_map<std::string, std::vector<Tripel>> parse_rdf_file(const std::string& filename) {
    std::unordered_map<std::string, std::vector<Tripel>> tripelStack;

    // Initialize the RDF library
    librdf_world* world = librdf_new_world();
    librdf_world_open(world);

    // Create a new RDF model
    librdf_storage* storage = librdf_new_storage(world, "memory", NULL, NULL);
    librdf_model* model = librdf_new_model(world, storage, NULL);

    // Parse RDF data from a string (for demonstration purposes)
    const char* rdf_file = filename.c_str();
    librdf_parser* parser = librdf_new_parser(world, "rdfxml", NULL, NULL);
    librdf_uri* uri = librdf_new_uri_from_filename(world, rdf_file);

    if (librdf_parser_parse_into_model(parser, uri, NULL, model)) {
        std::cerr << "Failed to parse RDF file" << std::endl;
        librdf_free_uri(uri);
        librdf_free_parser(parser);
        librdf_free_model(model);
        librdf_free_storage(storage);
        librdf_free_world(world);
        return tripelStack;
    }

    // Iterate over the statements in the model
    librdf_stream* stream = librdf_model_as_stream(model);
    while (!librdf_stream_end(stream)) {
        librdf_statement* statement = librdf_stream_get_object(stream);
        // Process the statement here (printing as an example)
        librdf_node* subject = librdf_statement_get_subject(statement);
        librdf_node* predicate = librdf_statement_get_predicate(statement);
        librdf_node* object = librdf_statement_get_object(statement);

        // Convert nodes to strings for output
        Tripel tripel;
        tripel.subject = reinterpret_cast<char*>(librdf_node_to_string(subject));
        tripel.subject.erase(0, 1);
        tripel.subject.erase(tripel.subject.length() - 1, 1);
        tripel.predicate = reinterpret_cast<char*>(librdf_node_to_string(predicate));
        tripel.predicate.erase(0, 1);
        tripel.predicate.erase(tripel.predicate.length() - 1, 1);
        tripel.object = reinterpret_cast<char*>(librdf_node_to_string(object));
        tripel.object.erase(0, 1);
        tripel.object.erase(tripel.object.length() - 1, 1);
        std::cout << tripel.subject << " " << tripel.predicate << " " << tripel.object << std::endl;


        tripelStack[tripel.subject].emplace_back(tripel);

        librdf_stream_next(stream);
    }

    // Clean up
    librdf_free_stream(stream);
    librdf_free_model(model);
    librdf_free_storage(storage);
    librdf_free_parser(parser);
    librdf_free_uri(uri);
    librdf_free_world(world);

    return tripelStack;
}

/**
 * creates Ontology out of TripelStack
 * @param tripelStack
 * @return Ontology
 */
Ontology create_ontology(const std::unordered_map<std::string, std::vector<Tripel>>& tripelStack) {
    Ontology newOntology;
    // first initialization of all Classes
    for (const std::pair<std::string, std::vector<Tripel>> tuple : tripelStack) {
        if (tuple.first.find('#') != std::string::npos){
            std::string typeString = tuple.second[0].object.substr(tuple.second[0].object.find('#')+1);
            if(typeString == "Class")
            {
                std::cout << tuple.first << (newOntology.add_class(tuple.first)?" added":" already exists") << std::endl;
            }
            else if(typeString == "DatatypeProperty")
            {
                std::cout << (newOntology.add_data_property(tuple.first)? " added":" already exists") << std::endl;
            }
        }
    }

    // creation of Sub/Top-classes and property
    for (const std::pair<std::string, std::vector<Tripel>>  tuple: tripelStack) {
        std::cout << tuple.first << std::endl;
        if (tuple.first.find('#') != std::string::npos)
        {
            std::string typeString = tuple.second[0].object.substr(tuple.second[0].object.find('#')+1);
            if(typeString == "Class")
            {
                for (auto x: tuple.second) {
                    if (x.predicate == RDF_SUBCLASSOF) {
                        newOntology.add_topclass(x.subject, x.object);
                        std::cout << "      is SubClass of " << x.predicate << std::endl;
                    }
                }
            }
            if(typeString == "DatatypeProperty")
            {
                for (const Tripel& triple: tuple.second)
                {
                    if (triple.predicate == RDF_DOMAIN) {
                        std::cout << (newOntology.add_domain_to_data_property(triple.subject, triple.object)?"true":"false") << std::endl;
                    }
                    else if (triple.predicate == RDF_RANGE) {
                        DataType::value dataType;
                        if(triple.object == RDF_INT)
                        {
                            dataType=DataType::INT;
                        }
                        else if(triple.object == RDF_FLOAT)
                        {
                            dataType=DataType::FLOAT;
                        }
                        else if(triple.object == RDF_DOUBLE)
                        {
                            dataType=DataType::DOUBLE;
                        }
                        else if(triple.object == RDF_BOOLEAN)
                        {
                            dataType=DataType::BOOL;
                        }
                        else if(triple.object == RDF_STRING)
                        {
                            dataType=DataType::STRING;
                        }
                        else
                        {
                            throw std::out_of_range("not supported datatype");
                        }
                        newOntology.set_range_of_data_property(triple.subject, dataType);
                    }
                }
            }

            std::cout << std::endl;
        }
    }

    return newOntology;
}