//
// Created by mo on 17.09.24.
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
        NaN, Var, Int, Float
    };
};

#endif //DATATYPE_H
