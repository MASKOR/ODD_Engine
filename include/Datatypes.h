//
// Project: ODD Engine
// Description: Header file for the Database class, which manages domain objects and their properties.
//
// This file is part of the ODD Engine and was developed in the Automated Driving Project 
// of the Fahrzeugsoftwarelabor at MASCOR Institute of FH Aachen - University of Applied Sciences.
//
// Maintainer: Moritz Rumpf, Joschua Schulte-Tigges, Till Voss
// 

#ifndef DATATYPE_H
#define DATATYPE_H

struct DataType
{
    enum value
    {
        ERROR, BOOL, INT, FLOAT, DOUBLE, STRING, OBJECT
    };
};

struct stringType {
    enum value {
        NaN, Var, Int, Float, Bool
    };
};

#endif //DATATYPE_H
