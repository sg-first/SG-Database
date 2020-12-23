#pragma once
#include <map>
#include "dbProcess.h"
#include "table.hpp"
#include "js.h"
using namespace std;
class userView{
    string userName;
    string viewName;
    string geneCode;
public:
    userView(string username,string viewname,string genecode):userName(username),viewName(viewname),geneCode(genecode){}

    string getviewName()const{
        return viewName;
    }

    shared_ptr<operatTable> load(){
        QString* mistake=nullptr;
        QVariant varResult=JSEval(QString::fromStdString(geneCode),"",mistake,AddJSVM());
        if(*mistake!=nullptr){
            throw(string("Wrong genecode"));
        }
        operatTable* opTable=varResult.value<operatTable*>();
        return shared_ptr<operatTable>(opTable);
    }
};
class viewManager{
    map<string,vector<userView>> allView;
public:
    void addView(const string& viewname,const string& genecode){
        allView[dbProcess::curOperatUser].push_back(userView(dbProcess::curOperatUser,viewname,genecode));
    }

    userView getView(const string& viewname){
        vector<userView> userAllView=allView[dbProcess::curOperatUser];
        for(const userView& view:userAllView){
            if(view.getviewName()==viewname){
                return view;
            }
        }
        throw(string("No such view"));
    }

    void delView(const string& viewname){
        vector<userView> userAllView=allView[dbProcess::curOperatUser];
        for(auto iter=userAllView.begin();iter!=userAllView.end();){
            if(iter->getviewName()==viewname){
                userAllView.erase(iter);
                return;
            }
            else{
                iter++;
            }
        }
        throw(string("No such view"));
    }
};




