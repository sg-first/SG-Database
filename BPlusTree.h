#pragma once
#include <utility>
using namespace std;

#define ORDER_V 2    /* 为简单起见，把v固定为2，实际的B+树v值应该是可配的。这里的v是内部节点中键的最小值 */

#define MAXNUM_KEY (ORDER_V * 2)    /* 内部结点中最多键个数，为2v */
#define MAXNUM_POINTER (MAXNUM_KEY + 1)    /* 内部结点中最多指向子树的指针个数，为2v */
#define MAXNUM_DATA (ORDER_V * 2)    /* 叶子结点中最多数据个数，为2v */

/* 存键值(basic.val)和数据段(下标)的结构 */
typedef pair<float,int> DATA_TYPE;
typedef float KEY_TYPE;
typedef int VAL_TYPE;

/* 结点类型 */
enum NODE_TYPE
{
NODE_TYPE_ROOT     = 1,    // 根结点
NODE_TYPE_INTERNAL = 2,    // 内部结点
NODE_TYPE_LEAF     = 3,    // 叶子结点
};

const DATA_TYPE INVALID=DATA_TYPE(-1,-1);

#define FLAG_LEFT 1
#define FLAG_RIGHT 2

/* 结点数据结构，为内部结点和叶子结点的父类 */
class CNode
{
public:
    CNode();
    virtual ~CNode();

    //获取和设置结点类型
    NODE_TYPE GetType() { return m_Type; }
    void SetType(NODE_TYPE type) {m_Type = type;}

    // 获取和设置有效数据个数
    int GetCount() { return m_Count;}
    void SetCount(int i) { m_Count = i; }

    // 获取和设置某个元素，对中间结点指键值，对叶子结点指数据
    virtual DATA_TYPE GetElement(int i) {return INVALID;}
    virtual void SetElement(int i, DATA_TYPE value) { }

    // 获取和设置某个指针，对中间结点指指针，对叶子结点无意义
    virtual CNode* GetPointer(int i) {return nullptr;}
    virtual void SetPointer(int i, CNode* pointer) { }

    // 获取和设置父结点
    CNode* GetFather() { return m_pFather;}
    void SetFather(CNode* father) { m_pFather = father; }

    // 获取一个最近的兄弟结点
    CNode* GetBrother(int& flag);

    // 删除结点
    void DeleteChildren();

protected:
    NODE_TYPE m_Type;    // 结点类型，取值为NODE_TPE类型
    int m_Count;    // 有效数据个数，对中间结点指键个数，对叶子结点指数据个数
    CNode* m_pFather;     // 指向父结点的指针，标准B+树中并没有该指针，加上是为了更快地实现结点分裂和旋转等操作
};

/* 内部结点数据结构 */
class CInternalNode : public CNode
{
public:
    CInternalNode();
    virtual ~CInternalNode();

    // 获取和设置键值，对用户来说，数字从1开始，实际在结点中是从0开始的
    DATA_TYPE GetElement(int i)
    {
        if ((i > 0 ) && (i <= MAXNUM_KEY))
        {
            return m_Keys[i - 1];
        }
        else
        {
            return INVALID;
        }
    }

    void SetElement(int i, DATA_TYPE key)
    {
        if ((i > 0 ) && (i <= MAXNUM_KEY))
        {
            m_Keys[i - 1] = key;
        }
    }

    // 获取和设置指针，对用户来说，数字从1开始
    CNode* GetPointer(int i)
    {
        if ((i > 0 ) && (i <= MAXNUM_POINTER))
        {
            return m_Pointers[i - 1];
        }
        else
        {
            return nullptr;
        }
    }

    void SetPointer(int i, CNode* pointer)
    {
        if ((i > 0 ) && (i <= MAXNUM_POINTER))
        {
            m_Pointers[i - 1] = pointer;
        }
    }

    // 在结点pNode上插入键value
    bool Insert(DATA_TYPE value, CNode* pNode);
    // 删除键value
    bool Delete(DATA_TYPE value);

    // 分裂结点
    DATA_TYPE Split(CInternalNode* pNode, DATA_TYPE key);
    // 结合结点(合并结点)
    bool Combine(CNode* pNode);
    // 从另一结点移一个元素到本结点
    bool MoveOneElement(CNode* pNode);

protected:
    DATA_TYPE m_Keys[MAXNUM_KEY];           // 键数组
    CNode* m_Pointers[MAXNUM_POINTER];     // 指针数组

};

/* 叶子结点数据结构 */
class CLeafNode : public CNode
{
public:
    CLeafNode();
    virtual ~CLeafNode();

    // 获取和设置数据
    DATA_TYPE GetElement(int i)
    {
        if ((i > 0 ) && (i <= MAXNUM_DATA))
        {
            return m_Datas[i - 1];
        }
        else
        {
            return INVALID;
        }
    }

    void SetElement(int i, DATA_TYPE data)
    {
        if ((i > 0 ) && (i <= MAXNUM_DATA))
        {
            m_Datas[i - 1] = data;
        }
    }

    // 获取和设置指针，对叶子结点无意义，只是实行父类的虚函数
    CNode* GetPointer(int i)
    {
        return nullptr;
    }

    // 插入数据
    bool Insert(DATA_TYPE value);
    // 删除数据
    bool Delete(KEY_TYPE value);

    // 分裂结点
    DATA_TYPE Split(CNode* pNode);
    // 结合结点
    bool Combine(CNode* pNode);

    // 以下两个变量用于实现双向链表
    CLeafNode* m_pPrevNode;                 // 前一个结点
    CLeafNode* m_pNextNode;                 // 后一个结点

protected:
    DATA_TYPE m_Datas[MAXNUM_DATA];    // 数据数组

};

/* B+树数据结构 */
class BPlusTree
{
public:
    BPlusTree();
    virtual ~BPlusTree();

    // 查找指定的数据
    VAL_TYPE Search(KEY_TYPE key);
    // 插入指定的数据
    bool Insert(DATA_TYPE data);
    // 删除指定的数据
    bool Delete(KEY_TYPE data);

    // 清除树
    void ClearTree();

    // 旋转树
    BPlusTree* RotateTree();

    // 检查树是否满足B+树的定义
    bool CheckTree();

    // 递归检查结点及其子树是否满足B+树的定义
    bool CheckNode(CNode* pNode);

    // 获取和设置根结点
    CNode* GetRoot()
    {
        return m_Root;
    }

    void SetRoot(CNode* root)
    {
        m_Root = root;
    }

    // 获取和设置深度
    int GetDepth()
    {
        return m_Depth;
    }

    void SetDepth(int depth)
    {
        m_Depth = depth;
    }

    // 深度加一
    void IncDepth()
    {
        m_Depth = m_Depth + 1;
    }

    // 深度减一
    void DecDepth()
    {
        if (m_Depth > 0)
        {
            m_Depth = m_Depth - 1;
        }
    }

    // 以下两个变量用于实现双向链表
    CLeafNode* m_pLeafHead;                 // 头结点
    CLeafNode* m_pLeafTail;                   // 尾结点

protected:
    // 为插入而查找叶子结点
    CLeafNode* SearchLeafNode(KEY_TYPE data);
    //插入键到中间结点
    bool InsertInternalNode(CInternalNode* pNode, DATA_TYPE key, CNode* pRightSon);
    // 在中间结点中删除键
    bool DeleteInternalNode(CInternalNode* pNode, DATA_TYPE key);

    CNode* m_Root;    // 根结点
    int m_Depth;      // 树的深度
};
