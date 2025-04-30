//
// Created by moritz on 18/06/24.
//
#include <unordered_map>
#include <iostream>
#include <chrono>
#include <utility>
#include <Ontology.h>
#include "../include/ODDEngine.h"


// Prototype beg
// Prototype end


/*
 * MAIN -------------------------------------------------
 */
int main(int argc, char *argv[]) {
    ODDEngine oddEngine;

    oddEngine.parse_ontology("../ontology/ontology_fsw.rdf");
    oddEngine.parse_odd("../config/config.yaml");


    auto xx_beg = std::chrono::high_resolution_clock::now();
    for(int x = 0; x < 1; ++x)
    {
        oddEngine.set_sub_value("v2x_avail","http://www.semanticweb.org/mo/ontologies/2024/7/untitled-ontology-8#full");
        oddEngine.set_data_property("our_routedistance_to_next_junction", x);
        std::cout << "ITER " << x << std::endl;


        std::vector<std::string> targets;

        oddEngine.inference(targets);
        for(const std::string& str: targets)
        {
            std::cout << str << std::endl;
        }
        std::cout << std::endl;
    }
  return 0;

    long timeee = std::chrono::duration_cast<std::chrono::nanoseconds>
              (std::chrono::high_resolution_clock::now() - xx_beg).count();

    std::cout << "Zeit für Inferenz von 10 Iterationen: " << timeee << "ns        \n"
              << "           " << timeee/1000 <<"µs\n"
              << "           " << timeee/1000000 <<"ms\n\n..............\n" << std::endl;


    //guardrailEngine.print_guardrail_map();
    /*guardrailEngine.set_variable("vehicle0","http://www.semanticweb.org/mo/ontologies/2024/6/testont#bike", o);
    guardrailEngine.set_data_property("vehicle0", "speed", 29.67);
    std::cout << "FAHREN=> " << (guardrailEngine.inference()?"true":"false") << std::endl;
    guardrailEngine.set_variable("vehicle0","http://www.semanticweb.org/mo/ontologies/2024/6/testont#car", o);
    guardrailEngine.set_data_property("vehicle0", "speed", 342.0);

    guardrailEngine.set_data_property("vehicle0", "speed", 342.0);

    std::cout << "FAHREN=> " << (guardrailEngine.inference()?"true":"false") << std::endl;*/

    std::cout << "fertig hier" << std::endl;
    return 3;
    std::cout << "\n\n\nAlles Tutti" << std::endl;
    std::string s0 = "    1+              _vitaminb12     &&   2!=78   &&   2";
    std::string s1 = "_vitaminb12*3+2";
    std::string s2 = "_vitaminb12<2";

    return 1;

    auto xxxx = std::chrono::high_resolution_clock::now();

    //double v = e.inference_prototyp();
    long time = std::chrono::duration_cast<std::chrono::nanoseconds>
            (std::chrono::high_resolution_clock::now() - xxxx).count();

    std::cout << "Zeit zum einlesen: " << time << "ns\n\n..............\n" << std::endl;


    //std::cout << v << std::endl;

    auto xx = std::chrono::high_resolution_clock::now();

    auto prepareTimeBegin = std::chrono::high_resolution_clock::now();


    std::cout << "Zeit zum einlesen: " << std::chrono::duration_cast<std::chrono::microseconds>
            (std::chrono::high_resolution_clock::now() - prepareTimeBegin).count() << "µs\n\n..............\n" << std::endl;


   // guardrailEngine.print_guardrail_map(false);

    std::cout << "\nfinito" << std::endl;
    return 12;
}
