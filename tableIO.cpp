#include "table.hpp"

QString table::toStr()
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
    return QString::fromStdString(data);;
}


void table::update_len(vector<vector<int> > & len_data,const string& data){
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
}

void table:: saveFile(string path) //将整个表的内容按约定格式写入空文件
{
    string data=this->toStr().toStdString();
    vector<vector<int>> len_data;
    this->update_len(len_data,data);
    IO::write_to_file(path,data);
    IO::write_to_len_file(path,len_data);
    this->allRecord.clear();
}

table* table::loadFile(string path,int mark) //按约定格式从文件中读取表
{
    string to_do=IO::read_from_file(path);
    vector<vector<int>> len_data;
    bool rewriteFlag=false;
    if(!IO::if_file_exist(IO::path_to_lenpath(path))){
        rewriteFlag=true;
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
    else{
        len_data=IO::read_from_len_file(path);
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
    for(int i=0;i<frame.size();i++){
       if('#'==frame[i][0][0]){
           rewriteFlag=true;
           frame.erase(frame.begin()+i);
           len_data.erase(len_data.begin()+i);
           i--;
       }
    }
    if(rewriteFlag==true){
        IO::write_to_len_file(path,len_data);
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
    return new table(IO::path_to_name(path),cols);
}

void table::updateFile(string path) //根据table.allRecord更新文件内容
{
    if(allRecord.empty()){
        return;
    }
    vector<vector<int>> len_data=IO::read_from_len_file(path);
    for(record rcd:this->allRecord){
        switch (rcd.type) {
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

