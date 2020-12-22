#pragma once
#include "col.hpp"
using namespace std;
class aggHelper {
private:
    aggHelper(){}

    static aggHelper* helper;

public:
    static aggHelper* getHelper(){
        if(helper==nullptr){
            helper=new aggHelper();
        }
        return helper;
    }

    vector<shared_ptr<Basic>> distinct(vector<shared_ptr<Basic>> data_vec){
        vector<int> del_vec;
        vector<shared_ptr<Basic>> result;
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

    shared_ptr<Basic> avg(vector<shared_ptr<Basic>> data_vec){
        if(data_vec.empty()){
            return shared_ptr<Basic>(nullptr);
        }
        if(data_vec[0]->getType()!=INT&&data_vec[0]->getType()!=FLOAT){
            throw string("type mismatch");
        }
        float result=0;
        for(shared_ptr<Basic> b:data_vec){
            if(data_vec[0]->getType()==INT){
                result+=(dynamic_pointer_cast<Int>(b))->val;
            }
            else{
                result+=(dynamic_pointer_cast<Float>(b))->val;
            }
        }
        int num=data_vec.size();
        return shared_ptr<Basic>(new Float (result/num));
    }

    shared_ptr<Basic> count(vector<shared_ptr<Basic>> data_vec){
        int num=data_vec.size();
        return shared_ptr<Basic>(new Int (num));
    }

    shared_ptr<Basic> first(vector<shared_ptr<Basic>> data_vec){
        if(data_vec.empty()){
            return shared_ptr<Basic>(nullptr);
        }
        shared_ptr<Basic> temp=typeHelper::copy(data_vec[0]);
        return temp;
    }

    shared_ptr<Basic> last(vector<shared_ptr<Basic>> data_vec){
        if(data_vec.empty()){
            return shared_ptr<Basic>(nullptr);
        }
        shared_ptr<Basic> temp=typeHelper::copy(data_vec[data_vec.size()-1]);
        return temp;
    }

    shared_ptr<Basic> max(vector<shared_ptr<Basic>> data_vec){
        if(data_vec.empty()){
            return shared_ptr<Basic>(nullptr);
        }
        TYPE tp=data_vec[0]->getType();
        if(tp!=INT&&tp!=FLOAT){
            throw string("type mismatch");
        }
        float result;
        for(int i=0;i<data_vec.size();++i){
            float temp;
            if(tp==INT){
                temp=(dynamic_pointer_cast<Int>(data_vec[i]))->val;
            }
            else{
                temp=(dynamic_pointer_cast<Float>(data_vec[i]))->val;
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
            return shared_ptr<Basic>(new Int((int)result));
        }
        return shared_ptr<Basic>(new Float(result));
    }

    shared_ptr<Basic> min(vector<shared_ptr<Basic>> data_vec){
        if(data_vec.empty()){
            return shared_ptr<Basic>(nullptr);
        }
        TYPE tp=data_vec[0]->getType();
        if(tp!=INT&&tp!=FLOAT){
            throw string("type mismatch");
        }
        float result;
        for(int i=0;i<data_vec.size();++i){
            float temp;
            if(tp==INT){
                temp=(dynamic_pointer_cast<Int>(data_vec[i]))->val;
            }
            else{
                temp=(dynamic_pointer_cast<Float>(data_vec[i]))->val;
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
            return shared_ptr<Basic>(new Int((int)result));
        }
        return shared_ptr<Basic>(new Float(result));
    }

    shared_ptr<Basic> sum(vector<shared_ptr<Basic>> data_vec){
        if(data_vec.empty()){
            return shared_ptr<Basic>(nullptr);
        }
        if(data_vec[0]->getType()!=INT&&data_vec[0]->getType()!=FLOAT){
            throw string("type mismatch");
        }
        float result=0;
        for(shared_ptr<Basic> b:data_vec){
            if(data_vec[0]->getType()==INT){
                result+=(dynamic_pointer_cast<Int>(b))->val;
            }
            else{
                result+=(dynamic_pointer_cast<Float>(b))->val;
            }
        }
        return shared_ptr<Basic>(new Float (result));
    }


};

