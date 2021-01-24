#include "tableManager.h"
#include <QRunnable>
#include <QThreadPool>
class multiSave :public QRunnable{
    const string tmp;
    const string data;
    const vector<vector<int>> len_data;
public:
    multiSave(const string& tmp,const string& data,const vector<vector<int>>& len_data):tmp(tmp),data(data),len_data(len_data){}
    virtual void run(){
        IO::write_to_file(tmp,data);
        IO::write_to_len_file(tmp,len_data);
    }
};

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
        return QString::fromStdString(data);
}


vector<string> table::toStr(const int& fileLen){
    vector<string> result;
    const string& allData=toStr().toStdString();
    int count=1;
    long rowNum=0;
    long start=0;
    string title;
    for(long i=0;i<allData.size();++i){
        if(allData[i]=='\n'){
            if(rowNum==0){
                title=allData.substr(start,i-start+1);
                start=i+1;
            }
            else if(rowNum==count*fileLen){
                result.push_back(title+allData.substr(start,i-start+1));
                ++count;
                start=i+1;
            }
            ++rowNum;
        }
    }
    if((rowNum-1)%IO::singleFileLen!=0){
        result.push_back(title+allData.substr(start));
    }
    if(result.empty()==true){
        result.push_back(title);
    }
    return result;
}

void table:: saveFile(const string& path) //将整个表的内容按约定格式写入空文件
{
    tableManager::tablemanager->deliManage(this);
    QThreadPool::globalInstance()->setMaxThreadCount(10);
    const vector<string>& allData=this->toStr(IO::singleFileLen);
    int num=1;
    for(const string& data:allData){
        const string& tmp=IO::path_to_splitpath(path,num);
        const vector<vector<int>>& len_data=IO::strdata_to_lendata(data);
        multiSave* ms= new multiSave(tmp,data,len_data);
        ms->setAutoDelete(true);
        QThreadPool::globalInstance()->start(ms);
        ++num;
    }
    while(IO::if_file_exist(IO::path_to_splitpath(path,num))){
        remove(IO::path_to_splitpath(path,num).c_str());
        remove(IO::path_to_lenpath(IO::path_to_splitpath(path,num)).c_str());
        ++num;
    }
    this->allRecord.clear();
}

table* table::loadFile(const string& path,int mark) //按约定格式从文件中读取表
{
    vector<col*> cols;
    vector<int> blocksLen;
    int num=1;
    while (IO::if_file_exist(IO::path_to_splitpath(path,num))) {
        if(num==1){
            cols=IO::get_empty_table_cols(IO::path_to_splitpath(path,num));
        }
        const int& tmpBlockLen=IO::read_single_diskblock(IO::path_to_splitpath(path,num),cols);
        blocksLen.push_back(tmpBlockLen);
        ++num;
    }
    if(num==1){
        throw string("No corresponding table was found");
    }
    return new table(IO::path_to_name(path),cols,blocksLen);
}

void table::updateFile(const string& path) //根据table.allRecord更新文件内容
{
    if(this->allRecord.empty()){
        return;
    }
    map<int,fstream> fileStore;
    map<int,vector<vector<int>>> lenStore;
    for(record rcd:this->allRecord){
        switch (rcd.type) {
        case ADD:{
            IO::Add(rcd.targetTuple,fileStore,lenStore,path,this->allBlocksLen);
            break;
        }
        case DEL:{
            IO::Del(rcd.opSub,fileStore,lenStore,path,this->allBlocksLen);
            break;
        }
        case MOD:{
            IO::Del(rcd.opSub,fileStore,lenStore,path,this->allBlocksLen);
            IO::Add(rcd.targetTuple,fileStore,lenStore,path,this->allBlocksLen);
            break;
        }
        }
    }
    for(auto iter=fileStore.begin();iter!=fileStore.end();iter++){
        iter->second.close();
    }
    for(auto iter=lenStore.begin();iter!=lenStore.end();iter++){
        IO::write_to_len_file(IO::path_to_splitpath(path,iter->first),iter->second);
    }
    //全部写入后清除record记录
    this->allRecord.clear();
}

