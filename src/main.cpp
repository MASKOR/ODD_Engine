//
// Project: ODD Engine
// Description: Header file for the Database class, which manages domain objects and their properties.
//
// This file is part of the ODD Engine and was developed in the Automated Driving Project 
// of the Fahrzeugsoftwarelabor at MASCOR Institute of FH Aachen - University of Applied Sciences.
//
// Maintainer: Moritz Rumpf, Joschua Schulte-Tigges, Till Voss
// 


#include <iostream>
#include <utility>
#include <Ontology.h>
#include "../include/ODDEngine.h"


/*
 * MAIN -------------------------------------------------
 */
int main(int argc, char *argv[]) {
    ODDEngine oddEngine;

    oddEngine.parse_ontology("../ontology/example_ontology.rdf");
    oddEngine.parse_odd("../config/odd_definition.yaml");

    oddEngine.set_data_property("loadingStatusisLoaded", true);
    std::vector<std::string> targets;
    oddEngine.inference(targets);

    bool accelerate = true;
    int x = 0;

    std::cout << "Accelerating: ";

    while(accelerate){
        oddEngine.set_data_property("egoVehiclespeed", x);
        std::cout << x << "...";

        std::vector<std::string> targets;
        oddEngine.inference(targets);
        for(const std::string& str: targets)
        {
            if(str == "SPEED_LIMIT_REACHED")
            {
                accelerate = false;
            }
        }
        x++;
    }
    std::cout << std::endl;
    std::cout << "SPEED_LIMIT_REACHED" <<std::endl;
    
    return 0;

}
