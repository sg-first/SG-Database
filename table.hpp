#pragma once
#include "col.hpp"
#include "index.hpp"
#include "IO.hpp"

class table
{
private:
    vector<col*> allCol; //push进来直接视为持有所有权
    list<record> allRecord;

    string table_to_str(vector<vector<int>>& len_data)
    {
        int m=(this->allCol[0]->getAllData()).size()+1;
        int n=this->allCol.size();
        vector<vector<string>> tableframe(m,vector<string>(n,""));
        for(int i=0;i<n;i++){
            int sort=this->allCol[i]->getType();
            tableframe[0][i]=(this->allCol[i]->ID)+":"+to_string(sort);
        }
        int i=0;
        for (col* c : this->allCol)
        {   int j=1;
            for (Basic* b : c->getAllData())
            {
                tableframe[j][i]=b->toStr();
                j++;
            }
            i++;
        }
        string data;
        for(int i=0;i<m;i++){
            for(int j=0;j<n;j++){
                if(j==n-1){
                    data=data+tableframe[i][j]+"\n";
                }
                else{
                    data=data+tableframe[i][j]+",";
                }
            }
        }
        int beg=0;
        int row=0;
        for(int i=0;i<data.length();i++){
            if(data[i]=='\n'){
                 len_data.push_back(vector<int>(2,0));
                 len_data[row][0]=(i-beg+1);
                 len_data[row][1]=0;
                 beg=i+1;
                 row++;
            }
        }
        return data;
    }

public:
    string ID;
    table(string ID, vector<col*>allCol) : allCol(allCol), ID(ID) {} //allCol中元素转移所有权
    table(const table& t) : ID(t.ID)
    {
        for(col* c : t.allCol)
            this->allCol.push_back(new col(*c));
    }

    const vector<col*>& getAllCol() { return this->allCol; } //对col数据的修改必须经过table对象完成，否则无法

    static table* loadFile(string path) //按约定格式从文件中读取表
    {
        string to_do=IO::read_from_file(path);
        vector<vector<int>> len_data;
        if(!IO::if_file_exist(IO::path_to_lenpath(path))){
            int beg_get_len=0;
            int row_get_len=0;
            for(int i=0;i<to_do.length();i++){
                if(to_do[i]=='\n'){
                     len_data.push_back(vector<int>(2,0));
                     len_data[row_get_len][0]=(i-beg_get_len+1);
                     len_data[row_get_len][1]=0;
                     beg_get_len=i+1;
                     row_get_len++;
                }
            }
         }
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
        if(IO::if_file_exist(IO::path_to_lenpath(path))){
            len_data=IO::read_from_len_file(path);
        }
        for(int i=0;i<frame.size();i++){
           if('#'==frame[i][0][0]){
               frame.erase(frame.begin()+i);
               len_data.erase(len_data.begin()+i);
           }
        }
        IO::write_to_len_file(path,len_data);
        frame.pop_back();

        vector<col*> _allCol;
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
                column->pushDate(typeHelper::strToBasic(frame[j][i],type));
            }
            _allCol.push_back(column);
        }

        return new table(IO::path_to_name(path), _allCol);
    }

    void saveFile(string path) //将整个表的内容按约定格式写入空文件
    {
        vector<vector<int>> len_data;
        string data=this->table_to_str(len_data);
        IO::write_to_file(path,data);
        IO::write_to_len_file(path,len_data);
    }

    void updateFile(string path) //根据table.allRecord更新文件内容
    {
        vector<vector<int>> len_data=IO::read_from_len_file(path);
        for(record rcd:this->allRecord)
        {
            switch (rcd.type)
            {
                case ADD:{
                    IO::do_add(rcd.targetTuple,path,len_data);
                    break;
                }
                case DEL:{
                    IO::do_del(rcd.opSub,path,len_data);
                    break;
                }
                case MOD:{
                    IO::do_del(rcd.opSub,path,len_data);
                    IO::do_add(rcd.targetTuple,path,len_data);
                    break;
                }
            }
        }
        IO::write_to_len_file(path,len_data);
        //全部写入后清除record记录
        this->allRecord.clear();
    }

    table* genNewTable(vector<int> subList) //注意这个视图是个新表，和原表没有对应关系
    {
        vector<col*> newAllCol;
        for(col* c : allCol)
        {
            col* selectCol=c->genNewCol(subList);
            newAllCol.push_back(selectCol);
        }
        table* result=new table(this->ID, newAllCol);
        return result;
    }

    void add(vector<Basic*> tuple)
    {
        if(tuple.size()!=this->allCol.size())
            throw string("Col Size mismatch");
        for(int i=0;i<this->allCol.size();i++)
        {
            col* c=this->allCol[i];
            c->pushDate(tuple[i]);
        }
        //写入记录
        this->allRecord.push_back(record(tuple));
    }

    void mod(int opSub, vector<Basic*> tuple)
    {
        if(tuple.size()!=this->allCol.size())
            throw string("Col Size mismatch");

        vector<Basic*> recordTuple;
        for(int i=0;i<this->allCol.size();i++)
        {
            col* c=this->allCol[i];
            bool modResult=c->mod(opSub,tuple[i]);
            if(modResult)
                recordTuple.push_back(typeHelper::copy(tuple[i]));
            else
            {
                #ifdef logStrategy
                recordTuple.push_back(new Placeholder()); //如果是不起实际作用的修改，就用Placeholder占位以免多占record空间
                #else
                recordTuple.push_back(c->getAllData()[opSub]);
                #endif
            }
        }

        //写入记录
        this->allRecord.push_back(record(opSub,recordTuple));
    }

    void del(int opSub)
    {
        for(int i=0;i<this->allCol.size();i++)
            this->allCol[i]->del(opSub);
        //写入记录
        this->allRecord.push_back(record(opSub));
    }

    void del(vector<int> allOpSub) //对选择出来的一系列下标一起进行删除，要求有序
    {
        int opFinishedNum=0;
        for(int i : allOpSub)
        {
            this->del(i-opFinishedNum); //前面的删掉了后面下标会向前串
            opFinishedNum++;
        }
    }

    ~table()
    {
        for(col* c : allCol)
            delete c;
    }
};
