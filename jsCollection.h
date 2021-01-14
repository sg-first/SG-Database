#pragma once
#include "col.hpp"

class jsCollection : public QObject, public manageable
{
    Q_OBJECT
    vector<int> intVec;
    vector<Basic*> basicVec;

public:
    jsCollection(const vector<int>& intVec) {
        this->intVec=intVec;
    }

    jsCollection(vector<Basic*> basicVec) {
        this->basicVec=basicVec;
    }

    vector<int> getintVec(){
        return intVec;
    }

    vector<Basic*> getbasicVec(){
        return basicVec;
    }
};
