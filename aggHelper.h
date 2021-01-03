#pragma once
#include <memory>
#include <QObject>
#include "col.hpp"
using namespace std;
class aggHelper:public QObject {
private:
    aggHelper(){}

public:
    static aggHelper* helper;

    static aggHelper* getHelper(){
        if(helper==nullptr){
            helper=new aggHelper();
        }
        return helper;
    }

    vector<Basic*> distinct(vector<Basic*> data_vec){
        vector<int> del_vec;
        vector<Basic*> result;
        for(int i=0;i<data_vec.size();++i){
            for(int j=i+1;j<data_vec.size();j++){
                if(typeHelper::isEqu(data_vec[i],data_vec[j])){
                    del_vec.push_back(i);
                    break;
                }
            }
        }
        for(int i=0;i<data_vec.size();++i){
            if(find(del_vec.begin(),del_vec.end(),i)==del_vec.end()){
                result.push_back(typeHelper::copy(data_vec[i]));
            }
        }
        return result;
    }

    Basic* avg(vector<Basic*> data_vec){
        if(data_vec.empty()){
            return nullptr;
        }
        if(data_vec[0]->getType()!=INT&&data_vec[0]->getType()!=FLOAT){
            throw string("type mismatch");
        }
        float result=0;
        for(Basic* b:data_vec){
            if(data_vec[0]->getType()==INT){
                result+=((Int*)b)->val;
            }
            else{
                result+=((Float*)b)->val;
            }
        }
        int num=data_vec.size();
        return new Float (result/num);
    }

    Basic* count(vector<Basic*> data_vec){
        int num=data_vec.size();
        return new Int (num);
    }

    Basic* first(vector<Basic*> data_vec){
        if(data_vec.empty()){
            return nullptr;
        }
        Basic* temp=typeHelper::copy(data_vec[0]);
        return temp;
    }

    Basic* last(vector<Basic*> data_vec){
        if(data_vec.empty()){
            return nullptr;
        }
        Basic* temp=typeHelper::copy(data_vec[data_vec.size()-1]);
        return temp;
    }

    Basic* max(vector<Basic*> data_vec){
        if(data_vec.empty()){
            return nullptr;
        }
        TYPE tp=data_vec[0]->getType();
        if(tp!=INT&&tp!=FLOAT){
            throw string("type mismatch");
        }
        float result;
        for(int i=0;i<data_vec.size();++i){
            float temp;
            if(tp==INT){
                temp=((Int*)(data_vec[i]))->val;
            }
            else{
                temp=((Float*)(data_vec[i]))->val;
            }
            if(i==0){
                result=temp;
                continue;
            }
            if(temp>result){
                result=temp;
            }
        }
        if(tp==INT){
            return new Int((int)result);
        }
        return new Float(result);
    }

    Basic* min(vector<Basic*> data_vec){
        if(data_vec.empty()){
            return nullptr;
        }
        TYPE tp=data_vec[0]->getType();
        if(tp!=INT&&tp!=FLOAT){
            throw string("type mismatch");
        }
        float result;
        for(int i=0;i<data_vec.size();++i){
            float temp;
            if(tp==INT){
                temp=((Int*)(data_vec[i]))->val;
            }
            else{
                temp=((Float*)(data_vec[i]))->val;
            }
            if(i==0){
                result=temp;
                continue;
            }
            if(temp<result){
                result=temp;
            }
        }
        if(tp==INT){
            return new Int((int)result);
        }
        return new Float(result);
    }

    Basic* sum(vector<Basic*> data_vec){
        if(data_vec.empty()){
            return nullptr;
        }
        if(data_vec[0]->getType()!=INT&&data_vec[0]->getType()!=FLOAT){
            throw string("type mismatch");
        }
        float result=0;
        for(Basic* b:data_vec){
            if(data_vec[0]->getType()==INT){
                result+=((Int*)(b))->val;
            }
            else{
                result+=((Float*)(b))->val;
            }
        }
        return new Float (result);
    }


};

