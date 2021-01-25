#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include "col.hpp"
using namespace std;

class IO
{
public:
    static int singleFileLen;

    static bool if_file_exist(const string& path)
    {
        ifstream f(path);
        return f.good();
    }

    static string path_to_name(const string& path)
    {
        int beg = path.find_last_of('\\');
        int end = path.find_last_of('.');
        return path.substr(beg + 1, end - (beg + 1));
    }

    static string path_to_lenpath(const string& path)
    {
        string len_path=path;
        len_path.insert(len_path.find_last_of("."),"_len");
        return len_path;
    }

    static string path_to_splitpath(const string& path,const int& num){
        string split_path=path;
        split_path.insert(split_path.find_last_of("."),"_part"+to_string(num));
        return split_path;
    }

    static string read_single_diskblock(const string& path){
        const string& to_do=IO::read_from_file(path);
        if(!IO::if_file_exist(IO::path_to_lenpath(path))){
            const vector<vector<int>>& len_data=IO::strdata_to_lendata(to_do);
            IO::write_to_len_file(path,len_data);
        }
        return to_do;
    }

    static int put_single_block_data(vector<col*> cols,const string& to_do){
        vector<vector<string>> frame;
        int beg = 0;
        int rowCount=-1;
        for (int i = 0; i < to_do.length(); i++) {
            if (to_do[i] == '\n') {
                const string& row = to_do.substr(beg, i - beg)+',';
                beg = i + 1;
                if(row[0]=='#'){
                    continue;
                }
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
                ++rowCount;
            }
        }
        for (int i = 0; i < frame[0].size(); i++) {
            col* column = cols[i];
            TYPE type=column->getType();
            for (int j = 1; j < frame.size(); j++) {
                column->add(typeHelper::typehelper->strToBasic(frame[j][i],type));
            }
        }
        return rowCount;
    }

    static string read_from_file(const string& path)
    {
        fstream process(path, ios::in);
        if (!process.is_open())
            throw string("fail to open the file(read)");
        stringstream buffer;
        buffer << process.rdbuf();
        string to_do = buffer.str();
        process.close();
        return to_do;
    }

    static void write_to_file(const string& path,const string& data)
    {
        fstream write(path,ios::out|ios::trunc);
        if (!write.is_open())
            throw string("fail to open the file(write)");
        write.seekp(0, ios::beg);
        write << data;
        write.close();
    }

    static vector<vector<int>> read_from_len_file(const string& path)
    {
        string ts=path_to_lenpath(path);
        fstream process(path_to_lenpath(path), ios::in);
        if (!process.is_open())
            throw string("fail to open the file(read)");
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

    static void write_to_len_file(const string& path,const vector<vector<int>>& len_data)
    {
        string len_string="";
        for(int i=0;i<len_data.size();i++){
            len_string=len_string+to_string(len_data[i][0])+","+to_string(len_data[i][1])+"\n";
        }
        fstream write(path_to_lenpath(path),ios::out|ios::trunc);
        if (!write.is_open())
            throw string("fail to open the file(write)");
        write.seekp(0, ios::beg);
        write << len_string;
        write.close();
    }

    static vector<vector<int>> strdata_to_lendata(const string& to_do){
        vector<vector<int>> len_data;
        int beg_get_len=0;
        int row_get_len=0;
        for(int i=0;i<to_do.length();i++){
            if(to_do[i]=='\n'){
                 len_data.push_back(vector<int>(2,0));
                 len_data[row_get_len][0]=(i-beg_get_len+1);
                 beg_get_len=i+1;
                 row_get_len++;
            }
        }
        return len_data;
    }

    static vector<col*> get_empty_table_cols(const string& tmpData){
        vector<col*> cols;
        string ID;
        TYPE type;
        int start=0;
        for(int i=0;i<tmpData.size();++i){
            char tmpChar=tmpData[i];
            if(tmpChar==':'){
                ID=tmpData.substr(start,i-start);
                start=i+1;
            }
            else if(tmpChar==','||tmpChar=='\n'){
                type=TYPE(stoi(tmpData.substr(start,i-start)));
                cols.push_back(new col(type, ID));
                if(tmpChar=='\n'){
                    break;
                }
                start=i+1;
            }
        }
        return cols;
    }

    static void do_add(const string& new_data,vector<vector<int>>& len_data,fstream& write)
    {
        write.seekp(0,ios::end);
        write << new_data;
        vector<int> item;
        item.push_back(new_data.length());
        item.push_back(len_data[len_data.size()-1][1]);
        len_data.push_back(item);
    }

    static void Add(vector<Basic*> turple,map<int,fstream>& fileStore,map<int,vector<vector<int>>>& lenStore,const string& path,vector<int>& blocksLen){
        string new_data="";
        for (int i=0;i<turple.size();i++){
            if(i!=turple.size()-1){
                new_data=new_data+(turple[i]->toStr())+",";
            }
            else{
                new_data=new_data+(turple[i]->toStr())+"\n";
            }
        }
        const string& splitPath=IO::path_to_splitpath(path,blocksLen.size());
        if(lenStore.find(blocksLen.size())==lenStore.end()){
            lenStore[blocksLen.size()]=IO::read_from_len_file(splitPath);
        }
        vector<vector<int>> lenData= lenStore[blocksLen.size()];
        if(lenData.size()>IO::singleFileLen){
            const string& data=IO::read_from_file(splitPath);
            const string& title=data.substr(0,data.find_first_of('\n')+1);
            const string& result=title+new_data;
            const string& tmp=IO::path_to_splitpath(path,blocksLen.size()+1);
            const vector<vector<int>>& len_data=IO::strdata_to_lendata(result);
            IO::write_to_file(tmp,result);
            IO::write_to_len_file(tmp,len_data);
            blocksLen.push_back(1);
        }
        else{
            if(fileStore.find(blocksLen.size())==fileStore.end()){
                fileStore[blocksLen.size()]=fstream(splitPath);
            }
            IO::do_add(new_data,lenStore[blocksLen.size()],fileStore[blocksLen.size()]);
            blocksLen[blocksLen.size()-1]=blocksLen[blocksLen.size()-1]+1;
        }
    }

    static void do_del(int opSub,vector<vector<int>>& len_data,fstream& write)
    {
        int opRow=opSub+len_data[opSub+1][1]+1;
        long loc=0;
        for(int i=0;i<opRow;i++){
            loc+=len_data[i][0];
        }
        write.seekp(loc+opRow,ios::beg);
        write << "#";
        for(int i=opSub+1;i<len_data.size();i++){
            len_data[i][1]=len_data[i][1]+1;
        }
    }

    static void Del(int opSub,map<int,fstream>& fileStore,map<int,vector<vector<int>>>& lenStore,const string& path,vector<int>& blocksLen){
        int num=1;
        for(const int& len:blocksLen){
            if(opSub-len>=0){
                opSub=opSub-len;
                ++num;
            }
            else{
                break;
            }
        }
        const string& splitPath=IO::path_to_splitpath(path,num);
        if(lenStore.find(num)==lenStore.end()){
            lenStore[num]=IO::read_from_len_file(splitPath);
        }
        if(fileStore.find(num)==fileStore.end()){
            fileStore[num]=fstream(splitPath);
        }
        IO::do_del(opSub,lenStore[num],fileStore[num]);
        blocksLen[num-1]=blocksLen[num-1]-1;
    }
};
