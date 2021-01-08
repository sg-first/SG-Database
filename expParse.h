#pragma once
#include "rule.hpp"
#include <iostream>
#include <stack>

class bracket {
public:
    string val;
    int left_index;
    int right_index;
    bool isNot;
    ruleExp* bracket_rule=nullptr;
    bracket(string val,int l_i,int r_i,bool isNot){
        this->val=val;
        this->left_index=l_i;
        this->right_index=r_i;
        this->isNot=isNot;
    }
    void clear(){
        val="";
        left_index=-1;
        right_index=-1;
    }
};



class left_bracket{
public:
    int index;
    bool isNot;
    left_bracket(int index,bool isNot=false){
        this->index=index;
        this->isNot=isNot;
    }
};



class expParse {
public:
    static ruleExp* total_strrule_parse(const string& strrule){
        if(strrule==""){
            return nullptr;
        }
        vector<bracket> bracket_vec;
        if(bracketCheck(strrule,bracket_vec)==false){
            throw string("Wrong expression");
        }
        for(int i=0;i<bracket_vec.size();++i){
            if(insert_rule(bracket_vec,bracket_vec[i],strrule)==false){
                inter_call(bracket_vec[i]);
            }
        }
        return bracket_vec[bracket_vec.size()-1].bracket_rule;
    }

    static bool insert_rule(vector<bracket>& bracket_vec,bracket& tmp,const string& strrule){
        vector<int> insert_vec;
        for(int i=0;i<bracket_vec.size();++i){
            if(tmp.left_index<bracket_vec[i].left_index&&bracket_vec[i].right_index<tmp.right_index){
                insert_vec.push_back(i);
            }
        }
        if(insert_vec.empty()){
            return false;
        }
        do_insert_rule(bracket_vec[insert_vec[0]],bracket_vec[insert_vec[1]],tmp,strrule);
        if(tmp.isNot==true){
            tmp.bracket_rule=new logExp (NOT,nullptr,tmp.bracket_rule);
        }
        for(int i=0;i<insert_vec.size();++i){
            bracket_vec[insert_vec[i]].clear();
        }
        return true;
    }

    static void do_insert_rule(const bracket& left_bk,const bracket& right_bk,bracket& tmp,const string& strrule){
        string symbol=strrule.substr(left_bk.right_index+1,2);
        if(symbol=="||"){
            tmp.bracket_rule=new logExp(OR,left_bk.bracket_rule,right_bk.bracket_rule);
        }
        else if(symbol=="&&"){
            tmp.bracket_rule=new logExp(AND,left_bk.bracket_rule,right_bk.bracket_rule);
        }
        else{
            throw string("Wrong expression");
        }
    }

    static bool bracketCheck(const string& strrule,vector<bracket>& bracket_val){
        stack<left_bracket> left_brackets;
        for(int i=0;i<strrule.size();++i){
            char c=strrule[i];
            if(c=='('){
                if(i-1>=0&&strrule[i-1]=='!'){
                    left_brackets.push(left_bracket(i,true));
                }
                else{
                    left_brackets.push(left_bracket(i,false));
                }
            }
            else if(c==')'){
                if(left_brackets.empty()){
                    return false;
                }
                left_bracket tmp_lb=left_brackets.top();
                left_brackets.pop();
                bracket_val.push_back(bracket(strrule.substr(tmp_lb.index+1,i-(tmp_lb.index+1)),tmp_lb.index,i,tmp_lb.isNot));
            }
        }
        if(left_brackets.empty()==false){
            return false;
        }
        return true;
    }

    static void inter_call(bracket& bk){
        if(bk.isNot==true){
            bk.bracket_rule=new logExp(NOT,nullptr,part_strrule_parse(bk.val));
        }
        else{
            bk.bracket_rule=part_strrule_parse(bk.val);
        }
    }

    static ruleExp* part_strrule_parse(const string& strrule){
        string temp="";
        for(int i=0;i<strrule.size();++i){
            char c=strrule[i];
            if(c=='='||c=='>'||c=='<'||c=='!'){
                temp=strrule.substr(i);
                break;
            }
        }
        if(temp==""){
            throw string("Wrong expression");
        }
        char c=temp[1];
        if(c=='='){
            return binary_strrule_parse(temp);
        }
        return single_strrule_parse(temp);
    }

    static ruleExp* single_strrule_parse(const string& strrule){
        string val=strrule.substr(1);
        if(strrule[0]=='<'){
            return new numExp(SMAL,typeHelper::typehelper->strToBasic(val));
        }
        else if(strrule[0]=='>'){
            return new numExp(GRAT,typeHelper::typehelper->strToBasic(val));
        }
        else{
            throw string("Wrong expression");
        }
    }

    static ruleExp* binary_strrule_parse(const string& strrule){
        string val=strrule.substr(2);
        if(strrule[0]=='='){
            return new ruleExp(EQU,typeHelper::typehelper->strToBasic(val));
        }
        else if(strrule[0]=='>')
            return new logExp(OR,new ruleExp(EQU,typeHelper::typehelper->strToBasic(val)),new numExp(GRAT,typeHelper::typehelper->strToBasic(val)));
        else if (strrule[0]=='<') {
            return new logExp(OR,new ruleExp(EQU,typeHelper::typehelper->strToBasic(val)),new numExp(SMAL,typeHelper::typehelper->strToBasic(val)));
        }
        else if(strrule[0]=='!'){
            return new logExp(NOT,nullptr,new ruleExp(EQU,typeHelper::typehelper->strToBasic(val)));
        }
        else{
            throw string("Wrong expression");
        }
    }
};
