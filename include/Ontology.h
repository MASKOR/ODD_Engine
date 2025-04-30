//
// Created by mo on 06.08.24.
//

#ifndef ONTOLOGY_H
#define ONTOLOGY_H

#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <unordered_map>

#include "Datatypes.h"

#define RDF_SUBCLASSOF  "http://www.w3.org/2000/01/rdf-schema#subClassOf"
#define RDF_DOMAIN      "http://www.w3.org/2000/01/rdf-schema#domain"
#define RDF_RANGE       "http://www.w3.org/2000/01/rdf-schema#range"

#define RDF_INT         "http://www.w3.org/2001/XMLSchema#int"
#define RDF_FLOAT       "http://www.w3.org/2001/XMLSchema#float"
#define RDF_DOUBLE      "http://www.w3.org/2001/XMLSchema#double"
#define RDF_BOOLEAN     "http://www.w3.org/2001/XMLSchema#boolean"
#define RDF_STRING      "http://www.w3.org/2001/XMLSchema#string"

struct OntologyClass;
class Ontology;

struct Tripel {
    std::string subject;
    std::string predicate;
    std::string object;
};


struct OntologyDataProperty
{
    std::string id;

    std::vector<std::shared_ptr<OntologyClass>> domains;//todo hier bitte weakptr vlt
    DataType::value range;
};

struct OntologyClass {
    std::string id;

    std::vector<std::shared_ptr<OntologyClass>> topClasses; //todo weak vlt
    std::vector<std::shared_ptr<OntologyClass>> subClasses; //todo bitte weak vlt

    std::vector<std::shared_ptr<OntologyDataProperty>> properties;
};

class Ontology {
public:
    Ontology() {
        classSize = 0;
    }

    bool add_class(const std::string& id);

    /*
     * declares subclass connection of class
     * function also sets id as topclass of subclass
     */
    bool add_subclass(const std::string& id, const std::string& subclass);

    /*
     * redundant function
     * calls add_subclass but in different direction
     */
    bool add_topclass(const std::string& id, const std::string& topclass);

    /*
     * returns true if id points to a class
     */
    bool find_class(const std::string& id );

    /*
     * returns the shared pointer of an Ontology that is linked to the key
     */
    std::shared_ptr<OntologyClass> get_class_ptr(const std::string& key);

    /*
     * adds a Data Property to dataProperties Map if not exists yet
     */
    bool add_data_property(const std::string& id);

    /*
     *
     */
    bool add_domain_to_data_property(const std::string& dataPropertyKey, const std::string& domainKey);

    /*
     * set range of DataProperty
     * Datatypes must be converted in ConditionDataType
     * at the moment only one range per DataProperty
     */
    bool set_range_of_data_property(const std::string& dataPropertyKey, DataType::value dataType);

    /*
     *
     */
    std::vector<std::shared_ptr<OntologyDataProperty>> get_properties_of_class(const std::string& key);

private:
    uint32_t classSize;
    uint32_t dataPropertySize;

    std::unordered_map<std::string, std::shared_ptr<OntologyClass>> ontologyClasses;
    std::unordered_map<std::string, std::shared_ptr<OntologyDataProperty>> dataProperties;

    std::shared_ptr<OntologyClass> root;
};

/*
 * returns a Map with all Tripels per subject/Keyword
 */
std::unordered_map<std::string, std::vector<Tripel>> parse_rdf_file(const std::string& filename);

/**
 * creates Ontology out of TripelStack
 * @param tripelStack
 * @return Ontology
 */
Ontology create_ontology(const std::unordered_map<std::string, std::vector<Tripel>>& tripelStack);
#endif //ONTOLOGY_H
