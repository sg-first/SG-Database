#pragma once
#include <vector>
#include <list>
#include "basicType.h"
#include "record.h"
#include "typeHelper.h"
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;
bool if_file_exist(const string& path){
    ifstream f(path);
    return f.good();
}
string path_to_name(const string& path) {
	int beg = path.find_last_of('\\');
	int end = path.find_last_of('.');
	return path.substr(beg + 1, end - (beg + 1));
}
string path_to_lenpath(const string& path){
    string len_path=path;
    len_path.insert(len_path.find_last_of("."),"_len");
    return len_path;
}
string read_from_file(const string& path){
    fstream process(path, ios::in);
    if (!process.is_open()) {
        cout<<"fail to open the file(read)"<<endl;
    }
    stringstream buffer;
    buffer << process.rdbuf();
    string to_do = buffer.str();
    process.close();
    return to_do;
}
void write_to_file(const string& path,const string& data){
    fstream write(path,ios::out|ios::trunc);
    if (!write.is_open()) {
        cout<<"fail to open the file(write)"<<endl ;
    }
    write.seekp(0, ios::beg);
    write << data;
    write.close();
}
vector<vector<int>> read_from_len_file(const string& path){
    fstream process(path_to_lenpath(path), ios::in);
    if (!process.is_open()) {
        cout<<"fail to open the file(read)"<<endl;
    }
    stringstream buffer;
    buffer << process.rdbuf();
    string len_data = buffer.str();
    process.close();
    vector<vector<int>> len_vec;
    int beg = 0;
    for (int i = 0; i < len_data.length(); i++) {
        if (len_data[i] == '\n') {
            string row = len_data.substr(beg, i - beg);
            row = row + ",";
            vector<int> row_vec;
            int row_beg = 0;
            for (int j = 0; j < row.length(); j++) {
                if (row[j] == ',') {
                    string item = row.substr(row_beg, j - row_beg);
                    row_vec.push_back(stoi(item));
                    row_beg = j + 1;
                }
            }
            len_vec.push_back(row_vec);
            beg = i + 1;
        }
    }
    return len_vec;
}
void write_to_len_file(const string& path,const vector<vector<int>>& len_data){
    string len_string="";
    for(int i=0;i<len_data.size();i++){
        len_string=len_string+to_string(len_data[i][0])+","+to_string(len_data[i][1])+"\n";
    }
    fstream write(path_to_lenpath(path),ios::out|ios::trunc);
    if (!write.is_open()) {
        cout<<"fail to open the file(write)"<<endl ;
    }
    write.seekp(0, ios::beg);
    write << len_string;
    write.close();
}
void do_add(vector<Basic*> turple,const string& path,vector<vector<int>>& len_data){
    string new_data="";
    for (int i=0;i<turple.size();i++){
        if(i!=turple.size()-1){
            new_data=new_data+(turple[i]->toStr())+",";
        }
        else{
            new_data=new_data+(turple[i]->toStr())+"\n";
        }   
    }
    fstream write(path,ios::app);
    if (!write.is_open()) {
        cout<<"fail to open the file(write)"<<endl ;
    }
    write << new_data;
    write.close();
    vector<int> item;
    item.push_back(new_data.length());
    item.push_back(len_data[len_data.size()-1][1]);
    len_data.push_back(item);
}
void do_del(int opSub,const string& path,vector<vector<int>>& len_data){
    int opRow=opSub+len_data[opSub+1][1];
    int loc=0;
    for(int i=0;i<(opRow+1);i++){
        loc+=len_data[i][0];
    }
    fstream write(path);
    if (!write.is_open()) {
        cout<<"fail to open the file(write)"<<endl ;
    }
    write.seekp(loc+opRow+1,ios::beg);
    write << "#";
    write.close();
    for(int i=opSub+1;i<len_data.size();i++){
        len_data[i][1]=len_data[i][1]+1;
    }
}
class col
{
private:
    const TYPE type;
    vector<Basic*> allData; //push进来直接视为持有所有权

public:
    col(TYPE type,string ID) : type(type), ID(ID) {}
    col(const col &c) : type(c.type), ID(c.ID)
    {
        for(Basic* v : this->allData)
            this->allData.push_back(typeHelper::copy(v));
    }
    string ID;
    //fix:还要有触发器和约束

    const vector<Basic*>& getAllData() { return this->allData; }

    TYPE getType() { return this->type; }

    void pushDate(Basic* v) //应该使用这个函数push
    {
        if(v->getType()!=this->getType())
            throw string("type mismatch");
        else
            this->allData.push_back(v);
    }

    col* genNewCol(vector<int> subList)
    {
        col* result=new col(type,ID);
        for(int i : subList)
            result->allData.push_back(typeHelper::copy(this->allData[i]));
        return result;
    }

    bool mod(int opSub,Basic* v) //会拷贝，返回对这个值的修改是否实际进行
    {
        if(v->getType()!=PLACEHOLDER && !typeHelper::isEqu(v,this->allData[opSub]))
        {
            delete this->allData[opSub];
            this->allData[opSub]=typeHelper::copy(v);
            return true;
        }
        return false;
    }

    void del(int opSub)
    {
        this->allData.erase(this->allData.begin()+opSub);
    }

    ~col()
    {
        for(Basic* v : allData)
            delete v;
    }
};


class table
{
private:
    vector<col*> allCol; //push进来直接视为持有所有权
    list<record> allRecord;

public:
    string ID;
    table(string ID) : ID(ID) {}
    table(string ID, vector<col*>allCol) : allCol(allCol), ID(ID) {}
    table(const table& t) : ID(t.ID)
    {
        for(col* c : t.allCol)
            this->allCol.push_back(new col(*c));
    }

    const vector<col*>& getAllCol() { return this->allCol; }//对col数据的修改必须经过table对象完成，否则无法

    string table_to_str(vector<vector<int>>& len_data){
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
	static table* loadFile(string path) //按约定格式从文件中读取表
	{
        string to_do=read_from_file(path);
        vector<vector<int>> len_data;
        if(!if_file_exist(path_to_lenpath(path))){
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
        if(if_file_exist(path_to_lenpath(path))){
            len_data=read_from_len_file(path);
        }
        for(int i=0;i<frame.size();i++){
           if('#'==frame[i][0][0]){
               frame.erase(frame.begin()+i);
               len_data.erase(len_data.begin()+i);
           }
        }
        write_to_len_file(path,len_data);
        frame.pop_back();
		table* table_frame = new table(path_to_name(path));
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
            table_frame->allCol.push_back(column);
		}
		return table_frame;
		//fix:编写此函数
	}


    void saveFile(string path) //将整个表的内容按约定格式写入空文件
    {
        vector<vector<int>> len_data;
        string data=this->table_to_str(len_data);
        write_to_file(path,data);
        write_to_len_file(path,len_data);
    }

    void updateFile(string path) //根据table.allRecord更新文件内容
    {
        //if((!if_file_exist(path))||(!if_file_exist(path_to_lenpath(path)))){
          //  this->saveFile(path);
        //}
        vector<vector<int>> len_data=read_from_len_file(path);
        for(record rcd:this->allRecord){
            switch (rcd.type) {
            case ADD:{
                do_add(rcd.targetTuple,path,len_data);
                break;
            }
            case DEL:{
                do_del(rcd.opSub,path,len_data);
                break;
            }
            case MOD:{
                do_del(rcd.opSub,path,len_data);
                do_add(rcd.targetTuple,path,len_data);
                break;
            }
            }
        }
        write_to_len_file(path,len_data);
        //全部写入后清除record记录
        this->allRecord.clear();
    }

    table* genNewTable(vector<int> subList) //注意这个视图是个新表，和原表没有对应关系
    {
        table* result=new table(this->ID);
        for(col* c : allCol)
        {
            col* selectCol=c->genNewCol(subList);
            result->allCol.push_back(selectCol);
        }
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
                recordTuple.push_back(new Placeholder()); //如果是不起实际作用的修改，就用Placeholder占位以免多占record空间
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
