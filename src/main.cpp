//
// Project: ODD Engine
// Description: Header file for the Database class, which manages domain objects and their
// properties.
//
// This file is part of the ODD Engine and was developed in the Automated Driving Project
// of the Fahrzeugsoftwarelabor at MASCOR Institute of FH Aachen - University of Applied Sciences.
//
// Maintainer: Moritz Rumpf, Joschua Schulte-Tigges, Till Voss
//

#include "../include/ODDEngine.h"
#include <Ontology.h>
#include <iostream>
#include <utility>

/*
 * MAIN -------------------------------------------------
 */
int main(int argc, char *argv[]) {
  ODDEngine oddEngine;

  oddEngine.parse_ontology("../../adp_odd/ontology_fsw.rdf");
  oddEngine.parse_odd("../../adp_odd/odd_fsw.yaml");
  
  oddEngine.set_data_property("egoVehicle.speed", 0.1);
  oddEngine.set_sub_value("nextSection", "http://www.semanticweb.org/mo/ontologies/2024/7/fsw_adp#tunnel");
  std::vector<std::string> t = oddEngine.inference();

  for (auto i : t){
    std::cout << i<< std::endl;
  }
  return 0;
}
