#pragma once
#include <functional>
#include <vector>
#include "rule.h"
#include "table.h"

using namespace std;

//每种索引是一个函数工厂，参数是col，返回根据这个col生成的查询函数，替换原先的查询对象
//查询对象输入一个条件，返回查询结果。也就是说查询对象里面要使用解析条件的组件
//返回的下标组必须排序
//如果操作的是视图，就把查询结果和视图的allSub取交集（返回的还是原表的sub而不是视图表的）

typedef function<vector<int>(ruleExp rule)> index;

class indexGenerator
{
public:
    index traversal(col* c)
    {
        auto result=[c](ruleExp rule)
        {
            vector<int> result;
            auto data=c->getAllData();
            for(int i=0;i<data.size();i++)
            {
                Basic* v=data[i];
                if(rule.eval(v))
                    result.push_back(i);
            }
            return result;
        };
        return result;
    }
};

