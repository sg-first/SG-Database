#pragma once
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValue>
#include <QVariant>
#include "js.h"
using namespace std;
class view: public QObject{
    Q_OBJECT
    table* viewTable;
    view(table* viewTable){this->viewTable=viewTable;}
public:

    static view* viewmanager;

    static view* getViewManager(){
        if(viewmanager==nullptr){
            viewmanager=new view (table::loadFile("Views"));
        }
        return viewmanager;
    }

    table* getViewTable(){
        return this->viewTable;
    }

    string viewNameToUserViewName(const string& viewName){
        return tableManager::getcurOperatUser()+":"+viewName;
    }

    JSVM* getViewJSVM()
    {
        JSVM* VM=AddJSVM();
        QScriptValue viewmanager=VM->newQObject(view::viewmanager);
        VM->globalObject().setProperty("viewmanager",viewmanager);
        return VM;
    }

    table* strDateToTable(const string& viewName,string& to_do){
        vector<vector<int>> len_data;
        to_do = to_do + "\n";
        vector<vector<string>> frame;
        int beg = 0;
        for (int i = 0; i < to_do.length(); i++) {
            if (to_do[i] == '\n') {
                string row = to_do.substr(beg, i - beg);
                row = row + ",";
                vector<string> row_vec;
                int row_beg = 0;
                for (int j = 0; j < row.length(); j++) {
                    if (row[j] == ',') {
                        string item = row.substr(row_beg, j - row_beg);
                        row_vec.push_back(item);
                        row_beg = j + 1;
                    }
                }
                frame.push_back(row_vec);
                beg = i + 1;
            }
        }
        frame.pop_back();
        vector<col*> cols;
        for (int i = 0; i < frame[0].size(); i++) {
            string ID;
            TYPE type;
            string head = frame[0][i];
            for (int cur = 0; cur < head.length(); cur++) {
                if (head[cur] == ':') {
                    ID = head.substr(0, cur - 0);
                    type = TYPE(stoi(head.substr(cur+1)));
                }
            }
            col* column = new col(type, ID);
            for (int j = 1; j < frame.size(); j++) {
                column->pushData(typeHelper::typehelper->strToBasic(frame[j][i],type));
            }
            cols.push_back(column);
        }
        return new table(viewName,cols);
    }

    Q_INVOKABLE void addView(const QString& viewName,const QString& viewCode){
        Basic* ts_1=typeHelper::typehelper->strToBasic(viewNameToUserViewName(viewName.toStdString()),STR);
        Basic* ts_2=typeHelper::typehelper->strToBasic(viewCode.toStdString(),STR);
        this->viewTable->add({typeHelper::typehelper->strToBasic(viewNameToUserViewName(viewName.toStdString()),STR),typeHelper::typehelper->strToBasic(viewCode.toStdString(),STR)});
    }

    Q_INVOKABLE bool delView(const QString& viewName){
        vector<int> viewIndex=this->viewTable->find({"(x=='"+viewNameToUserViewName(viewName.toStdString())+"')",""});
        if(viewIndex.empty()==true){
            return false;
        }
        this->viewTable->del(viewIndex);
        return true;
    }

    Q_INVOKABLE table* evalView(const QString& viewName){
        vector<int> viewIndex=this->viewTable->find({"(x=='"+viewNameToUserViewName(viewName.toStdString())+"')",""});
        if(viewIndex.empty()==true){
            throw string("The specified view was not found");
        }
        string viewCode=this->viewTable->getCol("viewCode")->getData(viewIndex)[0]->toStr();
        QString mistake;
        QVariant varResult=JSEval(QString::fromStdString(viewCode),"eval",&mistake,getViewJSVM());
        if(varResult==NULL){
            throw string("Wrong view code");
        }
        string strData=varResult.toString().toStdString();
        return strDateToTable(viewName.toStdString(),strData);
    }
};
