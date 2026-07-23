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

  oddEngine.parse_ontology("../ontology/ontology_fsw.rdf");
  oddEngine.parse_odd("../config/odd_fsw.yaml");

  return 0;
}
