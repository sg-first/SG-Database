#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "basicType.h"
using namespace std;

class IO
{
public:
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

    static void do_add(vector<Basic*> turple,const string& path,vector<vector<int>>& len_data)
    {
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
        if (!write.is_open())
            throw string("fail to open the file(write)");
        write << new_data;
        write.close();
        vector<int> item;
        item.push_back(new_data.length());
        item.push_back(len_data[len_data.size()-1][1]);
        len_data.push_back(item);
    }

    static void do_del(int opSub,const string& path,vector<vector<int>>& len_data)
    {
        int opRow=opSub+len_data[opSub+1][1];
        int loc=0;
        for(int i=0;i<(opRow+1);i++){
            loc+=len_data[i][0];
        }
        fstream write(path);
        if (!write.is_open())
            throw string("fail to open the file(write)");
        write.seekp(loc+opRow+1,ios::beg);
        write << "#";
        write.close();
        for(int i=opSub+1;i<len_data.size();i++){
            len_data[i][1]=len_data[i][1]+1;
        }
    }
};
