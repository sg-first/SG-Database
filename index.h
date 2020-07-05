#pragma once
#include <functional>
#include <vector>

using namespace std;

//每种索引是一个函数工厂，参数是col，返回根据这个col生成的查询函数，替换原先的查询对象
//查询对象输入一个条件，返回查询结果。也就是说查询对象里面要使用解析条件的组件
//返回的下标组必须排序
//如果操作的是视图，就把查询结果和视图的allSub取交集（返回的还是原表的sub而不是视图表的）

class indexGenerator
{
    //functional<vector<int>()> //fix:查询怎么表示？
};

